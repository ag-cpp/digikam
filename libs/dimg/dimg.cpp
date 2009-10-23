/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-06-14
 * Description : digiKam 8/16 bits image management API
 *
 * Copyright (C) 2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2005-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2009 by Marcel Wiesweg <marcel.wiesweg@gmx.de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "dimg.h"
#include "dimg_p.h"

// C ANSI includes

#ifndef Q_CC_MSVC
extern "C"
{
#endif
#include <stdint.h>
#ifndef Q_CC_MSVC
#include <math.h>
}
#endif

// C++ includes

#include <cstdio>

// Qt includes

#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QPixmap>
#include <QSysInfo>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

#if KDCRAW_VERSION < 0x000400
#include <libkdcraw/dcrawbinary.h>
#endif

// Local includes

#include "debug.h"
#include "pngloader.h"
#include "jpegloader.h"
#include "tiffloader.h"
#include "ppmloader.h"
#include "rawloader.h"
#include "jp2kloader.h"
#include "pgfloader.h"
#include "qimageloader.h"
#include "icctransform.h"
#include "exposurecontainer.h"
#include "dmetadata.h"
#include "dimgloaderobserver.h"

typedef uint64_t ullong;
typedef int64_t  llong;

namespace Digikam
{

DImg::DImg()
    : m_priv(new DImgPrivate)
{
}

DImg::DImg(const QByteArray& filePath, DImgLoaderObserver *observer,
           DRawDecoding rawDecodingSettings)
    : m_priv(new DImgPrivate)
{
    load(filePath, observer, rawDecodingSettings);
}

DImg::DImg(const QString& filePath, DImgLoaderObserver *observer,
           DRawDecoding rawDecodingSettings)
    : m_priv(new DImgPrivate)
{
    load(filePath, observer, rawDecodingSettings);
}

DImg::DImg(const DImg& image)
{
    m_priv = image.m_priv;
}

DImg::DImg(uint width, uint height, bool sixteenBit, bool alpha, uchar* data, bool copyData)
    : m_priv(new DImgPrivate)
{
    putImageData(width, height, sixteenBit, alpha, data, copyData);
}

DImg::DImg(const DImg& image, int w, int h)
    : m_priv(new DImgPrivate)
{
    // This private constructor creates a copy of everything except the data.
    // The image size is set to the given values and a buffer corresponding to these values is allocated.
    // This is used by copy and scale.
    copyImageData(image.m_priv);
    copyMetaData(image.m_priv);
    setImageDimension(w, h);
    allocateData();
}

DImg::DImg(const QImage& image)
    : m_priv(new DImgPrivate)
{
    if (!image.isNull())
    {
        QImage target = image.convertToFormat(QImage::Format_ARGB32);

        uint w      = target.width();
        uint h      = target.height();
        uchar* data = new uchar[w*h*4];
        uint*  sptr = (uint*)target.bits();
        uchar* dptr = data;

        for (uint i = 0 ; i < w*h ; ++i)
        {
            dptr[0] = qBlue(*sptr);
            dptr[1] = qGreen(*sptr);
            dptr[2] = qRed(*sptr);
            dptr[3] = qAlpha(*sptr);

            dptr += 4;
            ++sptr;
        }

        putImageData(w, h, false, image.hasAlphaChannel(), data, false);
    }
}

DImg::~DImg()
{
}


//---------------------------------------------------------------------------------------------------
// data management


DImg& DImg::operator=(const DImg& image)
{
    m_priv = image.m_priv;
    return *this;
}

bool DImg::operator==(const DImg& image) const
{
    return m_priv == image.m_priv;
}

void DImg::reset(void)
{
    m_priv = new DImgPrivate;
}

void DImg::detach()
{
    // are we being shared?
    if (!m_priv->hasMoreReferences())
    {
        return;
    }

    DSharedDataPointer<DImgPrivate> old = m_priv;

    m_priv = new DImgPrivate;
    copyImageData(old);
    copyMetaData(old);

    if (old->data)
    {
        int size = allocateData();
        memcpy(m_priv->data, old->data, size);
    }
}

void DImg::putImageData(uint width, uint height, bool sixteenBit, bool alpha, uchar *data, bool copyData)
{
    // set image data, metadata is untouched

    bool null = (width == 0) || (height == 0);
    // allocateData, or code below will set null to false
    setImageData(true, width, height, sixteenBit, alpha);

    // replace data
    delete [] m_priv->data;
    if (null)
    {
        // image is null - no data
        m_priv->data = 0;
    }
    else if (copyData)
    {
        int size = allocateData();
        if (data)
            memcpy(m_priv->data, data, size);
    }
    else
    {
        if (data)
        {
            m_priv->data = data;
            m_priv->null = false;
        }
        else
            allocateData();
    }
}

void DImg::putImageData(uchar *data, bool copyData)
{
    if (!data)
    {
        delete [] m_priv->data;
        m_priv->data = 0;
        m_priv->null = true;
    }
    else if (copyData)
    {
        memcpy(m_priv->data, data, numBytes());
    }
    else
    {
        m_priv->data = data;
    }
}

void DImg::resetMetaData()
{
    m_priv->attributes.clear();
    m_priv->embeddedText.clear();
    m_priv->metaData.clear();
}

uchar *DImg::stripImageData()
{
    uchar *data  = m_priv->data;
    m_priv->data = 0;
    m_priv->null = true;
    return data;
}

void DImg::copyMetaData(const DImgPrivate *src)
{
    m_priv->isReadOnly   = src->isReadOnly;
    m_priv->attributes   = src->attributes;
    m_priv->embeddedText = src->embeddedText;
    m_priv->iccProfile   = src->iccProfile;
    //FIXME: what about sharing and deleting lanczos_func?

    // since qbytearrays are explicitly shared, we need to make sure that they are
    // detached from any shared references

    for (QMap<int, QByteArray>::const_iterator it = src->metaData.constBegin();
         it != src->metaData.constEnd(); ++it)
    {
        // Insert a deep copy...
        m_priv->metaData.insert(it.key(), QByteArray(it.value()));
    }
}

void DImg::copyImageData(const DImgPrivate *src)
{
    setImageData(src->null, src->width, src->height, src->sixteenBit, src->alpha);
}

int DImg::allocateData()
{
    int size = m_priv->width * m_priv->height * (m_priv->sixteenBit ? 8 : 4);
    try {
        m_priv->data = new uchar[size];
    } catch (std::bad_alloc &ex) {
        m_priv->null = true;
        return 0;
    }
    m_priv->null = false;
    return size;
}

void DImg::setImageDimension(uint width, uint height)
{
    m_priv->width  = width;
    m_priv->height = height;
}

void DImg::setImageData(bool null, uint width, uint height, bool sixteenBit, bool alpha)
{
    m_priv->null       = null;
    m_priv->width      = width;
    m_priv->height     = height;
    m_priv->alpha      = alpha;
    m_priv->sixteenBit = sixteenBit;
}


//---------------------------------------------------------------------------------------------------
// load and save

bool DImg::loadImageInfo(const QString& filePath, bool loadMetadata, bool loadICCData, bool loadUniqueHash)
{
    DImgLoader::LoadFlags loadFlags = DImgLoader::LoadImageInfo;
    if (loadMetadata)
        loadFlags |= DImgLoader::LoadMetadata;
    if (loadICCData)
        loadFlags |= DImgLoader::LoadICCData;
    if (loadUniqueHash)
        loadFlags |= DImgLoader::LoadUniqueHash;

    return load(filePath, loadFlags, 0, DRawDecoding());
}

bool DImg::load(const QString& filePath, DImgLoaderObserver *observer,
                DRawDecoding rawDecodingSettings)
{
    return load(filePath, DImgLoader::LoadAll, observer, rawDecodingSettings);
}

bool DImg::load(const QString& filePath, bool loadMetadata, bool loadICCData, bool loadUniqueHash,
                DImgLoaderObserver *observer, DRawDecoding rawDecodingSettings)
{
    DImgLoader::LoadFlags loadFlags = DImgLoader::LoadImageInfo | DImgLoader::LoadImageData;
    if (loadMetadata)
        loadFlags |= DImgLoader::LoadMetadata;
    if (loadICCData)
        loadFlags |= DImgLoader::LoadICCData;
    if (loadUniqueHash)
        loadFlags |= DImgLoader::LoadUniqueHash;

    return load(filePath, loadFlags, observer, rawDecodingSettings);
}

bool DImg::load(const QString& filePath, int loadFlagsInt, DImgLoaderObserver *observer,
                DRawDecoding rawDecodingSettings)
{
    FORMAT format = fileFormat(filePath);
    DImgLoader::LoadFlags loadFlags = (DImgLoader::LoadFlags)loadFlagsInt;

    setAttribute("detectedFileFormat", format);
    setAttribute("originalFilePath", filePath);

    switch (format)
    {
        case(NONE):
        {
            kDebug(digiKamAreaCode) << filePath << " : Unknown image format !!!";
            return false;
            break;
        }
        case(JPEG):
        {
            kDebug(digiKamAreaCode) << filePath << " : JPEG file identified";
            JPEGLoader loader(this);
            loader.setLoadFlags(loadFlags);
            if (loader.load(filePath, observer))
            {
                m_priv->null       = !loader.hasLoadedData();
                m_priv->alpha      = loader.hasAlpha();
                m_priv->sixteenBit = loader.sixteenBit();
                m_priv->isReadOnly = loader.isReadOnly();
                return true;
            }
            break;
        }
        case(TIFF):
        {
            kDebug(digiKamAreaCode) << filePath << " : TIFF file identified";
            TIFFLoader loader(this);
            loader.setLoadFlags(loadFlags);
            if (loader.load(filePath, observer))
            {
                m_priv->null       = !loader.hasLoadedData();
                m_priv->alpha      = loader.hasAlpha();
                m_priv->sixteenBit = loader.sixteenBit();
                m_priv->isReadOnly = loader.isReadOnly();
                return true;
            }
            break;
        }
        case(PNG):
        {
            kDebug(digiKamAreaCode) << filePath << " : PNG file identified";
            PNGLoader loader(this);
            loader.setLoadFlags(loadFlags);
            if (loader.load(filePath, observer))
            {
                m_priv->null       = !loader.hasLoadedData();
                m_priv->alpha      = loader.hasAlpha();
                m_priv->sixteenBit = loader.sixteenBit();
                m_priv->isReadOnly = loader.isReadOnly();
                return true;
            }
            break;
        }
        case(PPM):
        {
            kDebug(digiKamAreaCode) << filePath << " : PPM file identified";
            PPMLoader loader(this);
            loader.setLoadFlags(loadFlags);
            if (loader.load(filePath, observer))
            {
                m_priv->null       = !loader.hasLoadedData();
                m_priv->alpha      = loader.hasAlpha();
                m_priv->sixteenBit = loader.sixteenBit();
                m_priv->isReadOnly = loader.isReadOnly();
                return true;
            }
            break;
        }
        case(RAW):
        {
            kDebug(digiKamAreaCode) << filePath << " : RAW file identified";
            RAWLoader loader(this, rawDecodingSettings);
            loader.setLoadFlags(loadFlags);
            if (loader.load(filePath, observer))
            {
                m_priv->null       = !loader.hasLoadedData();
                m_priv->alpha      = loader.hasAlpha();
                m_priv->sixteenBit = loader.sixteenBit();
                m_priv->isReadOnly = loader.isReadOnly();
                loader.postProcess(observer);
                return true;
            }
            break;
        }
        case(JP2K):
        {
            kDebug(digiKamAreaCode) << filePath << " : JPEG2000 file identified";
            JP2KLoader loader(this);
            loader.setLoadFlags(loadFlags);
            if (loader.load(filePath, observer))
            {
                m_priv->null       = !loader.hasLoadedData();
                m_priv->alpha      = loader.hasAlpha();
                m_priv->sixteenBit = loader.sixteenBit();
                m_priv->isReadOnly = loader.isReadOnly();
                return true;
            }
            break;
        }
        case(PGF):
        {
            kDebug(digiKamAreaCode) << filePath << " : PGF file identified";
            PGFLoader loader(this);
            loader.setLoadFlags(loadFlags);
            if (loader.load(filePath, observer))
            {
                m_priv->null       = !loader.hasLoadedData();
                m_priv->alpha      = loader.hasAlpha();
                m_priv->sixteenBit = loader.sixteenBit();
                m_priv->isReadOnly = loader.isReadOnly();
                return true;
            }
            break;
        }
        default:
        {
            kDebug(digiKamAreaCode) << filePath << " : QIMAGE file identified";
            QImageLoader loader(this);
            loader.setLoadFlags(loadFlags);
            if (loader.load(filePath, observer))
            {
                m_priv->null       = !loader.hasLoadedData();
                m_priv->alpha      = loader.hasAlpha();
                m_priv->sixteenBit = loader.sixteenBit();
                m_priv->isReadOnly = loader.isReadOnly();
                return true;
            }
            break;
        }
    }

    return false;
}

QString DImg::formatToMimeType(FORMAT frm)
{
    QString format;

    switch (frm)
    {
        case(NONE):
        {
            return false;
            break;
        }
        case(JPEG):
        {
            format = QString("JPG");
            break;
        }
        case(TIFF):
        {
            format = QString("TIF");
            break;
        }
        case(PNG):
        {
            format = QString("PNG");
            break;
        }
        case(PPM):
        {
            format = QString("PPM");
            break;
        }
        case(JP2K):
        {
            format = QString("JP2");
            break;
        }
        case(PGF):
        {
            format = QString("PGF");
            break;
        }
        default:
        {
            // For QImage based.
            break;
        }
    }
    return format;
}

bool DImg::save(const QString& filePath, FORMAT frm, DImgLoaderObserver *observer)
{
    if (isNull())
        return false;

    return( save(filePath, formatToMimeType(frm), observer) );
}

bool DImg::save(const QString& filePath, const QString& format, DImgLoaderObserver *observer)
{
    if (isNull())
        return false;

    if (format.isEmpty())
        return false;

    QString frm = format.toUpper();

    if (frm == "JPEG" || frm == "JPG" || frm == "JPE")
    {
        JPEGLoader loader(this);
        return loader.save(filePath, observer);
    }
    else if (frm == "PNG")
    {
        PNGLoader loader(this);
        return loader.save(filePath, observer);
    }
    else if (frm == "TIFF" || frm == "TIF")
    {
        TIFFLoader loader(this);
        return loader.save(filePath, observer);
    }
    else if (frm == "PPM")
    {
        PPMLoader loader(this);
        return loader.save(filePath, observer);
    }
    if (frm == "JP2" || frm == "J2K" || frm == "JPX" || frm == "JPC" || frm == "PGX")
    {
        JP2KLoader loader(this);
        return loader.save(filePath, observer);
    }
    if (frm == "PGF")
    {
        PGFLoader loader(this);
        return loader.save(filePath, observer);
    }
    else
    {
        setAttribute("format", format);
        QImageLoader loader(this);
        return loader.save(filePath, observer);
    }

    return false;
}

DImg::FORMAT DImg::fileFormat(const QString& filePath)
{
    if ( filePath.isNull() )
        return NONE;

    // In first we trying to check the file extension. This is mandatory because
    // some tiff files are detected like RAW files by dcraw::identify method.

    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists())
    {
        kDebug(digiKamAreaCode) << "File \"" << filePath << "\" does not exist";
        return NONE;
    }

#if KDCRAW_VERSION < 0x000400
    QString rawFilesExt(KDcrawIface::DcrawBinary::instance()->rawFiles());
#else
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
#endif
    QString ext = fileInfo.suffix().toUpper();

    if (!ext.isEmpty())
    {
        if (ext == QString("JPEG") || ext == QString("JPG") || ext == QString("JPE"))
            return JPEG;
        else if (ext == QString("PNG"))
            return PNG;
        else if (ext == QString("TIFF") || ext == QString("TIF"))
            return TIFF;
        else if (rawFilesExt.toUpper().contains(ext))
            return RAW;
        if (ext == QString("JP2") || ext == QString("JPX") || // JPEG2000 file format
            ext == QString("JPC") || ext == QString("J2K") || // JPEG2000 code stream
            ext == QString("PGX"))                            // JPEG2000 WM format
            return JP2K;
        else if (ext == QString("PGF"))
            return PGF;
    }

    // In second, we trying to parse file header.

    FILE* f = fopen(QFile::encodeName(filePath), "rb");

    if (!f)
    {
        kDebug(digiKamAreaCode) << "Failed to open file \"" << filePath << "\"";
        return NONE;
    }

    const int headerLen = 9;
    unsigned char header[headerLen];

    if (fread(&header, headerLen, 1, f) != 1)
    {
        kDebug(digiKamAreaCode) << "Failed to read header of file \"" << filePath << "\"";
        fclose(f);
        return NONE;
    }

    fclose(f);

    KDcrawIface::DcrawInfoContainer dcrawIdentify;
    uchar jpegID[2]    = { 0xFF, 0xD8 };
    uchar tiffBigID[2] = { 0x4D, 0x4D };
    uchar tiffLilID[2] = { 0x49, 0x49 };
    uchar pngID[8]     = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
    uchar jp2ID[5]     = { 0x6A, 0x50, 0x20, 0x20, 0x0D, };
    uchar jpcID[2]     = { 0xFF, 0x4F };
    uchar pgfID[3]     = { 0x50, 0x47, 0x46 };

    if (memcmp(&header, &jpegID, 2) == 0)            // JPEG file ?
    {
        return JPEG;
    }
    else if (memcmp(&header, &pngID, 8) == 0)        // PNG file ?
    {
        return PNG;
    }
    else if (memcmp(&header[0], "P", 1)  == 0 &&
             memcmp(&header[2], "\n", 1) == 0)       // PPM 16 bits file ?
    {
        int width, height, rgbmax;
        char nl;
        FILE *file = fopen(QFile::encodeName(filePath), "rb");

        if (fscanf (file, "P6 %d %d %d%c", &width, &height, &rgbmax, &nl) == 4)
        {
            if (rgbmax > 255)
            {
                pclose (file);
                return PPM;
            }
        }

        pclose (file);
    }
    else if (KDcrawIface::KDcraw::rawFileIdentify(dcrawIdentify, filePath)
              && dcrawIdentify.isDecodable)
    {
        // RAW File test using dcraw::identify method.
        // Need to test it before TIFF because any RAW file
        // formats using TIFF header.
        return RAW;
    }
    else if (memcmp(&header, &tiffBigID, 2) == 0 ||  // TIFF file ?
             memcmp(&header, &tiffLilID, 2) == 0)
    {
        return TIFF;
    }
    else if (memcmp(&header[4], &jp2ID, 5) == 0 ||   // JPEG2000 file ?
             memcmp(&header,    &jpcID, 2) == 0)
    {
        return JP2K;
    }
    else if (memcmp(&header, &pgfID, 3) == 0)        // PGF file ?
    {
        return PNG;
    }

    // In others cases, QImage will be used to try to open file.
    return QIMAGE;
}


//---------------------------------------------------------------------------------------------------
// accessing properties


bool DImg::isNull() const
{
    return m_priv->null;
}

uint DImg::width() const
{
    return m_priv->width;
}

uint DImg::height() const
{
    return m_priv->height;
}

QSize DImg::size() const
{
    return QSize(m_priv->width, m_priv->height);
}

uchar* DImg::bits() const
{
    return m_priv->data;
}

uchar* DImg::scanLine(uint i) const
{
    if ( i >= height() )
        return 0;

    uchar *data = bits() + (width() * bytesDepth() * i);
    return data;
}

bool DImg::hasAlpha() const
{
    return m_priv->alpha;
}

bool DImg::sixteenBit() const
{
    return m_priv->sixteenBit;
}

bool DImg::isReadOnly() const
{
    return m_priv->isReadOnly;
}

DImg::COLORMODEL DImg::originalColorModel() const
{
    if (m_priv->attributes.contains("originalColorModel"))
        return (COLORMODEL)m_priv->attributes.value("originalColorModel").toInt();
    else
        return COLORMODELUNKNOWN;
}

int DImg::originalBitDepth() const
{
    return m_priv->attributes.value("originalBitDepth").toInt();
}

DImg::FORMAT DImg::fileFormat() const
{
    if (m_priv->attributes.contains("detectedFileFormat"))
        return (FORMAT)m_priv->attributes.value("detectedFileFormat").toInt();
    else
        return NONE;
}

QByteArray DImg::getComments() const
{
    return metadata(COM);
}

QByteArray DImg::getExif() const
{
    return metadata(EXIF);
}

QByteArray DImg::getIptc() const
{
    return metadata(IPTC);
}

IccProfile DImg::getIccProfile() const
{
    return m_priv->iccProfile;
}

QByteArray DImg::getXmp() const
{
    return metadata(XMP);
}

void DImg::setComments(const QByteArray& commentsData)
{
    m_priv->metaData.insert(COM, commentsData);
}

void DImg::setExif(const QByteArray& exifData)
{
    m_priv->metaData.insert(EXIF, exifData);
}

void DImg::setIptc(const QByteArray& iptcData)
{
    m_priv->metaData.insert(IPTC, iptcData);
}

void DImg::setIccProfile(const IccProfile& profile)
{
    m_priv->iccProfile = profile;
    m_priv->metaData.insert(ICC, m_priv->iccProfile.data());
}

void DImg::setXmp(const QByteArray& xmpData)
{
    m_priv->metaData.insert(XMP, xmpData);
}

QByteArray DImg::metadata(DImg::METADATA key) const
{
    typedef QMap<int, QByteArray> MetaDataMap;

    for (MetaDataMap::const_iterator it = m_priv->metaData.constBegin(); it != m_priv->metaData.constEnd(); ++it)
    {
        if (it.key() == key)
            return it.value();
    }

    return QByteArray();
}

uint DImg::numBytes() const
{
    return (width() * height() * bytesDepth());
}

uint DImg::numPixels() const
{
    return (width() * height());
}

int DImg::bytesDepth() const
{
    if (m_priv->sixteenBit)
       return 8;

    return 4;
}

int DImg::bitsDepth() const
{
    if (m_priv->sixteenBit)
       return 16;

    return 8;
}

void DImg::setAttribute(const QString& key, const QVariant& value)
{
    m_priv->attributes.insert(key, value);
}

QVariant DImg::attribute(const QString& key) const
{
    if (m_priv->attributes.contains(key))
        return m_priv->attributes[key];

    return QVariant();
}

bool DImg::hasAttribute(const QString& key) const
{
    return m_priv->attributes.contains(key);
}

void DImg::removeAttribute(const QString& key)
{
    m_priv->attributes.remove(key);
}

void DImg::setEmbeddedText(const QString& key, const QString& text)
{
    m_priv->embeddedText.insert(key, text);
}

QString DImg::embeddedText(const QString& key) const
{
    if (m_priv->embeddedText.contains(key))
        return m_priv->embeddedText[key];

    return QString();
}

DColor DImg::getPixelColor(uint x, uint y) const
{

    if (m_priv->null || x >= m_priv->width || y >= m_priv->height)
    {
        kDebug(digiKamAreaCode) << "DImg::getPixelColor() : wrong pixel position!";
        return DColor();
    }

    int depth   = bytesDepth();
    uchar *data = m_priv->data + x*depth + (m_priv->width*y*depth);

    return( DColor(data, m_priv->sixteenBit) );
}

void DImg::prepareSubPixelAccess()
{
    if (m_priv->lanczos_func)
        return;

    /* Precompute the Lanczos kernel */
    LANCZOS_DATA_TYPE *lanczos_func = new LANCZOS_DATA_TYPE[LANCZOS_SUPPORT * LANCZOS_SUPPORT * LANCZOS_TABLE_RES];
    for (int i = 0; i < LANCZOS_SUPPORT * LANCZOS_SUPPORT * LANCZOS_TABLE_RES; i++)
    {
        if (i == 0)
        {
            lanczos_func [i] = LANCZOS_DATA_ONE;
        }
        else
        {
            float d = sqrt (((float)i) / LANCZOS_TABLE_RES);
            lanczos_func [i] = (LANCZOS_DATA_TYPE)((LANCZOS_DATA_ONE * LANCZOS_SUPPORT *
                               sin (M_PI * d) * sin ((M_PI / LANCZOS_SUPPORT) * d)) /
                               (M_PI * M_PI * d * d));
        }
    }

    m_priv->lanczos_func = lanczos_func;
}

#ifdef LANCZOS_DATA_FLOAT

static inline int normalizeAndClamp(float norm, int sum, int max)
{
    int r = 0;

    if (norm != 0.0)
        r = sum / norm;
    if (r < 0)
        r = 0;
    else if (r > max)
        r = max;
    return r;
}

#else /* LANCZOS_DATA_FLOAT */

static inline int normalizeAndClamp(int norm, int sum, int max)
{
    int r = 0;

    if (norm != 0)
        r = sum / norm;
    if (r < 0)
        r = 0;
    else if (r > max)
        r = max;
    return r;
}

#endif /* LANCZOS_DATA_FLOAT */

DColor DImg::getSubPixelColor(float x, float y) const
{
    if (isNull() || x >= width() || y >= height())
    {
        kDebug(digiKamAreaCode) << "DImg::getPixelColor() : wrong pixel position!";
        return DColor();
    }

    const LANCZOS_DATA_TYPE *lanczos_func = m_priv->lanczos_func;
    if (lanczos_func == 0)
        return DColor();

    Digikam::DColor col(0, 0, 0, 0xFFFF, sixteenBit());

#ifdef LANCZOS_DATA_FLOAT

    float xs = ::ceilf  (x) - LANCZOS_SUPPORT;
    float xe = ::floorf (x) + LANCZOS_SUPPORT;
    float ys = ::ceilf  (y) - LANCZOS_SUPPORT;
    float ye = ::floorf (y) + LANCZOS_SUPPORT;
    if (xs >= 0 && ys >= 0 && xe < width() && ye < height())
    {
        float norm = 0.0;
        float sumR = 0.0;
        float sumG = 0.0;
        float sumB = 0.0;
        float _dx  = x - xs;
        float dy   = y - ys;

        for (; ys <= ye; ys += 1.0, dy -= 1.0)
        {
            float xc, dx = _dx;
            for (xc = xs; xc <= xe; xc += 1.0, dx -= 1.0)
            {
                uchar *data = bits() + (int)(xs*bytesDepth()) + (int)(width()*ys*bytesDepth());
                DColor src = DColor(data, sixteenBit());

                float d = dx * dx + dy * dy;
                if (d >= LANCZOS_SUPPORT * LANCZOS_SUPPORT)
                    continue;

                d    = lanczos_func [(int)(d * LANCZOS_TABLE_RES)];
                norm += d;
                sumR += d * src.red();
                sumG += d * src.green();
                sumB += d * src.blue();
            }
        }

        int max = sixteenBit()? 65535 : 255;
        col.setRed(normalizeAndClamp(norm, sumR, max));
        col.setGreen(normalizeAndClamp(norm, sumG, max));
        col.setBlue(normalizeAndClamp(norm, sumB, max));
    }

#else /* LANCZOS_DATA_FLOAT */

    /* Do it in integer arithmetic, it's faster */
    int xx   = (int)x;
    int yy   = (int)y;
    int xs   = xx + 1 - LANCZOS_SUPPORT;
    int xe   = xx     + LANCZOS_SUPPORT;
    int ys   = yy + 1 - LANCZOS_SUPPORT;
    int ye   = yy     + LANCZOS_SUPPORT;
    int norm = 0;
    int sumR = 0;
    int sumG = 0;
    int sumB = 0;
    int _dx  = (int)(x * 4096.0) - (xs << 12);
    int dy   = (int)(y * 4096.0) - (ys << 12);

    for (; ys <= ye; ++ys, dy -= 4096)
    {
        int xc, dx = _dx;
        for (xc = xs; xc <= xe; ++xc, dx -= 4096)
        {
            DColor src(0, 0, 0, 0xFFFF, sixteenBit());
            if (xc >= 0 && ys >= 0 && xc < (int)width() && ys < (int)height())
            {
                uchar *data = bits() + xc*bytesDepth() + width()*ys*bytesDepth();
                src.setColor(data, sixteenBit());
            }

            int d = (dx * dx + dy * dy) >> 12;
            if (d >= 4096 * LANCZOS_SUPPORT * LANCZOS_SUPPORT)
                continue;

            d    = lanczos_func [(d * LANCZOS_TABLE_RES) >> 12];
            norm += d;
            sumR += d * src.red();
            sumG += d * src.green();
            sumB += d * src.blue();
        }
    }

    int max = sixteenBit()? 65535 : 255;
    col.setRed(normalizeAndClamp(norm, sumR, max));
    col.setGreen(normalizeAndClamp(norm, sumG, max));
    col.setBlue(normalizeAndClamp(norm, sumB, max));

#endif /* LANCZOS_DATA_FLOAT */

    return col;
}

DColor DImg::getSubPixelColorFast(float x, float y) const
{
    int xx    = (int)x;
    int yy    = (int)y;
    float d_x = x - (int)x;
    float d_y = y - (int)y;
    uchar *data;

    DColor d00, d01, d10, d11;
    DColor col;

    data = bits() + xx*bytesDepth() + yy*width()*bytesDepth();
    d00.setColor(data, sixteenBit());
    if ((xx+1) < (int)width())
    {
        data = bits() + (xx+1)*bytesDepth() + yy*width()*bytesDepth();
        d10.setColor(data, sixteenBit());
    }
    if ((yy+1) < (int)height())
    {
        data = bits() + xx*bytesDepth() + (yy+1)*width()*bytesDepth();
        d01.setColor(data, sixteenBit());
    }
    if ((xx+1) < (int)width() && (yy+1) < (int)height())
    {
        data = bits() + (xx+1)*bytesDepth() + (yy+1)*width()*bytesDepth();
        d11.setColor(data, sixteenBit());
    }

    d00.multiply(1.0-d_x);
    d00.multiply(1.0-d_y);

    d10.multiply(d_x);
    d10.multiply(1.0-d_y);

    d01.multiply(1.0-d_x);
    d01.multiply(d_y);

    d11.multiply(d_x);
    d11.multiply(d_y);

    col.blendAdd(d00);
    col.blendAdd(d10);
    col.blendAdd(d01);
    col.blendAdd(d11);

    if (sixteenBit())
        col.blendClamp16();
    else
        col.blendClamp8();

    return col;
}

void DImg::setPixelColor(uint x, uint y, const DColor& color)
{
    if (m_priv->null || x > m_priv->width || y > m_priv->height)
    {
        kDebug(digiKamAreaCode) << "DImg::setPixelColor() : wrong pixel position!";
        return;
    }

    if (color.sixteenBit() != m_priv->sixteenBit)
    {
        kDebug(digiKamAreaCode) << "DImg::setPixelColor() : wrong color depth!";
        return;
    }

    int depth   = bytesDepth();
    uchar *data = m_priv->data + x*depth + (m_priv->width*y*depth);
    color.setPixel(data);
}


//---------------------------------------------------------------------------------------------------
// copying operations


DImg DImg::copy()
{
    DImg img(*this);
    img.detach();
    return img;
}

DImg DImg::copyImageData()
{
    DImg img(width(), height(), sixteenBit(), hasAlpha(), bits(), true);
    return img;
}

DImg DImg::copyMetaData()
{
    DImg img;
    // copy width, height, alpha, sixteenBit, null
    img.copyImageData(m_priv);
    // deeply copy metadata
    img.copyMetaData(m_priv);
    // set image to null
    img.m_priv->null = true;
    return img;
}

DImg DImg::copy(const QRect& rect)
{
    return copy(rect.x(), rect.y(), rect.width(), rect.height());
}

DImg DImg::copy(int x, int y, int w, int h)
{
    if ( isNull() || w <= 0 || h <= 0)
    {
        kDebug(digiKamAreaCode) << " : return null image!";
        return DImg();
    }

    DImg image(*this, w, h);
    image.bitBltImage(this, x, y, w, h, 0, 0);

    return image;
}


//---------------------------------------------------------------------------------------------------
// bitwise operations


void DImg::bitBltImage(const DImg* src, int dx, int dy)
{
    bitBltImage(src, 0, 0, src->width(), src->height(), dx, dy);
}

void DImg::bitBltImage(const DImg* src, int sx, int sy, int dx, int dy)
{
    bitBltImage(src, sx, sy, src->width() - sx, src->height() - sy, dx, dy);
}

void DImg::bitBltImage(const DImg* src, int sx, int sy, int w, int h, int dx, int dy)
{
    if (isNull())
       return;

    if (src->sixteenBit() != sixteenBit())
    {
        kWarning(digiKamAreaCode) << "Blitting from 8-bit to 16-bit or vice versa is not supported";
        return;
    }

    if (w == -1 && h == -1)
    {
        w = src->width();
        h = src->height();
    }

    bitBlt(src->bits(), bits(), sx, sy, w, h, dx, dy,
           src->width(), src->height(), width(), height(), sixteenBit(), src->bytesDepth(), bytesDepth());
}

void DImg::bitBltImage(const uchar* src, int sx, int sy, int w, int h, int dx, int dy,
                       uint swidth, uint sheight, int sdepth)
{
    if (isNull())
        return;

    if (bytesDepth() != sdepth)
    {
        kWarning(digiKamAreaCode) << "Blitting from 8-bit to 16-bit or vice versa is not supported";
        return;
    }

    if (w == -1 && h == -1)
    {
        w = swidth;
        h = sheight;
    }

    bitBlt(src, bits(), sx, sy, w, h, dx, dy, swidth, sheight, width(), height(), sixteenBit(), sdepth, bytesDepth());
}

bool DImg::normalizeRegionArguments(int& sx, int& sy, int& w, int& h, int& dx, int& dy,
                                    uint swidth, uint sheight, uint dwidth, uint dheight)
{
    if (sx < 0)
    {
        // sx is negative, so + is - and - is +
        dx -= sx;
        w  += sx;
        sx = 0;
    }

    if (sy < 0)
    {
        dy -= sy;
        h  += sy;
        sy = 0;
    }

    if (dx < 0)
    {
        sx -= dx;
        w  += dx;
        dx = 0;
    }

    if (dy < 0)
    {
        sy -= dy;
        h  += dy;
        dy = 0;
    }

    if (sx + w > (int)swidth)
    {
        w = swidth - sx;
    }

    if (sy + h > (int)sheight)
    {
        h = sheight - sy;
    }

    if (dx + w > (int)dwidth)
    {
        w = dwidth - dx;
    }

    if (dy + h > (int)dheight)
    {
        h = dheight - dy;
    }

    // Nothing left to copy
    if (w <= 0 || h <= 0)
        return false;

    return true;
}

void DImg::bitBlt (const uchar *src, uchar *dest,
                         int sx, int sy, int w, int h, int dx, int dy,
                         uint swidth, uint sheight, uint dwidth, uint dheight,
                         bool /*sixteenBit*/, int sdepth, int ddepth)
{
    // Normalize
    if (!normalizeRegionArguments(sx, sy, w, h, dx, dy, swidth, sheight, dwidth, dheight))
        return;

    // Same pixels
    if (src == dest && dx==sx && dy==sy)
        return;

    const uchar *sptr;
    uchar *dptr;
    uint   slinelength = swidth * sdepth;
    uint   dlinelength = dwidth * ddepth;

    int scurY = sy;
    int dcurY = dy;
    for (int j = 0 ; j < h ; ++j, ++scurY, ++dcurY)
    {
        sptr  = &src [ scurY * slinelength ] + sx * sdepth;
        dptr  = &dest[ dcurY * dlinelength ] + dx * ddepth;

            // plain and simple bitBlt
        for (int i = 0; i < w * sdepth ; ++i, ++sptr, ++dptr)
        {
            *dptr = *sptr;
        }
    }
}

void DImg::bitBlendImage(DColorComposer *composer, const DImg* src,
                         int sx, int sy, int w, int h, int dx, int dy,
                         DColorComposer::MultiplicationFlags multiplicationFlags)
{
    if (isNull())
        return;

    if (src->sixteenBit() != sixteenBit())
    {
        kWarning(digiKamAreaCode) << "Blending from 8-bit to 16-bit or vice versa is not supported";
        return;
    }

    bitBlend(composer, src->bits(), bits(), sx, sy, w, h, dx, dy,
             src->width(), src->height(), width(), height(), sixteenBit(),
             src->bytesDepth(), bytesDepth(), multiplicationFlags);
}

void DImg::bitBlend (DColorComposer *composer, const uchar *src, uchar *dest,
                     int sx, int sy, int w, int h, int dx, int dy,
                     uint swidth, uint sheight, uint dwidth, uint dheight,
                     bool sixteenBit, int sdepth, int ddepth,
                     DColorComposer::MultiplicationFlags multiplicationFlags)
{
    // Normalize
    if (!normalizeRegionArguments(sx, sy, w, h, dx, dy, swidth, sheight, dwidth, dheight))
        return;

    const uchar *sptr;
    uchar *dptr;
    uint   slinelength = swidth * sdepth;
    uint   dlinelength = dwidth * ddepth;

    int scurY = sy;
    int dcurY = dy;
    for (int j = 0 ; j < h ; ++j, ++scurY, ++dcurY)
    {
        sptr  = &src [ scurY * slinelength ] + sx * sdepth;
        dptr  = &dest[ dcurY * dlinelength ] + dx * ddepth;

        // blend src and destination
        for (int i = 0 ; i < w ; ++i, sptr+=sdepth, dptr+=ddepth)
        {
            DColor src(sptr, sixteenBit);
            DColor dst(dptr, sixteenBit);

            // blend colors
            composer->compose(dst, src, multiplicationFlags);

            dst.setPixel(dptr);
        }
    }
}


//---------------------------------------------------------------------------------------------------
// QImage / QPixmap access


QImage DImg::copyQImage()
{
    if (isNull())
        return QImage();

    if (sixteenBit())
    {
        DImg img(*this);
        img.detach();
        img.convertDepth(32);
        return img.copyQImage();
    }

    QImage img(width(), height(), QImage::Format_ARGB32);

    if (img.isNull())
    {
        kError(digiKamAreaCode) << "Failed to allocate memory to copy DImg of size" << size() << "to QImage";
        return QImage();
    }

    uchar* sptr = bits();
    uint*  dptr = (uint*)img.bits();

    for (uint i=0; i < width()*height(); ++i)
    {
        *dptr++ = qRgba(sptr[2], sptr[1], sptr[0], sptr[3]);
        sptr += 4;
    }

    // NOTE: Qt4 do not provide anymore QImage::setAlphaChannel() because
    // alpha channel is auto-detected during QImage->QPixmap conversion

    return img;
}

QImage DImg::copyQImage(QRect rect)
{
    return (copyQImage(rect.x(), rect.y(), rect.width(), rect.height()));
}

QImage DImg::copyQImage(int x, int y, int w, int h)
{
    if (isNull())
        return QImage();

    DImg img = copy(x, y, w, h);

    if (img.sixteenBit())
        img.convertDepth(32);

    return img.copyQImage();
}

QPixmap DImg::convertToPixmap()
{
    if (isNull())
        return QPixmap();

    if (sixteenBit())
    {
        // make fastaaaa...
        return QPixmap::fromImage(copyQImage(0, 0, width(), height()));
    }

    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
    {
        QImage img(width(), height(), QImage::Format_ARGB32);

        uchar* sptr = bits();
        uint*  dptr = (uint*)img.bits();

        for (uint i=0; i<width()*height(); ++i)
        {
            *dptr++ = qRgba(sptr[2], sptr[1], sptr[0], sptr[3]);
            sptr += 4;
        }

        // NOTE: Qt4 do not provide anymore QImage::setAlphaChannel() because
        // alpha channel is auto-detected during QImage->QPixmap conversion

        return QPixmap::fromImage(img);
    }
    else
    {
        QImage img(bits(), width(), height(), QImage::Format_ARGB32);

        // NOTE: Qt4 do not provide anymore QImage::setAlphaChannel() because
        // alpha channel is auto-detected during QImage->QPixmap conversion

        return QPixmap::fromImage(img);
    }
}

QPixmap DImg::convertToPixmap(IccTransform& monitorICCtrans)
{
    if (isNull())
        return QPixmap();

    if (monitorICCtrans.outputProfile().isNull())
        return convertToPixmap();

    DImg img = copy();
    monitorICCtrans.apply(img);

    return (img.convertToPixmap());
}

QImage DImg::pureColorMask(ExposureSettingsContainer *expoSettings)
{
    if (isNull() || (!expoSettings->underExposureIndicator && !expoSettings->overExposureIndicator))
        return QImage();

    QImage img(size(), QImage::Format_ARGB32);
    img.fill(0x00000000);      // Full transparent.

    // NOTE: Qt4 do not provide anymore QImage::setAlphaChannel() because
    // alpha channel is auto-detected during QImage->QPixmap conversion

    uchar *bits = img.bits();
    int    max  = sixteenBit() ? 65535 : 255;
    int    index;
    DColor pix;

    for (uint x=0 ; x < width() ; ++x)
    {
        for (uint y=0 ; y<height() ; ++y)
        {
            pix   = getPixelColor(x, y);
            index = y*img.bytesPerLine() + x*4;

            if (expoSettings->underExposureIndicator &&
                pix.red() == 0 && pix.green() == 0 && pix.blue() == 0)
            {
                bits[index    ] = expoSettings->underExposureColor.blue();
                bits[index + 1] = expoSettings->underExposureColor.green();
                bits[index + 2] = expoSettings->underExposureColor.red();
                bits[index + 3] = 0xFF;
            }

            if (expoSettings->overExposureIndicator &&
                pix.red() == max && pix.green() == max && pix.blue() == max)
            {
                bits[index    ] = expoSettings->overExposureColor.blue();
                bits[index + 1] = expoSettings->overExposureColor.green();
                bits[index + 2] = expoSettings->overExposureColor.red();
                bits[index + 3] = 0xFF;
            }
        }
    }

    return img;
}


//---------------------------------------------------------------------------------------------------
// basic imaging operations


void DImg::crop(QRect rect)
{
    crop(rect.x(), rect.y(), rect.width(), rect.height());
}

void DImg::crop(int x, int y, int w, int h)
{
    if ( isNull() || w <= 0 || h <= 0)
        return;

    uint  oldw = width();
    uint  oldh = height();
    uchar *old = stripImageData();

    // set new image data, bits(), width(), height() change
    setImageDimension(w, h);
    allocateData();

    // copy image region (x|y), wxh, from old data to point (0|0) of new data
    bitBlt(old, bits(), x, y, w, h, 0, 0, oldw, oldh, width(), height(), sixteenBit(), bytesDepth(), bytesDepth());
    delete [] old;
}

void DImg::resize(int w, int h)
{
    if ( w <= 0 || h <= 0)
        return;

    DImg image = smoothScale(w, h);

    delete [] m_priv->data;
    m_priv->data = image.stripImageData();
    setImageDimension(w, h);
}

void DImg::rotate(ANGLE angle)
{
    if (isNull())
        return;

    switch (angle)
    {
    case(ROT90):
    {
        uint w  = height();
        uint h  = width();

        if (sixteenBit())
        {
            ullong* newData = new ullong[w*h];

            ullong *from = (ullong*) m_priv->data;
            ullong *to;

            for (int y = w-1; y >=0; --y)
            {
                to = newData + y;

                for (uint x=0; x < h; ++x)
                {
                    *to = *from++;
                    to += w;
                }
            }

            setImageDimension(w, h);

            delete [] m_priv->data;
            m_priv->data = (uchar*)newData;
        }
        else
        {
            uint* newData = new uint[w*h];

            uint *from = (uint*) m_priv->data;
            uint *to;

            for (int y = w-1; y >=0; --y)
            {
                to = newData + y;

                for (uint x=0; x < h; ++x)
                {
                    *to = *from++;
                    to += w;
                }
            }

            setImageDimension(w, h);

            delete [] m_priv->data;
            m_priv->data = (uchar*)newData;
        }

        break;
    }
    case(ROT180):
    {
        uint w  = width();
        uint h  = height();

        int middle_line = -1;
        if (h % 2)
           middle_line = h / 2;

        if (sixteenBit())
        {
            ullong *line1;
            ullong *line2;

            ullong* data = (ullong*) bits();
            ullong  tmp;

            // can be done inplace
            for (uint y = 0; y < (h+1)/2; ++y)
            {
                line1 = data + y * w;
                line2 = data + (h-y) * w;
                for (uint x=0; x < w; ++x)
                {
                    tmp    = *line1;
                    *line1 = *line2;
                    *line2 = tmp;

                    ++line1;
                    --line2;
                    if ((int)y == middle_line && x * 2 >= w)
                        break;
                }
            }
        }
        else
        {
            uint *line1;
            uint *line2;

            uint* data = (uint*) bits();
            uint  tmp;

            // can be done inplace
            for (uint y = 0; y < (h+1)/2; ++y)
            {
                line1 = data + y * w;
                line2 = data + (h-y) * w;

                for (uint x=0; x < w; ++x)
                {
                    tmp    = *line1;
                    *line1 = *line2;
                    *line2 = tmp;

                    ++line1;
                    --line2;
                    if ((int)y == middle_line && x * 2 >= w)
                        break;
                }
            }
        }

        break;
    }
    case(ROT270):
    {
        uint w  = height();
        uint h  = width();

        if (sixteenBit())
        {
            ullong* newData = new ullong[w*h];

            ullong *from = (ullong*) m_priv->data;
            ullong *to;

            for (uint y = 0; y < w; ++y)
            {
                to = newData + y + w*(h-1);

                for (uint x=0; x < h; ++x)
                {
                    *to = *from++;
                    to -= w;
                }
            }

            setImageDimension(w, h);

            delete [] m_priv->data;
            m_priv->data = (uchar*)newData;
        }
        else
        {
            uint* newData = new uint[w*h];

            uint *from = (uint*) m_priv->data;
            uint *to;

            for (uint y = 0; y < w; ++y)
            {
                to = newData + y + w*(h-1);

                for (uint x=0; x < h; ++x)
                {
                    *to = *from++;
                    to -= w;
                }
            }

            setImageDimension(w, h);

            delete [] m_priv->data;
            m_priv->data = (uchar*)newData;
        }

        break;
    }
    default:
        break;
    }
}

// 15-11-2005: This method have been tested indeep with valgrind by Gilles.

void DImg::flip(FLIP direction)
{
    if (isNull())
        return;

    switch (direction)
    {
        case(HORIZONTAL):
        {
            uint w  = width();
            uint h  = height();

            if (sixteenBit())
            {
                unsigned short  tmp[4];
                unsigned short *beg;
                unsigned short *end;

                unsigned short * data = (unsigned short *)bits();

                // can be done inplace
                for (uint y = 0 ; y < h ; ++y)
                {
                    beg = data + y * w * 4;
                    end = beg  + (w-1) * 4;

                    for (uint x=0 ; x < (w/2) ; ++x)
                    {
                        memcpy(&tmp, beg, 8);
                        memcpy(beg, end, 8);
                        memcpy(end, &tmp, 8);

                        beg+=4;
                        end-=4;
                    }
                }
            }
            else
            {
                uchar  tmp[4];
                uchar *beg;
                uchar *end;

                uchar* data = bits();

                // can be done inplace
                for (uint y = 0 ; y < h ; ++y)
                {
                    beg = data + y * w * 4;
                    end = beg  + (w-1) * 4;

                    for (uint x=0 ; x < (w/2) ; ++x)
                    {
                        memcpy(&tmp, beg, 4);
                        memcpy(beg, end, 4);
                        memcpy(end, &tmp, 4);

                        beg+=4;
                        end-=4;
                    }
                }
            }

            break;
        }
        case(VERTICAL):
        {
            uint w  = width();
            uint h  = height();

            if (sixteenBit())
            {
                unsigned short  tmp[4];
                unsigned short *line1;
                unsigned short *line2;

                unsigned short* data = (unsigned short*) bits();

                // can be done inplace
                for (uint y = 0 ; y < (h/2) ; ++y)
                {
                    line1 = data + y * w * 4;
                    line2 = data + (h-y-1) * w * 4;

                    for (uint x=0 ; x < w ; ++x)
                    {
                        memcpy(&tmp, line1, 8);
                        memcpy(line1, line2, 8);
                        memcpy(line2, &tmp, 8);

                        line1+=4;
                        line2+=4;
                    }
                }
            }
            else
            {
                uchar  tmp[4];
                uchar *line1;
                uchar *line2;

                uchar* data = bits();

                // can be done inplace
                for (uint y = 0 ; y < (h/2) ; ++y)
                {
                    line1 = data + y * w * 4;
                    line2 = data + (h-y-1) * w * 4;

                    for (uint x=0 ; x < w ; ++x)
                    {
                        memcpy(&tmp, line1, 4);
                        memcpy(line1, line2, 4);
                        memcpy(line2, &tmp, 4);

                        line1+=4;
                        line2+=4;
                    }
                }
            }

            break;
        }
        default:
            break;
    }
}

void DImg::convertToSixteenBit()
{
    convertDepth(64);
}

void DImg::convertToEightBit()
{
    convertDepth(32);
}

void DImg::convertToDepthOfImage(const DImg *otherImage)
{
    if (otherImage->sixteenBit())
        convertToSixteenBit();
    else
        convertToEightBit();
}

void DImg::convertDepth(int depth)
{
    if (isNull())
        return;

    if (depth != 32 && depth != 64)
    {
        kDebug(digiKamAreaCode) << " : wrong color depth!";
        return;
    }

    if (((depth == 32) && !sixteenBit()) ||
        ((depth == 64) && sixteenBit()))
        return;

    if (depth == 32)
    {
        // downgrading from 16 bit to 8 bit

        uchar*  data = new uchar[width()*height()*4];
        uchar*  dptr = data;
        ushort* sptr = (ushort*)bits();

        uint dim = width() * height() * 4;
        for (uint i = 0; i < dim; ++i)
        {
            *dptr++ = (*sptr++ * 255UL) / 65535UL;
        }

        delete [] m_priv->data;
        m_priv->data = data;
        m_priv->sixteenBit = false;
    }
    else if (depth == 64)
    {
        // upgrading from 8 bit to 16 bit

        uchar*  data = new uchar[width()*height()*8];
        ushort* dptr = (ushort*)data;
        uchar*  sptr = bits();

        uint dim = width() * height() * 4;
        for (uint i = 0; i < dim; ++i)
        {
            *dptr++ = (*sptr++ * 65535ULL) / 255ULL;
        }

        delete [] m_priv->data;
        m_priv->data = data;
        m_priv->sixteenBit = true;
    }
}

void DImg::fill(const DColor& color)
{
    if (isNull())
        return;

    if (sixteenBit())
    {
        unsigned short *imgData16 = (unsigned short *)m_priv->data;

        for (uint i = 0 ; i < width()*height()*4 ; i+=4)
        {
            imgData16[ i ] = (unsigned short)color.blue();
            imgData16[i+1] = (unsigned short)color.green();
            imgData16[i+2] = (unsigned short)color.red();
            imgData16[i+3] = (unsigned short)color.alpha();
        }
    }
    else
    {
        uchar *imgData = m_priv->data;

        for (uint i = 0 ; i < width()*height()*4 ; i+=4)
        {
            imgData[ i ] = (uchar)color.blue();
            imgData[i+1] = (uchar)color.green();
            imgData[i+2] = (uchar)color.red();
            imgData[i+3] = (uchar)color.alpha();
        }
    }
}

QByteArray DImg::getUniqueHash()
{
    if (m_priv->attributes.contains("uniqueHash"))
        return m_priv->attributes["uniqueHash"].toByteArray();

    if (!m_priv->attributes.contains("originalFilePath"))
    {
        kWarning(digiKamAreaCode) << "DImg::getUniqueHash called without originalFilePath property set!";
        return QByteArray();
    }

    QString filePath = m_priv->attributes.value("originalFilePath").toString();

    if (filePath.isEmpty())
        return QByteArray();

    QByteArray hash = DImgLoader::uniqueHash(filePath, *this, false);

    if (!hash.isNull())
        setAttribute("uniqueHash", hash);

    return hash;
}

QByteArray DImg::getUniqueHash(const QString& filePath)
{
    return DImgLoader::uniqueHash(filePath, DImg(), true);
}

void DImg::updateMetadata(const QString& destMimeType, const QString& originalFileName, bool setExifOrientationTag)
{
    // Get image Exif/IPTC data.
    DMetadata meta;
    meta.setExif(getExif());
    meta.setIptc(getIptc());
    meta.setXmp(getXmp());

    // Update IPTC preview.
    // NOTE: see B.K.O #130525. a JPEG segment is limited to 64K. If the IPTC byte array is
    // bigger than 64K during of image preview tag size, the target JPEG image will be
    // broken. Note that IPTC image preview tag is limited to 256K!!!
    // There is no limitation with TIFF and PNG about IPTC byte array size.

    // Before to update IPTC preview, we remove it.
    meta.removeIptcTag("Iptc.Application2.Preview");
    meta.removeIptcTag("Iptc.Application2.PreviewFormat");
    meta.removeIptcTag("Iptc.Application2.PreviewVersion");

    QSize previewSize = size();
    previewSize.scale(1280, 1024, Qt::KeepAspectRatio);
    QImage preview;
    // Ensure that preview is not upscaled
    if (previewSize.width() >= (int)width())
        preview = copyQImage();
    else
        preview = smoothScale(previewSize.width(), previewSize.height(), Qt::IgnoreAspectRatio).copyQImage();

    // With JPEG file, we don't store IPTC preview.
    // NOTE: only store preview if pixel number is at least two times bigger
    if (/* (2*(previewSize.width() * previewSize.height()) < (int)(d->image.width() * d->image.height())) &&*/
        (destMimeType.toUpper() != QString("JPG") && destMimeType.toUpper() != QString("JPEG") &&
         destMimeType.toUpper() != QString("JPE"))
       )
    {
        // Non JPEG file, we update IPTC preview
        meta.setImagePreview(preview);
    }

    // Update Exif thumbnail.
    QImage thumb = preview.scaled(160, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    meta.setExifThumbnail(thumb);

    // Update Exif Image dimensions.
    meta.setImageDimensions(size());

    // Update Exif Document Name tag with the original file name.
    if (!originalFileName.isEmpty())
        meta.setExifTagString("Exif.Image.DocumentName", originalFileName);

    // Update Exif Orientation tag if necessary.
    if(setExifOrientationTag)
        meta.setImageOrientation(DMetadata::ORIENTATION_NORMAL);

    // Store new Exif/IPTC/XMP data into image.
    setExif(meta.getExif());
    setIptc(meta.getIptc());
    setXmp(meta.getXmp());
}

}  // namespace Digikam

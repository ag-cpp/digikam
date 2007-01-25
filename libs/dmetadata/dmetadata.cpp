/* ============================================================
 * Author: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date  : 2006-02-23
 * Description : image metadata interface
 *
 * Copyright 2006 by Gilles Caulier
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

 // C++ includes.

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <string>

// Qt includes.

#include <qfile.h>
#include <qfileinfo.h>
#include <qtextcodec.h>
#include <qwmatrix.h>

// KDE includes.

#include <kapplication.h>
#include <kstringhandler.h>
#include <ktempfile.h>

// Exiv2 includes.

#include <exiv2/image.hpp>
#include <exiv2/jpgimage.hpp>
#include <exiv2/datasets.hpp>
#include <exiv2/tags.hpp>
#include <exiv2/types.hpp>
#include <exiv2/exif.hpp>

// Local includes.

#include "version.h"
#include "dcraw_parse.h"
#include "jpegmetaloader.h"
#include "pngmetaloader.h"
#include "tiffmetaloader.h"
#include "rawmetaloader.h"
#include "dmetadataprivate.h"
#include "ddebug.h"
#include "dmetadata.h"

namespace Digikam
{

DMetadata::DMetadata()
{
    d = new DMetadataPriv;
}

DMetadata::DMetadata(const QString& filePath, DImg::FORMAT ff)
{
    d = new DMetadataPriv;
    load(filePath, ff);
}

DMetadata::~DMetadata()
{
    delete d;
}

bool DMetadata::clearExif()
{
    try
    {    
        d->exifMetadata.clear();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot clear Exif data using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }    

    return false;       
}

bool DMetadata::clearIptc()
{
    try
    {    
        d->iptcMetadata.clear();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot clear Iptc data using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }    

    return false;       
}

bool DMetadata::applyChanges()
{
    return save(d->filePath, d->fileFormat);
}

QByteArray DMetadata::getComments() const
{
    return QByteArray().duplicate(d->imageComments.data(), d->imageComments.size());
}

QByteArray DMetadata::getExif() const
{
    try
    {    
        if (!d->exifMetadata.empty())
        {

            Exiv2::ExifData& exif = d->exifMetadata;
            Exiv2::DataBuf c2 = exif.copy();
            QByteArray data(c2.size_);
            memcpy(data.data(), c2.pData_, c2.size_);
            return data;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            DDebug() << "From file " << d->filePath << endl;

        DDebug() << "Cannot get Exif data using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }       
    
    return QByteArray();
}

QByteArray DMetadata::getIptc(bool addIrbHeader) const
{
    try
    {    
        if (!d->iptcMetadata.empty())
        {                
            Exiv2::IptcData& iptc = d->iptcMetadata;
            Exiv2::DataBuf c2;

            if (addIrbHeader) 
            {
#ifdef EXIV2_CHECK_VERSION 
                if (EXIV2_CHECK_VERSION(0,10,0))
                    c2 = Exiv2::Photoshop::setIptcIrb(0, 0, iptc);
                else
                {
                    DDebug() << "Exiv2 version is to old. Cannot add Irb header to IPTC metadata" << endl;
                    return QByteArray();
                }
#endif
            }
            else 
                c2 = iptc.copy();

            QByteArray data(c2.size_);
            memcpy(data.data(), c2.pData_, c2.size_);
            return data;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            DDebug() << "From file " << d->filePath << endl;

        DDebug() << "Cannot get Iptc data using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }       
    
    return QByteArray();
}

void DMetadata::setComments(const QByteArray& data)
{
    d->imageComments = std::string(data.data(), data.size());
}

bool DMetadata::setExif(const QByteArray& data)
{
    try
    {    
        if (!data.isEmpty())
        {
            if (d->exifMetadata.load((const Exiv2::byte*)data.data(), data.size()) != 0)
                return false;
            else
                return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            DDebug() << "From file " << d->filePath << endl;

        DDebug() << "Cannot set Exif data using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

bool DMetadata::setExif(Exiv2::DataBuf const data)
{
    try
    {    
        if (data.size_ != 0)
        {
            if (d->exifMetadata.load(data.pData_, data.size_) != 0)
                return false;
            else
                return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            DDebug() << "From file " << d->filePath << endl;

        DDebug() << "Cannot set Exif data using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }    

    return false;    
}

bool DMetadata::setIptc(const QByteArray& data)
{
    try
    {    
        if (!data.isEmpty())
        {
            if (d->iptcMetadata.load((const Exiv2::byte*)data.data(), data.size()) != 0)
                return false;
            else
                return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            DDebug() << "From file " << d->filePath << endl;

        DDebug() << "Cannot set Iptc data using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        

    return false;
}

bool DMetadata::setIptc(Exiv2::DataBuf const data)
{
    try
    {    
        if (data.size_ != 0)
        {
            if (d->iptcMetadata.load(data.pData_, data.size_) != 0)
                return false;
            else
                return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            DDebug() << "From file " << d->filePath << endl;

        DDebug() << "Cannot set Iptc data using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        

    return false;
}

bool DMetadata::load(const QString& filePath, DImg::FORMAT ff)
{
    DImg::FORMAT format = ff;
    
    if (format == DImg::NONE)
        format = DImg::fileFormat(filePath);

    d->fileFormat = format;
    d->filePath   = filePath;

    switch (d->fileFormat)
    {
        case(DImg::JPEG):
        {
            JPEGMetaLoader loader(this);
            return (loader.load(filePath));
            break;
        }
        case(DImg::PNG):
        {
            PNGMetaLoader loader(this);
            return (loader.load(filePath));
            break;
        }
        case(DImg::RAW):
        {
            RAWMetaLoader loader(this);
            return (loader.load(filePath));
            break;
        }
        case(DImg::TIFF):
        {
            TIFFMetaLoader loader(this);
            return (loader.load(filePath));
            break;
        }
        default:
        {
            return false;
            break;
        }
    }

    return false;
}

QString DMetadata::getFilePath() const
{
    return d->filePath;
}

bool DMetadata::save(const QString& filePath, DImg::FORMAT ff)
{
    // NOTE: see B.K.O #137770 & #138540 : never touch the file if is read only.
    QFileInfo finfo(filePath); 
    QFileInfo dinfo(finfo.dirPath()); 
    if (!finfo.isWritable())
    {
        DDebug() << "File '" << finfo.fileName() << "' is read-only. Metadata not saved." << endl;
        return false;
    }
    if (!dinfo.isWritable())
    {
        DDebug() << "Dir '" << dinfo.filePath() << "' is read-only. Metadata not saved." << endl;
        return false;
    }

    switch (ff)
    {
        case(DImg::JPEG):
        {
            JPEGMetaLoader loader(this);
            return (loader.save(filePath));
            break;
        }
        case(DImg::PNG):
        {
            PNGMetaLoader loader(this);
            return (loader.save(filePath));
            break;
        }
        case(DImg::RAW):
        {
            RAWMetaLoader loader(this);
            return (loader.save(filePath));
            break;
        }
        case(DImg::TIFF):
        {
            TIFFMetaLoader loader(this);
            return (loader.save(filePath));
            break;
        }
        default:
        {
            return false;
            break;
        }
    }

    return false;
}

QImage DMetadata::getExifThumbnail(bool fixOrientation) const
{
    QImage thumbnail;
    
    if (d->exifMetadata.empty())
       return thumbnail;
    
    try
    {    
        Exiv2::DataBuf const c1(d->exifMetadata.copyThumbnail());
        thumbnail.loadFromData(c1.pData_, c1.size_);
        
        if (!thumbnail.isNull())
        {
            if (fixOrientation)
            {
                Exiv2::ExifKey key("Exif.Thumbnail.Orientation");
                Exiv2::ExifData exifData(d->exifMetadata);
                Exiv2::ExifData::iterator it = exifData.findKey(key);
                if (it != exifData.end())
                {
                    QWMatrix matrix;
                    long orientation = it->toLong();
                    DDebug() << " Exif Thumbnail Orientation: " << orientation << endl;
                    
                    switch (orientation) 
                    {
                        case ORIENTATION_HFLIP:
                            matrix.scale(-1, 1);
                            break;
                    
                        case ORIENTATION_ROT_180:
                            matrix.rotate(180);
                            break;
                    
                        case ORIENTATION_VFLIP:
                            matrix.scale(1, -1);
                            break;
                    
                        case ORIENTATION_ROT_90_HFLIP:
                            matrix.scale(-1, 1);
                            matrix.rotate(90);
                            break;
                    
                        case ORIENTATION_ROT_90:
                            matrix.rotate(90);
                            break;
                    
                        case ORIENTATION_ROT_90_VFLIP:
                            matrix.scale(1, -1);
                            matrix.rotate(90);
                            break;
                    
                        case ORIENTATION_ROT_270:
                            matrix.rotate(270);
                            break;
                            
                        default:
                            break;
                    }
        
                    if ( orientation != ORIENTATION_NORMAL )
                        thumbnail = thumbnail.xForm( matrix );
                }
                    
                return thumbnail;
            }
        }
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot get Exif Thumbnail using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return thumbnail;
}

bool DMetadata::setExifThumbnail(const QImage& thumb)
{
    try
    {   
        KTempFile thumbFile(QString::null, "DigikamDMetadataThumb");
        thumbFile.setAutoDelete(true);
        thumb.save(thumbFile.name(), "JPEG");

        const std::string &fileName( (const char*)(QFile::encodeName(thumbFile.name())) );
        d->exifMetadata.setJpegThumbnail( fileName );
        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot set Exif Thumbnail using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

DMetadata::ImageColorWorkSpace DMetadata::getImageColorWorkSpace()
{
    if (d->exifMetadata.empty())
        return WORKSPACE_UNSPECIFIED;

    try
    {    
        // Try to get Exif.Image tags
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifKey key("Exif.Photo.ColorSpace");
        Exiv2::ExifData::iterator it = exifData.findKey(key);
       
        if (it != exifData.end())
        {
            switch (it->toLong())
            {
                case 1:
                    return WORKSPACE_SRGB;
                    break;
                case 2:
                    return WORKSPACE_ADOBERGB;
                    break;
                case 65535:
                    return WORKSPACE_UNCALIBRATED;
                    break;
                default:
                    return WORKSPACE_UNSPECIFIED;
                    break;
            }
        }
        
        // TODO : add here Makernote parsing if necessary.
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot parse image color workspace tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return WORKSPACE_UNSPECIFIED;    
}

bool DMetadata::setImageColorWorkSpace(ImageColorWorkSpace workspace)
{
    if (d->exifMetadata.empty())
       return false;

    try
    {    
        d->exifMetadata["Exif.Photo.ColorSpace"] = (uint16_t)workspace;
        DDebug() << "Exif color workspace tag set to: " << workspace << endl;
        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot set Exif color workspace tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

QSize DMetadata::getImageDimensions()
{
    if (d->exifMetadata.empty())
        return QSize();

    try
    {    
        long width=-1, height=-1;

        // Try to get Exif.Photo tags
        
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifKey key("Exif.Photo.PixelXDimension");
        Exiv2::ExifData::iterator it = exifData.findKey(key);
       
        if (it != exifData.end())
            width = it->toLong();

        Exiv2::ExifKey key2("Exif.Photo.PixelYDimension");
        Exiv2::ExifData::iterator it2 = exifData.findKey(key2);
        
        if (it2 != exifData.end())
            height = it2->toLong();

        if (width != -1 && height != -1)
            return QSize(width, height);

        // Try to get Exif.Image tags
                
        width=-1;
        height=-1;

        Exiv2::ExifKey key3("Exif.Image.ImageWidth");
        Exiv2::ExifData::iterator it3 = exifData.findKey(key3);
       
        if (it3 != exifData.end())
            width = it3->toLong();

        Exiv2::ExifKey key4("Exif.Image.ImageLength");
        Exiv2::ExifData::iterator it4 = exifData.findKey(key4);
        
        if (it4 != exifData.end())
            height = it4->toLong();
        
        if (width != -1 && height != -1)
            return QSize(width, height);
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot parse image dimensions tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return QSize();
}

bool DMetadata::setImageDimensions(const QSize& size)
{
    try
    {    
        d->exifMetadata["Exif.Image.ImageWidth"]      = size.width();
        d->exifMetadata["Exif.Image.ImageLength"]     = size.height();
        d->exifMetadata["Exif.Photo.PixelXDimension"] = size.width();
        d->exifMetadata["Exif.Photo.PixelYDimension"] = size.height();
        
        setImageProgramId();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot set image dimensions using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

DMetadata::ImageOrientation DMetadata::getImageOrientation()
{
    if (d->exifMetadata.empty())
       return ORIENTATION_UNSPECIFIED;

    // Workaround for older Exiv2 versions which do not support
    // Minolta Makernotes and throw an error for such keys.
    bool supportMinolta = true;
    try
    {
        Exiv2::ExifKey minoltaKey1("Exif.MinoltaCs7D.Rotation");
        Exiv2::ExifKey minoltaKey2("Exif.MinoltaCs5D.Rotation");
    }
    catch( Exiv2::Error &e )
    {
        supportMinolta = false;
    }

    try
    {
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifData::iterator it;
        long orientation;
        ImageOrientation imageOrient = ORIENTATION_NORMAL;

        // Because some camera set a wrong standard exif orientation tag, 
        // We need to check makernote tags in first!

        // -- Minolta Cameras ----------------------------------

        if (supportMinolta)
        {
            Exiv2::ExifKey minoltaKey1("Exif.MinoltaCs7D.Rotation");
            it = exifData.findKey(minoltaKey1);

            if (it != exifData.end())
            {
                orientation = it->toLong();
                DDebug() << "Minolta Makernote Orientation: " << orientation << endl;
                switch(orientation)
                {
                    case 76:
                        imageOrient = ORIENTATION_ROT_90;
                        break;
                    case 82:
                        imageOrient = ORIENTATION_ROT_270;
                        break;
                }
                return imageOrient;
            }

            Exiv2::ExifKey minoltaKey2("Exif.MinoltaCs5D.Rotation");
            it = exifData.findKey(minoltaKey2);

            if (it != exifData.end())
            {
                orientation = it->toLong();
                DDebug() << "Minolta Makernote Orientation: " << orientation << endl;
                switch(orientation)
                {
                    case 76:
                        imageOrient = ORIENTATION_ROT_90;
                        break;
                    case 82:
                        imageOrient = ORIENTATION_ROT_270;
                        break;
                }
                return imageOrient;
            }
        }

        // -- Standard Exif tag --------------------------------

        Exiv2::ExifKey keyStd("Exif.Image.Orientation");
        it = exifData.findKey(keyStd);

        if (it != exifData.end())
        {
            orientation = it->toLong();
            DDebug() << "Exif Orientation: " << orientation << endl;
            return (ImageOrientation)orientation;
        }
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot parse Exif Orientation tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return ORIENTATION_UNSPECIFIED;
}

bool DMetadata::setImageOrientation(ImageOrientation orientation)
{
    if (d->exifMetadata.empty())
       return false;

    // Workaround for older Exiv2 versions which do not support
    // Minolta Makernotes and throw an error for such keys.
    bool supportMinolta = true;
    try
    {
        Exiv2::ExifKey minoltaKey1("Exif.MinoltaCs7D.Rotation");
        Exiv2::ExifKey minoltaKey2("Exif.MinoltaCs5D.Rotation");
    }
    catch( Exiv2::Error &e )
    {
        supportMinolta = false;
    }

    try
    {    
        if (orientation < ORIENTATION_UNSPECIFIED || orientation > ORIENTATION_ROT_270)
        {
            DDebug() << k_funcinfo << "Exif orientation tag value is not correct!" << endl;
            return false;
        }
        
        d->exifMetadata["Exif.Image.Orientation"] = (uint16_t)orientation;
        DDebug() << "Exif orientation tag set to: " << orientation << endl;

        // -- Minolta Cameras ----------------------------------

        if (supportMinolta)
        {
            // Minolta camera store image rotation in Makernote.
            // We remove these informations to prevent duplicate values. 
    
            Exiv2::ExifData::iterator it;

            Exiv2::ExifKey minoltaKey1("Exif.MinoltaCs7D.Rotation");
            it = d->exifMetadata.findKey(minoltaKey1);
            if (it != d->exifMetadata.end())
            {
                d->exifMetadata.erase(it);
                DDebug() << "Removing Exif.MinoltaCs7D.Rotation tag" << endl;
            }
        
            Exiv2::ExifKey minoltaKey2("Exif.MinoltaCs5D.Rotation");
            it = d->exifMetadata.findKey(minoltaKey2);
            if (it != d->exifMetadata.end())
            {
                d->exifMetadata.erase(it);
                DDebug() << "Removing Exif.MinoltaCs5D.Rotation tag" << endl;
            }
        }

        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot set Exif Orientation tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

QDateTime DMetadata::getImageDateTime() const
{
    try
    {    
        // In first, trying to get Date & time from Exif tags.
        
        if (!d->exifMetadata.empty())
        {        
            // Try Exif date time original.
    
            Exiv2::ExifData exifData(d->exifMetadata);
            Exiv2::ExifKey key2("Exif.Photo.DateTimeOriginal");
            Exiv2::ExifData::iterator it2 = exifData.findKey(key2);
            
            if (it2 != exifData.end())
            {
                QDateTime dateTime = QDateTime::fromString(it2->toString().c_str(), Qt::ISODate);
    
                if (dateTime.isValid())
                {
                    // DDebug() << "DateTime (Exif original): " << dateTime << endl;
                    return dateTime;
                }
            }
    
            // Bogus Exif date time original entry. Try Exif date time digitized.
    
            Exiv2::ExifKey key3("Exif.Photo.DateTimeDigitized");
            Exiv2::ExifData::iterator it3 = exifData.findKey(key3);
            
            if (it3 != exifData.end())
            {
                QDateTime dateTime = QDateTime::fromString(it3->toString().c_str(), Qt::ISODate);
    
                if (dateTime.isValid())
                {
                    // DDebug() << "DateTime (Exif digitalized): " << dateTime << endl;
                    return dateTime;
                }
            }

            // Bogus Exif date time digitalized entry. Try standard Exif date time entry.
    
            Exiv2::ExifKey key("Exif.Image.DateTime");
            Exiv2::ExifData::iterator it = exifData.findKey(key);
            
            if (it != exifData.end())
            {
                QDateTime dateTime = QDateTime::fromString(it->toString().c_str(), Qt::ISODate);
    
                if (dateTime.isValid())
                {
                    // DDebug() << "DateTime (Exif standard): " << dateTime << endl;
                    return dateTime;
                }
            }
    
        }
        
        // In second, trying to get Date & time from Iptc tags.
            
        if (!d->iptcMetadata.empty())
        {        
            // Try creation Iptc date time entries.

            Exiv2::IptcKey keyDateCreated("Iptc.Application2.DateCreated");
            Exiv2::IptcData iptcData(d->iptcMetadata);
            Exiv2::IptcData::iterator it = iptcData.findKey(keyDateCreated);
                        
            if (it != iptcData.end())
            {
                QString IptcDateCreated(it->toString().c_str());
    
                Exiv2::IptcKey keyTimeCreated("Iptc.Application2.TimeCreated");
                Exiv2::IptcData::iterator it2 = iptcData.findKey(keyTimeCreated);
                
                if (it2 != iptcData.end())
                {
                    QString IptcTimeCreated(it2->toString().c_str());
                    
                    QDate date = QDate::fromString(IptcDateCreated, Qt::ISODate);
                    QTime time = QTime::fromString(IptcTimeCreated, Qt::ISODate);
                    QDateTime dateTime = QDateTime(date, time);
                    
                    if (dateTime.isValid())
                    {
                        // DDebug() << "Date (IPTC created): " << dateTime << endl;
                        return dateTime;
                    }                    
                }
            }                        
            
            // Try digitization Iptc date time entries.
    
            Exiv2::IptcKey keyDigitizationDate("Iptc.Application2.DigitizationDate");
            Exiv2::IptcData::iterator it3 = iptcData.findKey(keyDigitizationDate);
                        
            if (it3 != iptcData.end())
            {
                QString IptcDateDigitization(it3->toString().c_str());
    
                Exiv2::IptcKey keyDigitizationTime("Iptc.Application2.DigitizationTime");
                Exiv2::IptcData::iterator it4 = iptcData.findKey(keyDigitizationTime);
                
                if (it4 != iptcData.end())
                {
                    QString IptcTimeDigitization(it4->toString().c_str());
                    
                    QDate date = QDate::fromString(IptcDateDigitization, Qt::ISODate);
                    QTime time = QTime::fromString(IptcTimeDigitization, Qt::ISODate);
                    QDateTime dateTime = QDateTime(date, time);
                    
                    if (dateTime.isValid())
                    {
                        // DDebug() << "Date (IPTC digitalized): " << dateTime << endl;
                        return dateTime;
                    }                    
                }
            }                       
        }
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot parse Exif date & time tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return QDateTime();
}

bool DMetadata::setImageDateTime(const QDateTime& dateTime, bool setDateTimeDigitized)
{
    if(!dateTime.isValid())
        return false;
    
    try
    {    
        // In first we write date & time into Exif.
        
        // DateTimeDigitized is set by slide scanners etc. when a picture is digitized.
        // DateTimeOriginal specifies the date/time when the picture was taken.
        // For digital cameras, these dates should be both set, and identical.
        // Reference: http://www.exif.org/Exif2-2.PDF, chapter 4.6.5, table 4, section F.

        const std::string &exifdatetime(dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).ascii());
        d->exifMetadata["Exif.Image.DateTime"] = exifdatetime;
        d->exifMetadata["Exif.Photo.DateTimeOriginal"] = exifdatetime;
        if(setDateTimeDigitized)
            d->exifMetadata["Exif.Photo.DateTimeDigitized"] = exifdatetime;
        
        // In Second we write date & time into Iptc.

        const std::string &iptcdate(dateTime.date().toString(Qt::ISODate).ascii());
        const std::string &iptctime(dateTime.time().toString(Qt::ISODate).ascii());
        d->iptcMetadata["Iptc.Application2.DateCreated"] = iptcdate;
        d->iptcMetadata["Iptc.Application2.TimeCreated"] = iptctime;
        if(setDateTimeDigitized)
        {
            d->iptcMetadata["Iptc.Application2.DigitizationDate"] = iptcdate;
            d->iptcMetadata["Iptc.Application2.DigitizationTime"] = iptctime;
        }
        
        setImageProgramId();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot set Date & Time into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

QString DMetadata::getImageComment() const
{
    try
    {
        if (d->filePath.isEmpty())
            return QString();

        // In first we trying to get image comments, outside of Exif and IPTC.

        QString comments = detectEncodingAndDecode(d->imageComments);

        if (!comments.isEmpty())
            return comments;

        // In second, we trying to get Exif comments

        if (!d->exifMetadata.empty())
        {
            Exiv2::ExifKey key("Exif.Photo.UserComment");
            Exiv2::ExifData exifData(d->exifMetadata);
            Exiv2::ExifData::iterator it = exifData.findKey(key);

            if (it != exifData.end())
            {
                QString exifComment = convertCommentValue(*it);

                // some cameras fill the UserComment with whitespace
                if (!exifComment.isEmpty() && !exifComment.stripWhiteSpace().isEmpty())
                  return exifComment;
            }
        }

        // In third, we trying to get IPTC comments

        if (!d->iptcMetadata.empty())
        {
            Exiv2::IptcKey key("Iptc.Application2.Caption");
            Exiv2::IptcData iptcData(d->iptcMetadata);
            Exiv2::IptcData::iterator it = iptcData.findKey(key);

            if (it != iptcData.end())
            {
                QString IptcComment = QString::fromLatin1(it->toString().c_str());

                if (!IptcComment.isEmpty() && !IptcComment.stripWhiteSpace().isEmpty())
                  return IptcComment;
            }
        }

    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot get Image comments using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return QString();
}

bool DMetadata::setImageComment(const QString& comment)
{
    try
    {
        /*
        See bug #139313: An empty string is also a valid value
        if (comment.isEmpty())
            return false;
        */

        DDebug() << d->filePath << " ==> Comment: " << comment << endl;

        // In first we trying to set image comments, outside of Exif and IPTC.

        const std::string str(comment.utf8());
        d->imageComments = str;

        // In Second we write comments into Exif.

        // Write as Unicode only when necessary.
        QTextCodec *latin1Codec = QTextCodec::codecForName("iso8859-1");
        if (latin1Codec->canEncode(comment))
        {
            // write as ASCII
            std::string exifComment("charset=\"Ascii\" ");
            exifComment += comment.latin1();
            d->exifMetadata["Exif.Photo.UserComment"] = exifComment;
        }
        else
        {
            // write as Unicode (UCS-2)

            // Be aware that we are dealing with a UCS-2 string.
            // Null termination means \0\0, strlen does not work,
            // do not use any const-char*-only methods,
            // pass a std::string and not a const char * to ExifDatum::operator=().
            const unsigned short *ucs2 = comment.ucs2();
            std::string exifComment("charset=\"Unicode\" ");
            exifComment.append((const char*)ucs2, sizeof(unsigned short) * comment.length());
            d->exifMetadata["Exif.Photo.UserComment"] = exifComment;
        }

        // In Third we write comments into Iptc.
        // Note that Caption IPTC tag is limited to 2000 char and ASCII charset.

        QString commentIptc = comment;
        commentIptc.truncate(2000);
        d->iptcMetadata["Iptc.Application2.Caption"] = commentIptc.latin1();

        setImageProgramId();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot set Comment into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return false;
}

QString DMetadata::convertCommentValue(const Exiv2::Exifdatum &exifDatum)
{
    try
    {
        std::string comment;
        std::string charset;
#ifdef EXIV2_CHECK_VERSION
        if (EXIV2_CHECK_VERSION(0,11,0))
        {
            comment = exifDatum.toString();
        }
        else
        {
            // workaround for bug in TIFF parser: CommentValue is loaded as DataValue
            const Exiv2::Value &value = exifDatum.value();
            Exiv2::byte *data = new Exiv2::byte[value.size()];
            value.copy(data, Exiv2::invalidByteOrder);
            Exiv2::CommentValue commentValue;
            // this read method is hidden in CommentValue
            static_cast<Exiv2::Value &>(commentValue).read(data, value.size(), Exiv2::invalidByteOrder);
            comment = commentValue.toString();
            delete [] data;
        }
#else
        comment = exifDatum.toString();
#endif

        // libexiv2 will prepend "charset=\"SomeCharset\" " if charset is specified
        // Before conversion to QString, we must know the charset, so we stay with std::string for a while
        if (comment.length() > 8 && comment.substr(0, 8) == "charset=")
        {
            // the prepended charset specification is followed by a blank
            std::string::size_type pos = comment.find_first_of(' ');
            if (pos != std::string::npos)
            {
                // extract string between the = and the blank
                charset = comment.substr(8, pos-8);
                // get the rest of the string after the charset specification
                comment = comment.substr(pos+1);
            }
        }

        if (charset == "\"Unicode\"")
        {
            // QString expects a null-terminated UCS-2 string.
            // Is it already null terminated? In any case, add termination "\0\0" for safety.
            comment.resize(comment.length() + 2, '\0');
            return QString::fromUcs2((unsigned short *)comment.data());
        }
        else if (charset == "\"Jis\"")
        {
            QTextCodec *codec = QTextCodec::codecForName("JIS7");
            return codec->toUnicode(comment.c_str());
        }
        else if (charset == "\"Ascii\"")
        {
            return QString::fromLatin1(comment.c_str());
        }
        else
        {
            return detectEncodingAndDecode(comment);
        }
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot convert Comment using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return QString();
}

QString DMetadata::detectEncodingAndDecode(const std::string &value)
{
    // For charset autodetection, we could use sophisticated code
    // (Mozilla chardet, KHTML's autodetection, QTextCodec::codecForContent),
    // but that is probably too much.
    // We check for UTF8, Local encoding and ASCII.

    if (value.empty())
        return QString();

#if KDE_IS_VERSION(3,2,0)
    if (KStringHandler::isUtf8(value.c_str()))
    {
        return QString::fromUtf8(value.c_str());
    }
#else
    // anyone who is still running KDE 3.0 or 3.1 is missing so many features
    // that he will have to accept this missing feature.
    return QString::fromUtf8(value.c_str());
#endif

    // Utf8 has a pretty unique byte pattern.
    // Thats not true for ASCII, it is not possible
    // to reliably autodetect different ISO-8859 charsets.
    // We try if QTextCodec can decide here, otherwise we use Latin1.
    // Or use local8Bit as default?

    // load QTextCodecs
    QTextCodec *latin1Codec = QTextCodec::codecForName("iso8859-1");
    //QTextCodec *utf8Codec   = QTextCodec::codecForName("utf8");
    QTextCodec *localCodec  = QTextCodec::codecForLocale();

    // make heuristic match
    int latin1Score = latin1Codec->heuristicContentMatch(value.c_str(), value.length());
    int localScore  = localCodec->heuristicContentMatch(value.c_str(), value.length());

    // convert string:
    // Use whatever has the larger score, local or ASCII
    if (localScore >= 0 && localScore >= latin1Score)
    {
        // workaround for bug #134999:
        // The QLatin15Codec may crash if strlen < value.length()
        int length = value.length();
        if (localCodec->name() == QString::fromLatin1("ISO 8859-15"))
            length = strlen(value.c_str());
        return localCodec->toUnicode(value.c_str(), length);
    }
    else
        return QString::fromLatin1(value.c_str());
}


/*
Iptc.Application2.Urgency <==> digiKam Rating links:

digiKam     IPTC
Rating      Urgency

0 star  <=>  8          // Least important
1 star  <=>  7
1 star  <==  6
2 star  <=>  5
3 star  <=>  4
4 star  <==  3
4 star  <=>  2
5 star  <=>  1          // Most important
*/

int DMetadata::getImageRating() const
{
    try
    {    
        if (d->filePath.isEmpty())
            return -1;
            
        if (!d->iptcMetadata.empty())
        {
            Exiv2::IptcKey key("Iptc.Application2.Urgency");
            Exiv2::IptcData iptcData(d->iptcMetadata);
            Exiv2::IptcData::iterator it = iptcData.findKey(key);
            
            if (it != iptcData.end())
            {
                QString IptcUrgency(it->toString().c_str());
    
                if (IptcUrgency == QString("1"))
                    return 5;
                else if (IptcUrgency == QString("2"))
                    return 4;
                else if (IptcUrgency == QString("3"))
                    return 4;
                else if (IptcUrgency == QString("4"))
                    return 3;
                else if (IptcUrgency == QString("5"))
                    return 2;
                else if (IptcUrgency == QString("6"))
                    return 1;
                else if (IptcUrgency == QString("7"))
                    return 1;
                else if (IptcUrgency == QString("8"))
                    return 0;
            }
        }
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot get Image Rating tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return -1;
}

bool DMetadata::setImageRating(int rating)
{
    try
    {    
        if (rating < 0 || rating > 5)
        {
            DDebug() << k_funcinfo << "Rating value to write out of range!" << endl;
            return false;
        }

        DDebug() << d->filePath << " ==> Rating: " << rating << endl;
            
        setImageProgramId();
        QString urgencyTag;
        
        switch(rating)
        {
            case 0:
                urgencyTag = QString("8");
                break;
            case 1:
                urgencyTag = QString("7");
                break;
            case 2:
                urgencyTag = QString("5");
                break;
            case 3:
                urgencyTag = QString("4");
                break;
            case 4:
                urgencyTag = QString("3");
                break;
            case 5:
                urgencyTag = QString("1");
                break;
        }
        
        d->iptcMetadata["Iptc.Application2.Urgency"] = urgencyTag.ascii();

        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot set Rating tag into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

QStringList DMetadata::getImageKeywords() const
{
    try
    {    
        if (!d->iptcMetadata.empty())
        {
            QStringList keywords;          
            Exiv2::IptcData iptcData(d->iptcMetadata);

            for (Exiv2::IptcData::iterator it = iptcData.begin(); it != iptcData.end(); ++it)
            {
                QString key = QString::fromLocal8Bit(it->key().c_str());
                
                if (key == QString("Iptc.Application2.Keywords"))
                {
                    QString val(it->toString().c_str());
                    keywords.append(val);
                }
            }
            
            return keywords;
        }
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot get IPTC Keywords from image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return QStringList();
}

bool DMetadata::setImageKeywords(const QStringList& oldKeywords, const QStringList& newKeywords)
{
    try
    {    
        QStringList oldkeys = oldKeywords;
        QStringList newkeys = newKeywords;
        
        setImageProgramId();
        DDebug() << d->filePath << " ==> Keywords: " << newkeys << endl;
        
        // Remove all old keywords.
        Exiv2::IptcData iptcData(d->iptcMetadata);
        Exiv2::IptcData::iterator it = iptcData.begin();

        while(it != iptcData.end())
        {
            QString key = QString::fromLocal8Bit(it->key().c_str());
            QString val(it->toString().c_str());

            // Also remove new keywords to avoid duplicates. They will be added again below.
            if ( key == QString("Iptc.Application2.Keywords") &&
                 (oldKeywords.contains(val) || newKeywords.contains(val))
               )
                it = iptcData.erase(it);
            else 
                ++it;
        };

        // Add new keywords. Note that Keywords IPTC tag is limited to 64 char but can be redondant.

        Exiv2::IptcKey iptcTag("Iptc.Application2.Keywords");

        for (QStringList::iterator it = newkeys.begin(); it != newkeys.end(); ++it)
        {
            QString key = *it;
            key.truncate(64);
            
            Exiv2::Value::AutoPtr val = Exiv2::Value::create(Exiv2::asciiString);
            val->read(key.latin1());
            iptcData.add(iptcTag, val.get());        
        }

        d->iptcMetadata = iptcData;

        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot set IPTC Keywords into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

bool DMetadata::setImagePhotographerId(const QString& author, const QString& authorTitle)
{
    try
    {    
        setImageProgramId();

        // Byline IPTC tag is limited to 32 char.
        QString Byline = author;
        Byline.truncate(32);
        DDebug() << d->filePath << " ==> Author: " << Byline << endl;
        d->iptcMetadata["Iptc.Application2.Byline"] = Byline.latin1();
        
        // BylineTitle IPTC tag is limited to 32 char.
        QString BylineTitle = authorTitle;
        BylineTitle.truncate(32);
        DDebug() << d->filePath << " ==> Author Title: " << BylineTitle << endl;
        d->iptcMetadata["Iptc.Application2.BylineTitle"] = BylineTitle.latin1();

        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot set Photographer identity into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

bool DMetadata::setImageCredits(const QString& credit, const QString& source, const QString& copyright)
{
    try
    {    
        setImageProgramId();

        // Credit IPTC tag is limited to 32 char.
        QString Credit = credit;
        Credit.truncate(32);
        DDebug() << d->filePath << " ==> Credit: " << Credit << endl;
        d->iptcMetadata["Iptc.Application2.Credit"] = Credit.latin1();

        // Source IPTC tag is limited to 32 char.
        QString Source = source;
        Source.truncate(32);
        DDebug() << d->filePath << " ==> Source: " << Source << endl;
        d->iptcMetadata["Iptc.Application2.Source"] = Source.latin1();

        // Copyright IPTC tag is limited to 128 char.
        QString Copyright = copyright;
        Copyright.truncate(128);
        DDebug() << d->filePath << " ==> Copyright: " << Copyright << endl;
        d->iptcMetadata["Iptc.Application2.Copyright"] = Copyright.latin1();

        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot set Credits identity into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

bool DMetadata::setImageProgramId()
{
    try
    {
        QString software("digiKam-");
        software.append(digikam_version);
        d->exifMetadata["Exif.Image.Software"] = software.ascii();

        d->iptcMetadata["Iptc.Application2.Program"] = "digiKam";
        d->iptcMetadata["Iptc.Application2.ProgramVersion"] = digikam_version;
        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot set Program identity into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return false;
}

PhotoInfoContainer DMetadata::getPhotographInformations() const
{
    try
    {
        if (!d->exifMetadata.empty())
        {
            PhotoInfoContainer photoInfo;
            photoInfo.dateTime = getImageDateTime();
            photoInfo.make     = getExifTagString("Exif.Image.Make");
            photoInfo.model    = getExifTagString("Exif.Image.Model");

            photoInfo.aperture = getExifTagString("Exif.Photo.FNumber");
            if (photoInfo.aperture.isEmpty())
                photoInfo.aperture = getExifTagString("Exif.Photo.ApertureValue");

            photoInfo.exposureTime = getExifTagString("Exif.Photo.ExposureTime");
            if (photoInfo.exposureTime.isEmpty())
                photoInfo.exposureTime = getExifTagString("Exif.Photo.ShutterSpeedValue");

            photoInfo.exposureMode    = getExifTagString("Exif.Photo.ExposureMode");
            photoInfo.exposureProgram = getExifTagString("Exif.Photo.ExposureProgram");

            photoInfo.focalLenght     = getExifTagString("Exif.Photo.FocalLength");
            photoInfo.focalLenght35mm = getExifTagString("Exif.Photo.FocalLengthIn35mmFilm");

            photoInfo.sensitivity = getExifTagString("Exif.Photo.ISOSpeedRatings");
            if (photoInfo.sensitivity.isEmpty())
                photoInfo.sensitivity = getExifTagString("Exif.Photo.ExposureIndex");

            photoInfo.flash = getExifTagString("Exif.Photo.Flash");
            photoInfo.whiteBalance = getExifTagString("Exif.Photo.WhiteBalance");

            return photoInfo;
        }
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot get Photograph informations into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return PhotoInfoContainer();
}

QString DMetadata::getExifTagString(const char* exifTagName, bool escapeCR) const
{
    try
    {
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifData::iterator it = exifData.findKey(exifKey);
        if (it != exifData.end())
        {
            std::ostringstream os;
            os << *it;
            QString tagValue = QString::fromLocal8Bit(os.str().c_str());

            if (escapeCR)
                tagValue.replace("\n", " ");

            return tagValue;
        }
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot find Exif key '"
                  << exifTagName << "' into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return QString();
}

bool DMetadata::setExifTagString(const char *exifTagName, const QString& value)
{
    try
    {
        d->exifMetadata[exifTagName] = value.ascii();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot set Exif tag string into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return false;
}

QString DMetadata::getIptcTagString(const char* iptcTagName, bool escapeCR) const
{
    try
    {
        Exiv2::IptcKey iptcKey(iptcTagName);
        Exiv2::IptcData iptcData(d->iptcMetadata);
        Exiv2::IptcData::iterator it = iptcData.findKey(iptcKey);
        if (it != iptcData.end())
        {
            std::ostringstream os;
            os << *it;
            QString tagValue = QString::fromLocal8Bit(os.str().c_str());

            if (escapeCR)
                tagValue.replace("\n", " ");

            return tagValue;
        }
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot find Iptc key '"
                  << iptcTagName << "' into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return QString();
}

bool DMetadata::setIptcTagString(const char *iptcTagName, const QString& value)
{
    try
    {
        d->iptcMetadata[iptcTagName] = value.ascii();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot set Iptc tag string into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return false;
}

QByteArray DMetadata::getExifTagData(const char* exifTagName) const
{
    try
    {
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifData::iterator it = exifData.findKey(exifKey);
        if (it != exifData.end())
        {
            QByteArray data((*it).size());
            (*it).copy((Exiv2::byte*)data.data(), exifData.byteOrder());
            return data;
        }
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot find Exif key '"
                  << exifTagName << "' into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return QByteArray();
}

QByteArray DMetadata::getIptcTagData(const char *iptcTagName) const
{
    try
    {
        Exiv2::IptcKey iptcKey(iptcTagName);
        Exiv2::IptcData iptcData(d->iptcMetadata);
        Exiv2::IptcData::iterator it = iptcData.findKey(iptcKey);
        if (it != iptcData.end())
        {
            QByteArray data((*it).size());
            (*it).copy((Exiv2::byte*)data.data(), Exiv2::bigEndian);
            return data;
        }
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot find Iptc key '"
                  << iptcTagName << "' into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return QByteArray();
}

bool DMetadata::removeExifTag(const char *exifTagName)
{
    try
    {  
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData::iterator it = d->exifMetadata.findKey(exifKey);
        if (it != d->exifMetadata.end())
        {
            d->exifMetadata.erase(it);
            return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot remove Exif tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

bool DMetadata::removeIptcTag(const char *iptcTagName)
{
    try
    {  
        Exiv2::IptcKey iptcKey(iptcTagName);
        Exiv2::IptcData::iterator it = d->iptcMetadata.findKey(iptcKey);
        if (it != d->iptcMetadata.end())
        {
            d->iptcMetadata.erase(it);
            return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot remove Iptc tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

bool DMetadata::getImagePreview(QImage& preview)
{
    try
    {
        // In first we trying to get from Iptc preview tag.
        if (preview.loadFromData(getIptcTagData("Iptc.Application2.Preview")) )
            return true;

        // TODO : Added here Makernotes preview extraction when Exiv2 will be fixed for that.
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot get image preview using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return false;
}

bool DMetadata::setImagePreview(const QImage& preview)
{
    try
    {
        KTempFile previewFile(QString::null, "DigikamDMetadataPreview");
        previewFile.setAutoDelete(true);
        // A little bit compressed preview jpeg image to limit IPTC size.
        preview.save(previewFile.name(), "JPEG");

        QFile file(previewFile.name());
        if ( !file.open(IO_ReadOnly) ) 
            return false;

        DDebug() << "(" << preview.width() << "x" << preview.height() 
                  << ") JPEG image preview size: " << file.size() 
                  << " bytes" << endl;
        
        QByteArray data(file.size());
        QDataStream stream( &file );
        stream.readRawBytes(data.data(), data.size());
        file.close();
        
        Exiv2::DataValue val;
        val.read((Exiv2::byte *)data.data(), data.size());
        d->iptcMetadata["Iptc.Application2.Preview"] = val;
        
        // See http://www.iptc.org/std/IIM/4.1/specification/IIMV4.1.pdf Appendix A for details.
        d->iptcMetadata["Iptc.Application2.PreviewFormat"]  = 11;  // JPEG 
        d->iptcMetadata["Iptc.Application2.PreviewVersion"] = 1;
        
        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot get image preview using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return false;
}

bool DMetadata::getGPSInfo(double& altitude, double& latitude, double& longitude)
{
    try
    {    
        double num, den, min, sec;
        latitude=0.0, longitude=0.0, altitude=0.0;
        
        // Get the reference in first.

        QByteArray latRef = getExifTagData("Exif.GPSInfo.GPSLatitudeRef");
        if (latRef.isEmpty()) return false;

        QByteArray lngRef = getExifTagData("Exif.GPSInfo.GPSLongitudeRef");
        if (lngRef.isEmpty()) return false;

        QByteArray altRef = getExifTagData("Exif.GPSInfo.GPSAltitudeRef");

        // Latitude decoding.

        Exiv2::ExifKey exifKey("Exif.GPSInfo.GPSLatitude");
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifData::iterator it = exifData.findKey(exifKey);
        if (it != exifData.end())
        {
            num = (double)((*it).toRational(0).first);
            den = (double)((*it).toRational(0).second);
            latitude = num/den;

            num = (double)((*it).toRational(1).first);
            den = (double)((*it).toRational(1).second);
            min = num/den;
            if (min != -1.0)
                latitude = latitude + min/60.0;

            num = (double)((*it).toRational(2).first);
            den = (double)((*it).toRational(2).second);
            sec = num/den;
            if (sec != -1.0)
                latitude = latitude + sec/3600.0;
        }
        else 
            return false;
        
        if (latRef[0] == 'S') latitude *= -1.0;
    
        // Longitude decoding.

        Exiv2::ExifKey exifKey2("Exif.GPSInfo.GPSLongitude");
        it = exifData.findKey(exifKey2);
        if (it != exifData.end())
        {
            num = (double)((*it).toRational(0).first);
            den = (double)((*it).toRational(0).second);
            longitude = num/den;

            num = (double)((*it).toRational(1).first);
            den = (double)((*it).toRational(1).second);
            min = num/den;
            if (min != -1.0)
                longitude = longitude + min/60.0;

            num = (double)((*it).toRational(2).first);
            den = (double)((*it).toRational(2).second);
            sec = num/den;
            if (sec != -1.0)
                longitude = longitude + sec/3600.0;
        }
        else 
            return false;
        
        if (lngRef[0] == 'W') longitude *= -1.0;

        // Altitude decoding.

        if (!altRef.isEmpty()) 
        {
            Exiv2::ExifKey exifKey3("Exif.GPSInfo.GPSAltitude");
            it = exifData.findKey(exifKey3);
            if (it != exifData.end())
            {
                num = (double)((*it).toRational(0).first);
                den = (double)((*it).toRational(0).second);
                altitude = num/den;
            }
        
            if (altRef[0] == '1') altitude *= -1.0;
        }

        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot get Exif GPS tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

bool DMetadata::setGPSInfo(double altitude, double latitude, double longitude)
{
    try
    {    
        // In first, we need to clean up all existing GPS info.
        removeGPSInfo();

        char scratchBuf[100];
        long int nom, denom;
        long int deg, min;
        
        // Do all the easy constant ones first.
        // GPSVersionID tag: standard says is should be four bytes: 02 00 00 00
        // (and, must be present).
        Exiv2::Value::AutoPtr value = Exiv2::Value::create(Exiv2::unsignedByte);
        value->read("2 0 0 0");
        d->exifMetadata.add(Exiv2::ExifKey("Exif.GPSInfo.GPSVersionID"), value.get());

        // Datum: the datum of the measured data. If not given, we insert WGS-84.
        d->exifMetadata["Exif.GPSInfo.GPSMapDatum"] = "WGS-84";
        
        // Now start adding data.

        // ALTITUDE.
        // Altitude reference: byte "00" meaning "sea level".
        value = Exiv2::Value::create(Exiv2::unsignedByte);
        value->read("0");
        d->exifMetadata.add(Exiv2::ExifKey("Exif.GPSInfo.GPSAltitudeRef"), value.get());
        
        // And the actual altitude.
        value = Exiv2::Value::create(Exiv2::signedRational);
        convertToRational(altitude, &nom, &denom, 4);
        snprintf(scratchBuf, 100, "%ld/%ld", nom, denom);
        value->read(scratchBuf);
        d->exifMetadata.add(Exiv2::ExifKey("Exif.GPSInfo.GPSAltitude"), value.get());

        // LATTITUDE
        // Latitude reference: "N" or "S".
        if (latitude < 0)
        {
            // Less than Zero: ie, minus: means
            // Southern hemisphere. Where I live.
            d->exifMetadata["Exif.GPSInfo.GPSLatitudeRef"] = "S";
        } 
        else 
        {
            // More than Zero: ie, plus: means
            // Northern hemisphere.
            d->exifMetadata["Exif.GPSInfo.GPSLatitudeRef"] = "N";
        }
        
        // Now the actual lattitude itself.
        // This is done as three rationals.
        // I choose to do it as:
        //   dd/1 - degrees.
        //   mmmm/100 - minutes
        //   0/1 - seconds
        // Exif standard says you can do it with minutes
        // as mm/1 and then seconds as ss/1, but its
        // (slightly) more accurate to do it as
        //  mmmm/100 than to split it.
        // We also absolute the value (with fabs())
        // as the sign is encoded in LatRef.
        // Further note: original code did not translate between
        //   dd.dddddd to dd mm.mm - that's why we now multiply
        //   by 6000 - x60 to get minutes, x100 to get to mmmm/100.
        value = Exiv2::Value::create(Exiv2::signedRational);
        deg   = (int)floor(fabs(latitude)); // Slice off after decimal.
        min   = (int)floor((fabs(latitude) - floor(fabs(latitude))) * 6000);
        snprintf(scratchBuf, 100, "%ld/1 %ld/100 0/1", deg, min);
        value->read(scratchBuf);
        d->exifMetadata.add(Exiv2::ExifKey("Exif.GPSInfo.GPSLatitude"), value.get());
        
        // LONGITUDE
        // Longitude reference: "E" or "W".
        if (longitude < 0)
        {
            // Less than Zero: ie, minus: means
            // Western hemisphere.
            d->exifMetadata["Exif.GPSInfo.GPSLongitudeRef"] = "W";
        } 
        else 
        {
            // More than Zero: ie, plus: means
            // Eastern hemisphere. Where I live.
            d->exifMetadata["Exif.GPSInfo.GPSLongitudeRef"] = "E";
        }

        // Now the actual longitude itself.
        // This is done as three rationals.
        // I choose to do it as:
        //   dd/1 - degrees.
        //   mmmm/100 - minutes
        //   0/1 - seconds
        // Exif standard says you can do it with minutes
        // as mm/1 and then seconds as ss/1, but its
        // (slightly) more accurate to do it as
        //  mmmm/100 than to split it.
        // We also absolute the value (with fabs())
        // as the sign is encoded in LongRef.
        // Further note: original code did not translate between
        //   dd.dddddd to dd mm.mm - that's why we now multiply
        //   by 6000 - x60 to get minutes, x100 to get to mmmm/100.
        value = Exiv2::Value::create(Exiv2::signedRational);
        deg   = (int)floor(fabs(longitude)); // Slice off after decimal.
        min   = (int)floor((fabs(longitude) - floor(fabs(longitude))) * 6000);
        snprintf(scratchBuf, 100, "%ld/1 %ld/100 0/1", deg, min);
        value->read(scratchBuf);
        d->exifMetadata.add(Exiv2::ExifKey("Exif.GPSInfo.GPSLongitude"), value.get());
    
        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot set Exif GPS tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

bool DMetadata::removeGPSInfo()
{
    try
    {  
        QStringList gpsTagsKeys;
  
        for (Exiv2::ExifData::iterator it = d->exifMetadata.begin();
             it != d->exifMetadata.end(); ++it)
        {
            QString key = QString::fromLocal8Bit(it->key().c_str());

            if (key.section(".", 1, 1) == QString("GPSInfo"))
                gpsTagsKeys.append(key);
        }

        for(QStringList::Iterator it2 = gpsTagsKeys.begin(); it2 != gpsTagsKeys.end(); ++it2)             
        {
            Exiv2::ExifKey gpsKey((*it2).ascii());
            Exiv2::ExifData::iterator it3 = d->exifMetadata.findKey(gpsKey);
            if (it3 != d->exifMetadata.end())
                d->exifMetadata.erase(it3);
        }
        
        return true;
    }
    catch( Exiv2::Error &e )
    {
        DDebug() << "Cannot remove Exif GPS tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

void DMetadata::convertToRational(double number, long int* numerator, 
                                   long int* denominator, int rounding)
{
    // This function converts the given decimal number
    // to a rational (fractional) number.
    //
    // Examples in comments use Number as 25.12345, Rounding as 4.
    
    // Split up the number.
    double whole      = trunc(number);
    double fractional = number - whole;

    // Calculate the "number" used for rounding.
    // This is 10^Digits - ie, 4 places gives us 10000.
    double rounder = pow(10, rounding);

    // Round the fractional part, and leave the number
    // as greater than 1.
    // To do this we: (for example)
    //  0.12345 * 10000 = 1234.5
    //  floor(1234.5) = 1234 - now bigger than 1 - ready...
    fractional = trunc(fractional * rounder);

    // Convert the whole thing to a fraction.
    // Fraction is:
    //     (25 * 10000) + 1234   251234
    //     ------------------- = ------ = 25.1234
    //           10000            10000
    double numTemp = (whole * rounder) + fractional;
    double denTemp = rounder;

    // Now we should reduce until we can reduce no more.
    
    // Try simple reduction...
    // if   Num
    //     ----- = integer out then....
    //      Den
    if (trunc(numTemp / denTemp) == (numTemp / denTemp))
    {
        // Divide both by Denominator.
        numTemp /= denTemp;
        denTemp /= denTemp;
    }
    
    // And, if that fails, brute force it.
    while (1)
    {
        // Jump out if we can't integer divide one.
        if ((numTemp / 2) != trunc(numTemp / 2)) break;
        if ((denTemp / 2) != trunc(denTemp / 2)) break;
        // Otherwise, divide away.
        numTemp /= 2;
        denTemp /= 2;
    }

    // Copy out the numbers.
    *numerator   = (int)numTemp;
    *denominator = (int)denTemp;
}

}  // NameSpace Digikam

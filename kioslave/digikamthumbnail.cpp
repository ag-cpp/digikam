/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 * 
 * Date        : 2003-01-15
 * Description : digiKam KIO slave to get image thumbnails. 
 *               This kio-slave support this freedesktop 
 *               specification about thumbnails mamagement:
 *               http://jens.triq.net/thumbnail-spec
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2003-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#define XMD_H
#define PNG_BYTES_TO_CHECK 4
#define DigiKamFingerPrint "Digikam Thumbnail Generator"

// C++ includes.

#include <cstdlib>
#include <cstdio>
#include <cstring>

// Qt Includes.

#include <qcstring.h>
#include <qstring.h>
#include <qimage.h>
#include <qdatastream.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qwmatrix.h>
#include <qregexp.h>
#include <qapplication.h>

// KDE includes.

#include <kdebug.h>
#include <kurl.h>
#include <kinstance.h>
#include <kimageio.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kmdcodec.h>
#include <ktempfile.h>
#include <ktrader.h>
#include <klibloader.h>
#include <kmimetype.h>
#include <kprocess.h>
#include <kio/global.h>
#include <kio/thumbcreator.h>
#include <kfilemetainfo.h>

// LibKDcraw includes.

#include <libkdcraw/rawfiles.h>
#include <libkdcraw/kdcraw.h>

// Local includes

#include "dimg.h"
#include "dmetadata.h"
#include "jpegutils.h"
#include "digikamthumbnail.h"
#include "digikam_export.h"

// C Ansi includes.

extern "C"
{
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <png.h>
}

using namespace KIO;
using namespace Digikam;

kio_digikamthumbnailProtocol::kio_digikamthumbnailProtocol(int argc, char** argv) 
                            : SlaveBase("kio_digikamthumbnail", argv[2], argv[3])
{
    argc_ = argc;
    argv_ = argv;
    app_  = 0;

    createThumbnailDirs();
}

kio_digikamthumbnailProtocol::~kio_digikamthumbnailProtocol()
{
}

void kio_digikamthumbnailProtocol::get(const KURL& url )
{
    int  size =  metaData("size").toInt();
    bool exif = (metaData("exif") == "yes");

    cachedSize_ = (size <= 128) ? 128 : 256;

    if (cachedSize_ <= 0)
    {
        error(KIO::ERR_INTERNAL, i18n("No or invalid size specified"));
        kdWarning() << "No or invalid size specified" << endl;
        return;
    }

    // generate the thumbnail path
    QString uri = "file://" + QDir::cleanDirPath(url.path(-1));
    KMD5 md5( QFile::encodeName(uri) );
    QString thumbPath = (cachedSize_ == 128) ? smallThumbPath_ : bigThumbPath_;
    thumbPath += QFile::encodeName( md5.hexDigest() ) + ".png";

    QImage img;
    bool regenerate = true;

    // stat the original file
    struct stat st;
    if (::stat(QFile::encodeName(url.path(-1)), &st) != 0)
    {
        error(KIO::ERR_INTERNAL, i18n("File does not exist"));
        return;
    }

    // NOTE: if thumbnail have not been generated by digiKam (konqueror for example),
    //       force to recompute it, else we use it.

    img = loadPNG(thumbPath);
    if (!img.isNull())
    {
        if (img.text("Thumb::MTime") == QString::number(st.st_mtime) &&
            img.text("Software")     == QString(DigiKamFingerPrint))
            regenerate = false;
    }

    if (regenerate)
    {
        // To speed-up thumb extraction, we trying to load image using the file extension.
        if ( !loadByExtension(img, url.path()) )
        {
            // Try JPEG loading : JPEG files without using Exif Thumb.
            if ( !loadJPEG(img, url.path()) )
            {
                // Try to load with dcraw : RAW files.
                if (!KDcrawIface::KDcraw::loadDcrawPreview(img, url.path()) )
                {
                    // Try to load with DImg : TIFF, PNG, etc.
                    if (!loadDImg(img, url.path()) )
                    {
                        // Try to load with KDE thumbcreators : video files and others stuff.
                        loadKDEThumbCreator(img, url.path());
                    }
                }
            }
        }

        if (img.isNull())
        {
            error(KIO::ERR_INTERNAL, i18n("Cannot create thumbnail for %1")
                  .arg(url.prettyURL()));
            kdWarning() << "Cannot create thumbnail for " << url.path() << endl;
            return;
        }

        if (QMAX(img.width(),img.height()) != cachedSize_)
            img = img.smoothScale(cachedSize_, cachedSize_, QImage::ScaleMin);

        if (img.depth() != 32)
            img = img.convertDepth(32);

        if (exif)
            exifRotate(url.path(), img);

        img.setText(QString("Thumb::URI").latin1(),   0, uri);
        img.setText(QString("Thumb::MTime").latin1(), 0, QString::number(st.st_mtime));
        img.setText(QString("Software").latin1(),     0, QString(DigiKamFingerPrint));

        KTempFile temp(thumbPath + "-digikam-", ".png");
        if (temp.status() == 0)
        {
            img.save(temp.name(), "PNG", 0);
            ::rename(QFile::encodeName(temp.name()),
                     QFile::encodeName(thumbPath));
        }
    }

    img = img.smoothScale(size, size, QImage::ScaleMin);

    if (img.isNull())
    {
        error(KIO::ERR_INTERNAL, "Thumbnail is null");
        return;
    }

    QByteArray imgData;
    QDataStream stream( imgData, IO_WriteOnly );

    QString shmid = metaData("shmid");

    if (shmid.isEmpty())
    {
        stream << img;
    }
    else
    {
        void *shmaddr = shmat(shmid.toInt(), 0, 0);

        if (shmaddr == (void *)-1)
        {
            error(KIO::ERR_INTERNAL, "Failed to attach to shared memory segment " + shmid);
            kdWarning() << "Failed to attach to shared memory segment " << shmid << endl;
            return;
        }

        if (img.width() * img.height() > cachedSize_ * cachedSize_)
        {
            error(KIO::ERR_INTERNAL, "Image is too big for the shared memory segment");
            kdWarning() << "Image is too big for the shared memory segment" << endl;
            shmdt((char*)shmaddr);
            return;
        }

        stream << img.width() << img.height() << img.depth();
        memcpy(shmaddr, img.bits(), img.numBytes());
        shmdt((char*)shmaddr);
    }

    data(imgData);
    finished();
}

bool kio_digikamthumbnailProtocol::loadByExtension(QImage& image, const QString& path)
{
    QFileInfo fileInfo(path);
    if (!fileInfo.exists())
        return false;

    // Try to use embedded preview image from metadata.    
    DMetadata metadata(path);
    if (metadata.getImagePreview(image))
    {
        kdDebug() << "Use Exif/Iptc preview extraction. Size of image: " 
                  << image.width() << "x" << image.height() << endl;
        return true;
    }
        
    // Else, use the right way depending of image file extension.
    QString ext = fileInfo.extension(false).upper();
    QString rawFilesExt(raw_file_extentions);

    if (!ext.isEmpty())
    {
        if (ext == QString("JPEG") || ext == QString("JPG") || ext == QString("JPE"))
            return (loadJPEG(image, path));
        else if (ext == QString("PNG"))
            return (loadDImg(image, path));
        else if (ext == QString("TIFF") || ext == QString("TIF"))
            return (loadDImg(image, path));
        else if (rawFilesExt.upper().contains(ext))
            return (KDcrawIface::KDcraw::loadDcrawPreview(image, path));
    }

    return false;
}

bool kio_digikamthumbnailProtocol::loadJPEG(QImage& image, const QString& path)
{
    return Digikam::loadJPEGScaled(image, path, cachedSize_);
}

void kio_digikamthumbnailProtocol::exifRotate(const QString& filePath, QImage& thumb)
{
    // Rotate thumbnail based on metadata orientation information

    DMetadata metadata(filePath);
    DMetadata::ImageOrientation orientation = metadata.getImageOrientation();

    if (orientation == DMetadata::ORIENTATION_NORMAL ||
        orientation == DMetadata::ORIENTATION_UNSPECIFIED)
        return;

    QWMatrix matrix;

    switch (orientation)
    {
        case DMetadata::ORIENTATION_NORMAL:
        case DMetadata::ORIENTATION_UNSPECIFIED:
            break;

        case DMetadata::ORIENTATION_HFLIP:
            matrix.scale(-1, 1);
            break;

        case DMetadata::ORIENTATION_ROT_180:
            matrix.rotate(180);
            break;

        case DMetadata::ORIENTATION_VFLIP:
            matrix.scale(1, -1);
            break;

        case DMetadata::ORIENTATION_ROT_90_HFLIP:
            matrix.scale(-1, 1);
            matrix.rotate(90);
            break;

        case DMetadata::ORIENTATION_ROT_90:
            matrix.rotate(90);
            break;

        case DMetadata::ORIENTATION_ROT_90_VFLIP:
            matrix.scale(1, -1);
            matrix.rotate(90);
            break;

        case DMetadata::ORIENTATION_ROT_270:
            matrix.rotate(270);
            break;
    }

    // transform accordingly
    thumb = thumb.xForm( matrix );
}

QImage kio_digikamthumbnailProtocol::loadPNG(const QString& path)
{
    png_uint_32  w32, h32;
    int          w, h;
    bool         has_alpha;
    bool         has_grey;
    FILE        *f;
    png_structp  png_ptr = NULL;
    png_infop    info_ptr = NULL;
    int          bit_depth, color_type, interlace_type;

    has_alpha = 0;
    has_grey  = 0;

    QImage qimage;

    f = fopen(path.latin1(), "rb");
    if (!f)
        return qimage;

    unsigned char buf[PNG_BYTES_TO_CHECK];

    fread(buf, 1, PNG_BYTES_TO_CHECK, f);
    if (!png_check_sig(buf, PNG_BYTES_TO_CHECK))
    {
        fclose(f);
        return qimage;
    }
    rewind(f);

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fclose(f);
        return qimage;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(f);
        return qimage;
    }

    if (setjmp(png_ptr->jmpbuf))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(f);
        return qimage;
    }

    png_init_io(png_ptr, f);
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *) (&w32),
                 (png_uint_32 *) (&h32), &bit_depth, &color_type,
                 &interlace_type, NULL, NULL);

    w = w32;
    h = h32;

    qimage.create(w, h, 32);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png_ptr);

    if (info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
        has_alpha = 1;

    if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
        has_alpha = 1;
        has_grey = 1;
    }

    if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY)
        has_grey = 1;

    unsigned char **lines;
    int             i;

    if (has_alpha)
        png_set_expand(png_ptr);

    if (QImage::systemByteOrder() == QImage::LittleEndian)
    {
        png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
        png_set_bgr(png_ptr);
    }
    else
    {
        png_set_swap_alpha(png_ptr);
        png_set_filler(png_ptr, 0xff, PNG_FILLER_BEFORE);
    }

    /* 16bit color -> 8bit color */
    if ( bit_depth == 16 )
        png_set_strip_16(png_ptr);

    /* pack all pixels to byte boundaires */

    png_set_packing(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand(png_ptr);

    lines = (unsigned char **)malloc(h * sizeof(unsigned char *));
    if (!lines)
    {
        png_read_end(png_ptr, info_ptr);
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(f);
        return qimage;
    }

    if (has_grey)
    {
        png_set_gray_to_rgb(png_ptr);
        if (png_get_bit_depth(png_ptr, info_ptr) < 8)
            png_set_gray_1_2_4_to_8(png_ptr);
    }

    int sizeOfUint = sizeof(unsigned int);
    for (i = 0 ; i < h ; i++)
        lines[i] = ((unsigned char *)(qimage.bits())) + (i * w * sizeOfUint);

    png_read_image(png_ptr, lines);
    free(lines);

    png_textp text_ptr;
    int num_text=0;
    png_get_text(png_ptr,info_ptr,&text_ptr,&num_text);
    while (num_text--) 
    {
        qimage.setText(text_ptr->key,0,text_ptr->text);
        text_ptr++;
    }

    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
    fclose(f);

    return qimage;
}

// -- Load using DImg ---------------------------------------------------------------------

bool kio_digikamthumbnailProtocol::loadDImg(QImage& image, const QString& path)
{
    Digikam::DImg dimg_im;

    // to disable raw loader - does not work from ioslave
    dimg_im.setAttribute("noeventloop", true);

    if (!dimg_im.load(path))
    {
        return false;
    }

    image = dimg_im.copyQImage();
    org_width_  = image.width();
    org_height_ = image.height();

    if ( QMAX(org_width_, org_height_) != cachedSize_ )
    {
        QSize sz(dimg_im.width(), dimg_im.height());
        sz.scale(cachedSize_, cachedSize_, QSize::ScaleMin);
        image.scale(sz.width(), sz.height());
    }

    new_width_  = image.width();
    new_height_ = image.height();

    image.setAlphaBuffer(true) ;

    return true;
}

// -- Load using KDE API ---------------------------------------------------------------------

bool kio_digikamthumbnailProtocol::loadKDEThumbCreator(QImage& image, const QString& path)
{
    // this sucks royally. some of the thumbcreators need an instance of
    // app running so that they can use pixmap. till they get their 
    // code fixed, we will have to create a qapp instance.
    if (!app_)
        app_ = new QApplication(argc_, argv_);

    QString mimeType = KMimeType::findByURL(path)->name();
    if (mimeType.isEmpty())
    {
        kdDebug() << "Mimetype not found" << endl;
        return false;
    }

    QString mimeTypeAlt = mimeType.replace(QRegExp("/.*"), "/*");

    QString plugin;

    KTrader::OfferList plugins = KTrader::self()->query("ThumbCreator");
    for (KTrader::OfferList::ConstIterator it = plugins.begin(); it != plugins.end(); ++it)
    {
        QStringList mimeTypes = (*it)->property("MimeTypes").toStringList();
        for (QStringList::ConstIterator mt = mimeTypes.begin(); mt != mimeTypes.end(); ++mt)
        {
            if  ((*mt) == mimeType || (*mt) == mimeTypeAlt)
            {
                plugin=(*it)->library();
                break;
            }
        }

        if (!plugin.isEmpty())
            break;
    }

    if (plugin.isEmpty())
    {
        kdDebug() << "No relevant plugin found " << endl;
        return false;
    }

    KLibrary *library = KLibLoader::self()->library(QFile::encodeName(plugin));
    if (!library)
    {
        kdDebug() << "Plugin library not found " << plugin << endl;
        return false;
    }

    ThumbCreator *creator = 0;
    newCreator create = (newCreator)library->symbol("new_creator");
    if (create)
        creator = create();

    if (!creator)
    {
        kdDebug() << "Cannot load ThumbCreator " << plugin << endl;
        return false;
    }

    if (!creator->create(path, cachedSize_, cachedSize_, image))
    {
        kdDebug() << "Cannot create thumbnail for " << path << endl;
        delete creator;
        return false;
    }  

    delete creator;
    return true;
}

void kio_digikamthumbnailProtocol::createThumbnailDirs()
{
    QString path = QDir::homeDirPath() + "/.thumbnails/";

    smallThumbPath_ = path + "normal/";
    bigThumbPath_   = path + "large/";

    KStandardDirs::makeDir(smallThumbPath_, 0700);
    KStandardDirs::makeDir(bigThumbPath_, 0700);
}

// -- KIO slave registration ---------------------------------------------------------------------

extern "C"
{
    DIGIKAM_EXPORT int kdemain(int argc, char **argv)
    {
        KLocale::setMainCatalogue("digikam");
        KInstance instance( "kio_digikamthumbnail" );
        ( void ) KGlobal::locale();

        if (argc != 4) 
        {
            kdDebug() << "Usage: kio_digikamthumbnail  protocol domain-socket1 domain-socket2"
                      << endl;
            exit(-1);
        }

        KImageIO::registerFormats();

        kio_digikamthumbnailProtocol slave(argc, argv);
        slave.dispatchLoop();

        return 0;
    }
}

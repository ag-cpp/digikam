/* ============================================================
 * Author: Gilles Caulier <caulier dot gilles at kdemail dot net> 
 * Date  : 2005-11-01
 * Description : A PNG files loader for DImg framework.
 * 
 * Copyright 2005-2006 by Gilles Caulier
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

// This line must be commented to prevent any latency time
// when we use threaded image loader interface for each image
// files io. Uncomment this line only for debugging.
//#define ENABLE_DEBUG_MESSAGES 

#define PNG_BYTES_TO_CHECK 4

// C Ansi includes.

extern "C"
{
#include <unistd.h>
#include <stdarg.h> 
}

// C++  includes.

#include <cstdlib>
#include <cstdio>

// QT includes.

#include <qfile.h>
#include <qcstring.h>

// KDE includes.

#include <kdebug.h>

// Local includes.

#include "version.h"
#include "dimg.h"
#include "pngloader.h"

namespace Digikam
{

PNGLoader::PNGLoader(DImg* image)
         : DImgLoader(image)
{
    m_hasAlpha   = false;
    m_sixteenBit = false;
}

bool PNGLoader::load(const QString& filePath, DImgLoaderObserver *observer)
{
    png_uint_32  w32, h32;
    int          width, height;
    FILE        *f;
    int          bit_depth, color_type, interlace_type;
    png_structp  png_ptr  = NULL;
    png_infop    info_ptr = NULL;
    
    // -------------------------------------------------------------------
    // Open the file
    
    f = fopen(QFile::encodeName(filePath), "rb");
    if ( !f )
    {
        kdDebug() << k_funcinfo << "Cannot open image file." << endl;
        return false;
    }

    unsigned char buf[PNG_BYTES_TO_CHECK];

    fread(buf, 1, PNG_BYTES_TO_CHECK, f);
    if (!png_check_sig(buf, PNG_BYTES_TO_CHECK))
    {
        kdDebug() << k_funcinfo << "Not a PNG image file." << endl;
        fclose(f);
        return false;
    }
    rewind(f);

    // -------------------------------------------------------------------
    // Initialize the internal structures
    
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        kdDebug() << k_funcinfo << "Invalid PNG image file structure." << endl;
        fclose(f);
        return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        kdDebug() << k_funcinfo << "Cannot reading PNG image file structure." << endl;
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(f);
        return false;
    }

    // -------------------------------------------------------------------
    // PNG error handling. If an error occurs during reading, libpng
    // will jump here
    
    if (setjmp(png_ptr->jmpbuf))
    {
        kdDebug() << k_funcinfo << "Internal libPNG error during reading file. Process aborted!" << endl;
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(f);
        return false;
    }

    png_init_io(png_ptr, f);
    
    // -------------------------------------------------------------------
    // Read all PNG info up to image data
    
    png_read_info(png_ptr, info_ptr);
    
    png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *) (&w32),
                 (png_uint_32 *) (&h32), &bit_depth, &color_type,
                 &interlace_type, NULL, NULL);

    width  = (int)w32;
    height = (int)h32;

    if (bit_depth == 16)                       
    {
#ifdef ENABLE_DEBUG_MESSAGES    
        kdDebug() << "PNG in 16 bits/color/pixel." << endl;
#endif
        m_sixteenBit = true;
        
        switch (color_type)
        {
            case PNG_COLOR_TYPE_RGB :		// RGB 
#ifdef ENABLE_DEBUG_MESSAGES    
                kdDebug() << "PNG in PNG_COLOR_TYPE_RGB" << endl;
#endif
                m_hasAlpha = false;
                
                if (QImage::systemByteOrder() == QImage::LittleEndian)       // Intel
                    png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_AFTER);
                else                                                         // PPC
                    png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_BEFORE);

                break;

            case PNG_COLOR_TYPE_RGB_ALPHA :	// RGBA 
#ifdef ENABLE_DEBUG_MESSAGES    
                kdDebug() << "PNG in PNG_COLOR_TYPE_RGB_ALPHA" << endl;
#endif
                m_hasAlpha = true;
                break;

            case PNG_COLOR_TYPE_GRAY :		// Grayscale 
#ifdef ENABLE_DEBUG_MESSAGES    
                kdDebug() << "PNG in PNG_COLOR_TYPE_GRAY" << endl;
#endif
                png_set_gray_to_rgb(png_ptr);

                if (QImage::systemByteOrder() == QImage::LittleEndian)       // Intel
                    png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_AFTER);
                else                                                         // PPC
                    png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_BEFORE);

                m_hasAlpha = false;
                break;

            case PNG_COLOR_TYPE_GRAY_ALPHA :	// Grayscale + Alpha 
#ifdef ENABLE_DEBUG_MESSAGES    
                kdDebug() << "PNG in PNG_COLOR_TYPE_GRAY_ALPHA" << endl;
#endif
                png_set_gray_to_rgb(png_ptr);
                m_hasAlpha = true;
                break;

            case PNG_COLOR_TYPE_PALETTE :	// Indexed 
#ifdef ENABLE_DEBUG_MESSAGES    
                kdDebug() << "PNG in PNG_COLOR_TYPE_PALETTE" << endl;
#endif
                png_set_palette_to_rgb(png_ptr);

                if (QImage::systemByteOrder() == QImage::LittleEndian)       // Intel
                    png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_AFTER);
                else                                                         // PPC
                    png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_BEFORE);

                m_hasAlpha = false;
                break;

            default:		
#ifdef ENABLE_DEBUG_MESSAGES    
                kdDebug() << k_funcinfo << "PNG color type unknown." << endl;
#endif
                return false;
        }
    }
    else
    {
#ifdef ENABLE_DEBUG_MESSAGES    
        kdDebug() << k_funcinfo << "PNG in >=8 bits/color/pixel." << endl;
#endif
        m_sixteenBit = false;
        png_set_packing(png_ptr);
            
        switch (color_type)
        {            
            case PNG_COLOR_TYPE_RGB :		// RGB 
#ifdef ENABLE_DEBUG_MESSAGES    
                kdDebug() << "PNG in PNG_COLOR_TYPE_RGB" << endl;
#endif
                if (QImage::systemByteOrder() == QImage::LittleEndian)       // Intel
                    png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);
                else                                                         // PPC
                    png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_BEFORE);

                m_hasAlpha = false;
                break;

            case PNG_COLOR_TYPE_RGB_ALPHA :	// RGBA 
#ifdef ENABLE_DEBUG_MESSAGES    
                kdDebug() << "PNG in PNG_COLOR_TYPE_RGB_ALPHA" << endl;
#endif
                m_hasAlpha = true;
                break;

            case PNG_COLOR_TYPE_GRAY :		// Grayscale 
#ifdef ENABLE_DEBUG_MESSAGES    
                kdDebug() << "PNG in PNG_COLOR_TYPE_GRAY" << endl;
#endif
                png_set_gray_1_2_4_to_8(png_ptr);
                png_set_gray_to_rgb(png_ptr);

                if (QImage::systemByteOrder() == QImage::LittleEndian)       // Intel
                    png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);
                else                                                         // PPC
                    png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_BEFORE);

                m_hasAlpha = false;
                break;

            case PNG_COLOR_TYPE_GRAY_ALPHA :	// Grayscale + alpha 
#ifdef ENABLE_DEBUG_MESSAGES    
                kdDebug() << "PNG in PNG_COLOR_TYPE_GRAY_ALPHA" << endl;
#endif
                png_set_gray_to_rgb(png_ptr);
                m_hasAlpha = true;
                break;

            case PNG_COLOR_TYPE_PALETTE :	// Indexed 
#ifdef ENABLE_DEBUG_MESSAGES    
                kdDebug() << "PNG in PNG_COLOR_TYPE_PALETTE" << endl;
#endif
                png_set_packing(png_ptr);
                png_set_palette_to_rgb(png_ptr);

                if (QImage::systemByteOrder() == QImage::LittleEndian)       // Intel
                    png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);
                else                                                         // PPC
                    png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_BEFORE);

                m_hasAlpha = true;
                break;

            default:		
#ifdef ENABLE_DEBUG_MESSAGES    
                kdDebug() << k_funcinfo << "PNG color type unknown." << endl;
#endif
                return false;
        }
    }

    if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    if (QImage::systemByteOrder() == QImage::LittleEndian)  // Intel 
        png_set_bgr(png_ptr);
    else                                                    // PPC
        png_set_swap_alpha(png_ptr);

    if (observer)
        observer->progressInfo(m_image, 0.1);

    // -------------------------------------------------------------------
    // Get image data.
    
    png_read_update_info(png_ptr, info_ptr);

    uchar *data  = 0;
    
    if (m_sixteenBit)
        data = new uchar[width*height*8];  // 16 bits/color/pixel
    else
        data = new uchar[width*height*4];  // 8 bits/color/pixel

    uchar **lines = 0;
    lines = (uchar **)malloc(height * sizeof(uchar *));
    if (!lines)
    {
        kdDebug() << k_funcinfo << "Cannot allocate memory to load PNG image data." << endl;
        png_read_end(png_ptr, info_ptr);
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(f);
        delete [] data;
        return false;
    }

    for (int i = 0; i < height; i++)
    {
        if (m_sixteenBit)
            lines[i] = data + (i * width * 8);
        else
            lines[i] = data + (i * width * 4);
    }

    // The easy way to read the whole image
    // png_read_image(png_ptr, lines);
    // The other way to read images is row by row. Necessary for observer.
    // Now we need to deal with interlacing.

    // for non-interlaced images number_passes will be 1
    int number_passes = png_set_interlace_handling(png_ptr);
    for (int pass = 0; pass < number_passes; pass++)
    {
        int y;
        int checkPoint = 0;
        for (y = 0; y < height; y++)
        {
            if (observer && y == checkPoint)
            {
                checkPoint += granularity(observer, height, 0.7);
                if (!observer->continueQuery(m_image))
                {
                    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
                    fclose(f);
                    delete [] data;
                    free(lines);
                    return false;
                }
                // use 10% - 80% for progress while reading rows
                observer->progressInfo(m_image, 0.1 + (0.7 * ( ((float)y)/((float)height) )) );
            }

            png_read_rows(png_ptr, lines+y, NULL, 1);

        }
    }

    free(lines);

    // Swap bytes in 16 bits/color/pixel for DImg

    if (m_sixteenBit)
    {
        uchar ptr[8];   // One pixel to swap

        for (int p = 0; p < width*height*8; p+=8)
        {
            memcpy (&ptr[0], &data[p], 8);  // Current pixel

            data[ p ] = ptr[1];  // Blue
            data[p+1] = ptr[0];
            data[p+2] = ptr[3];  // Green
            data[p+3] = ptr[2];
            data[p+4] = ptr[5];  // Red
            data[p+5] = ptr[4];
            data[p+6] = ptr[7];  // Alpha
            data[p+7] = ptr[6];
        }
    }

    if (observer)
        observer->progressInfo(m_image, 0.9);

    // -------------------------------------------------------------------
    // Read image ICC profile
    
    png_charp   profile_name, profile_data=NULL;
    png_uint_32 profile_size;
    int         compression_type;
    
    png_get_iCCP(png_ptr, info_ptr, &profile_name, &compression_type, &profile_data, &profile_size);
    
    if (profile_data != NULL) 
    {
        QByteArray profile_rawdata = imageICCProfil();
        profile_rawdata.resize(profile_size);
        memcpy(profile_rawdata.data(), profile_data, profile_size);
    }
    
    // -------------------------------------------------------------------
    // Get embbeded text data and metadata.
    
    QMap<int, QByteArray>& metaData = imageMetaData();
    metaData.clear();

    png_text* text_ptr;
    int num_comments = png_get_text(png_ptr, info_ptr, &text_ptr, NULL);
    
    /*
    Standard Embedded text includes in PNG :
    
    Title            Short (one line) title or caption for image
    Author           Name of image's creator
    Description      Description of image (possibly long)
    Copyright        Copyright notice
    Creation Time    Time of original image creation
    Software         Software used to create the image
    Disclaimer       Legal disclaimer
    Warning          Warning of nature of content
    Source           Device used to create the image
    Comment          Miscellaneous comment; conversion from GIF comment

    Extra Raw profiles tag are used by ImageMAgick and defines at this URL :
    http://search.cpan.org/src/EXIFTOOL/Image-ExifTool-5.87/html/TagNames/PNG.html#TextualData
    */    
        
    for (int i = 0; i < num_comments; i++)
    {
        // Check if we have a Raw profile embeded using ImageMagick technic.

        if (memcmp(text_ptr[i].key, "Raw profile type exif", 21) != 0 ||
            memcmp(text_ptr[i].key, "Raw profile type APP1", 21) != 0 ||
            memcmp(text_ptr[i].key, "Raw profile type iptc", 21) != 0)
        {
            imageSetEmbbededText(text_ptr[i].key, text_ptr[i].text);
            
#ifdef ENABLE_DEBUG_MESSAGES    
            kdDebug() << "Reading PNG Embedded text: key=" << text_ptr[i].key << " text=" << text_ptr[i].text << endl;
#endif
    
            QString key(text_ptr[i].key);
    
            if (key == "Model")
                imageSetCameraModel(key);
    
            if (key == "Make")
               imageSetCameraConstructor(key);
        }
    }
    
    // -------------------------------------------------------------------
    
    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
    fclose(f);

    if (observer)
        observer->progressInfo(m_image, 1.0);

    imageWidth()  = width;
    imageHeight() = height;
    imageSetAttribute("format", "PNG");
    imageData() = data;
        
    readMetadata(filePath, DImg::PNG);
    
    return true;
}

bool PNGLoader::save(const QString& filePath, DImgLoaderObserver *observer)
{
    FILE          *f;
    png_structp    png_ptr;
    png_infop      info_ptr;
    uchar         *ptr, *data = 0;
    uint           x, y, j;
    png_bytep      row_ptr;
    png_color_8    sig_bit;
    int            quality = 75;
    int            compression = 3;
    
    // -------------------------------------------------------------------
    // Open the file
    
    f = fopen(QFile::encodeName(filePath), "wb");
    if ( !f )
    {
        kdDebug() << k_funcinfo << "Cannot open target image file." << endl;
        return false;
    }


    // -------------------------------------------------------------------
    // Initialize the internal structures
    
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        kdDebug() << k_funcinfo << "Invalid target PNG image file structure." << endl;
        fclose(f);
        return false;
    }
    
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        kdDebug() << k_funcinfo << "Cannot create PNG image file structure." << endl;
        png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
        fclose(f);
        return false;
    }
    
    // -------------------------------------------------------------------
    // PNG error handling. If an error occurs during writing, libpng
    // will jump here
        
    if (setjmp(png_ptr->jmpbuf))
    {
        kdDebug() << k_funcinfo << "Internal libPNG error during writing file. Process aborted!" << endl;
        fclose(f);
        png_destroy_write_struct(&png_ptr, (png_infopp) & info_ptr);
        png_destroy_info_struct(png_ptr, (png_infopp) & info_ptr);
        return false;
    }

    png_init_io(png_ptr, f);
    
    if (QImage::systemByteOrder() == QImage::LittleEndian)  // Intel 
        png_set_bgr(png_ptr);
    else                                                    // PPC
        png_set_swap_alpha(png_ptr);
    
    if (imageHasAlpha())
    {
        png_set_IHDR(png_ptr, info_ptr, imageWidth(), imageHeight(), imageBitsDepth(),
                     PNG_COLOR_TYPE_RGB_ALPHA,  PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        if (imageSixteenBit())
            data = new uchar[imageWidth() * 8 * sizeof(uchar)];
        else
            data = new uchar[imageWidth() * 4 * sizeof(uchar)];        
    }
    else
    {
        png_set_IHDR(png_ptr, info_ptr, imageWidth(), imageHeight(), imageBitsDepth(), 
                     PNG_COLOR_TYPE_RGB,        PNG_INTERLACE_NONE, 
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    
        if (imageSixteenBit())
            data = new uchar[imageWidth() * 6 * sizeof(uchar)];
        else
            data = new uchar[imageWidth() * 3 * sizeof(uchar)];        
    }
    
    sig_bit.red   = imageBitsDepth();
    sig_bit.green = imageBitsDepth();
    sig_bit.blue  = imageBitsDepth();
    sig_bit.alpha = imageBitsDepth();
    png_set_sBIT(png_ptr, info_ptr, &sig_bit);
        
    // -------------------------------------------------------------------
    // Quality to convert to compression 
    
    QVariant qualityAttr = imageGetAttribute("quality");
    quality = qualityAttr.isValid() ? qualityAttr.toInt() : 90;
    
    if (quality < 1)
        quality = 1;
    if (quality > 99)
        quality = 99;

    quality     = quality / 10;
    compression = 9 - quality;
    
    if (compression < 0)
        compression = 0;
    if (compression > 9)
        compression = 9;
    
    png_set_compression_level(png_ptr, compression);
    
    // -------------------------------------------------------------------
    // Write ICC profil.
    
    QByteArray profile_rawdata = imageICCProfil();
    
    if (profile_rawdata.data() != 0)
    {
        png_set_iCCP(png_ptr, info_ptr, "icc", PNG_COMPRESSION_TYPE_BASE, profile_rawdata.data(), profile_rawdata.size());
    }    

    // -------------------------------------------------------------------
    // Write embbeded Text
    
    typedef QMap<QString, QString> EmbeddedTextMap;
    EmbeddedTextMap map = imageEmbeddedText();
    
    for (EmbeddedTextMap::iterator it = map.begin(); it != map.end(); ++it)
    {
        if (it.key() != QString("Software"))
        {
            png_text text;
            text.key  = (char*)it.key().ascii();
            text.text = (char*)it.data().ascii();
#ifdef ENABLE_DEBUG_MESSAGES    
            kdDebug() << "Writing PNG Embedded text: key=" << text.key << " text=" << text.text << endl;
#endif
            text.compression = PNG_TEXT_COMPRESSION_zTXt;
            png_set_text(png_ptr, info_ptr, &(text), 1);
        }
    }

    // Update 'Software' text tag to 'digiKam'.
    QString software("digiKam ");
    software.append(digikam_version);
    png_text text;
    text.key  = "Software";
    text.text = (char *)software.ascii();
#ifdef ENABLE_DEBUG_MESSAGES
    kdDebug() << "Writing PNG Embedded text: key=" << text.key << " text=" << text.text << endl;
#endif
    text.compression = PNG_TEXT_COMPRESSION_zTXt;
    png_set_text(png_ptr, info_ptr, &(text), 1);
    
    // Write embeded Raw profiles metadata in text tag using ImageMagick technic.
    
    typedef QMap<int, QByteArray> MetaDataMap;
    MetaDataMap metaDataMap = imageMetaData();
    
    for (MetaDataMap::iterator it = metaDataMap.begin(); it != metaDataMap.end(); ++it)
    {
        QByteArray ba = it.data();
        
        switch (it.key())
        {
            case(DImg::COM):
            {
                writeRawProfile(png_ptr, info_ptr, "jcom", ba.data(), (png_uint_32) ba.size());
                break;
            }
            case(DImg::EXIF):
            {
                writeRawProfile(png_ptr, info_ptr, "exif", ba.data(), (png_uint_32) ba.size());
                break;
            }
            case(DImg::IPTC):
            {
                writeRawProfile(png_ptr, info_ptr, "iptc", ba.data(), (png_uint_32) ba.size());
                break;
            }
            default:
                break;
        }
    }

    if (observer)
        observer->progressInfo(m_image, 0.2);

    // -------------------------------------------------------------------
    // Write image data

    png_write_info(png_ptr, info_ptr);
    png_set_shift(png_ptr, &sig_bit);
    png_set_packing(png_ptr);
    ptr = imageData();
    
    uint checkPoint = 0;
    for (y = 0; y < imageHeight(); y++)
    {

        if (observer && y == checkPoint)
        {
            checkPoint += granularity(observer, imageHeight(), 0.8);
            if (!observer->continueQuery(m_image))
            {
                delete [] data;
                fclose(f);
                png_destroy_write_struct(&png_ptr, (png_infopp) & info_ptr);
                png_destroy_info_struct(png_ptr, (png_infopp) & info_ptr);
                return false;
            }
            observer->progressInfo(m_image, 0.2 + (0.8 * ( ((float)y)/((float)imageHeight()) )));
        }

        j = 0;
        
        for (x = 0; x < imageWidth()*imageBytesDepth(); x+=imageBytesDepth())
        {
            if (imageSixteenBit())
            {
                if (imageHasAlpha())
                {
                    data[j++] = ptr[x+1];  // Blue
                    data[j++] = ptr[ x ];
                    data[j++] = ptr[x+3];  // Green
                    data[j++] = ptr[x+2];  
                    data[j++] = ptr[x+5];  // Red
                    data[j++] = ptr[x+4];
                    data[j++] = ptr[x+7];  // Alpha
                    data[j++] = ptr[x+6];
                }
                else
                {    
                    data[j++] = ptr[x+1];  // Blue
                    data[j++] = ptr[ x ];
                    data[j++] = ptr[x+3];  // Green
                    data[j++] = ptr[x+2];  
                    data[j++] = ptr[x+5];  // Red
                    data[j++] = ptr[x+4];
                }
            }
            else
            {
                if (imageHasAlpha())
                {
                    data[j++] = ptr[ x ];  // Blue
                    data[j++] = ptr[x+1];  // Green
                    data[j++] = ptr[x+2];  // Red
                    data[j++] = ptr[x+3];  // Alpha
                }
                else
                {    
                    data[j++] = ptr[ x ];  // Blue
                    data[j++] = ptr[x+1];  // Green
                    data[j++] = ptr[x+2];  // Red
                }
            }
        }
        
        row_ptr = (png_bytep) data;
        
        png_write_rows(png_ptr, &row_ptr, 1);
        ptr += (imageWidth() * imageBytesDepth());
    }
    
    delete [] data;
        
    // -------------------------------------------------------------------
            
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, (png_infopp) & info_ptr);
    png_destroy_info_struct(png_ptr, (png_infopp) & info_ptr);
    
    fclose(f);

    imageSetAttribute("savedformat", "PNG");

    // Here there is no writeMetadata() call until Exiv2 will support PNG file format.
    
    return true;
}

bool PNGLoader::hasAlpha() const
{
    return m_hasAlpha;    
}

bool PNGLoader::sixteenBit() const
{
    return m_sixteenBit;    
}

uchar* PNGLoader::readRawProfile(png_textp text, png_uint_32 *length, int ii)
{
    uchar          *info = 0;
    
    register long   i;
    
    register uchar *dp;
    
    register        png_charp sp;
    
    png_uint_32     nibbles;
    
    unsigned char unhex[103]={0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
                              0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
                              0,0,0,0,0,0,0,0,0,1, 2,3,4,5,6,7,8,9,0,0,
                              0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
                              0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,10,11,12,
                              13,14,15};
    
    sp = text[ii].text+1;
    
    // Look for newline 

    while (*sp != '\n')
        sp++;
    
    // Look for length 

    while (*sp == '\0' || *sp == ' ' || *sp == '\n')
        sp++;
    
    *length = (png_uint_32) atol(sp);
    
    while (*sp != ' ' && *sp != '\n')
        sp++;
    
    // Allocate space 
    
    if (*length == 0)
    {
        kdDebug() << "Unable To Copy Raw Profile: invalid profile length"  << endl;
        return (false);
    }
    
    info = new uchar[*length];
    
    if (!info)
    {
        kdDebug() << "Unable To Copy Raw Profile: cannot allocate memory"  << endl;
        return (false);
    }
    
    // Copy profile, skipping white space and column 1 "=" signs 

    dp      = info;
    nibbles = *length * 2;
    
    for (i = 0; i < (long) nibbles; i++)
    {
        while (*sp < '0' || (*sp > '9' && *sp < 'a') || *sp > 'f')
        {
            if (*sp == '\0')
            {
                kdDebug() << "Unable To Copy Raw Profile: ran out of data" << endl;
                return (false);
            }
            
            sp++;
        }
    
        if (i%2 == 0)
            *dp = (uchar) (16*unhex[(int) *sp++]);
        else
            (*dp++) += unhex[(int) *sp++];
    }
    
    return info;
}

void PNGLoader::writeRawProfile(png_struct *ping, png_info *ping_info, char *profile_type, 
                                char *profile_data, png_uint_32 length)
{
    png_textp      text;
    
    register long  i;
    
    uchar         *sp;
    
    png_charp      dp;
    
    png_uint_32    allocated_length, description_length;
    
    uchar hex[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    
    kdDebug() << "Writing Raw profile: type=" << profile_type << ", length=" << length << endl;
    
    text               = (png_textp) png_malloc(ping, (png_uint_32) sizeof(png_text));
    description_length = strlen((const char *) profile_type);
    allocated_length   = (png_uint_32) (length*2 + (length >> 5) + 20 + description_length);
    
    text[0].text   = (png_charp) png_malloc(ping,allocated_length);
    text[0].key    = (png_charp) png_malloc(ping, (png_uint_32) 80);
    text[0].key[0] = '\0';
    
    concatenateString(text[0].key, "Raw profile type ", 4096);
    concatenateString(text[0].key, (const char *) profile_type, 62);
    
    sp = (uchar*)profile_data;
    dp = text[0].text;
    *dp++='\n';
    
    copyString(dp, (const char *) profile_type, allocated_length);
    
    dp += description_length;
    *dp++='\n';
    
    formatString(dp, allocated_length-strlen(text[0].text), "%8lu ", length);
    
    dp += 8;
    
    for (i=0; i < (long) length; i++)
    {
        if (i%36 == 0)
            *dp++='\n';

        *(dp++)=(char) hex[((*sp >> 4) & 0x0f)];
        *(dp++)=(char) hex[((*sp++ ) & 0x0f)]; 
    }

    *dp++='\n';
    *dp='\0';
    text[0].text_length = (png_size_t) (dp-text[0].text);
    text[0].compression = -1;

    if (text[0].text_length <= allocated_length)
        png_set_text(ping, ping_info,text, 1);

    png_free(ping, text[0].text);
    png_free(ping, text[0].key);
    png_free(ping, text);
}

size_t PNGLoader::concatenateString(char *destination, const char *source, const size_t length)
{
    register char       *q;
    
    register const char *p;
    
    register size_t      i;
    
    size_t               count;
  
    if ( !destination || !source || length == 0 )
        return 0;

    p = source;
    q = destination;
    i = length;

    while ((i-- != 0) && (*q != '\0'))
        q++;

    count = (size_t) (q-destination);
    i     = length-count;

    if (i == 0)
        return(count+strlen(p));

    while (*p != '\0')
    {
        if (i != 1)
        {
            *q++=(*p);
            i--;
        }
        p++;
    }

    *q='\0';
    
    return(count+(p-source));
}

size_t PNGLoader::copyString(char *destination, const char *source, const size_t length)
{
    register char       *q;
    
    register const char *p;
    
    register size_t      i;
        
    if ( !destination || !source || length == 0 )
        return 0;

    p = source;
    q = destination;
    i = length;

    if ((i != 0) && (--i != 0))
    {
        do
        {
            if ((*q++=(*p++)) == '\0')
                break;
        } 
        while (--i != 0);
    }

    if (i == 0)
    {
        if (length != 0)
            *q='\0';
  
        while (*p++ != '\0');
    }
    
    return((size_t) (p-source-1));
}

long PNGLoader::formatString(char *string, const size_t length, const char *format,...)
{
    long n;
    
    va_list operands;
    
    va_start(operands,format);
    n = (long) formatStringList(string, length, format, operands);
    va_end(operands);
    return(n);
}

long PNGLoader::formatStringList(char *string, const size_t length, const char *format, va_list operands)
{
  int n = vsnprintf(string, length, format, operands);

  if (n < 0)
    string[length-1] = '\0';

  return((long) n);
}

}  // NameSpace Digikam

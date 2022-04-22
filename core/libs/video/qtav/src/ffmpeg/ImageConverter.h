/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *               ImageConverter: Base class for image resizing & color model convertion
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef QTAV_IMAGE_CONVERTER_H
#define QTAV_IMAGE_CONVERTER_H

#include "QtAV_Global.h"
#include "VideoFormat.h"
#include <QVector>

namespace QtAV
{

typedef int ImageConverterId;

class ImageConverterPrivate;

class ImageConverter // export is not needed
{
    DPTR_DECLARE_PRIVATE(ImageConverter)
public:

    enum
    {
        DataAlignment = 16
    };

    ImageConverter();
    virtual ~ImageConverter();

    // the real data starts with DataAlignment (16bit) aligned address

    QByteArray outData() const;

    // return false if i/o format not supported, or size is not valid.

    // TODO: use isSupported(i/o format);

    virtual bool check() const;
    void setInSize(int width, int height);
    void setOutSize(int width, int height);
    void setInFormat(const VideoFormat& format);
    void setInFormat(VideoFormat::PixelFormat format);
    void setInFormat(int format);
    void setOutFormat(const VideoFormat& format);
    void setOutFormat(VideoFormat::PixelFormat format);
    void setOutFormat(int formate);

    // default is full range.

    void setInRange(ColorRange range);
    ColorRange inRange() const;

    // default is full range

    void setOutRange(ColorRange range);
    ColorRange outRange() const;

    /*!
     * brightness, contrast, saturation: -100~100
     * If value changes, setup sws
     */
    void setBrightness(int value);
    int brightness() const;
    void setContrast(int value);
    int contrast() const;
    void setSaturation(int value);
    int saturation() const;
    QVector<quint8*> outPlanes() const;
    QVector<int> outLineSizes() const;
    virtual bool convert(const quint8 *const src[], const int srcStride[]);
    virtual bool convert(const quint8 *const src[], const int srcStride[], quint8 *const dst[], const int dstStride[]) = 0;

public:

    template<class C> static bool Register(ImageConverterId id, const char* name) { return Register(id, create<C>, name);}
    static ImageConverter* create(ImageConverterId id);
    static ImageConverter* create(const char* name);

    /*!
     * \brief next
     * \param id NULL to get the first id address
     * \return address of id or NULL if not found/end
     */
    static ImageConverterId* next(ImageConverterId* id = 0);
    static const char* name(ImageConverterId id);
    static ImageConverterId id(const char* name);

private:

    template<class C> static ImageConverter* create()
    {
        return new C();
    }

    typedef ImageConverter* (*ImageConverterCreator)();
    static bool Register(ImageConverterId id, ImageConverterCreator, const char *name);

protected:

    ImageConverter(ImageConverterPrivate& d);

    // Allocate memory for out data. Called in setOutFormat()

    virtual bool prepareData(); // Allocate memory for out data
    DPTR_DECLARE(ImageConverter)
};

class ImageConverterFFPrivate;

/*!
 * \brief The ImageConverterFF class
 * based on libswscale
 */
class ImageConverterFF Q_DECL_FINAL: public ImageConverter // Q_AV_EXPORT is not needed
{
    DPTR_DECLARE_PRIVATE(ImageConverterFF)

public:

    ImageConverterFF();
    bool check() const Q_DECL_OVERRIDE;

    // FIXME: why match to the pure virtual one if not declare here?

    bool convert(const quint8 *const src[], const int srcStride[]) Q_DECL_OVERRIDE { return ImageConverter::convert(src, srcStride);}
    bool convert(const quint8 *const src[], const int srcStride[], quint8 *const dst[], const int dstStride[]) Q_DECL_OVERRIDE;
};

typedef ImageConverterFF ImageConverterSWS;

//ImageConverter* c = ImageConverter::create(ImageConverterId_FF);

extern ImageConverterId ImageConverterId_FF;

} // namespace QtAV

#endif // QTAV_IMAGE_CONVERTER_H

/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-08-04
 * Description : RAW preview widget.
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_RAW_PREVIEW_H
#define DIGIKAM_RAW_PREVIEW_H

// Qt includes

#include <QImage>
#include <QPixmap>
#include <QResizeEvent>
#include <QUrl>

// Local includes

#include "dimg.h"
#include "graphicsdimgview.h"
#include "loadingdescription.h"

class QPixmap;

using namespace Digikam;

namespace DigikamRawImportNativePlugin
{

class  RawPreview : public GraphicsDImgView
{
    Q_OBJECT

public:

    explicit RawPreview(const QUrl& url, QWidget* const parent);
    ~RawPreview()                    override;

    DImg& demosaicedImage()    const;
    DImg  postProcessedImage() const;

    void setDecodingSettings(const DRawDecoding& settings);
    void setPostProcessedImage(const DImg& image);

    void ICCSettingsChanged();
    void exposureSettingsChanged();

    void cancelLoading();

Q_SIGNALS:

    void signalLoadingStarted();
    void signalLoadingProgress(float);
    void signalLoadingFailed();
    void signalDemosaicedImage();
    void signalPostProcessedImage();

private Q_SLOTS:

    void slotLoadingProgress(const LoadingDescription& description, float progress);
    void slotImageLoaded(const LoadingDescription& description, const DImg& image);

private:

    void   setdemosaicedImg(const DImg& image);
    void   postProcessing(const DRawDecoding& settings);
    int    previewWidth()    const;
    int    previewHeight()   const;
    bool   previewIsNull()   const;
    QImage previewToQImage() const;
    void   resetPreview();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamRawImportNativePlugin

#endif // DIGIKAM_RAW_PREVIEW_H

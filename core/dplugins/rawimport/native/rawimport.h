/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-08-20
 * Description : Raw import tool
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_RAW_IMPORT_DLG_H
#define DIGIKAM_RAW_IMPORT_DLG_H

// Qt includes

#include <QUrl>

// Local includes

#include "editortool.h"
#include "dimg.h"

using namespace Digikam;

namespace DigikamRawImportNativePlugin
{

class DRawDecoderSettings;

class RawImport : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit RawImport(const QUrl& url, QObject* const parent);
    ~RawImport()                                    override;

    DRawDecoding rawDecodingSettings()      const;
    DImg         postProcessedImage()       const;
    bool         hasPostProcessedImage()    const;
    bool         demosaicingSettingsDirty() const;

private:

    void setBusy(bool busy)                         override;
    void preparePreview()                           override;
    void setPreviewImage()                          override;
    void setBackgroundColor(const QColor& bg)       override;
    void ICCSettingsChanged()                       override;
    void exposureSettingsChanged()                  override;

private Q_SLOTS:

    void slotInit() override;

    void slotLoadingStarted();
    void slotDemosaicedImage();
    void slotLoadingFailed();
    void slotLoadingProgress(float);
    void slotScaleChanged()                         override;

    void slotUpdatePreview();
    void slotAbort()                                override;

    void slotOk() override;
    void slotCancel()                               override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamRawImportNativePlugin

#endif // DIGIKAM_RAW_IMPORT_DLG_H

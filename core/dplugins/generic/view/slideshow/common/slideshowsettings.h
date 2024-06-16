/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-02-13
 * Description : slide show settings container.
 *
 * SPDX-FileCopyrightText: 2007-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2019-2020 by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QMap>
#include <QColor>
#include <QString>
#include <QFont>
#include <QFontDatabase>
#include <QUrl>

// Local includes

#include "digikam_export.h"
#include "previewsettings.h"
#include "dinfointerface.h"
#include "dplugingeneric.h"

using namespace Digikam;

namespace DigikamGenericSlideShowPlugin
{

/**
 * This class contain all settings to perform a slide show of a group of pictures
 */
class SlideShowSettings
{

public:

    SlideShowSettings()  = default;
    ~SlideShowSettings() = default;

    void readFromConfig();
    void writeToConfig();

    int  indexOf(const QUrl&) const;
    int  count()              const;

    void suffleImages();

public:

    /// Global Slide Show Settings

    /**
     * Start Slide with current selected item
     */
    bool                         startWithCurrent           = false;

    /**
     * Auto-rotate image accordingly with Exif Rotation tag
     */
    bool                         exifRotate                 = true;

    /**
     * Print picture file name while slide
     */
    bool                         printName                  = true;

    /**
     * Print picture creation date while slide
     */
    bool                         printDate                  = false;

    /**
     * Print camera Aperture and Focal while slide
     */
    bool                         printApertureFocal         = false;

    /**
     * Print camera Make and Model while slide
     */
    bool                         printMakeModel             = false;

    /**
     * Print camera Lens model while slide
     */
    bool                         printLensModel             = false;

    /**
     * Print camera Exposure and Sensitivity while slide
     */
    bool                         printExpoSensitivity       = false;

    /**
     * Print picture comment while slide
     */
    bool                         printComment               = false;

    /**
     * Print image title while slide
     */
    bool                         printTitle                 = false;

    /**
     * Print image captions if no title available while slide
     */
    bool                         printCapIfNoTitle          = false;

    /**
     * Print tag names while slide
     */
    bool                         printTags                  = false;

    /**
     * Print color label and pick label while slide
     */
    bool                         printLabels                = false;

    /**
     * Print rating while slide
     */
    bool                         printRating                = false;

    /**
     * Slide pictures in loop
     */
    bool                         loop                       = false;

    /**
     * Suffle pictures
     */
    bool                         suffle                     = false;

    /**
     * Delay in seconds
     */
    int                          delay                      = 5;

    /**
     * Whether to enable the auto-move feature.
     */
    bool                         autoPlayEnabled            = true;

    /**
     * Screen to use in case of multi-monitor computer.
     */
    int                          slideScreen                = -2;

    /**
     * Show progress indicator
     */
    bool                         showProgressIndicator      = true;

    /**
     * Load images (previews) in full size, not reduced version
     */
    PreviewSettings              previewSettings;

    /**
     * List of pictures URL to slide
     */
    QList<QUrl>                  fileList;

    /**
     * Original list of pictures URL to slide
     */
    QList<QUrl>                  originalFileList;

    /**
     * URL of the first image to show if requested
     */
    QUrl                         imageUrl;

    /**
     * Font for the display of caption text
     */
    QFont                        captionFont                = QFontDatabase::systemFont(QFontDatabase::GeneralFont);

    /**
     * Interface to access to host application data
     */
    DInfoInterface*              iface                      = nullptr;

    /**
     * The plugin instance.
     */
    DPluginGeneric*              plugin                     = nullptr;

private:

    const QString configGroupName                           = QLatin1String("ImageViewer Settings");
    const QString configSlideShowStartCurrentEntry          = QLatin1String("SlideShowStartCurrent");
    const QString configSlideShowDelayEntry                 = QLatin1String("SlideShowDelay");
    const QString configSlideShowLoopEntry                  = QLatin1String("SlideShowLoop");
    const QString configSlideShowSuffleEntry                = QLatin1String("SlideShowSuffle");
    const QString configSlideShowPrintApertureFocalEntry    = QLatin1String("SlideShowPrintApertureFocal");
    const QString configSlideShowPrintCommentEntry          = QLatin1String("SlideShowPrintComment");
    const QString configSlideShowPrintTitleEntry            = QLatin1String("SlideShowPrintTitle");
    const QString configSlideShowPrintCapIfNoTitleEntry     = QLatin1String("SlideShowPrintCapIfNoTitle");
    const QString configSlideShowPrintDateEntry             = QLatin1String("SlideShowPrintDate");
    const QString configSlideShowPrintExpoSensitivityEntry  = QLatin1String("SlideShowPrintExpoSensitivity");
    const QString configSlideShowPrintMakeModelEntry        = QLatin1String("SlideShowPrintMakeModel");
    const QString configSlideShowPrintLensModelEntry        = QLatin1String("SlideShowPrintLensModel");
    const QString configSlideShowPrintNameEntry             = QLatin1String("SlideShowPrintName");
    const QString configSlideShowPrintTagsEntry             = QLatin1String("SlideShowPrintTags");
    const QString configSlideShowPrintLabelsEntry           = QLatin1String("SlideShowPrintLabels");
    const QString configSlideShowPrintRatingEntry           = QLatin1String("SlideShowPrintRating");
    const QString configSlideShowProgressIndicatorEntry     = QLatin1String("SlideShowProgressIndicator");
    const QString configSlideShowCaptionFontEntry           = QLatin1String("SlideShowCaptionFont");
    const QString configSlideScreenEntry                    = QLatin1String("SlideScreen");
};

} // namespace DigikamGenericSlideShowPlugin

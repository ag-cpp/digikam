/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : frame on screen display settings container.
 *
 * SPDX-FileCopyrightText: 2021-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2021      by Quoc Hưng Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FRAME_OSD_SETTINGS_H
#define DIGIKAM_FRAME_OSD_SETTINGS_H

// Qt includes

#include <QList>
#include <QString>
#include <QFont>

// Local includes

#include "digikam_export.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT FrameOsdSettings
{

public:

    explicit FrameOsdSettings();
    ~FrameOsdSettings();

    /**
     * Read and write settings in config file between sessions.
     */
    void  readSettings(KConfigGroup& group);
    void  writeSettings(KConfigGroup& group);

public:

    bool                        printName            = true;  ///< Print picture file name while streaming
    bool                        printDate            = true;  ///< Print picture creation date while streaming
    bool                        printTitle           = false; ///< Print image title while streaming
    bool                        printTags            = false; ///< Print tags title while streaming
    bool                        printCapIfNoTitle    = false; ///< Print image captions if no title available while streaming
    bool                        printComment         = false; ///< Print picture comment while streaming
    bool                        printMakeModel       = false; ///< Print camera Make and Model while streaming
    bool                        printLensModel       = false; ///< Print camera Lens model while streaming
    bool                        printExpoSensitivity = false; ///< Print camera Exposure and Sensitivity while streaming
    bool                        printApertureFocal   = false; ///< Print camera Aperture and Focal while streaming
    bool                        printRating          = false; ///< Print rating while streaming
    QFont                       osdFont;                      ///< Font for the display of osd text
};

} // namespace Digikam

#endif // DIGIKAM_FRAME_OSD_SETTINGS_H

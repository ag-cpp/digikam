/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-12-31
 * Description : Geolocation plugin about dialog
 *
 * SPDX-FileCopyrightText: 2018-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_GEOLOCATION_PLUGIN_ABOUT_DLG_H
#define DIGIKAM_GEOLOCATION_PLUGIN_ABOUT_DLG_H

// Qt includes

#include <QDialog>

// Local includes

#include "PluginInterface.h"
#include "digikam_export.h"

using namespace Marble;

namespace Digikam
{

class DIGIKAM_EXPORT GeoPluginAboutDlg : public QDialog
{
    Q_OBJECT

public:

    explicit GeoPluginAboutDlg(PluginInterface* const tool, QWidget* const parent = nullptr);
    ~GeoPluginAboutDlg() override;

private Q_SLOTS:

    void slotOnlineHandbook();

private:

    Q_DISABLE_COPY(GeoPluginAboutDlg)

private:

    PluginInterface* m_tool = nullptr;
};

} // namespace Digikam

#endif // DIGIKAM_GEOLOCATION_PLUGIN_ABOUT_DLG_H

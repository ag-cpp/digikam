/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-02-03
 * Description : digiKam setup dialog.
 *
 * SPDX-FileCopyrightText: 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2003-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "digikam_config.h"
#include "setuptemplate.h"
#include "template.h"
#include "searchtextbar.h"
#include "dconfigdlg.h"

namespace Digikam
{

class Setup : public DConfigDlg
{
    Q_OBJECT

public:

    enum Page
    {
        LastPageUsed    = -1,

        DatabasePage    = 0,
        CollectionsPage,
        AlbumViewPage,
        ToolTipPage,
        MetadataPage,
        TemplatePage,
        EditorPage,
        ICCPage,
        LightTablePage,

#ifdef HAVE_GEOLOCATION

        GeolocationPage,

#endif

        ImageQualityPage,
        CameraPage,
        PluginsPage,
        MiscellaneousPage,

        SetupPageEnumLast
    };

public:

    /**
     * Show a setup dialog. The specified page will be selected.
     * True is returned if the dialog was closed with Ok.
     */
    static bool execDialog(Page page = LastPageUsed);
    static bool execDialog(QWidget* const parent, Page page = LastPageUsed);

    /**
     * Show a setup dialog. Only the specified page will be available.
     */
    static bool execSinglePage(Page page);
    static bool execSinglePage(QWidget* const parent, Page page);

    static void onlineVersionCheck();

    static bool execTemplateEditor(QWidget* const parent, const Template& t);
    void setTemplate(const Template& t);

    static bool execMetadataFilters(QWidget* const parent, int tab);
    static bool execExifTool(QWidget* const parent);

    static bool execLocalize(QWidget* const parent);

    static bool execImageQualitySorter(QWidget* const parent);

#ifdef HAVE_GEOLOCATION

    static bool execGeolocation(QWidget* const parent, int tab);

#endif

    QSize sizeHint() const  override;

private Q_SLOTS:

    void slotHelp();
    void slotOkClicked();
    void slotCancelClicked();

private:

    explicit Setup(QWidget* const parent = nullptr);
    ~Setup()                override;

    Setup::Page activePageIndex() const;
    void        showPage(Setup::Page page);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam

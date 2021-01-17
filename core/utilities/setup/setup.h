/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-02-03
 * Description : digiKam setup dialog.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2003-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_SETUP_H
#define DIGIKAM_SETUP_H

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

    QSize sizeHint() const  override;

private Q_SLOTS:

    void slotHelp();
    void slotOkClicked();

private:

    explicit Setup(QWidget* const parent = nullptr);
    ~Setup()                override;

    Setup::Page activePageIndex() const;
    void        showPage(Setup::Page page);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SETUP_H

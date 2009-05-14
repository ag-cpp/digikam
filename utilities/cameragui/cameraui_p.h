/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-09-16
 * Description : Camera interface
 *
 * Copyright (C) 2004-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef CAMERAUIPRIVATE_H
#define CAMERAUIPRIVATE_H

// Qt includes

#include <QString>
#include <QDateTime>

// KDE includes

#include <kurl.h>
#include <kselectaction.h>

namespace Digikam
{

class CameraUIPriv
{
public:

    enum DateFormatOptions
    {
        IsoDateFormat=0,
        TextDateFormat,
        LocalDateFormat
    };

    CameraUIPriv()
    {
        busy                      = false;
        closed                    = false;
        fullScreen                = false;
        removeFullScreenButton    = false;
        fullScreenHideToolBar     = false;
        helpMenu                  = 0;
        advBox                    = 0;
        downloadMenu              = 0;
        deleteMenu                = 0;
        imageMenu                 = 0;
        splitter                  = 0;
        rightSideBar              = 0;
        fixDateTimeCheck          = 0;
        autoRotateCheck           = 0;
        autoAlbumDateCheck        = 0;
        autoAlbumExtCheck         = 0;
        controller                = 0;
        view                      = 0;
        renameCustomizer          = 0;
        anim                      = 0;
        dateTimeEdit              = 0;
        setPhotographerId         = 0;
        setCredits                = 0;
        losslessFormat            = 0;
        convertJpegCheck          = 0;
        formatLabel               = 0;
        folderDateLabel           = 0;
        folderDateFormat          = 0;
        fullScreenAction          = 0;
        increaseThumbsAction      = 0;
        decreaseThumbsAction      = 0;
        downloadSelectedAction    = 0;
        downloadDelSelectedAction = 0;
        downloadAllAction         = 0;
        downloadDelAllAction      = 0;
        deleteSelectedAction      = 0;
        deleteAllAction           = 0;
        cameraInfoAction          = 0;
        cameraCaptureAction       = 0;
        statusProgressBar         = 0;
        statusNavigateBar         = 0;
        statusZoomBar             = 0;
        selectAllAction           = 0;
        selectNoneAction          = 0;
        selectInvertAction        = 0;
        selectNewItemsAction      = 0;
        selectLockedItemsAction   = 0;
        uploadAction              = 0;
        lockAction                = 0;
        donateMoneyAction         = 0;
        cameraCancelAction        = 0;
        imageViewAction           = 0;
        albumLibraryFreeSpace     = 0;
        cameraFreeSpace           = 0;
        rawCameraListAction       = 0;
        libsInfoAction            = 0;
        themeMenuAction           = 0;
        contributeAction          = 0;
        showMenuBarAction         = 0;
        renameFileLabel           = 0;
        autoAlbumLabel            = 0;
        onFlyLabel                = 0;
        kdeJob                    = 0;
    }

    bool                          busy;
    bool                          closed;
    bool                          fullScreen;
    bool                          removeFullScreenButton;
    bool                          fullScreenHideToolBar;

    QString                       cameraTitle;

    QStringList                   currentlyDeleting;
    QStringList                   cameraFolderList;
    QSet<QString>                 foldersToScan;

    KMenu                        *downloadMenu;
    KMenu                        *deleteMenu;
    KMenu                        *imageMenu;

    KVBox                        *advBox;

    QCheckBox                    *autoRotateCheck;
    QCheckBox                    *autoAlbumDateCheck;
    QCheckBox                    *autoAlbumExtCheck;
    QCheckBox                    *fixDateTimeCheck;
    QCheckBox                    *setPhotographerId;
    QCheckBox                    *setCredits;
    QCheckBox                    *convertJpegCheck;

    QLabel                       *formatLabel;
    QLabel                       *folderDateLabel;

    KUrl::List                    kdeTodo;

    SidebarSplitter              *splitter;

    KAction                      *cameraCancelAction;
    KAction                      *cameraCaptureAction;
    KAction                      *cameraInfoAction;
    KAction                      *contributeAction;
    KAction                      *decreaseThumbsAction;
    KAction                      *deleteAllAction;
    KAction                      *deleteSelectedAction;
    KAction                      *donateMoneyAction;
    KAction                      *downloadAllAction;
    KAction                      *downloadDelAllAction;
    KAction                      *downloadDelSelectedAction;
    KAction                      *downloadSelectedAction;
    KAction                      *fullScreenAction;
    KAction                      *imageViewAction;
    KAction                      *increaseThumbsAction;
    KAction                      *libsInfoAction;
    KAction                      *lockAction;
    KAction                      *rawCameraListAction;
    KAction                      *selectAllAction;
    KAction                      *selectInvertAction;
    KAction                      *selectLockedItemsAction;
    KAction                      *selectNewItemsAction;
    KAction                      *selectNoneAction;
    KAction                      *uploadAction;
    KToggleAction                *showMenuBarAction;

    KComboBox                    *losslessFormat;
    KComboBox                    *folderDateFormat;

    KSelectAction                *themeMenuAction;

    KUrl                          lastDestURL;

    KHelpMenu                    *helpMenu;

    KDateTimeEdit                *dateTimeEdit;

    KIO::PreviewJob              *kdeJob;

    CameraController             *controller;

    CameraIconView               *view;

    RenameCustomizer             *renameCustomizer;

    DLogoAction                  *anim;

    ImagePropertiesSideBarCamGui *rightSideBar;

    StatusZoomBar                *statusZoomBar;
    StatusProgressBar            *statusProgressBar;
    StatusNavigateBar            *statusNavigateBar;

    FreeSpaceWidget              *albumLibraryFreeSpace;
    FreeSpaceWidget              *cameraFreeSpace;

    DLabelExpander               *renameFileLabel;
    DLabelExpander               *autoAlbumLabel;
    DLabelExpander               *onFlyLabel;
};

}  // namespace Digikam

#endif /* CAMERAUIPRIVATE_H */

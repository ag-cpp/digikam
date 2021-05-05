/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-06-29
 * Description : a tool to export images to Twitter social network
 *
 * Copyright (C) 2018 by Tarek Talaat <tarektalaat93 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_TWITTER_WINDOW_H
#define DIGIKAM_TWITTER_WINDOW_H

// Qt includes

#include <QList>

// Local includes

#include "dinfointerface.h"
#include "wstooldialog.h"

using namespace Digikam;

class QCloseEvent;
class QUrl;

namespace DigikamGenericTwitterPlugin
{

class TwAlbum;

class TwWindow : public WSToolDialog
{
    Q_OBJECT

public:

    explicit TwWindow(DInfoInterface* const iface, QWidget* const parent);
    ~TwWindow()                     override;

    void reactivate();

    void setItemsList(const QList<QUrl>& urls);

private:

    void readSettings();
    void writeSettings();

    void uploadNextPhoto();

    void buttonStateChange(bool state);
    void closeEvent(QCloseEvent*)   override;

private Q_SLOTS:

    void slotImageListChanged();
    void slotUserChangeRequest();
    void slotNewAlbumRequest();
/*
    void slotReloadAlbumsRequest();
*/
    void slotStartTransfer();

    void slotBusy(bool);
    void slotSignalLinkingFailed();
    void slotSignalLinkingSucceeded();
    void slotSetUserName(const QString& msg);
    void slotListAlbumsFailed(const QString& msg);
    void slotListAlbumsDone(const QList<QPair<QString, QString> >& list);
    void slotCreateFolderFailed(const QString& msg);
    void slotCreateFolderSucceeded();
    void slotAddPhotoFailed(const QString& msg);
    void slotAddPhotoSucceeded();
    void slotTransferCancel();

    void slotFinished();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericTwitterPlugin

#endif // DIGIKAM_TWITTER_WINDOW_H

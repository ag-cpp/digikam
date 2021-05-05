/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-17-06
 * Description : a tool to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_FLICKR_WINDOW_H
#define DIGIKAM_FLICKR_WINDOW_H

// Qt includes

#include <QUrl>
#include <QHash>
#include <QList>
#include <QPair>
#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>

// Local includes

#include "wscomboboxintermediate.h"
#include "dinfointerface.h"
#include "wstooldialog.h"

using namespace Digikam;

namespace DigikamGenericFlickrPlugin
{

class FlickrWindow : public WSToolDialog
{
    Q_OBJECT

public:

    explicit FlickrWindow(DInfoInterface* const iface,
                          QWidget* const parent,
                          const QString& serviceName = QLatin1String("Flickr"));
    ~FlickrWindow()                                                   override;

    /**
     * Use this method to (re-)activate the dialog after it has been created
     * to display it. This also loads the currently selected images.
     */
    void reactivate();

    void setItemsList(const QList<QUrl>& urls);

private Q_SLOTS:

    void slotLinkingSucceeded();
    void slotBusy(bool val);
    void slotError(const QString& msg);
    void slotFinished();
    void slotUser1();
    void slotCancelClicked();

    void slotCreateNewPhotoSet();
    void slotUserChangeRequest();
    void slotRemoveAccount();
    void slotPopulatePhotoSetComboBox();
    void slotAddPhotoNext();
    void slotAddPhotoSucceeded(const QString& photoId);
    void slotAddPhotoFailed(const QString& msg);
    void slotAddPhotoSetSucceeded();
    void slotListPhotoSetsFailed(const QString& msg);
    void slotAddPhotoCancelAndClose();
    void slotAuthCancel();
    void slotImageListChanged();
    void slotReloadPhotoSetRequest();

private:

    QString guessSensibleSetName(const QList<QUrl>& urlList)    const;

    void closeEvent(QCloseEvent*)                                     override;
    void readSettings(const QString& uname);
    void writeSettings();

    void setUiInProgressState(bool inProgress);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericFlickrPlugin

#endif // DIGIKAM_FLICKR_WINDOW_H

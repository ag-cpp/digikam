/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-02-02
 * Description : a tool to export items to ImageShack web service
 *
 * Copyright (C) 2012      by Dodon Victor <dodonvictor at gmail dot com>
 * Copyright (C) 2013-2018 by Caulier Gilles <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_IMAGE_SHACK_WINDOW_H
#define DIGIKAM_IMAGE_SHACK_WINDOW_H

// Qt includes

#include <QString>
#include <QWidget>

// Local includes

#include "wstooldialog.h"
#include "ditemslist.h"
#include "dinfointerface.h"

using namespace Digikam;

namespace DigikamGenericImageShackPlugin
{

class ImageShackWindow : public WSToolDialog
{
    Q_OBJECT

public:

    explicit ImageShackWindow(DInfoInterface* const iface, QWidget* const parent);
    ~ImageShackWindow() override;

public:

    DItemsList* getImagesList() const;

Q_SIGNALS:

    void signalBusy(bool val);

private Q_SLOTS:

    void slotImageListChanged();
    void slotStartTransfer();
    void slotBusy(bool val);
    void slotFinished();
    void slotCancelClicked();

    void slotJobInProgress(int step, int, const QString& format);
    void slotLoginDone(int errCode, const QString& errMsg);
    void slotGetGalleriesDone(int errCode, const QString& errMsg);
    void slotGetGalleries();

    void slotAddPhotoDone(int errCode, const QString& errMsg);

    void authenticate();

private:

    void readSettings();
    void saveSettings();

    void authenticationDone(int errCode, const QString& errMsg);

    void uploadNextItem();

    void closeEvent(QCloseEvent* e) override;

private Q_SLOTS:

    void slotChangeRegistrantionCode();
    void slotNewAlbumRequest();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericImageShackPlugin

#endif // DIGIKAM_IMAGE_SHACK_WINDOW_H

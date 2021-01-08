/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : a tool to export images to WikiMedia web service
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Parthasarathy Gopavarapu <gparthasarathy93 at gmail dot com>
 * Copyright (C) 2013      by Peter Potrowl <peter dot potrowl at gmail dot com>
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

#ifndef DIGIKAM_MEDIAWIKI_WINDOW_H
#define DIGIKAM_MEDIAWIKI_WINDOW_H

// Local includes

#include "wstooldialog.h"
#include "dinfointerface.h"

class QCloseEvent;

class KJob;

using namespace Digikam;

namespace DigikamGenericMediaWikiPlugin
{

class MediaWikiWindow : public WSToolDialog
{
    Q_OBJECT

public:

    explicit MediaWikiWindow(DInfoInterface* const iface, QWidget* const parent);
    ~MediaWikiWindow() override;

public:

    void reactivate();
    bool prepareImageForUpload(const QString& imgPath);

private Q_SLOTS:

    void slotFinished();
    void slotProgressCanceled();
    void slotStartTransfer();
    void slotChangeUserClicked();
    void slotDoLogin(const QString& login, const QString& pass, const QString& wikiName, const QUrl& wikiUrl);
    void slotEndUpload();
    int  slotLoginHandle(KJob* loginJob);

private:

    bool eventFilter(QObject* obj, QEvent* event) override;
    void closeEvent(QCloseEvent*)                 override;
    void readSettings();
    void saveSettings();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericMediaWikiPlugin

#endif // DIGIKAM_MEDIAWIKI_WINDOW_H

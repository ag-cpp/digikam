/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-04
 * Description : Online version checker
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ONLINE_VERSION_CHECKER_H
#define DIGIKAM_ONLINE_VERSION_CHECKER_H

// Qt includes

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDateTime>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT OnlineVersionChecker : public QObject
{
    Q_OBJECT

public:

    explicit OnlineVersionChecker(QObject* const parent);
    ~OnlineVersionChecker();

    void checkForNewVersion();
    void cancelCheck();

private:

    QNetworkAccessManager   manager;
    QNetworkReply*          curRequest;

private Q_SLOTS:

    void downloadFinished(QNetworkReply* reply);
    void error(QNetworkReply::NetworkError code);

Q_SIGNALS:

    void signalNewVersionAvailable(const QString& newTag);
    void signalNewVersionCheckError(QNetworkReply::NetworkError error);
};

} // namespace Digikam

#endif // DIGIKAM_ONLINE_VERSION_CHECKER_H

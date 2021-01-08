/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-11-09
 * Description : resize image threads manager.
 *
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_IMAGE_RESIZE_THREAD_H
#define DIGIKAM_IMAGE_RESIZE_THREAD_H

// Qt includes

#include <QString>
#include <QUrl>

// Local includes

#include "actionthreadbase.h"
#include "mailsettings.h"

using namespace Digikam;

namespace DigikamGenericSendByMailPlugin
{

class ImageResizeThread : public ActionThreadBase
{
    Q_OBJECT

public:

    explicit ImageResizeThread(QObject* const parent);
    ~ImageResizeThread() override;

    void resize(MailSettings* const settings);
    void cancel();

Q_SIGNALS:

    void startingResize(const QUrl& orgUrl);
    void finishedResize(const QUrl& orgUrl, const QUrl& emailUrl, int percent);
    void failedResize(const QUrl& orgUrl, const QString& errString, int percent);

private:

    int* m_count;    ///< Although it is private, it's address is passed to Task
};

} // namespace DigikamGenericSendByMailPlugin

#endif // DIGIKAM_IMAGE_RESIZE_THREAD_H

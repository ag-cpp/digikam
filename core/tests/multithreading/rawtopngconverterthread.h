/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-12-28
 * Description : a class to manage Raw to Png conversion using threads
 *
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2014 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
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

#ifndef DIGIKAM_RAW_TO_PNG_CONVERTER_THREAD_H
#define DIGIKAM_RAW_TO_PNG_CONVERTER_THREAD_H

// Qt includes

#include <QUrl>

// Local includes

#include "drawdecodersettings.h"
#include "actionthreadbase.h"

using namespace Digikam;

class RAWToPNGConverterThread : public ActionThreadBase
{
    Q_OBJECT

public:

    explicit RAWToPNGConverterThread(QObject* const parent);
    ~RAWToPNGConverterThread() override;

    void convertRAWtoPNG(const QList<QUrl>& list,
                         const DRawDecoderSettings& settings,
                         int priority = 0);

Q_SIGNALS:

    void starting(const QUrl& url);
    void finished(const QUrl& url);
    void failed(const QUrl& url, const QString& err);
    void progress(const QUrl& url, int percent);

private Q_SLOTS:

    void slotJobDone();
    void slotJobProgress(int);
    void slotJobStarted();
};

#endif // DIGIKAM_RAW_TO_PNG_CONVERTER_THREAD_H

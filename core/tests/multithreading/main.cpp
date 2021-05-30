/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-12-28
 * Description : test for implementation of thread manager api
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

// Qt includes

#include <QString>
#include <QStringList>
#include <QApplication>
#include <QStandardPaths>
#include <QUrl>

// Local includes

#include "digikam_debug.h"
#include "drawfiles.h"
#include "processordlg.h"
#include "dfiledialog.h"

using namespace Digikam;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QList<QUrl> list;

    if (argc <= 1)
    {
        QString filter = QString::fromLatin1("Raw Files") + QString::fromLatin1(" (%1)").arg(Digikam::s_rawFileExtensions());
        qCDebug(DIGIKAM_TESTS_LOG) << filter;

        QStringList files = DFileDialog::getOpenFileNames(nullptr, QString::fromLatin1("Select RAW files to process"),
                                                         QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first(),
                                                         filter);

        foreach (const QString& f, files)
        {
            list.append(QUrl::fromLocalFile(f));
        }
    }
    else
    {
        for (int i = 1 ; i < argc ; ++i)
        {
            list.append(QUrl::fromLocalFile(QString::fromLocal8Bit(argv[i])));
        }
    }

    if (!list.isEmpty())
    {

        ProcessorDlg* const dlg = new ProcessorDlg(list);
        dlg->show();
        app.exec();
    }

    return 0;
}

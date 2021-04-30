/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-29
 * Description : ExifTool configuration panel.
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

#include "exiftoolconfpanel.h"

// Qt includes

#include <QGridLayout>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QStringList>
#include <QGroupBox>
#include <QApplication>
#include <QStyle>
#include <QHeaderView>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "exiftoolbinary.h"
#include "exiftoolparser.h"
#include "dbinarysearch.h"
#include "metaenginesettings.h"

namespace Digikam
{

class Q_DECL_HIDDEN ExifToolConfPanel::Private
{
public:

    explicit Private()
      : exifToolBinWidget(nullptr),
        exifToolFormats  (nullptr)
    {
    }

public:

    DBinarySearch* exifToolBinWidget;
    ExifToolBinary exifToolBin;

    QTreeWidget*   exifToolFormats;
};

// --------------------------------------------------------

ExifToolConfPanel::ExifToolConfPanel(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    QGridLayout* const grid        = new QGridLayout;
    QLabel* const exifToolBinLabel = new QLabel(i18nc("@info",
                                                "ExifTool is an open-source software program for reading, writing, "
                                                "and manipulating multimedia files. It is platform independent "
                                                "available as a command-line application. ExifTool is commonly "
                                                "incorporated into different types of digital workflows and supports "
                                                "many types of metadata including Exif, IPTC, XMP, JFIF, GeoTIFF, ICC Profile, "
                                                "Photoshop IRB, as well as the manufacturer-specific metadata formats of "
                                                "many digital cameras.\n\n"
                                                "Here you can configure location where ExifTool binary is located. "
                                                "Application will try to find this binary automatically if they are "
                                                "already installed on your computer."),
                                                this);
    exifToolBinLabel->setWordWrap(true);

    d->exifToolBinWidget         = new DBinarySearch(this);
    d->exifToolBinWidget->addBinary(d->exifToolBin);

    QGroupBox* const exifToolBox = new QGroupBox(i18n("Supported File Formats"), this);
    d->exifToolFormats           = new QTreeWidget(exifToolBox);
    d->exifToolFormats->setSortingEnabled(true);
    d->exifToolFormats->sortByColumn(0, Qt::AscendingOrder);
    d->exifToolFormats->setSelectionMode(QAbstractItemView::SingleSelection);
    d->exifToolFormats->setAllColumnsShowFocus(true);
    d->exifToolFormats->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->exifToolFormats->setColumnCount(4);
    d->exifToolFormats->setHeaderHidden(false);
    d->exifToolFormats->setHeaderLabels(QStringList() << i18n("Extension") << i18n("Read") << i18n("Write") << i18n("Description"));

    QVBoxLayout* const vlay      = new QVBoxLayout(exifToolBox);
    vlay->addWidget(d->exifToolFormats);

    grid->addWidget(exifToolBinLabel,     0, 0, 1, 2);
    grid->addWidget(d->exifToolBinWidget, 1, 0, 1, 2);
    grid->addWidget(exifToolBox,          2, 0, 1, 2);
    grid->setRowStretch(2, 10);
    setLayout(grid);

    // ---

    connect(d->exifToolBinWidget, SIGNAL(signalBinariesFound(bool)),
            this, SLOT(slotExifToolBinaryFound(bool)));

    foreach (const QString& path, MetaEngineSettings::instance()->settings().defaultExifToolSearchPaths())
    {
        d->exifToolBinWidget->addDirectory(path);
    }

    d->exifToolBinWidget->allBinariesFound();
}

ExifToolConfPanel::~ExifToolConfPanel()
{
    delete d;
}

QString ExifToolConfPanel::exifToolDirectory() const
{
    return d->exifToolBin.directory();
}

void ExifToolConfPanel::setExifToolDirectory(const QString& dir)
{
    d->exifToolBin.setup(dir);
}

void ExifToolConfPanel::slotExifToolBinaryFound(bool found)
{
    if (!found)
    {
        return;
    }

    d->exifToolFormats->clear();
    ExifToolParser parser;
    parser.setExifToolProgram(exifToolDirectory());
    ExifToolParser::ExifToolData parsed;
    QStringList read;
    QStringList write;

    if (parser.readableFormats())
    {
        parsed = parser.currentData();
        read   = parsed.find(QLatin1String("READ_FORMATS")).value()[0].toStringList();
    }

    if (parser.writableFormats())
    {
        parsed = parser.currentData();
        write  = parsed.find(QLatin1String("WRITE_FORMATS")).value()[0].toStringList();
    }

    foreach (const QString& frm, read)
    {
        new QTreeWidgetItem(d->exifToolFormats, QStringList() << frm
                                                              << i18n("yes")
                                                              << (write.contains(frm) ? i18n("yes") : i18n("no"))
                                                              << QString());
    }
}

} // namespace Digikam

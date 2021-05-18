/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-07-18
 * Description : setup Metadata tab.
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SHOW_FOTO_SETUP_METADATA_H
#define SHOW_FOTO_SETUP_METADATA_H

// Qt includes

#include <QScrollArea>

namespace ShowFoto
{

class ShowfotoSetupMetadata : public QScrollArea
{
    Q_OBJECT

public:

    enum MetadataTab
    {
        Behavior = 0,
        ExifViewer,
        MakernotesViewer,
        IptcViewer,
        XmpViewer,
        ExifTool
    };

public:

    explicit ShowfotoSetupMetadata(QWidget* const parent = nullptr);
    ~ShowfotoSetupMetadata() override;

    void applySettings();
    void setActiveTab(MetadataTab tab);

private:

    void readSettings();

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOW_FOTO_SETUP_METADATA_H

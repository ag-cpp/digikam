/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-04-02
 * Description : setup Misc tab.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SHOW_FOTO_SETUP_MISC_H
#define SHOW_FOTO_SETUP_MISC_H

// Qt includes

#include <QScrollArea>

namespace ShowFoto
{

class ShowfotoSetupMisc : public QScrollArea
{
    Q_OBJECT

public:

    enum SortOrder
    {
        SortByDate = 0,
        SortByName,
        SortByFileSize
    };

    enum MiscTab
    {
        Behaviour = 0,
        Appearance,
        System
    };

public:

    explicit ShowfotoSetupMisc(QWidget* const parent = nullptr);
    ~ShowfotoSetupMisc() override;

    bool checkSettings();
    void applySettings();

private:

    void readSettings();

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOW_FOTO_SETUP_MISC_H

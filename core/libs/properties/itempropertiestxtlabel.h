/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-11-05
 * Description : simple text labels to display item
 *               properties meta infos
 *
 * Copyright (C) 2008-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ITEM_PROPERTIES_TXT_LABEL_H
#define DIGIKAM_ITEM_PROPERTIES_TXT_LABEL_H

// Qt includes

#include <QLabel>
#include <QString>
#include <QListWidget>
#include <QTextBrowser>

// Local includes

#include "dexpanderbox.h"

namespace Digikam
{

class DTextLabelName : public DAdjustableLabel
{

public:

    explicit DTextLabelName(const QString& name, QWidget* const parent=nullptr);
    ~DTextLabelName();
};

// -------------------------------------------------------------------

class DTextLabelValue : public DAdjustableLabel
{

public:

    explicit DTextLabelValue(const QString& value, QWidget* const parent=nullptr);
    ~DTextLabelValue();
};

// -------------------------------------------------------------------

class DTextBrowser : public QTextBrowser
{
public:

    explicit DTextBrowser(const QString& text, QWidget* const parent=nullptr);
    ~DTextBrowser();

    void setLinesNumber(int l);
};

// -------------------------------------------------------------------

class DTextList : public QListWidget
{
public:

    explicit DTextList(const QStringList& list, QWidget* const parent=nullptr);
    ~DTextList();

    void setLinesNumber(int l);
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_PROPERTIES_TXT_LABEL_H

/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a tool to export images to Dropbox web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
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

#ifndef DB_WIDGET_H
#define DB_WIDGET_H

// Qt includes

#include <QWidget>

// Local includes

#include "settingswidget.h"

class QLabel;
class QSpinBox;
class QCheckBox;
class QButtonGroup;
class QComboBox;
class QPushButton;

namespace Digikam
{

class DBWidget : public SettingsWidget
{
    Q_OBJECT

public:

    explicit DBWidget(QWidget* const parent, const QString& pluginName);
    ~DBWidget();

    void updateLabels(const QString& name = QString(), const QString& url = QString()) Q_DECL_OVERRIDE;

    friend class DBWindow;
};

} // namespace Digikam

#endif // DB_WIDGET_H

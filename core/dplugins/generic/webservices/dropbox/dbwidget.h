/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a tool to export images to Dropbox web service
 *
 * Copyright (C) 2013      by Pankaj Kumar <me at panks dot me>
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DB_WIDGET_H
#define DIGIKAM_DB_WIDGET_H

// Qt includes

#include <QWidget>

// Local includes

#include "wssettingswidget.h"
#include "dinfointerface.h"

class QLabel;
class QSpinBox;
class QCheckBox;
class QButtonGroup;
class QComboBox;
class QPushButton;

using namespace Digikam;

namespace DigikamGenericDropBoxPlugin
{

class DBWidget : public WSSettingsWidget
{
    Q_OBJECT

public:

    explicit DBWidget(QWidget* const parent,
                      DInfoInterface* const iface,
                      const QString& toolName);
    ~DBWidget() override;

    void updateLabels(const QString& name = QString(), const QString& url = QString()) override;

private:

    friend class DBWindow;
};

} // namespace DigikamGenericDropBoxPlugin

#endif // DIGIKAM_DB_WIDGET_H

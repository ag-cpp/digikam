/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2018-06-29
 * Description : a tool to export images to Onedrive web service
 *
 * Copyright (C) 2013      by Pankaj Kumar <me at panks dot me>
 * Copyright (C) 2013-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_TW_WIDGET_H
#define DIGIKAM_TW_WIDGET_H

// Qt includes

#include <QWidget>

//local includes

#include "wssettingswidget.h"
#include "dinfointerface.h"

namespace Digikam
{

class TwWidget : public WSSettingsWidget
{
    Q_OBJECT

public:

    explicit TwWidget(QWidget* const parent,
                      DInfoInterface* const iface,
                      const QString& toolName);
    ~TwWidget();

    void updateLabels(const QString& name = QString(),
                      const QString& url = QString()) Q_DECL_OVERRIDE;

Q_SIGNALS:

    void reloadAlbums(long long userID);

private Q_SLOTS:

    //void slotReloadAlbumsRequest();

private:

    friend class TwWindow;
};

} // namespace Digikam

#endif // DIGIKAM_TW_WIDGET_H

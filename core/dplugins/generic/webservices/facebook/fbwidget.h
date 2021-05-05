/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-12-26
 * Description : a tool to export items to Facebook web service
 *
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_FB_WIDGET_H
#define DIGIKAM_FB_WIDGET_H

// Qt includes

#include <QWidget>

//local includes

#include "wssettingswidget.h"
#include "dinfointerface.h"

using namespace Digikam;

namespace DigikamGenericFaceBookPlugin
{

class FbWidget : public WSSettingsWidget
{
    Q_OBJECT

public:

    explicit FbWidget(QWidget* const parent,
                      DInfoInterface* const iface,
                      const QString& toolName);
    ~FbWidget();

    void updateLabels(const QString& name = QString(),
                      const QString& url = QString()) override;

Q_SIGNALS:

    void reloadAlbums(long long userID);

private Q_SLOTS:

    void slotReloadAlbumsRequest();

private:

    friend class FbWindow;
};

} // namespace DigikamGenericFaceBookPlugin

#endif // DIGIKAM_FB_WIDGET_H

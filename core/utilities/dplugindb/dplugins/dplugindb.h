/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-17
 * Description : Database manager digiKam plugin definition.
 *
 * Copyright (C) 2021 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#ifndef DIGIKAM_DPLUGIN_DB_H
#define DIGIKAM_DPLUGIN_DB_H

// Local includes

#include "dplugin.h"
#include "dpluginloader.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT DPluginDb : public DPlugin
{
    Q_OBJECT

public:

    /**
     * Constructor with optional parent object
     */
    explicit DPluginDb(QObject* const parent = nullptr);
    ~DPluginDb() override;

public:

    bool hasVisibilityProperty() const override;

    /**
     * Holds whether the plugin can be seen in parent view.
     */
    void setVisible(bool b) override;

    /**
     * Return the amount of tools registered.
     */
    int count() const override;

    /**
     * Return a list of batch tool group categories as strings registered in this plugin.
     */
    QStringList categories() const override;

    /**
     * Return the plugin interface identifier.
     */
    QString ifaceIid() const override { return QLatin1String(DIGIKAM_DPLUGIN_DB_IID); };

Q_SIGNALS:

    void signalVisible(bool);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

Q_DECLARE_INTERFACE(Digikam::DPluginDb, DIGIKAM_DPLUGIN_DB_IID)

#endif // DIGIKAM_DPLUGIN_DB_H

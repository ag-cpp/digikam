/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a BQM plugin to apply local contrast
 *
 * Copyright (C) 2018-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_LOCAL_CONTRAST_PLUGIN_H
#define DIGIKAM_LOCAL_CONTRAST_PLUGIN_H

// Local includes

#include "dpluginbqm.h"

#define DPLUGIN_IID "org.kde.digikam.plugin.bqm.LocalContrast"

using namespace Digikam;

namespace DigikamBqmLocalContrastPlugin
{

class LocalContrastPlugin : public DPluginBqm
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DPLUGIN_IID)
    Q_INTERFACES(Digikam::DPluginBqm)

public:

    explicit LocalContrastPlugin(QObject* const parent = nullptr);
    ~LocalContrastPlugin()               override;

    QString name()                 const override;
    QString iid()                  const override;
    QIcon   icon()                 const override;
    QString details()              const override;
    QString description()          const override;
    QList<DPluginAuthor> authors() const override;

    void setup(QObject* const)           override;
};

} // namespace DigikamBqmLocalContrastPlugin

#endif // DIGIKAM_LOCAL_CONTRAST_PLUGIN_H

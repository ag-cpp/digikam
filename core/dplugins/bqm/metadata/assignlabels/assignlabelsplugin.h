/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-09-23
 * Description : a BQM plugin to assign labels metadata.
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ASSIGN_LABELS_PLUGIN_H
#define DIGIKAM_ASSIGN_LABELS_PLUGIN_H

// Local includes

#include "dpluginbqm.h"

#define DPLUGIN_IID "org.kde.digikam.plugin.bqm.AssignLabels"

using namespace Digikam;

namespace DigikamBqmAssignLabelsPlugin
{

class AssignLabelsPlugin : public DPluginBqm
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DPLUGIN_IID)
    Q_INTERFACES(Digikam::DPluginBqm)

public:

    explicit AssignLabelsPlugin(QObject* const parent = nullptr);
    ~AssignLabelsPlugin()                override;

    QString name()                 const override;
    QString iid()                  const override;
    QIcon   icon()                 const override;
    QString details()              const override;
    QString description()          const override;
    QList<DPluginAuthor> authors() const override;

    void setup(QObject* const)           override;
};

} // namespace DigikamBqmAssignLabelsPlugin

#endif // DIGIKAM_ASSIGN_LABELS_PLUGIN_H

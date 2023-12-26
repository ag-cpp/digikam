/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a BQM plugin to convert to PGF.
 *
 * SPDX-FileCopyrightText: 2018-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_CONVERT_TO_PGF_PLUGIN_H
#define DIGIKAM_CONVERT_TO_PGF_PLUGIN_H

// Local includes

#include "dpluginbqm.h"

#define DPLUGIN_IID "org.kde.digikam.plugin.bqm.ConvertToPgf"

using namespace Digikam;

namespace DigikamBqmConvertToPgfPlugin
{

class ConvertToPgfPlugin : public DPluginBqm
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DPLUGIN_IID)
    Q_INTERFACES(Digikam::DPluginBqm)

public:

    explicit ConvertToPgfPlugin(QObject* const parent = nullptr);
    ~ConvertToPgfPlugin()                override;

    QString name()                 const override;
    QString iid()                  const override;
    QIcon   icon()                 const override;
    QString details()              const override;
    QString description()          const override;
    QList<DPluginAuthor> authors() const override;
    QString handbookSection()      const override;
    QString handbookChapter()      const override;
    QString handbookReference()    const override;

    void setup(QObject* const)           override;
};

} // namespace DigikamBqmConvertToPgfPlugin

#endif // DIGIKAM_CONVERT_TO_PGF_PLUGIN_H

/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-22
 * Description : PNG DImg plugin.
 *
 * Copyright (C) 2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_DIMG_PNG_PLUGIN_H
#define DIGIKAM_DIMG_PNG_PLUGIN_H

// Qt includes

#include <QByteArray>
#include <QWidget>

// Local includes

#include "dplugindimg.h"
#include "dimg.h"
#include "dimgloader.h"

#define DPLUGIN_IID "org.kde.digikam.plugin.dimg.PNG"

using namespace Digikam;

namespace DigikamPNGDImgPlugin
{

class DImgPNGPlugin : public DPluginDImg
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DPLUGIN_IID)
    Q_INTERFACES(Digikam::DPluginDImg)

public:

    explicit DImgPNGPlugin(QObject* const parent = nullptr);
    ~DImgPNGPlugin();

    QString name()                 const override;
    QString iid()                  const override;
    QIcon   icon()                 const override;
    QString details()              const override;
    QString description()          const override;
    QList<DPluginAuthor> authors() const override;

    void setup(QObject* const)           override;

    QString     loaderName()                                                                const;
    QString     typeMimes()                                                                 const;
    bool        canRead(const QString& filePath)                                            const;
    bool        canWrite(const QString& format)                                             const;
    DImgLoader* loader(DImg* const image, const DRawDecoding& rawSettings = DRawDecoding()) const;
};

} // namespace DigikamPNGDImgPlugin

#endif // DIGIKAM_DIMG_PNG_PLUGIN_H

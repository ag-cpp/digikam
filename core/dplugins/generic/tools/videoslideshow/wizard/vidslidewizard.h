/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a tool to generate video slideshow from images.
 *
 * Copyright (C) 2017-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_VIDSLIDE_WIZARD_H
#define DIGIKAM_VIDSLIDE_WIZARD_H

// Qt includes

#include <QList>
#include <QUrl>

// Local includes

#include "dwizarddlg.h"
#include "dinfointerface.h"
#include "vidslidesettings.h"

using namespace Digikam;

namespace DigikamGenericVideoSlideShowPlugin
{

class VidSlideWizard : public DWizardDlg
{
    Q_OBJECT

public:

    explicit VidSlideWizard(QWidget* const parent, DInfoInterface* const iface = nullptr);
    ~VidSlideWizard()                  override;

    bool validateCurrentPage()         override;
    int  nextId()                const override;

    DInfoInterface*   iface()    const;
    VidSlideSettings* settings() const;

    void setItemsList(const QList<QUrl>& urls);

private Q_SLOTS:

    void slotCurrentIdChanged(int);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericVideoSlideShowPlugin

#endif // DIGIKAM_VIDSLIDE_WIZARD_H

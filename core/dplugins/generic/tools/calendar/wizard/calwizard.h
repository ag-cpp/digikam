/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-11-03
 * Description : a tool to create calendar.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006      by Tom Albers <tomalbers at kde dot nl>
 * Copyright (C) 2007-2008 by Orgad Shaneh <orgads at gmail dot com>
 * Copyright (C) 2012      by Angelo Naselli <anaselli at linux dot it>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_CAL_WIZARD_H
#define DIGIKAM_CAL_WIZARD_H

// Qt includes

#include <QMap>

// Local includes

#include "dwizarddlg.h"
#include "dwizardpage.h"
#include "dinfointerface.h"

using namespace Digikam;

namespace DigikamGenericCalendarPlugin
{

class CalWizard : public DWizardDlg
{
    Q_OBJECT

public:

    explicit CalWizard(QWidget* const parent, DInfoInterface* const iface = nullptr);
    ~CalWizard() override;

    DInfoInterface* iface() const;

private:

    void print();

private Q_SLOTS:

    void slotPageSelected(int current);
    void printComplete();
    void updatePage(int page);

private:

    class Private;
    Private* const d;
};

} // Namespace Digikam

#endif // DIGIKAM_CAL_WIZARD_H

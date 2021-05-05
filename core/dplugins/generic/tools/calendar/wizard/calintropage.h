/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-06-27
 * Description : a tool to create calendar.
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

#ifndef DIGIKAM_CAL_INTRO_PAGE_H
#define DIGIKAM_CAL_INTRO_PAGE_H

// Qt includes

#include <QString>

// Local includes

#include "dwizardpage.h"

using namespace Digikam;

namespace DigikamGenericCalendarPlugin
{

class CalIntroPage : public DWizardPage
{
    Q_OBJECT

public:

    explicit CalIntroPage(QWizard* const dialog, const QString& title);
    ~CalIntroPage() override;
};

} // namespace DigikamGenericCalendarPlugin

#endif // DIGIKAM_CAL_INTRO_PAGE_H

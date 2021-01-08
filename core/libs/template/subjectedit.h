/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-07-09
 * Description : Subjects panel.
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_SUBJECT_EDIT_H
#define DIGIKAM_SUBJECT_EDIT_H

// Local includes

#include "subjectwidget.h"

namespace Digikam
{

class SubjectEdit : public SubjectWidget
{
    Q_OBJECT

public:

    explicit SubjectEdit(QWidget* const parent);
    ~SubjectEdit()        override;

private Q_SLOTS:

    void slotRefChanged() override;
};

} // namespace Digikam

#endif // DIGIKAM_SUBJECT_EDIT_H

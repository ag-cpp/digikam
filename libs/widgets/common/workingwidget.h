/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2010-07-27
 * Description : Widget showing a throbber ("working" animation)
 *
 * Copyright (C) 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
 * Copyright (C) 2010-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef WORKINGWIDGET_H
#define WORKINGWIDGET_H

// Qt includes

#include <QLabel>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT WorkingWidget : public QLabel
{
    Q_OBJECT

public:

    explicit WorkingWidget(QWidget* const parent = 0);
    ~WorkingWidget();

Q_SIGNALS:

    void animationStep();

public Q_SLOTS:

    void toggleTimer(bool turnOn = false);

private Q_SLOTS:

    void changeImage();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // WORKINGWIDGET_H

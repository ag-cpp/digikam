/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-09-12
 * Description : Vertical and horizontal layout widget helpers.
 *
 * Copyright (C) 2014-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DLAYOUT_BOX_H
#define DIGIKAM_DLAYOUT_BOX_H

// Qt includes

#include <QWidget>
#include <QFrame>
#include <QSize>
#include <QMargins>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

/**
 * An Horizontal widget to host children widgets
 */
class DIGIKAM_EXPORT DHBox : public QFrame
{
    Q_OBJECT
    Q_DISABLE_COPY(DHBox)

public:

    explicit DHBox(QWidget* const parent = nullptr);
    ~DHBox()                          override;

    void setSpacing(int space);
    void setContentsMargins(const QMargins& margins);
    void setContentsMargins(int left, int top, int right, int bottom);
    void setStretchFactor(QWidget* const widget, int stretch);

    QSize sizeHint()            const override;
    QSize minimumSizeHint()     const override;

protected:

    DHBox(bool vertical, QWidget* const parent);

    void childEvent(QChildEvent* e)   override;
};

// ------------------------------------------------------------------------------------

/**
 * A Vertical widget to host children widgets
 */
class DIGIKAM_EXPORT DVBox : public DHBox
{
    Q_OBJECT
    Q_DISABLE_COPY(DVBox)

  public:

    explicit DVBox(QWidget* const parent = nullptr);
    ~DVBox()                          override;
};

} // namespace Digikam

#endif // DIGIKAM_DLAYOUT_BOX_H

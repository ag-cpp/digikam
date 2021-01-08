/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-08-16
 * Description : a combo box widget re-implemented with a
 *               reset button to switch to a default item
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DCOMBO_BOX_H
#define DIGIKAM_DCOMBO_BOX_H

// Qt includes

#include <QWidget>
#include <QComboBox>
#include <QVariant>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DComboBox : public QWidget
{

    Q_OBJECT

public:

    explicit DComboBox(QWidget* const parent=nullptr);
    ~DComboBox() override;

    void setCurrentIndex(int d);
    int  currentIndex() const;

    void setDefaultIndex(int d);
    int  defaultIndex() const;

    QComboBox* combo()  const;

    void addItem(const QString& t,
                 const QVariant& data = QVariant());
    void insertItem(int index, const QString& t,
                    const QVariant& data = QVariant());

Q_SIGNALS:

    void reset();
    void activated(int);
    void currentIndexChanged(int);

public Q_SLOTS:

    void slotReset();

private Q_SLOTS:

    void slotItemActivated(int);
    void slotCurrentIndexChanged(int);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DCOMBO_BOX_H

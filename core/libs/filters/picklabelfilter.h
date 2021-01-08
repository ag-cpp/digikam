/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-16
 * Description : pick label filter
 *
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_PICK_LABEL_FILTER_H
#define DIGIKAM_PICK_LABEL_FILTER_H

// Qt includes

#include <QList>
#include <QWidget>

// Local includes

#include "picklabelwidget.h"

namespace Digikam
{

class TAlbum;

class PickLabelFilter : public PickLabelWidget
{
    Q_OBJECT

public:

    explicit PickLabelFilter(QWidget* const parent=nullptr);
    ~PickLabelFilter() override;

    QList<TAlbum*> getCheckedPickLabelTags();

    void reset();

Q_SIGNALS:

    void signalPickLabelSelectionChanged(const QList<PickLabel>&);

private Q_SLOTS:

    void slotPickLabelSelectionChanged();
};

} // namespace Digikam

#endif // DIGIKAM_PICK_LABEL_FILTER_H

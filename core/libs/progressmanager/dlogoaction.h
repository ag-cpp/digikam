/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-27-08
 * Description : a tool bar action object to display animated logo
 *
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DLOGO_ACTION_H
#define DIGIKAM_DLOGO_ACTION_H

// Qt includes

#include <QWidgetAction>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DLogoAction : public QWidgetAction
{
    Q_OBJECT

public:

    explicit DLogoAction(QObject* const parent, bool alignOnright=true);
    ~DLogoAction() override;

    void start();
    void stop();
    bool running() const;

protected:

    QWidget* createWidget(QWidget* parent) override;
    void     deleteWidget(QWidget* widget) override;

private Q_SLOTS:

    void slotProgressTimerDone();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DLOGO_ACTION_H

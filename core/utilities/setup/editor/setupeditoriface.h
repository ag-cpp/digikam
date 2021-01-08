/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-08-03
 * Description : setup Image Editor interface.
 *
 * Copyright (C) 2004-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_SETUP_EDITOR_IFACE_H
#define DIGIKAM_SETUP_EDITOR_IFACE_H

// Qt includes

#include <QScrollArea>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT SetupEditorIface : public QScrollArea
{
    Q_OBJECT

public:

    explicit SetupEditorIface(QWidget* const parent = nullptr);
    ~SetupEditorIface() override;

    void applySettings();

private Q_SLOTS:

    void slotThemeBackgroundColor(bool);
    void slotExpoSettingsChanged();
    void slotShowOverExpoHistogramGuide(double);
    void slotShowUnderExpoHistogramGuide(double);

private:

    void readSettings();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SETUP_EDITOR_IFACE_H

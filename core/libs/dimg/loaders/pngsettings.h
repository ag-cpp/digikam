/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-08-02
 * Description : save PNG image options.
 *
 * Copyright (C) 2007-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_PNG_SETTINGS_H
#define DIGIKAM_PNG_SETTINGS_H

// Qt includes

#include <QWidget>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT PNGSettings : public QWidget
{
    Q_OBJECT

public:

    explicit PNGSettings(QWidget* const parent = nullptr);
    ~PNGSettings() override;

    void setCompressionValue(int val);
    int  getCompressionValue() const;

    static int convertCompressionForLibPng(int value);

Q_SIGNALS:

    void signalSettingsChanged();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_PNG_SETTINGS_H

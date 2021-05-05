/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-06
 * Description : save PGF image options.
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_PGF_SETTINGS_H
#define DIGIKAM_PGF_SETTINGS_H

// Qt includes

#include <QWidget>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT PGFSettings : public QWidget
{
    Q_OBJECT

public:

    explicit PGFSettings(QWidget* const parent = nullptr);
    ~PGFSettings() override;

    void setCompressionValue(int val);
    int  getCompressionValue()    const;

    void setLossLessCompression(bool b);
    bool getLossLessCompression() const;

Q_SIGNALS:

    void signalSettingsChanged();

private Q_SLOTS:

    void slotTogglePGFLossLess(bool);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_PGF_SETTINGS_H

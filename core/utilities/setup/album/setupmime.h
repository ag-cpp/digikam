/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-05-03
 * Description : mime types setup tab.
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

#ifndef DIGIKAM_SETUP_MIME_H
#define DIGIKAM_SETUP_MIME_H

// Qt includes

#include <QScrollArea>

namespace Digikam
{

class SetupMime : public QScrollArea
{
    Q_OBJECT

public:

    explicit SetupMime(QWidget* const parent = nullptr);
    ~SetupMime() override;

    void applySettings();
    void readSettings();

private Q_SLOTS:

    void slotShowCurrentImageSettings();
    void slotShowCurrentMovieSettings();
    void slotShowCurrentAudioSettings();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SETUP_MIME_H

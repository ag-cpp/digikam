/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-07-07
 * Description : a tool to export images to Smugmug web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_SMUG_MPFORM_H
#define DIGIKAM_SMUG_MPFORM_H

// Qt includes

#include <QByteArray>
#include <QString>

namespace DigikamGenericSmugPlugin
{

class SmugMPForm
{

public:

    explicit SmugMPForm();
    ~SmugMPForm();

public:

    void finish();
    void reset();

    bool addPair(const QString& name,
                 const QString& value,
                 const QString& type = QLatin1String("text/plain"));

    bool addFile(const QString& name,
                 const QString& path);

    QString    contentType() const;
    QByteArray formData()    const;
    QString    boundary()    const;

private:

    QByteArray m_buffer;
    QByteArray m_boundary;
};

} // namespace DigikamGenericSmugPlugin

#endif // DIGIKAM_SMUG_MPFORM_H

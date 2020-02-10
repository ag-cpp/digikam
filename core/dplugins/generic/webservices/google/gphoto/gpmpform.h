/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-07-07
 * Description : a tool to export items to Google web services
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2020 by Caulier Gilles <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_GP_MPFORM_H
#define DIGIKAM_GP_MPFORM_H

// Qt includes

#include <QByteArray>
#include <QString>

namespace DigikamGenericGoogleServicesPlugin
{

class GPMPForm
{

public:

    explicit GPMPForm();
    ~GPMPForm();

    void finish();
    void reset();

    bool addPair(const QString& name,
                 const QString& value,
                 const QString& contentType = QString());

    bool addFile(const QString& name,
                 const QString& path);

    QString    contentType() const;
    QByteArray formData()    const;
    QString    boundary()    const;

private:

    QByteArray m_buffer;
    QByteArray m_boundary;
};

} // namespace DigikamGenericGoogleServicesPlugin

#endif // DIGIKAM_GP_MPFORM_H

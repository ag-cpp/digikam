/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-07-07
 * Description : a tool to export images to Smugmug web service
 *
 * SPDX-FileCopyrightText: 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * SPDX-FileCopyrightText: 2008-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QByteArray>
#include <QString>

// Local includes

#include "wstoolutils.h"

using namespace Digikam;

namespace DigikamGenericSmugPlugin
{

class SmugMPForm
{

public:

    SmugMPForm()  = default;
    ~SmugMPForm() = default;

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
    QByteArray m_boundary = QByteArray("----------") +
                            WSToolUtils::randomString(42 + 13).toLatin1();
};

} // namespace DigikamGenericSmugPlugin

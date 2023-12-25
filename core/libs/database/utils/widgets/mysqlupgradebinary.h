/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-10-21
 * Description : Autodetects Mysql upgrade program and version
 *
 * SPDX-FileCopyrightText: 2016-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_MYSQL_UPGRADE_BINARY_H
#define DIGIKAM_MYSQL_UPGRADE_BINARY_H

// Local includes

#include "digikam_export.h"
#include "dbinaryiface.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT MysqlUpgradeBinary : public DBinaryIface
{
    Q_OBJECT

public:

    MysqlUpgradeBinary();
    ~MysqlUpgradeBinary() override;

private:

    // Disabled
    explicit MysqlUpgradeBinary(QObject*) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_MYSQL_UPGRADE_BINARY_H

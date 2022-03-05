/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-07-11
 * Description : shared libraries list dialog common to digiKam and Showfoto
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_LIBS_INFO_DLG_H
#define DIGIKAM_LIBS_INFO_DLG_H

// Qt includes

#include <QMap>
#include <QTreeWidgetItem>

// Local includes

#include "digikam_export.h"
#include "infodlg.h"

namespace Digikam
{

class DIGIKAM_EXPORT LibsInfoDlg : public InfoDlg
{
    Q_OBJECT

public:

    explicit LibsInfoDlg(QWidget* const parent);
    ~LibsInfoDlg();

protected:

    QTreeWidgetItem* m_features  = nullptr;
    QTreeWidgetItem* m_libraries = nullptr;

private:

    QString checkTriState(int value) const;
};

} // namespace Digikam

#endif // DIGIKAM_LIBS_INFO_DLG_H

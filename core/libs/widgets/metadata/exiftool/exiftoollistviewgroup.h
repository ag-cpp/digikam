/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-18
 * Description : ExifTool metadata list view group.
 *
 * SPDX-FileCopyrightText: 2021-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_EXIF_TOOL_LIST_VIEW_GROUP_H
#define DIGIKAM_EXIF_TOOL_LIST_VIEW_GROUP_H

// Qt includes

#include <QTreeWidget>
#include <QString>

namespace Digikam
{

class ExifToolListView;

class ExifToolListViewGroup : public QObject,
                              public QTreeWidgetItem
{
    Q_OBJECT

public:

    ExifToolListViewGroup(ExifToolListView* const parent,
                          const QString& group);
    ~ExifToolListViewGroup() override;

private:

    Q_DISABLE_COPY(ExifToolListViewGroup)
};

} // namespace Digikam

#endif // DIGIKAM_EXIF_TOOL_LIST_VIEW_GROUP_H

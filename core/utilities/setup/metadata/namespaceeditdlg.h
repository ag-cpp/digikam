/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-07-03
 * Description : dialog to edit and create digiKam xmp namespaces
 *
 * Copyright (C) 2015 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
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

#ifndef DIGIKAM_NAMESPACE_EDIT_DLG_H
#define DIGIKAM_NAMESPACE_EDIT_DLG_H

// Qt includes

#include <QMap>
#include <QString>
#include <QKeySequence>
#include <QDialog>

// Local includes

#include "dmetadatasettingscontainer.h"

namespace Digikam
{

class NamespaceEditDlg : public QDialog
{
    Q_OBJECT

public:

    explicit NamespaceEditDlg(bool create,
                              NamespaceEntry& entry,
                              QWidget* const parent = nullptr);
    ~NamespaceEditDlg() override;

    void saveData(NamespaceEntry& entry);

    static bool create(QWidget* const parent, NamespaceEntry& entry);
    static bool edit(QWidget* const parent, NamespaceEntry& entry);

public Q_SLOTS:

    void accept()       override;

private Q_SLOTS:

    void slotHelp();

private:

    void setupTagGui(NamespaceEntry& entry);
    void populateFields(NamespaceEntry& entry);
    void setType(NamespaceEntry::NamespaceType type);
    void makeReadOnly();
    bool validifyCheck(QString& errMsg);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_NAMESPACE_EDIT_DLG_H

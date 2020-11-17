/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-12-19
 * Description : Workflow properties dialog.
 *
 * Copyright (C) 2012-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_BQM_WORK_FLOW_DLG_H
#define DIGIKAM_BQM_WORK_FLOW_DLG_H

// Qt includes

#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QDialog>

// Locale includes

#include "workflowmanager.h"

namespace Digikam
{

class WorkflowDlg : public QDialog
{
    Q_OBJECT

public:

    explicit WorkflowDlg(const Workflow& wf, bool create = false);
    ~WorkflowDlg() override;

    QString     title()           const;
    QString     description()     const;

public:

    static bool editProps(Workflow& wf);
    static bool createNew(Workflow& wf);

private Q_SLOTS:

    void slotTitleChanged(const QString& text);
    void slotHelp();

private:

    // Disable
    WorkflowDlg(QWidget*);

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_BQM_WORK_FLOW_DLG_H

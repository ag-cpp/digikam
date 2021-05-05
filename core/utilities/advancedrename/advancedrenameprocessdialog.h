/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-11-14
 * Description : process dialog for renaming files
 *
 * Copyright (C) 2010-2012 by Andi Clemens <andi dot clemens at gmail dot com>
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

#ifndef DIGIKAM_ADVANCED_RENAME_PROCESS_DIALOG_H
#define DIGIKAM_ADVANCED_RENAME_PROCESS_DIALOG_H

// Local includes

#include "advancedrenamedialog.h"
#include "dprogressdlg.h"
#include "digikam_export.h"

class QCloseEvent;
class QPixmap;
class QUrl;

namespace Digikam
{

class LoadingDescription;

class DIGIKAM_GUI_EXPORT AdvancedRenameProcessDialog : public DProgressDlg
{
    Q_OBJECT

public:

    explicit AdvancedRenameProcessDialog(const NewNamesList& list, QWidget* const parent = nullptr);
    ~AdvancedRenameProcessDialog() override;

    QList<QUrl> failedUrls() const;

private:

    void abort();
    void complete();
    void processOne();
    void getNextThumbnail();

protected:

    void closeEvent(QCloseEvent* e) override;

protected Q_SLOTS:

    void slotCancel();
    void slotRenameFinished();
    void slotRenameFailed(const QUrl& url);

private Q_SLOTS:

    void slotRenameImages();
    void slotGotThumbnail(const LoadingDescription& desc, const QPixmap& pix);

private:

    // Disable
    AdvancedRenameProcessDialog(const AdvancedRenameProcessDialog&)            = delete;
    AdvancedRenameProcessDialog& operator=(const AdvancedRenameProcessDialog&) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ADVANCED_RENAME_PROCESS_DIALOG_H

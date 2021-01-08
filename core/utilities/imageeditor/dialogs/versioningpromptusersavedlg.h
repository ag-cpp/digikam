/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-09-16
 * Description : Dialog to prompt users about versioning
 *
 * Copyright (C) 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_VERSIONING_PROMPT_USER_SAVE_DLG_H
#define DIGIKAM_VERSIONING_PROMPT_USER_SAVE_DLG_H

// Qt includes

#include <QDialog>

class QAbstractButton;

namespace Digikam
{

class VersioningPromptUserSaveDialog : public QDialog
{
    Q_OBJECT

public:

    explicit VersioningPromptUserSaveDialog(QWidget* const parent);
    ~VersioningPromptUserSaveDialog() override;

    bool shallSave()    const;
    bool newVersion()   const;
    bool shallDiscard() const;

private Q_SLOTS:

    void slotButtonClicked(QAbstractButton*);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_VERSIONING_PROMPT_USER_SAVE_DLG_H

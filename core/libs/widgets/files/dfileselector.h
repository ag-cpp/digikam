/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * date        : 2014-09-12
 * Description : a file or folder selector widget
 *
 * Copyright (C) 2014-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_DFILE_SELECTOR_H
#define DIGIKAM_DFILE_SELECTOR_H

// Qt includes

#include <QUrl>
#include <QString>
#include <QLineEdit>
#include <QPushButton>

// Local includes

#include "dlayoutbox.h"
#include "dfiledialog.h"
#include "digikam_export.h"

namespace Digikam
{

/**
 * A widget to chosse a single local file or path.
 * Use line edit and file dialog properties to customize operation modes.
 */
class DIGIKAM_EXPORT DFileSelector : public DHBox
{
    Q_OBJECT

public:

    explicit DFileSelector(QWidget* const parent = nullptr);
    ~DFileSelector() override;

    QLineEdit* lineEdit() const;

    void setFileDlgPath(const QString& path);
    QString fileDlgPath() const;

    void setFileDlgMode(QFileDialog::FileMode mode);
    void setFileDlgFilter(const QString& filter);
    void setFileDlgTitle(const QString& title);
    void setFileDlgOptions(QFileDialog::Options opts);

Q_SIGNALS:

    void signalOpenFileDialog();
    void signalUrlSelected(const QUrl&);

private Q_SLOTS:

    void slotBtnClicked();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DFILE_SELECTOR_H

/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-08-29
 * Description : a widget to edit a long text with limited string size.
 *
 * Copyright (C) 2020-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_LIMITED_TEXT_EDIT_H
#define DIGIKAM_LIMITED_TEXT_EDIT_H

// Qt includes

#include <QKeyEvent>
#include <QMimeData>

// Local includes

#include "dtextedit.h"

using namespace Digikam;

namespace DigikamGenericMetadataEditPlugin
{

class LimitedTextEdit : public DPlainTextEdit
{
    Q_OBJECT

public:

    explicit LimitedTextEdit(QWidget* const parent = nullptr);
    ~LimitedTextEdit()                               override;

    void setMaxLength(int length);
    int  maxLength()     const;

    int leftCharacters() const;

protected:

    void insertFromMimeData(const QMimeData* source) override;

    void keyPressEvent(QKeyEvent* e)                 override;

private Q_SLOTS:

    void slotChanged();

private:

    int m_maxLength    = 0;
};

} // namespace DigikamGenericMetadataEditPlugin

#endif // DIGIKAM_LIMITED_TEXT_EDIT_H

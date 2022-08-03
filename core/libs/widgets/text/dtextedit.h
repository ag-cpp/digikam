/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-01
 * Description : a text edit with spell checker capabilities
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DTEXT_EDIT_H
#define DIGIKAM_DTEXT_EDIT_H

// Qt includes

#include <QString>
#include <QTextEdit>
#include <QPlainTextEdit>

// Local includes

#include "digikam_export.h"

namespace Sonnet
{
    class SpellCheckDecorator;
}

namespace Digikam
{

class DIGIKAM_EXPORT DTextEdit : public QTextEdit
{
    Q_OBJECT

public:

    DTextEdit(const QString& contents, QWidget* const parent = nullptr);
    explicit DTextEdit(QWidget* const parent = nullptr);
    ~DTextEdit() override;

    QString text() const;

    void setLinesVisible(unsigned int lines);
    unsigned int linesVisible() const;

private:

    void init();

private:

    Sonnet::SpellCheckDecorator* m_spellChecker = nullptr;
    unsigned int m_lines                        = 2;
};

// ---------------------------------------------------------------------------

class DIGIKAM_EXPORT DPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:

    DPlainTextEdit(const QString& contents, QWidget* const parent = nullptr);
    explicit DPlainTextEdit(QWidget* const parent = nullptr);
    ~DPlainTextEdit() override;

    QString text() const;

    void setLinesVisible(unsigned int lines);
    unsigned int linesVisible() const;

private:

    void init();

private:

    Sonnet::SpellCheckDecorator* m_spellChecker = nullptr;
    unsigned int m_lines                        = 2;
};

} // namespace Digikam

#endif // DIGIKAM_LINE_EDIT_SPELL_CHECKER_H

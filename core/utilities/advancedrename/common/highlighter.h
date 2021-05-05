/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-19
 * Description : syntax highlighter for AdvancedRename utility
 *
 * Copyright (C) 2009-2012 by Andi Clemens <andi dot clemens at gmail dot com>
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

#ifndef DIGIKAM_HIGH_LIGHTER_H
#define DIGIKAM_HIGH_LIGHTER_H

// Qt includes

#include <QSyntaxHighlighter>

class QTextDocument;

namespace Digikam
{

class Parser;

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:

    Highlighter(QTextDocument* const document,
                Parser* const _parser);
    ~Highlighter()                           override;

protected:

    void highlightBlock(const QString& text) override;

private:

    Highlighter(const Highlighter&);
    Highlighter& operator=(const Highlighter&);

    void setupHighlightingGrammar();

private:

    enum PatternType
    {
        OptionPattern = 0,
        ModifierPattern,
        QuotedTextPattern,
        ParameterPattern
    };

    struct HighlightingRule
    {
        PatternType     type;
        QRegExp         pattern;
        QTextCharFormat format;
    };

private:

    QVector<HighlightingRule> highlightingRules;
    HighlightingRule          quotationRule;

    QTextCharFormat           optionFormat;
    QTextCharFormat           parameterFormat;
    QTextCharFormat           modifierFormat;
    QTextCharFormat           quotationFormat;
    QTextCharFormat           errorFormat;

    Parser* const             parser;
};

} // namespace Digikam

#endif // DIGIKAM_HIGH_LIGHTER_H

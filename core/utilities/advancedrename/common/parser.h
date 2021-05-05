/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-08-11
 * Description : the main parser object for the AdvancedRename utility
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

#ifndef DIGIKAM_PARSER_H
#define DIGIKAM_PARSER_H

// Qt includes

#include <QList>
#include <QMap>
#include <QString>

// Local includes

#include "modifier.h"
#include "option.h"

namespace Digikam
{

class Modifier;
class Option;

class Parser
{

public:

    explicit Parser();
    virtual ~Parser();

    void          reset();

    QString       parse(ParseSettings& settings);

    RulesList     options()   const;
    RulesList     modifiers() const;

    bool          tokenAtPosition(ParseSettings& settings, int pos);
    bool          tokenAtPosition(ParseSettings& settings, int pos, int& start, int& length);

    ParseResults  invalidModifiers(ParseSettings& settings);

    /**
     * check if the given parse string is valid
     * @param str the parse string
     * @return true if valid / can be parsed
     */
    static bool parseStringIsValid(const QString& str);

protected:

    void registerOption(Rule* option);
    void unregisterOption(Rule* option);

    void registerModifier(Rule* modifier);
    void unregisterModifier(Rule* modifier);

private:

    ParseResults results(ParseSettings& settings);

    /**
     * Applies modifiers to the given ParseResults.
     * @param   parseString     the parse string to analyze
     * @param   results         the ParseResults object the modifiers should be applied to
     * @return  a ParseResults object with invalid modifiers (modifiers that have a wrong position in the parse string)
     */
    ParseResults applyModifiers(const QString& parseString, ParseResults& results);

private:

    // Disable
    Parser(const Parser&)            = delete;
    Parser& operator=(const Parser&) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_PARSER_H

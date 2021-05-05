/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-09-14
 * Description : a class to build the tooltip for a renameparser and its options
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

#ifndef DIGIKAM_TOOL_TIPCREATOR_H
#define DIGIKAM_TOOL_TIPCREATOR_H

// Qt includes

#include <QString>

// Local includes

#include "rule.h"

namespace Digikam
{

class Parser;

class TooltipCreator
{
public:

    ~TooltipCreator()
    {
    };

    static TooltipCreator& getInstance();

    QString tooltip(Parser* parser);

    QString getInfoIconResourceName();
    QIcon getInfoIcon();

private:

    /// common methods

    QString markOption(const QString& str);
    QString tableStart();
    QString tableStart(int widthPercentage);
    QString tableEnd();

    QString additionalInformation();

    /// parse object related methods

    QString createEntries(const RulesList& data);
    QString createSection(const QString& sectionName, const RulesList& data, bool lastSection = false);
    QString createHeader(const QString& str);

private:

    // Disable
    explicit TooltipCreator()                        = default;
    TooltipCreator(const TooltipCreator&)            = delete;
    TooltipCreator& operator=(const TooltipCreator&) = delete;

};

} // namespace Digikam

#endif // DIGIKAM_TOOL_TIPCREATOR_H

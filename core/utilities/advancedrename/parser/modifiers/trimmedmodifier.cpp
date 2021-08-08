/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-09-14
 * Description : trimmed token modifier
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

#include "trimmedmodifier.h"

// Qt includes

#include <QRegularExpression>

// KDE includes

#include <klocalizedstring.h>

namespace Digikam
{

TrimmedModifier::TrimmedModifier()
    : Modifier(i18n("Trimmed"),
               i18n("Remove leading, trailing and extra whitespace"),
               QLatin1String("edit-cut"))
{
    QString token(QLatin1String("{trim}"));
    addToken(token, description());

    QRegularExpression reg(escapeToken(token));
    reg.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    setRegExp(reg);
}

QString TrimmedModifier::parseOperation(ParseSettings& settings, const QRegularExpressionMatch &match)
{
    return settings.str2Modify.simplified();
}

} // namespace Digikam

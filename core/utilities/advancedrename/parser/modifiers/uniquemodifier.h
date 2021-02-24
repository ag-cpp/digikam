/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-27
 * Description : A modifier for appending a suffix number to a renaming option.
 *               This guarantees a unique string for duplicate values.
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

#ifndef DIGIKAM_UNIQUE_MODIFIER_H
#define DIGIKAM_UNIQUE_MODIFIER_H

// Qt includes

#include <QStringList>

// Local includes

#include "modifier.h"
#include "parseresults.h"

namespace Digikam
{

class UniqueModifier : public Modifier
{
    Q_OBJECT

public:

    explicit UniqueModifier();
    QString  parseOperation(ParseSettings& settings) override;
    void     reset()                                 override;

private:

    // Disable
    explicit UniqueModifier(QObject*) = delete;
    UniqueModifier(const UniqueModifier&);
    UniqueModifier& operator=(const UniqueModifier&);

private:

    QMap<ParseResults::ResultsKey, QStringList> cache;
};

} // namespace Digikam

#endif // DIGIKAM_UNIQUE_MODIFIER_H

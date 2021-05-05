/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-08-08
 * Description : a modifier for deleting duplicate words
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

#ifndef DIGIKAM_REMOVE_DOUBLES_MODIFIER_H
#define DIGIKAM_REMOVE_DOUBLES_MODIFIER_H

// Local includes

#include "modifier.h"
#include "ruledialog.h"

namespace Digikam
{

class RemoveDoublesModifier : public Modifier
{
    Q_OBJECT

public:

    explicit RemoveDoublesModifier();
    QString parseOperation(ParseSettings& settings) override;

private:

    // Disable
    explicit RemoveDoublesModifier(QObject*)                       = delete;
    RemoveDoublesModifier(const RemoveDoublesModifier&)            = delete;
    RemoveDoublesModifier& operator=(const RemoveDoublesModifier&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_REMOVE_DOUBLES_MODIFIER_H

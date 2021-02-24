/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-11
 * Description : a modifier for setting a default value if option parsing failed
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

#ifndef DIGIKAM_DEFAULT_VALUE_MODIFIER_H
#define DIGIKAM_DEFAULT_VALUE_MODIFIER_H

// Local includes

#include "modifier.h"
#include "ruledialog.h"

class QLineEdit;

namespace Digikam
{

class DefaultValueDialog : public RuleDialog
{
    Q_OBJECT

public:

    explicit DefaultValueDialog(Rule* parent);
    ~DefaultValueDialog()                           override;

    QLineEdit* valueInput;

private:

    // Disable
    explicit DefaultValueDialog(QWidget*) = delete;
    DefaultValueDialog(const DefaultValueDialog&);
    DefaultValueDialog& operator=(const DefaultValueDialog&);
};

// --------------------------------------------------------

class DefaultValueModifier : public Modifier
{
    Q_OBJECT

public:

    explicit DefaultValueModifier();
    QString parseOperation(ParseSettings& settings) override;

private Q_SLOTS:

    void slotTokenTriggered(const QString& token)   override;

private:

    // Disable
    explicit DefaultValueModifier(QObject*) = delete;
    DefaultValueModifier(const DefaultValueModifier&);
    DefaultValueModifier& operator=(const DefaultValueModifier&);
};

} // namespace Digikam

#endif // DIGIKAM_DEFAULT_VALUE_MODIFIER_H

/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-05-19
 * Description : an option to provide database information to the parser
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

#ifndef DIGIKAM_DATABASE_OPTION_H
#define DIGIKAM_DATABASE_OPTION_H

// Qt includes

#include <QString>
#include <QMap>

// Local includes

#include "option.h"
#include "ruledialog.h"

class QLineEdit;

namespace Digikam
{
class DbKeysCollection;
class DbKeySelectorView;

class DatabaseOptionDialog : public RuleDialog
{
    Q_OBJECT

public:

    explicit DatabaseOptionDialog(Rule* const parent);
    ~DatabaseOptionDialog() override;

    DbKeySelectorView* dbkeySelectorView;
    QLineEdit*         separatorLineEdit;

private:

    // Disable
    explicit DatabaseOptionDialog(QWidget*) = delete;
    DatabaseOptionDialog(const DatabaseOptionDialog&);
    DatabaseOptionDialog& operator=(const DatabaseOptionDialog&);
};

// --------------------------------------------------------

typedef QMap<QString, DbKeysCollection*> DbOptionKeysMap;

// --------------------------------------------------------

class DatabaseOption : public Option
{
    Q_OBJECT

public:

    explicit DatabaseOption();
    ~DatabaseOption()                               override;

protected:

    QString parseOperation(ParseSettings& settings) override;

private Q_SLOTS:

    void slotTokenTriggered(const QString& token)   override;

private:

    // Disable
    explicit DatabaseOption(QObject*) = delete;
    DatabaseOption(const DatabaseOption&);
    DatabaseOption& operator=(const DatabaseOption&);

    QString parseDatabase(const QString& keyword, ParseSettings& settings);
    void addDbKeysCollection(DbKeysCollection* key);

    void registerKeysCollection();
    void unregisterKeysCollection();

private:

    DbOptionKeysMap m_map;
};

} // namespace Digikam

#endif // DIGIKAM_DATABASE_OPTION_H

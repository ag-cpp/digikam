/*
    SPDX-License-Identifier: LGPL-2.1-or-later

    SPDX-FileCopyrightText: 2013 Ander Pijoan <ander.pijoan@deusto.es>
    SPDX-FileCopyrightText: 2019 John Zaitseff <J.Zaitseff@zap.org.au>
*/

#pragma once

#include "ParsingRunner.h"

namespace Marble
{

class JsonRunner : public ParsingRunner
{
    Q_OBJECT

public:

    explicit JsonRunner(QObject *parent = nullptr);
    ~JsonRunner() override;

    GeoDataDocument* parseFile(const QString &fileName, DocumentRole role, QString& error) override;
};

} // namespace Marble

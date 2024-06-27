// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#pragma once

#include <QObject>
#include "digikam_export.h"

#include "GeoDataDocument.h"

namespace Marble
{

class DIGIKAM_EXPORT ParsingRunner : public QObject
{
    Q_OBJECT

public:

    explicit ParsingRunner( QObject *parent = nullptr );

    /**
      * Start a file parsing.
      * If implemented in a plugin, make sure to include Parsing in the
      * plugin capabilities, otherwise MarbleRunnerManager will ignore the plugin
      */
    virtual GeoDataDocument* parseFile( const QString &fileName, DocumentRole role, QString& error ) = 0;
};

} // namespace Marble

// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#include "GpxRunner.h"

#include <QFile>

#include "GeoDataDocument.h"
#include "GpxParser.h"

#include "digikam_debug.h"

namespace Marble
{

GpxRunner::GpxRunner(QObject *parent) :
    ParsingRunner(parent)
{
}

GpxRunner::~GpxRunner()
{
}

GeoDataDocument *GpxRunner::parseFile(const QString &fileName, DocumentRole role, QString &error)
{
    QFile file( fileName );
    if ( !file.exists() ) {
        error = QStringLiteral("File %1 does not exist").arg(fileName);
        qCDebug(DIGIKAM_MARBLE_LOG) << error;
        return nullptr;
    }

    // Open file in right mode
    file.open( QIODevice::ReadOnly );

    GpxParser parser;

    if ( !parser.read( &file ) ) {
        error = parser.errorString();
        qCDebug(DIGIKAM_MARBLE_LOG) << error;
        return nullptr;
    }
    GeoDocument* document = parser.releaseDocument();
    Q_ASSERT( document );
    GeoDataDocument* doc = static_cast<GeoDataDocument*>( document );
    doc->setDocumentRole( role );
    doc->setFileName( fileName );

    file.close();
    return doc;
}

}

#include "moc_GpxRunner.cpp"

// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlColorStyleTagWriter.h"

#include "GeoDataTypes.h"
#include "GeoDataColorStyle.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

KmlColorStyleTagWriter::KmlColorStyleTagWriter(const QString &elementName)
    : m_elementName( elementName )
{
    // nothing to do
}

bool KmlColorStyleTagWriter::write( const Marble::GeoNode *node, GeoWriter &writer ) const
{
    GeoDataColorStyle const *colorStyle = static_cast<const GeoDataColorStyle*>(node);

    if ( colorStyle->id().isEmpty() &&
         colorStyle->targetId().isEmpty() &&
         colorStyle->color() == defaultColor() &&
         colorStyle->colorMode() == GeoDataColorStyle::Normal &&
         isEmpty( node ) ) {
        return true;
    }

    writer.writeStartElement( m_elementName );

    KmlObjectTagWriter::writeIdentifiers( writer, colorStyle);
    writer.writeOptionalElement( QString::fromUtf8(kml::kmlTag_color), formatColor( colorStyle->color() ), formatColor( defaultColor() ) );
    QString const colorMode = colorStyle->colorMode() == GeoDataColorStyle::Random ? QString::fromUtf8("random") : QString::fromUtf8("normal");
    writer.writeOptionalElement( QString::fromUtf8(kml::kmlTag_colorMode), colorMode, QString::fromUtf8("normal") );

    bool const result = writeMid( node, writer );
    writer.writeEndElement();
    return result;
}

QString KmlColorStyleTagWriter::formatColor( const QColor &color )
{
    QChar const fill = QLatin1Char('0');
    return QString::fromUtf8( "%1%2%3%4" )
                 .arg( color.alpha(), 2, 16, fill )
                 .arg( color.blue(), 2, 16, fill )
                 .arg( color.green(), 2, 16, fill )
                 .arg( color.red(), 2, 16, fill );
}

QColor KmlColorStyleTagWriter::defaultColor() const
{
    return QColor( Qt::white );
}

}

// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlMapTagWriter.h"

#include "GeoSceneTypes.h"
#include "GeoWriter.h"
#include "GeoSceneMap.h"
#include "GeoSceneLayer.h"
#include "DgmlElementDictionary.h"

#include <QColor>

namespace Marble
{

static GeoTagWriterRegistrar s_writerMap( GeoTagWriter::QualifiedName( QString::fromUtf8(GeoSceneTypes::GeoSceneMapType), QString::fromUtf8(dgml::dgmlTag_nameSpace20) ),
                                               new DgmlMapTagWriter() );

bool DgmlMapTagWriter::write(const GeoNode *node, GeoWriter& writer) const
{
    const GeoSceneMap *map = static_cast<const GeoSceneMap*>( node );
    writer.writeStartElement( QString::fromUtf8(dgml::dgmlTag_Map) );
    writer.writeAttribute( QString::fromUtf8("bgcolor"), map->backgroundColor().name() );
    writer.writeAttribute( QString::fromUtf8("labelColor"), map->labelColor().name() );

    writer.writeStartElement( QString::fromUtf8("canvas") );
    writer.writeEndElement();

    if (!map->center().isEmpty()) {
        if (map->center().count() == 2) {
            writer.writeElement(QString::fromUtf8(dgml::dgmlTag_Center),
                                  map->center().at(0).toString() + QString::fromUtf8(",")
                                + map->center().at(1).toString());
        }
        else if (map->center().count() == 4) {
            writer.writeElement(QString::fromUtf8(dgml::dgmlTag_Center),
                                  map->center().at(0).toString() + QString::fromUtf8(",")
                                + map->center().at(1).toString() + QString::fromUtf8(",")
                                + map->center().at(2).toString() + QString::fromUtf8(",")
                                + map->center().at(3).toString());
        }
    }

    writer.writeStartElement( QString::fromUtf8(dgml::dgmlTag_Target));
    writer.writeEndElement();

    for( int i = 0; i < map->layers().count(); ++i )
    {
        writeElement( map->layers().at( i ), writer );
    }

    writer.writeEndElement();
    return true;
}

}

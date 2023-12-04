// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlSettingsTagWriter.h"

#include "GeoSceneTypes.h"
#include "GeoWriter.h"
#include "GeoSceneSettings.h"
#include "GeoSceneProperty.h"
#include "DgmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerSettings( GeoTagWriter::QualifiedName( QString::fromUtf8(GeoSceneTypes::GeoSceneSettingsType), QString::fromUtf8(dgml::dgmlTag_nameSpace20) ),
                                                                            new DgmlSettingsTagWriter() );

bool DgmlSettingsTagWriter::write(const GeoNode *node, GeoWriter& writer) const
{
    const GeoSceneSettings *settings = static_cast<const GeoSceneSettings*>( node );
    writer.writeStartElement( QString::fromUtf8(dgml::dgmlTag_Settings) );
    
    for( int i = 0; i < settings->allProperties().count(); ++i )
    {
        const GeoSceneProperty *property = settings->allProperties().at( i );
        writer.writeStartElement( QString::fromUtf8(dgml::dgmlTag_Property) );
        writer.writeAttribute( QString::fromUtf8("name"), property->name()  );
        writer.writeElement( QString::fromUtf8(dgml::dgmlTag_Value), property->defaultValue() ? QString::fromUtf8("true") : QString::fromUtf8("false") );
        writer.writeElement( QString::fromUtf8(dgml::dgmlTag_Available), property->available() ? QString::fromUtf8("true") : QString::fromUtf8("false") );
        writer.writeEndElement();
    }
    
    writer.writeEndElement();
    return true;
}

}

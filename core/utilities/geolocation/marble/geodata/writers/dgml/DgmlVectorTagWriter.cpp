// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlVectorTagWriter.h"

#include "GeoWriter.h"
#include "GeoSceneVector.h"
#include "GeoSceneTypes.h"
#include "DgmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerVector( GeoTagWriter::QualifiedName( QString::fromUtf8(GeoSceneTypes::GeoSceneVectorType), QString::fromUtf8(dgml::dgmlTag_nameSpace20) ), new DgmlVectorTagWriter() );

bool DgmlVectorTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoSceneVector *vector = static_cast<const GeoSceneVector*>( node );
    writer.writeStartElement( QString::fromUtf8(dgml::dgmlTag_Vector) );
    writer.writeAttribute( "name", vector->name() );
    writer.writeAttribute( "feature", vector->feature() );
    
    writer.writeStartElement( QString::fromUtf8(dgml::dgmlTag_SourceFile) );
    writer.writeAttribute( "format", vector->fileFormat() );
    writer.writeCharacters( vector->sourceFile() );
    writer.writeEndElement();
    
    writer.writeStartElement( QString::fromUtf8(dgml::dgmlTag_Pen) );
    writer.writeAttribute( "color", vector->pen().color().name() );
    writer.writeEndElement();
        
    writer.writeEndElement();
    return true;
}

}

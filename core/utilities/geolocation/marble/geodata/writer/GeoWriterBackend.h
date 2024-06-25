// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

#pragma once

#include "digikam_export.h"

#include <QString>

class QIODevice;

namespace Marble
{

class GeoDataDocument;

class DIGIKAM_EXPORT GeoWriterBackend
{
public:
    virtual ~GeoWriterBackend();

    /**
     * Write the contents of the given document to the device. This method is to be implemented by plugins.
     * @param device An I/O device open for writing
     * @param document A GeoDataDocument with content to write
     * @return True if the content is successfully written to the device, false otherwise
     */
    virtual bool write(QIODevice* device, const GeoDataDocument &document) = 0;
};

/**
 * Helper class for writer backend registration. This class is commonly used through the MARBLE_ADD_WRITER macro
 */
class DIGIKAM_EXPORT GeoWriterBackendRegistrar
{
public:
    GeoWriterBackendRegistrar(GeoWriterBackend* writer, const QString &fileExtension);
    ~GeoWriterBackendRegistrar();

private:
    GeoWriterBackend* m_writer;
    QString m_fileExtension;
};

#ifndef STATIC_BUILD
#define MARBLE_ADD_WRITER(Class, fileExtension) \
    static GeoWriterBackendRegistrar s_##Class##Registrar(new Class, fileExtension);
#else
#define MARBLE_ADD_WRITER(Class, fileExtension)
#endif

}

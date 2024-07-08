/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "GeoWriterBackend.h"

// Local includes

#include "GeoDataDocumentWriter.h"

namespace Marble
{

GeoWriterBackend::~GeoWriterBackend()
{
    // does nothing
}

GeoWriterBackendRegistrar::GeoWriterBackendRegistrar(GeoWriterBackend* writer, const QString& fileExtension) :
    m_writer(writer),
    m_fileExtension(fileExtension)
{
    GeoDataDocumentWriter::registerWriter(writer, fileExtension);
}

GeoWriterBackendRegistrar::~GeoWriterBackendRegistrar()
{
    GeoDataDocumentWriter::unregisterWriter(m_writer, m_fileExtension);
}

} // namespace Marble

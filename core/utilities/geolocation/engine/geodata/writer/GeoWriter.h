/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *               (c) 2007-2022 Marble Team
 *               https://invent.kde.org/education/marble/-/raw/master/data/credits_authors.html
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QXmlStreamWriter>
#include <QVariant>

// Local includes

#include "digikam_export.h"

namespace Marble
{

class GeoNode;

/**
 * @brief Standard Marble way of writing XML
 * This class is intended to be a standardised way of writing XML for marble.
 * It works with the GeoData classes and writes XML based on the type of output
 * format that the writer is currently working with.
 */
class DIGIKAM_EXPORT GeoWriter : public QXmlStreamWriter
{
public:

    GeoWriter();

    /**
     * @brief The main API call to use the XML writer.
     * To use the XML writer you need to provide an IODevice to write the XML to
     * and a QList of GeoDataFeatures which contains the data you wish to write.
     * To define the type of XML document that is to be written you need to set
     * the current Document Type for this GeoWriter. See @see setDocumentType()
     */
    bool write(QIODevice* device, const GeoNode* feature);

    /**
     * @brief Set the current document type.
     * The current Document Type defines which set of handlers are to be used
     * when writing the GeoDocument. This string should correspond with the
     * string used to register the required Tag Writers in @see GeoTagWriter
     */
    void setDocumentType(const QString& documentType);

    /**
     * @brief Convenience method to write \<key\>value\</key\> with key prefixed format
     * @p namespaceUri
     */
    void writeElement(const QString& namespaceUri, const QString& key, const QString& value);

    /**
     * @brief Convenience method to write \<key\>value\</key\>
     *
     **/
    void writeElement(const QString& key, const QString& value);

    /**
     * @brief Convenience method to write \<key\>value\</key\> if value is
     *   not equal to defaultValue. Otherwise, nothing is written.
     *
     **/
    void writeOptionalElement(const QString& key, const QString& value, const QString& defaultValue = QString());

    /**
     * @brief writeOptionalAttribute Convenience method to write k=v attributes
     * if value is not equal to defaultValue
     */
    void writeOptionalAttribute(const QString& key, const QString& value, const QString& defaultValue = QString());

    template<class T>
    void writeOptionalElement(const QString& key, const T& value, const T& defaultValue = T())
    {
        if (value != defaultValue)
        {
            writeElement(key, QVariant::fromValue(value).toString());
        }
    }

private:

    friend class GeoTagWriter;
    friend class GeoDataDocumentWriter;
    bool writeElement(const GeoNode* object);

private:

    QString m_documentType;
};

} // namespace Marble

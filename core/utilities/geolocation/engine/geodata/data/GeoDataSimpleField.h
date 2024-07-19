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

// Local includes

#include "GeoDocument.h"
#include "digikam_export.h"

class QDataStream;
class QString;

namespace Marble
{

class GeoDataSimpleFieldPrivate;

class DIGIKAM_EXPORT GeoDataSimpleField : public GeoNode
{
public:

    GeoDataSimpleField();
    GeoDataSimpleField(const GeoDataSimpleField& other);
    bool operator==(const GeoDataSimpleField& other) const;
    bool operator!=(const GeoDataSimpleField& other) const;
    ~GeoDataSimpleField() override;

    /*
     * @brief Enum for different values of type attribute of SimpleField
     */
    enum SimpleFieldType
    {
        String,
        Int,
        UInt,
        Short,
        UShort,
        Float,
        Double,
        Bool
    };

    /*
     * @brief Return the value of type attribute of simple field
     */
    SimpleFieldType type() const;

    /*
     * @brief Sets the value of type attribute
     * @param type  The of type attribute
     */
    void setType(SimpleFieldType type);

    /*
     * @brief Returns the value of name attribute of simple field
     */
    QString name() const;

    /*
     * @brief Set the value of name attribute of SimpleField
     * @param value  The value to be set as name attribute
     */
    void setName(const QString& value);

    /*
     * @brief Returns the value of displayField child element of SimpleField tag
     */
    QString displayName() const;

    /*
     * @brief Set the value for displayName tag
     * @param displayName  The value to be set for displayName tag
     */
    void setDisplayName(const QString& displayName);

    /*
     * @brief The assignment operator
     * @param rhs  The object to be duplicated
     */
    GeoDataSimpleField& operator=(const GeoDataSimpleField& rhs);

    /*
     * @brief Provides information for downcasting a GeoNode
     */
    const char* nodeType() const override;

    /*
     * @brief Serialize SimpleField to a stream
     * @param stream  The stream
     */
    virtual void pack(QDataStream& stream) const;

    /*
     * @brief Unserialize SimpleField from a stream
     * @param stream  The stream
     */
    virtual void unpack(QDataStream& stream);

private:

    GeoDataSimpleFieldPrivate* const d = nullptr;
};

} // namespace Marble

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

#include <QMetaType>

// Local includes

#include "GeoDocument.h"
#include "Serializable.h"
#include "digikam_export.h"

namespace Marble
{

class GeoDataObjectPrivate;

/**
 * @short A base class for all geodata objects
 *
 * GeoDataObject is the base class for all geodata classes. It is
 * never instantiated by itself, but is always used as part of a
 * derived object.
 *
 * The Geodata objects are all modeled after the Google KML files as
 * defined in
 * https://developers.google.com/kml/documentation/kmlreference.
 *
 * A GeoDataObject contains 2 properties, both corresponding directly
 * to tags in the KML files: the <b>id</b>, which is a unique
 * identifier of the object, and a <b>targetId</b> which is used to
 * reference other objects that have already been loaded.
 *
 * The <b>id</b> property must only be set if the <b>Update</b>
 * mechanism of KML is used, which is currently not supported by
 * Marble.
 */
class DIGIKAM_EXPORT GeoDataObject : public GeoNode,
    public Serializable
{
public:

    GeoDataObject();
    GeoDataObject(const GeoDataObject&);
    GeoDataObject& operator=(const GeoDataObject&);
    ~GeoDataObject() override;

    /// Provides the parent of the object in GeoDataContainers
    const GeoDataObject* parent() const;
    GeoDataObject* parent();

    /// Sets the parent of the object
    void setParent(GeoDataObject* parent);

    /**
     * @brief Get the id of the object.
     */
    QString id() const;
    /**
     * @brief Set the id of the object
     * @param value the new id value
     */
    void setId(const QString& value);

    /**
     * @brief Get the targetId of the object to be replaced
     */
    QString targetId() const;
    /**
     * @brief set a new targetId of this object
     * @param value the new targetId value
     */
    void setTargetId(const QString& value);

    QString resolvePath(const QString& relativePath) const;

    /// Reimplemented from Serializable
    void pack(QDataStream& stream) const override;
    /// Reimplemented from Serializable
    void unpack(QDataStream& steam) override;

private:

    GeoDataObjectPrivate* d = nullptr;

protected:
    /**
     * @brief Compares the value of id and targetId of the two objects
     * @return true if they these values are equal or false otherwise
     */
    virtual bool equals(const GeoDataObject& other) const;
};

/**
 * Returns the given node cast to type T if the node was instantiated as type T; otherwise returns 0.
 * If node is 0 then it will also return 0.
 *
 * @param node pointer to GeoNode object to be casted
 * @return the given node as type T if cast is successful, otherwise 0
 */
template<typename T>
T* geodata_cast(GeoDataObject* node)
{
    if (node == nullptr)
    {
        return nullptr;
    }

    if (typeid(*node) == typeid(T))
    {
        return static_cast<T*>(node);
    }

    return nullptr;
}

/**
 * Returns the given node cast to type const T if the node was instantiated as type T; otherwise returns 0.
 * If node is 0 then it will also return 0.
 *
 * @param node pointer to GeoNode object to be casted
 * @return the given node as type const T if cast is successful, otherwise 0
 */
template<typename T>
const T* geodata_cast(const GeoDataObject* node)
{
    if (node == nullptr)
    {
        return nullptr;
    }

    if (typeid(*node) == typeid(T))
    {
        return static_cast<const T*>(node);
    }

    return nullptr;
}

} // namespace Marble

Q_DECLARE_METATYPE(Marble::GeoDataObject*)

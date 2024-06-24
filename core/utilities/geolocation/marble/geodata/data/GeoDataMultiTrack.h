// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Thibaut Gridel <tgridel@free.fr>

#pragma once

#include "digikam_export.h"

#include "GeoDataGeometry.h"

#include <QVector>

namespace Marble
{

class GeoDataMultiTrackPrivate;
class GeoDataTrack;

/**
 */
class DIGIKAM_EXPORT GeoDataMultiTrack : public GeoDataGeometry
{
public:
    GeoDataMultiTrack();
    explicit GeoDataMultiTrack( const GeoDataGeometry& other );

    ~GeoDataMultiTrack() override;

    const char *nodeType() const override;

    EnumGeometryId geometryId() const override;

    GeoDataGeometry *copy() const override;

    bool operator==( const GeoDataMultiTrack& other ) const;
    bool operator!=( const GeoDataMultiTrack& other ) const;

    const GeoDataLatLonAltBox& latLonAltBox() const override;

    int size() const;
    GeoDataTrack& at( int pos );
    const GeoDataTrack& at( int pos ) const;
    GeoDataTrack& operator[]( int pos );
    const GeoDataTrack& operator[]( int pos ) const;

    GeoDataTrack& first();
    const GeoDataTrack& first() const;
    GeoDataTrack& last();
    const GeoDataTrack& last() const;

    /**
     * @brief  returns the requested child item
     */
    GeoDataTrack* child( int );

    /**
     * @brief  returns the requested child item
     */
    const GeoDataTrack* child( int ) const;

    /**
     * @brief returns the position of an item in the list
     */
    int childPosition( const GeoDataTrack *child ) const;

    /**
    * @brief add an element
    */
    void append( GeoDataTrack *other );

    GeoDataMultiTrack& operator << ( const GeoDataTrack& value );

    QVector<GeoDataTrack*>::Iterator begin();
    QVector<GeoDataTrack*>::Iterator end();
    QVector<GeoDataTrack*>::ConstIterator constBegin() const;
    QVector<GeoDataTrack*>::ConstIterator constEnd() const;
    void clear();
    QVector<GeoDataTrack> vector() const;

    QVector<GeoDataTrack*>::Iterator erase ( QVector<GeoDataTrack*>::Iterator pos );
    QVector<GeoDataTrack*>::Iterator erase ( QVector<GeoDataTrack*>::Iterator begin,
                                                  QVector<GeoDataTrack*>::Iterator end );

    // Serialize the Placemark to @p stream
    void pack( QDataStream& stream ) const override;
    // Unserialize the Placemark from @p stream
    void unpack( QDataStream& stream ) override;

 private:
    Q_DECLARE_PRIVATE(GeoDataMultiTrack)
};

}

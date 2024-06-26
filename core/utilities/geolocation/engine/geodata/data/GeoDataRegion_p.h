// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Torsten Rahn <rahn@kde.org>
//

#pragma once

#include "GeoDataRegion.h"

#include "GeoDataLatLonAltBox.h"
#include "GeoDataTypes.h"
#include "GeoDataLod.h"

namespace Marble
{

class GeoDataRegionPrivate
{
  public:
    GeoDataRegionPrivate()
         : m_parent( nullptr ),
           m_latLonAltBox( nullptr ),
           m_lod( nullptr )
    {
    }

    GeoDataRegionPrivate( const GeoDataRegionPrivate& other )
         : m_parent( other.m_parent )
    {
        if ( other.m_latLonAltBox ) {
            m_latLonAltBox = new GeoDataLatLonAltBox( *other.m_latLonAltBox );
        }
        else {
            m_latLonAltBox = nullptr;
        }

        if ( other.m_lod ) {
            m_lod = new GeoDataLod( *other.m_lod );
        }
        else {
            m_lod = nullptr;
        }
    }


    explicit GeoDataRegionPrivate( GeoDataFeature * feature )
         : m_parent( feature ),
           m_latLonAltBox( nullptr ),
           m_lod( nullptr )
    {
    }

    ~GeoDataRegionPrivate()
    {
        delete m_latLonAltBox;
        delete m_lod;
    }

    GeoDataFeature* m_parent            = nullptr;
    GeoDataLatLonAltBox* m_latLonAltBox = nullptr;
    GeoDataLod* m_lod                   = nullptr;

  private:

    // Preventing usage of operator=
    GeoDataRegionPrivate &operator=( const GeoDataRegionPrivate& ) = delete;
};

} // namespace Marble

// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#pragma once

#include "GeoDataObject.h"
#include "digikam_export.h"

namespace Marble
{

class GeoDataViewVolumePrivate;

/**
 */
class DIGIKAM_EXPORT GeoDataViewVolume : public GeoDataObject
{
public:
    GeoDataViewVolume();

    GeoDataViewVolume( const GeoDataViewVolume &other );

    GeoDataViewVolume& operator=( const GeoDataViewVolume &other );
    bool operator==( const GeoDataViewVolume &other ) const;
    bool operator!=( const GeoDataViewVolume &other ) const;

    ~GeoDataViewVolume() override;

    /** Provides type information for downcasting a GeoNode */
    const char* nodeType() const override;

    qreal leftFov() const;
    void setLeftFov(qreal leftFov);

    qreal rightFov() const;
    void setRightFov(qreal rightFov);

    qreal bottomFov() const;
    void setBottomFov(qreal bottomFov);

    qreal topFov() const;
    void setTopFov(qreal topFov);

    qreal near() const;
    void setNear(qreal near);

private:
    GeoDataViewVolumePrivate* const d = nullptr;
};

} // namespace Marble

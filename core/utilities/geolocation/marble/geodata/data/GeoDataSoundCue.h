// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#pragma once

#include "GeoDataTourPrimitive.h"

#include <QString>

namespace Marble
{

/**
 */
class DIGIKAM_EXPORT GeoDataSoundCue : public GeoDataTourPrimitive
{
public:
    GeoDataSoundCue();
    ~GeoDataSoundCue() override;

    bool operator==( const GeoDataSoundCue &other ) const;
    bool operator!=( const GeoDataSoundCue &other ) const;
    const char *nodeType() const override;

    QString href() const;
    void setHref(const QString &url);

    double delayedStart() const;
    void setDelayedStart(double pause);

private:
    QString m_href;
    double m_delayedStart;
};

} // namespace Marble

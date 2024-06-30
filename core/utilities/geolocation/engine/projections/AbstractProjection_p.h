// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
//

#pragma once

namespace Marble
{

class AbstractProjection;

class AbstractProjectionPrivate
{
public:

    explicit AbstractProjectionPrivate( AbstractProjection * parent );

    virtual ~AbstractProjectionPrivate() {};

    int levelForResolution(qreal resolution) const;

    Q_DECLARE_PUBLIC( AbstractProjection )

public:

    qreal                       m_maxLat;
    qreal                       m_minLat;
    mutable qreal               m_previousResolution;
    mutable qreal               m_level;

    AbstractProjection* const   q_ptr = nullptr;
};

} // namespace Marble

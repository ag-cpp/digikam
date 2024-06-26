// SPDX-FileCopyrightText: 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QtGlobal>

#include "Blending.h"

namespace Marble
{

class SunLocator;

class SunLightBlending: public Blending
{
 public:
    explicit SunLightBlending( const SunLocator * sunLocator );
    ~SunLightBlending() override;
    void blend( QImage * const bottom, TextureTile const * const top ) const override;

    void setLevelZeroLayout( int levelZeroColumns, int levelZeroRows );

 private:

    static int maxDivisor( int maximum, int fullLength );

    const SunLocator * const m_sunLocator = nullptr;
    int m_levelZeroColumns;
    int m_levelZeroRows;
};

} // Namespace Marble

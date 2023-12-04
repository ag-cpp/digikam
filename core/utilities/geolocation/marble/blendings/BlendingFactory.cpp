// SPDX-FileCopyrightText: 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "BlendingFactory.h"

#include <QDebug>

#include "blendings/SunLightBlending.h"
#include "BlendingAlgorithms.h"

namespace Marble
{

void BlendingFactory::setLevelZeroLayout( int levelZeroColumns, int levelZeroRows )
{
    m_sunLightBlending->setLevelZeroLayout( levelZeroColumns, levelZeroRows );
}

Blending const * BlendingFactory::findBlending( QString const & name ) const
{
    if ( name.isEmpty() )
        return nullptr;

    Blending const * const result = m_blendings.value( name, 0 );
    if ( !result )
        qWarning() << "Unknown blending" << name << "specified in .dgml file. Please fix the .dgml file.";
    return result;
}

BlendingFactory::BlendingFactory( const SunLocator *sunLocator )
    : m_sunLightBlending( new SunLightBlending( sunLocator ) )
{
    m_blendings.insert( QString::fromUtf8("OverpaintBlending"), new OverpaintBlending );

    // Neutral blendings
    m_blendings.insert( QString::fromUtf8("AllanonBlending"), new AllanonBlending );
    m_blendings.insert( QString::fromUtf8("ArcusTangentBlending"), new ArcusTangentBlending );
    m_blendings.insert( QString::fromUtf8("GeometricMeanBlending"), new GeometricMeanBlending );
    m_blendings.insert( QString::fromUtf8("LinearLightBlending"), new LinearLightBlending );
    //m_blendings.insert( "NoiseBlending", new NoiseBlending );
    m_blendings.insert( QString::fromUtf8("OverlayBlending"), new OverlayBlending );
    //m_blendings.insert( "ParallelBlending", new ParallelBlending );
    //m_blendings.insert( "TextureBlending", new TextureBlending );

    // Darkening blendings
    m_blendings.insert( QString::fromUtf8("AlphaBlending"), new OverpaintBlending ); // for backwards compatibility
    m_blendings.insert( QString::fromUtf8("ColorBurnBlending"), new ColorBurnBlending );
    m_blendings.insert( QString::fromUtf8("DarkBlending"), new DarkBlending );
    m_blendings.insert( QString::fromUtf8("DarkenBlending"), new DarkenBlending );
    m_blendings.insert( QString::fromUtf8("DivideBlending"), new DivideBlending );
    m_blendings.insert( QString::fromUtf8("GammaDarkBlending"), new GammaDarkBlending );
    m_blendings.insert( QString::fromUtf8("LinearBurnBlending"), new LinearBurnBlending );
    m_blendings.insert( QString::fromUtf8("MultiplyBlending"), new MultiplyBlending );
    m_blendings.insert( QString::fromUtf8("SubtractiveBlending"), new SubtractiveBlending );

    // Lightening blendings
    m_blendings.insert( QString::fromUtf8("AdditiveBlending"), new AdditiveBlending );
    m_blendings.insert( QString::fromUtf8("ColorDodgeBlending"), new ColorDodgeBlending );
    m_blendings.insert( QString::fromUtf8("GammaLightBlending"), new GammaLightBlending );
    m_blendings.insert( QString::fromUtf8("HardLightBlending"), new HardLightBlending );
    m_blendings.insert( QString::fromUtf8("LightBlending"), new LightBlending );
    m_blendings.insert( QString::fromUtf8("LightenBlending"), new LightenBlending );
    m_blendings.insert( QString::fromUtf8("PinLightBlending"), new PinLightBlending );
    m_blendings.insert( QString::fromUtf8("ScreenBlending"), new ScreenBlending );
    m_blendings.insert( QString::fromUtf8("SoftLightBlending"), new SoftLightBlending );
    m_blendings.insert( QString::fromUtf8("VividLightBlending"), new VividLightBlending );

    // Inverter blendings
    //m_blendings.insert( "AdditiveSubtractiveBlending", new AdditiveSubtractiveBlending );
    m_blendings.insert( QString::fromUtf8("BleachBlending"), new BleachBlending );
    m_blendings.insert( QString::fromUtf8("DifferenceBlending"), new DifferenceBlending );
    m_blendings.insert( QString::fromUtf8("EquivalenceBlending"), new EquivalenceBlending );
    m_blendings.insert( QString::fromUtf8("HalfDifferenceBlending"), new HalfDifferenceBlending );

    // Special purpose blendings
    m_blendings.insert( QString::fromUtf8("CloudsBlending"), new CloudsBlending );
    m_blendings.insert( QString::fromUtf8("SunLightBlending"), m_sunLightBlending );
    m_blendings.insert( QString::fromUtf8("GrayscaleBlending"), new GrayscaleBlending );
    m_blendings.insert( QString::fromUtf8("InvertColorBlending"), new InvertColorBlending );
    m_blendings.insert( QString::fromUtf8("InvertHueBlending"), new InvertHueBlending );
}

BlendingFactory::~BlendingFactory()
{
    m_blendings.remove( QString::fromUtf8("SunLightBlending") );
    delete m_sunLightBlending;
    qDeleteAll( m_blendings );
}

}

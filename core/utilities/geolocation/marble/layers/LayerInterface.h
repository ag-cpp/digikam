// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2008 Inge Wallin <inge@lysator.liu.se>
//

#pragma once

#include "digikam_export.h"

#include <QtGlobal>
#include <QStringList>
#include <QString>

namespace Marble {

class RenderState;

class GeoPainter;
class GeoSceneLayer;
class ViewportParams;

class DIGIKAM_EXPORT LayerInterface
{
public:

    /** Destructor */
    virtual ~LayerInterface();

    /**
     * @brief Preferred level in the layer stack for the rendering
     *
     * Gives a preferred level in the existing layer stack where
     * the render() method of this plugin should get executed.
     *
     * Possible Values:
     * "NONE"
     * "STARS"
     * "BEHIND_TARGET"
     * "SURFACE"
     * "HOVERS_ABOVE_SURFACE"
     * "ATMOSPHERE"
     * "ORBIT"
     * "ALWAYS_ON_TOP"
     * "FLOAT_ITEM"
     * "USER_TOOLS"
     */
    virtual QStringList renderPosition() const = 0;

    /**
     * @brief Renders the content provided by the layer on the viewport.
     * @param painter  painter object allowing to paint on the map
     * @param viewport  metadata, such as current projection, screen dimension, etc.
     * @param renderPos  current render position
     * @param layer  deprecated, always zero (NULL)
     * @return @c true  Returns whether the rendering has been successful
     */
    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
       const QString &renderPos, GeoSceneLayer *layer ) = 0;

    /**
      * @brief Returns the z value of the layer (default: 0.0). If two layers are painted
      * at the same render position, the one with the higher z value is painted on top.
      * If both have the same z value, their paint order is undefined.
      */
    virtual qreal zValue() const;

    virtual RenderState renderState() const;

    /**
      * @brief Returns a debug line for perfo/tracing issues
      */
    virtual QString runtimeTrace() const;
};

} // namespace Marble

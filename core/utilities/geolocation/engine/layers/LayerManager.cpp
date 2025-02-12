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

#include "LayerManager.h"

// Qt includes

#include <QElapsedTimer>

// Local includes

#include "AbstractDataPlugin.h"
#include "AbstractDataPluginItem.h"
#include "GeoPainter.h"
#include "RenderPlugin.h"
#include "LayerInterface.h"
#include "RenderState.h"
#include "digikam_debug.h"

namespace Marble
{

class Q_DECL_HIDDEN LayerManager::Private
{
public:

    Private(LayerManager* parent);
    ~Private();

    void updateVisibility(bool visible, const QString& nameId);

public:

    LayerManager* const         q = nullptr;

    QList<RenderPlugin*>        m_renderPlugins;
    QList<AbstractDataPlugin*>  m_dataPlugins;
    QList<LayerInterface*>      m_internalLayers;

    RenderState                 m_renderState;

    bool                        m_showBackground;
    bool                        m_showRuntimeTrace;
};

LayerManager::Private::Private(LayerManager* parent)
    : q(parent),
      m_renderPlugins(),
      m_showBackground(true),
      m_showRuntimeTrace(false)
{
}

LayerManager::Private::~Private()
{
}

void LayerManager::Private::updateVisibility(bool visible, const QString& nameId)
{
    Q_EMIT q->visibilityChanged(nameId, visible);
}

LayerManager::LayerManager(QObject* parent)
    : QObject(parent),
      d(new Private(this))
{
}

LayerManager::~LayerManager()
{
    delete d;
}

bool LayerManager::showBackground() const
{
    return d->m_showBackground;
}

bool LayerManager::showRuntimeTrace() const
{
    return d->m_showRuntimeTrace;
}

void LayerManager::addRenderPlugin(RenderPlugin* renderPlugin)
{
    d->m_renderPlugins.append(renderPlugin);

    QObject::connect(renderPlugin, SIGNAL(settingsChanged(QString)),
                     this, SIGNAL(pluginSettingsChanged()));

    QObject::connect(renderPlugin, SIGNAL(repaintNeeded(QRegion)),
                     this, SIGNAL(repaintNeeded(QRegion)));

    QObject::connect(renderPlugin, SIGNAL(visibilityChanged(bool,QString)),
                     this, SLOT(updateVisibility(bool,QString)));

    // get data plugins

    AbstractDataPlugin* const dataPlugin = qobject_cast<AbstractDataPlugin*>(renderPlugin);

    if (dataPlugin)
    {
        d->m_dataPlugins.append(dataPlugin);
    }
}

QList<AbstractDataPlugin*> LayerManager::dataPlugins() const
{
    return d->m_dataPlugins;
}

QList<AbstractDataPluginItem*> LayerManager::whichItemAt(const QPoint& curpos) const
{
    QList<AbstractDataPluginItem*> itemList;

    for (auto* plugin : d->m_dataPlugins)
    {
        itemList.append(plugin->whichItemAt(curpos));
    }

    return itemList;
}

void LayerManager::renderLayers(GeoPainter* painter, ViewportParams* viewport)
{
    d->m_renderState = RenderState(QStringLiteral("Marble"));
    QElapsedTimer totalTime;
    totalTime.start();

    QStringList renderPositions;

    if (d->m_showBackground)
    {
        renderPositions
                << QStringLiteral("STARS")
                << QStringLiteral("BEHIND_TARGET");
    }

    renderPositions
            << QStringLiteral("SURFACE")
            << QStringLiteral("HOVERS_ABOVE_SURFACE")
            << QStringLiteral("GRATICULE")
            << QStringLiteral("PLACEMARKS")
            << QStringLiteral("ATMOSPHERE")
            << QStringLiteral("ORBIT")
            << QStringLiteral("ALWAYS_ON_TOP")
            << QStringLiteral("FLOAT_ITEM")
            << QStringLiteral("USER_TOOLS");

    QStringList traceList;

    for (const auto& renderPosition : renderPositions)
    {
        QList<LayerInterface*> layers;

        // collect all RenderPlugins of current renderPosition

        for (auto* renderPlugin : d->m_renderPlugins)
        {
            if (renderPlugin && renderPlugin->renderPosition().contains(renderPosition))
            {
                if (renderPlugin->enabled() && renderPlugin->visible())
                {
                    if (!renderPlugin->isInitialized())
                    {
                        renderPlugin->initialize();

                        Q_EMIT renderPluginInitialized(renderPlugin);
                    }

                    layers.push_back(renderPlugin);
                }
            }
        }

        // collect all internal LayerInterfaces of current renderPosition

        for (auto* layer : d->m_internalLayers)
        {
            if (layer && layer->renderPosition().contains(renderPosition))
            {
                layers.push_back(layer);
            }
        }

        // sort them according to their zValue()s

        std::sort(layers.begin(), layers.end(),
                  [](const LayerInterface * const one, const LayerInterface * const two) -> bool
            {
                Q_ASSERT(one&& two);

                return one->zValue() < two->zValue();
            }
        );

        // render the layers of the current renderPosition

        QElapsedTimer timer;

        for (auto* layer : layers)
        {
            timer.start();
            layer->render(painter, viewport, renderPosition, nullptr);
            d->m_renderState.addChild(layer->renderState());
            traceList.append(QString::fromUtf8("%2 ms %3").arg(timer.elapsed(), 3).arg(layer->runtimeTrace()));
        }
    }

    if (d->m_showRuntimeTrace)
    {
        const int totalElapsed = totalTime.elapsed();
        const int fps          = 1000.0 / totalElapsed;
        traceList.append(QString::fromUtf8("Total: %1 ms (%2 fps)").arg(totalElapsed, 3).arg(fps));

        painter->save();
        painter->setBackgroundMode(Qt::OpaqueMode);
        painter->setBackground(Qt::gray);
        painter->setFont(QFont(QStringLiteral("Sans Serif"), 10, QFont::Bold));

        int i                = 0;
        int const top        = 150;
        int const lineHeight = painter->fontMetrics().height();

        for (const auto& text : traceList)
        {
            painter->setPen(Qt::black);
            painter->drawText(QPoint(10, top + 1 + lineHeight * i), text);
            painter->setPen(Qt::white);
            painter->drawText(QPoint(9, top + lineHeight * i), text);
            ++i;
        }

        painter->restore();
    }
}

void LayerManager::setShowBackground(bool show)
{
    d->m_showBackground = show;
}

void LayerManager::setShowRuntimeTrace(bool show)
{
    d->m_showRuntimeTrace = show;
}

void LayerManager::addLayer(LayerInterface* layer)
{
    if (!d->m_internalLayers.contains(layer))
    {
        d->m_internalLayers.push_back(layer);
    }
}

void LayerManager::removeLayer(LayerInterface* layer)
{
    d->m_internalLayers.removeAll(layer);
}

QList<LayerInterface*> LayerManager::internalLayers() const
{
    return d->m_internalLayers;
}

RenderState LayerManager::renderState() const
{
    return d->m_renderState;
}

} // namespace Marble

#include "moc_LayerManager.cpp"

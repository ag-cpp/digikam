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

#include "PopupLayer.h"

// Qt includes

#include <QSizeF>

// Local includes

#include "GeoDataCoordinates.h"
#include "GeoPainter.h"
#include "MarbleWidget.h"
#include "ViewportParams.h"
#include "RenderPlugin.h"
#include "RenderState.h"

namespace Marble
{

class Q_DECL_HIDDEN PopupLayer::Private
{
public:

    Private(MarbleWidget* marbleWidget, PopupLayer* q);

    /**
     * @brief Sets size of the popup item, based on the requested size and viewport size
     * @param viewport required to compute the maximum dimensions
     */
    void setAppropriateSize(const ViewportParams* viewport);

    static QString filterEmptyShortDescription(const QString& description);
    void setupDialogSatellite(const GeoDataPlacemark* index);
    void setupDialogCity(const GeoDataPlacemark* index);
    void setupDialogNation(const GeoDataPlacemark* index);
    void setupDialogGeoPlaces(const GeoDataPlacemark* index);
    void setupDialogSkyPlaces(const GeoDataPlacemark* index);

public:

    MarbleWidget* const m_widget = nullptr;
    QSizeF              m_requestedSize;
    bool                m_hasCrosshairsPlugin;
    bool                m_crosshairsVisible;
};

PopupLayer::Private::Private(MarbleWidget* marbleWidget, PopupLayer* /*q*/)
    : m_widget(marbleWidget),
      m_hasCrosshairsPlugin(false),
      m_crosshairsVisible(false)
{
}

PopupLayer::PopupLayer(MarbleWidget* marbleWidget, QObject* parent)
    : QObject(parent),
      d(new Private(marbleWidget, this))
{
    /*
        for (const RenderPlugin *renderPlugin: d->m_widget->renderPlugins()) {
            if (renderPlugin->nameId() == QLatin1String("crosshairs")) {
                d->m_hasCrosshairsPlugin = true;
                break;
            }
        }

        connect( d->m_popupItem, SIGNAL(repaintNeeded()), this, SIGNAL(repaintNeeded()) );
        connect( d->m_popupItem, SIGNAL(hide()), this, SLOT(hidePopupItem()) );
    */
}

PopupLayer::~PopupLayer()
{
    delete d;
}

QStringList PopupLayer::renderPosition() const
{
    return QStringList(QStringLiteral("ALWAYS_ON_TOP"));
}

bool PopupLayer::render(GeoPainter* /*painter*/, ViewportParams* /*viewport*/,
                        const QString&, GeoSceneLayer*)
{
    /*
        if ( visible() ) {
            d->setAppropriateSize( viewport );
            d->m_popupItem->paintEvent( painter, viewport );
        }
    */
    return false;
}
/*
bool PopupLayer::eventFilter( QObject *object, QEvent *e )
{
    return visible() && d->m_popupItem->eventFilter( object, e );
}

qreal PopupLayer::zValue() const
{
    return 4711.23;
}

RenderState PopupLayer::renderState() const
{
    return RenderState(QStringLiteral("Popup Window"));
}
*/
bool PopupLayer::visible() const
{
    return false; //d->m_popupItem->visible();
}

void PopupLayer::setVisible(bool /*visible*/)
{
    /*
        d->m_popupItem->setVisible( visible );
        if ( !visible ) {
            disconnect( d->m_popupItem, SIGNAL(repaintNeeded()), this, SIGNAL(repaintNeeded()) );
            d->m_popupItem->clearHistory();
            Q_EMIT repaintNeeded();
        }
        else {
            connect( d->m_popupItem, SIGNAL(repaintNeeded()), this, SIGNAL(repaintNeeded()) );
        }
    */
}

void PopupLayer::popup()
{
    /*
        GeoDataCoordinates coords = d->m_popupItem->coordinate();
        ViewportParams viewport( d->m_widget->viewport()->projection(),
                                 coords.longitude(), coords.latitude(), d->m_widget->viewport()->radius(),
                                 d->m_widget->viewport()->size() );
        qreal sx, sy, lon, lat;
        viewport.screenCoordinates( coords, sx, sy );
        sx = viewport.radius() < viewport.width() ? 0.5 * (viewport.width() + viewport.radius()) : 0.75 * viewport.width();
        viewport.geoCoordinates( sx, sy, lon, lat, GeoDataCoordinates::Radian );
        coords.setLatitude( lat );
        coords.setLongitude( lon );
        d->m_widget->centerOn( coords, true );

        if( d->m_hasCrosshairsPlugin ) {
            d->m_crosshairsVisible = d->m_widget->showCrosshairs();

            if( d->m_crosshairsVisible ) {
                d->m_widget->setShowCrosshairs( false );
            }
        }
    */
    setVisible(false);
}

void PopupLayer::setCoordinates(const GeoDataCoordinates& /*coordinates*/, Qt::Alignment /*alignment*/)
{
    /*
        d->m_popupItem->setCoordinate( coordinates );
        d->m_popupItem->setAlignment( alignment );
    */
}

void PopupLayer::setUrl(const QUrl& /*url*/)
{
    //    d->m_popupItem->setUrl( url );
}

void PopupLayer::setContent(const QString& /*html*/, const QUrl& /*baseUrl*/)
{
    //    d->m_popupItem->setContent( html, baseUrl );
}

void PopupLayer::setBackgroundColor(const QColor& /*color*/)
{
    /*
        if(color.isValid()) {
            d->m_popupItem->setBackgroundColor(color);
        }
    */
}

void PopupLayer::setTextColor(const QColor& /*color*/)
{
    /*
        if(color.isValid()) {
            d->m_popupItem->setTextColor(color);
        }
    */
}

void PopupLayer::setSize(const QSizeF& size)
{
    d->m_requestedSize = size;
}

void PopupLayer::Private::setAppropriateSize(const ViewportParams* /*viewport*/)
{
    /*
        qreal margin = 15.0;

        QSizeF maximumSize;
        maximumSize.setWidth( viewport->width() - margin );
        maximumSize.setHeight( viewport->height() - margin );

        QSizeF minimumSize( 100.0, 100.0 );

        m_popupItem->setSize( m_requestedSize.boundedTo( maximumSize ).expandedTo( minimumSize ) );
    */
}

void PopupLayer::hidePopupItem()
{
    /*
        if( d->m_hasCrosshairsPlugin && d->m_crosshairsVisible ) {
            d->m_widget->setShowCrosshairs( d->m_crosshairsVisible );
        }
    */
    setVisible(false);
}

} // namespace Marble

#include "moc_PopupLayer.cpp"

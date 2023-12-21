// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "MarbleConfigView.h"

// Qt includes

#include <QNetworkProxy>
#include <QApplication>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QWidget>
#include <QDateTime>
#include <QTimer>
#include <QSettings>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "MarbleDirs.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "RenderPlugin.h"
#include "RenderPluginModel.h"
#include "MarbleClock.h"
#include "MarblePluginSettingsWidget.h"
#include "ui_MarbleViewSettingsWidget.h"

#include "ui_ProxySettingsWidget.h"

#include "digikam_debug.h"

namespace Marble
{

class MarbleConfigViewPrivate
{

public:

    MarbleConfigViewPrivate(MarbleWidget* const marbleWidget)
        : ui_viewSettings (),
          ui_proxySettings(),
          m_marbleWidget  (marbleWidget),
          m_pluginModel   ()
    {
    }

    Ui::MarbleViewSettingsWidget       ui_viewSettings;
    Ui::ProxySettingsWidget            ui_proxySettings;
    MarblePluginSettingsWidget*        w_pluginSettings = nullptr;

    QSettings                          m_settings;

    MarbleWidget* const                m_marbleWidget;

    RenderPluginModel                  m_pluginModel;

    QHash<int, int>                    m_timezone;
};

MarbleConfigView::MarbleConfigView(MarbleWidget* const marbleWidget,
                                   QWidget* const parent)
    : QTabWidget(parent),
      d      (new MarbleConfigViewPrivate(marbleWidget))
{
    // view page

    QWidget* const w_viewSettings = new QWidget( this );

    d->ui_viewSettings.setupUi( w_viewSettings );
    addTab( w_viewSettings, i18n( "View" ) );

    connect( d->ui_viewSettings.button_clearVolatileCache,
             SIGNAL(clicked()), SIGNAL(clearVolatileCacheClicked()) );

    connect( d->ui_viewSettings.button_clearPersistentCache,
             SIGNAL(clicked()), SIGNAL(clearPersistentCacheClicked()) );

    // proxy page

    QWidget* const w_proxySettings = new QWidget( this );

    d->ui_proxySettings.setupUi( w_proxySettings );
    addTab( w_proxySettings, i18n( "Proxy" ) );

    // plugin page

    d->m_pluginModel.setRenderPlugins( d->m_marbleWidget->renderPlugins() );
    d->w_pluginSettings = new MarblePluginSettingsWidget( this );
    d->w_pluginSettings->setModel( &d->m_pluginModel );
    d->w_pluginSettings->setObjectName( QLatin1String("plugin_page") );
    addTab( d->w_pluginSettings, i18n( "Plugins" ) );

    // Setting the icons for the plugin dialog.

    d->w_pluginSettings->setConfigIcon(QIcon(QStringLiteral(":/icons/settings-configure.png")));

    connect( this, SIGNAL(rejected()),
             &d->m_pluginModel, SLOT(retrievePluginState()) );

    connect( this, SIGNAL(accepted()),
             &d->m_pluginModel, SLOT(applyPluginState()) );

    // When the settings have been changed, write to disk.

    connect( this, SIGNAL(settingsChanged()),
             this, SLOT(syncSettings()) );
}

MarbleConfigView::~MarbleConfigView()
{
    delete d;
}

void MarbleConfigView::readSettings()
{
    // Sync settings to make sure that we read the current settings.

    d->m_settings.sync();

    QNetworkProxy proxy;

    // Make sure that no proxy is used for an empty string or the default value:

    if (proxyUrl().isEmpty() || (proxyUrl() == QLatin1String("http://")))
    {
        proxy.setType( QNetworkProxy::NoProxy );
    }
    else
    {
        if      ( proxyType() == Marble::Socks5Proxy )
        {
            proxy.setType( QNetworkProxy::Socks5Proxy );
        }
        else if ( proxyType() == Marble::HttpProxy )
        {
            proxy.setType( QNetworkProxy::HttpProxy );
        }
        else
        {
            qCDebug(DIGIKAM_MARBLE_LOG) << "Unknown proxy type! Using Http Proxy instead.";
            proxy.setType( QNetworkProxy::HttpProxy );
        }
    }

    proxy.setHostName( proxyUrl() );
    proxy.setPort( proxyPort() );

    if ( proxyAuth() )
    {
        proxy.setUser( proxyUser() );
        proxy.setPassword( proxyPass() );
    }

    QNetworkProxy::setApplicationProxy(proxy);

    // View

    d->ui_viewSettings.kcfg_distanceUnit->setCurrentIndex( measurementSystem() );
    d->ui_viewSettings.kcfg_angleUnit->setCurrentIndex( angleUnit() );
    d->ui_viewSettings.kcfg_stillQuality->setCurrentIndex( stillQuality() );
    d->ui_viewSettings.kcfg_animationQuality->setCurrentIndex( animationQuality() );
    d->ui_viewSettings.kcfg_mapFont->setCurrentFont( mapFont() );
    d->ui_viewSettings.kcfg_inertialEarthRotation->setChecked( inertialEarthRotation() );
    d->ui_viewSettings.kcfg_mouseViewRotation->setChecked( mouseViewRotation() );
    d->ui_viewSettings.kcfg_volatileTileCacheLimit->setValue( volatileTileCacheLimit() );
    d->ui_viewSettings.kcfg_persistentTileCacheLimit->setValue( persistentTileCacheLimit() );

    // Proxy

    d->ui_proxySettings.kcfg_proxyUrl->setText( proxyUrl() );
    d->ui_proxySettings.kcfg_proxyPort->setValue( proxyPort() );
    d->ui_proxySettings.kcfg_proxyUser->setText( proxyUser() );
    d->ui_proxySettings.kcfg_proxyPass->setText( proxyPass() );
    d->ui_proxySettings.kcfg_proxyType->setCurrentIndex( proxyType() );
    d->ui_proxySettings.kcfg_proxyAuth->setChecked( proxyAuth() );

    // Read the settings of the plugins

    d->m_marbleWidget->readPluginSettings( d->m_settings );
}

void MarbleConfigView::applySettings()
{
    d->m_settings.beginGroup( QLatin1String("View") );
    d->m_settings.setValue( QLatin1String("distanceUnit"), d->ui_viewSettings.kcfg_distanceUnit->currentIndex() );
    d->m_settings.setValue( QLatin1String("angleUnit"), d->ui_viewSettings.kcfg_angleUnit->currentIndex() );
    d->m_settings.setValue( QLatin1String("stillQuality"), d->ui_viewSettings.kcfg_stillQuality->currentIndex() );
    d->m_settings.setValue( QLatin1String("animationQuality"), d->ui_viewSettings.kcfg_animationQuality->currentIndex() );
    d->m_settings.setValue( QLatin1String("mapFont"), d->ui_viewSettings.kcfg_mapFont->currentFont() );
    d->m_settings.setValue( QLatin1String("inertialEarthRotation"), d->ui_viewSettings.kcfg_inertialEarthRotation->isChecked() );
    d->m_settings.setValue( QLatin1String("mouseViewRotation"), d->ui_viewSettings.kcfg_mouseViewRotation->isChecked() );
    d->m_settings.setValue( QLatin1String("volatileTileCacheLimit"), d->ui_viewSettings.kcfg_volatileTileCacheLimit->value() );
    d->m_settings.setValue( QLatin1String("persistentTileCacheLimit"), d->ui_viewSettings.kcfg_persistentTileCacheLimit->value() );
    d->m_settings.endGroup();

    d->m_settings.beginGroup( QLatin1String("Proxy") );
    d->m_settings.setValue( QLatin1String("proxyUrl"), d->ui_proxySettings.kcfg_proxyUrl->text() );
    d->m_settings.setValue( QLatin1String("proxyPort"), d->ui_proxySettings.kcfg_proxyPort->value() );
    d->m_settings.setValue( QLatin1String("proxyType"), d->ui_proxySettings.kcfg_proxyType->currentIndex() );

    if ( d->ui_proxySettings.kcfg_proxyAuth->isChecked() )
    {
        d->m_settings.setValue( QLatin1String("proxyAuth"), true );
        d->m_settings.setValue( QLatin1String("proxyUser"), d->ui_proxySettings.kcfg_proxyUser->text() );
        d->m_settings.setValue( QLatin1String("proxyPass"), d->ui_proxySettings.kcfg_proxyPass->text() );
    }
    else
    {
        d->m_settings.setValue( QLatin1String("proxyAuth"), false );
    }

    d->m_settings.endGroup();

    // Plugins

    d->m_marbleWidget->writePluginSettings( d->m_settings );
}

MarbleLocale::MeasurementSystem MarbleConfigView::measurementSystem() const
{
    if ( d->m_settings.contains( QString::fromUtf8("View/distanceUnit") ) )
    {
        return (MarbleLocale::MeasurementSystem)d->m_settings.value( QString::fromUtf8("View/distanceUnit") ).toInt();
    }

    MarbleLocale* const locale = MarbleGlobal::getInstance()->locale();

    return locale->measurementSystem();
}

Marble::AngleUnit MarbleConfigView::angleUnit() const
{
    return (Marble::AngleUnit) d->m_settings.value( QLatin1String("View/angleUnit"),
                               Marble::DMSDegree ).toInt();
}

void MarbleConfigView::setAngleUnit(Marble::AngleUnit unit)
{
    d->m_settings.setValue( QLatin1String("View/angleUnit"), (int)unit );
    d->ui_viewSettings.kcfg_angleUnit->setCurrentIndex( angleUnit() );
}

Marble::MapQuality MarbleConfigView::stillQuality() const
{
    return (Marble::MapQuality) d->m_settings.value( QLatin1String("View/stillQuality"),
                                Marble::HighQuality ).toInt();
}

Marble::MapQuality MarbleConfigView::animationQuality() const
{
    return (Marble::MapQuality) d->m_settings.value( QLatin1String("View/animationQuality"),
                                Marble::LowQuality ).toInt();
}

QFont MarbleConfigView::mapFont() const
{
    return d->m_settings.value( QLatin1String("View/mapFont"),
                                QApplication::font() ).value<QFont>();
}

bool MarbleConfigView::inertialEarthRotation() const
{
    return d->m_settings.value( QLatin1String("Navigation/inertialEarthRotation"),
                                true ).toBool();
}

bool MarbleConfigView::mouseViewRotation() const
{
    return d->m_settings.value( QLatin1String("Navigation/mouseViewRotation"),
                                true ).toBool();
}

int MarbleConfigView::volatileTileCacheLimit() const
{
    int defaultValue = (MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen) ? 6 : 100;

    return d->m_settings.value( QLatin1String("Cache/volatileTileCacheLimit"),
                                defaultValue ).toInt();
}

int MarbleConfigView::persistentTileCacheLimit() const
{
    return d->m_settings.value( QLatin1String("Cache/persistentTileCacheLimit"),
                                0 ).toInt(); // default to unlimited
}

QString MarbleConfigView::proxyUrl() const
{
    return d->m_settings.value( QLatin1String("Proxy/proxyUrl"),
                                QString::fromUtf8("") ).toString();
}

int MarbleConfigView::proxyPort() const
{
    return d->m_settings.value( QLatin1String("Proxy/proxyPort"),
                                8080 ).toInt();
}

QString MarbleConfigView::proxyUser() const
{
    return d->m_settings.value( QLatin1String("Proxy/proxyUser"),
                                QString::fromUtf8("") ).toString();
}

QString MarbleConfigView::proxyPass() const
{
    return d->m_settings.value( QLatin1String("Proxy/proxyPass"),
                                QString::fromUtf8("") ).toString();
}

bool MarbleConfigView::proxyType() const
{
    return d->m_settings.value( QLatin1String("Proxy/proxyType"),
                                Marble::HttpProxy ).toInt();
}

bool MarbleConfigView::proxyAuth() const
{
    return d->m_settings.value( QLatin1String("Proxy/proxyAuth"),
                                false ).toBool();
}

} // namespace Marble

#include "moc_MarbleConfigView.cpp"

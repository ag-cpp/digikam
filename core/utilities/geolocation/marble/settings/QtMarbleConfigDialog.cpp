// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

// Own
#include "QtMarbleConfigDialog.h"

#include "ui_MarbleCacheSettingsWidget.h"
#include "ui_MarbleViewSettingsWidget.h"

// Qt
#include <QSettings>
#include <QNetworkProxy>
#include <QApplication>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QDateTime>
#include <QTimer>

#include <klocalizedstring.h>

// Marble
#include "MarbleDirs.h"
#include "MarblePluginSettingsWidget.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "RenderPlugin.h"
#include "RenderPluginModel.h"
#include "MarbleClock.h"

#include "digikam_debug.h"

namespace Marble
{

class QtMarbleConfigDialogPrivate
{

public:

    QtMarbleConfigDialogPrivate(MarbleWidget* const marbleWidget)
        : ui_viewSettings      (),
          ui_cacheSettings     (),
          m_marbleWidget       (marbleWidget),
          m_pluginModel        ()
    {
    }

    Ui::MarbleViewSettingsWidget       ui_viewSettings;
    Ui::MarbleCacheSettingsWidget      ui_cacheSettings;
    MarblePluginSettingsWidget*        w_pluginSettings = nullptr;

    QSettings                          m_settings;

    MarbleWidget* const                m_marbleWidget;

    RenderPluginModel                  m_pluginModel;

    QHash<int, int>                    m_timezone;
};

QtMarbleConfigDialog::QtMarbleConfigDialog(MarbleWidget* const marbleWidget,
                                           QWidget* const parent)
    : QDialog(parent),
      d(new QtMarbleConfigDialogPrivate(marbleWidget))
{
    QTabWidget* tabWidget     = new QTabWidget( this );
    QDialogButtonBox* buttons =
    new QDialogButtonBox(QDialogButtonBox::Ok    |
                         QDialogButtonBox::Apply |
                         QDialogButtonBox::Cancel,
                         Qt::Horizontal,
                         this);

    // Connect the signals of the ButtonBox
    // to the corresponding slots of the dialog.

    connect( buttons, SIGNAL(accepted()),
             this, SLOT(accept()) );                                // Ok

    connect( buttons, SIGNAL(rejected()),
             this, SLOT(reject()) );                                // Cancel

    connect( buttons->button( QDialogButtonBox::Apply ), SIGNAL(clicked()),
             this, SLOT(writeSettings()) );                         // Apply

    // If the dialog is accepted. Save the settings.

    connect( this, SIGNAL(accepted()),
             this, SLOT(writeSettings()) );

    // view page

    QWidget* w_viewSettings = new QWidget( this );

    d->ui_viewSettings.setupUi( w_viewSettings );
    tabWidget->addTab( w_viewSettings, i18n( "View" ) );

    // cache page

    QWidget* w_cacheSettings = new QWidget( this );

    d->ui_cacheSettings.setupUi( w_cacheSettings );
    tabWidget->addTab( w_cacheSettings, i18n( "Cache and Proxy" ) );

    // Forwarding clear button signals

    connect( d->ui_cacheSettings.button_clearVolatileCache,
             SIGNAL(clicked()), SIGNAL(clearVolatileCacheClicked()) );

    connect( d->ui_cacheSettings.button_clearPersistentCache,
             SIGNAL(clicked()), SIGNAL(clearPersistentCacheClicked()) );

    // plugin page

    d->m_pluginModel.setRenderPlugins( d->m_marbleWidget->renderPlugins() );
    d->w_pluginSettings = new MarblePluginSettingsWidget( this );
    d->w_pluginSettings->setModel( &d->m_pluginModel );
    d->w_pluginSettings->setObjectName( QLatin1String("plugin_page") );
    tabWidget->addTab( d->w_pluginSettings, i18n( "Plugins" ) );

    // Setting the icons for the plugin dialog.

    d->w_pluginSettings->setConfigIcon(QIcon(QStringLiteral(":/icons/settings-configure.png")));

    connect( this, SIGNAL(rejected()),
             &d->m_pluginModel, SLOT(retrievePluginState()) );

    connect( this, SIGNAL(accepted()),
             &d->m_pluginModel, SLOT(applyPluginState()) );

    // Layout

    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->addWidget( tabWidget );
    layout->addWidget( buttons );

    this->setLayout( layout );

    // When the settings have been changed, write to disk.

    connect( this, SIGNAL(settingsChanged()),
             this, SLOT(syncSettings()) );
}

QtMarbleConfigDialog::~QtMarbleConfigDialog()
{
    delete d;
}

void QtMarbleConfigDialog::syncSettings()
{
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
}

void QtMarbleConfigDialog::readSettings()
{
    // Sync settings to make sure that we read the current settings.

    syncSettings();

    // View

    d->ui_viewSettings.kcfg_distanceUnit->setCurrentIndex( measurementSystem() );
    d->ui_viewSettings.kcfg_angleUnit->setCurrentIndex( angleUnit() );
    d->ui_viewSettings.kcfg_stillQuality->setCurrentIndex( stillQuality() );
    d->ui_viewSettings.kcfg_animationQuality->setCurrentIndex( animationQuality() );
    d->ui_viewSettings.kcfg_mapFont->setCurrentFont( mapFont() );
    d->ui_viewSettings.kcfg_inertialEarthRotation->setChecked( inertialEarthRotation() );
    d->ui_viewSettings.kcfg_mouseViewRotation->setChecked( mouseViewRotation() );

    // Cache

    d->ui_cacheSettings.kcfg_volatileTileCacheLimit->setValue( volatileTileCacheLimit() );
    d->ui_cacheSettings.kcfg_persistentTileCacheLimit->setValue( persistentTileCacheLimit() );
    d->ui_cacheSettings.kcfg_proxyUrl->setText( proxyUrl() );
    d->ui_cacheSettings.kcfg_proxyPort->setValue( proxyPort() );
    d->ui_cacheSettings.kcfg_proxyUser->setText( proxyUser() );
    d->ui_cacheSettings.kcfg_proxyPass->setText( proxyPass() );
    d->ui_cacheSettings.kcfg_proxyType->setCurrentIndex( proxyType() );
    d->ui_cacheSettings.kcfg_proxyAuth->setChecked( proxyAuth() );

    // Read the settings of the plugins

    d->m_marbleWidget->readPluginSettings( d->m_settings );

    // The settings loaded in the config dialog have been changed.

    Q_EMIT settingsChanged();
}

void QtMarbleConfigDialog::writeSettings()
{
    syncSettings();

    d->m_settings.beginGroup( QLatin1String("View") );
    d->m_settings.setValue( QLatin1String("distanceUnit"), d->ui_viewSettings.kcfg_distanceUnit->currentIndex() );
    d->m_settings.setValue( QLatin1String("angleUnit"), d->ui_viewSettings.kcfg_angleUnit->currentIndex() );
    d->m_settings.setValue( QLatin1String("stillQuality"), d->ui_viewSettings.kcfg_stillQuality->currentIndex() );
    d->m_settings.setValue( QLatin1String("animationQuality"), d->ui_viewSettings.kcfg_animationQuality->currentIndex() );
    d->m_settings.setValue( QLatin1String("mapFont"), d->ui_viewSettings.kcfg_mapFont->currentFont() );
    d->m_settings.setValue( QLatin1String("inertialEarthRotation"), d->ui_viewSettings.kcfg_inertialEarthRotation->isChecked() );
    d->m_settings.setValue( QLatin1String("mouseViewRotation"), d->ui_viewSettings.kcfg_mouseViewRotation->isChecked() );
    d->m_settings.endGroup();

    d->m_settings.beginGroup( QLatin1String("Cache") );
    d->m_settings.setValue( QLatin1String("volatileTileCacheLimit"), d->ui_cacheSettings.kcfg_volatileTileCacheLimit->value() );
    d->m_settings.setValue( QLatin1String("persistentTileCacheLimit"), d->ui_cacheSettings.kcfg_persistentTileCacheLimit->value() );
    d->m_settings.setValue( QLatin1String("proxyUrl"), d->ui_cacheSettings.kcfg_proxyUrl->text() );
    d->m_settings.setValue( QLatin1String("proxyPort"), d->ui_cacheSettings.kcfg_proxyPort->value() );
    d->m_settings.setValue( QLatin1String("proxyType"), d->ui_cacheSettings.kcfg_proxyType->currentIndex() );

    if ( d->ui_cacheSettings.kcfg_proxyAuth->isChecked() )
    {
        d->m_settings.setValue( QLatin1String("proxyAuth"), true );
        d->m_settings.setValue( QLatin1String("proxyUser"), d->ui_cacheSettings.kcfg_proxyUser->text() );
        d->m_settings.setValue( QLatin1String("proxyPass"), d->ui_cacheSettings.kcfg_proxyPass->text() );
    }
    else
    {
        d->m_settings.setValue( QLatin1String("proxyAuth"), false );
    }

    d->m_settings.endGroup();

    // Plugins

    d->m_marbleWidget->writePluginSettings( d->m_settings );

    Q_EMIT settingsChanged();
}

MarbleLocale::MeasurementSystem QtMarbleConfigDialog::measurementSystem() const
{
    if ( d->m_settings.contains( QString::fromUtf8("View/distanceUnit") ) )
    {
        return (MarbleLocale::MeasurementSystem)d->m_settings.value( QString::fromUtf8("View/distanceUnit") ).toInt();
    }

    MarbleLocale* locale = MarbleGlobal::getInstance()->locale();

    return locale->measurementSystem();
}

Marble::AngleUnit QtMarbleConfigDialog::angleUnit() const
{
    return (Marble::AngleUnit) d->m_settings.value( QLatin1String("View/angleUnit"), Marble::DMSDegree ).toInt();
}

void QtMarbleConfigDialog::setAngleUnit(Marble::AngleUnit unit)
{
    d->m_settings.setValue( QLatin1String("View/angleUnit"), (int)unit );
    d->ui_viewSettings.kcfg_angleUnit->setCurrentIndex( angleUnit() );

    Q_EMIT settingsChanged();
}

Marble::MapQuality QtMarbleConfigDialog::stillQuality() const
{
    return (Marble::MapQuality) d->m_settings.value( QLatin1String("View/stillQuality"),
                                Marble::HighQuality ).toInt();
}

Marble::MapQuality QtMarbleConfigDialog::animationQuality() const
{
    return (Marble::MapQuality) d->m_settings.value( QLatin1String("View/animationQuality"),
                                Marble::LowQuality ).toInt();
}

QFont QtMarbleConfigDialog::mapFont() const
{
    return d->m_settings.value( QLatin1String("View/mapFont"), QApplication::font() ).value<QFont>();
}

bool QtMarbleConfigDialog::inertialEarthRotation() const
{
    return d->m_settings.value( QLatin1String("Navigation/inertialEarthRotation"), true ).toBool();
}

bool QtMarbleConfigDialog::mouseViewRotation() const
{
    return d->m_settings.value( QLatin1String("Navigation/mouseViewRotation"), true ).toBool();
}

int QtMarbleConfigDialog::volatileTileCacheLimit() const
{
    int defaultValue = (MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen) ? 6 : 100;

    return d->m_settings.value( QLatin1String("Cache/volatileTileCacheLimit"), defaultValue ).toInt();
}

int QtMarbleConfigDialog::persistentTileCacheLimit() const
{
    return d->m_settings.value( QLatin1String("Cache/persistentTileCacheLimit"), 0 ).toInt(); // default to unlimited
}

QString QtMarbleConfigDialog::proxyUrl() const
{
    return d->m_settings.value( QLatin1String("Cache/proxyUrl"), QString::fromUtf8("") ).toString();
}

int QtMarbleConfigDialog::proxyPort() const
{
    return d->m_settings.value( QLatin1String("Cache/proxyPort"), 8080 ).toInt();
}

QString QtMarbleConfigDialog::proxyUser() const
{
    return d->m_settings.value( QLatin1String("Cache/proxyUser"), QString::fromUtf8("") ).toString();
}

QString QtMarbleConfigDialog::proxyPass() const
{
    return d->m_settings.value( QLatin1String("Cache/proxyPass"), QString::fromUtf8("") ).toString();
}

bool QtMarbleConfigDialog::proxyType() const
{
    return d->m_settings.value( QLatin1String("Cache/proxyType"), Marble::HttpProxy ).toInt();
}

bool QtMarbleConfigDialog::proxyAuth() const
{
    return d->m_settings.value( QLatin1String("Cache/proxyAuth"), false ).toBool();
}

}

#include "moc_QtMarbleConfigDialog.cpp"

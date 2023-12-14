// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Utku Aydın <utkuaydin34@gmail.com>
//

#include "OwncloudSyncBackend.h"

#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "digikam_debug.h"
#include "GeoDocument.h"
#include "MarbleWidget.h"
#include "RenderPlugin.h"
#include "Route.h"
#include "RoutingModel.h"
#include "GeoDataParser.h"
#include "GeoDataFolder.h"
#include "RoutingManager.h"
#include "RouteItem.h"
#include "GeoDataDocument.h"
#include "CloudRouteModel.h"
#include "GeoDataPlacemark.h"
#include "CloudSyncManager.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFileInfo>
#include <QBuffer>
#include <QDir>

#include <klocalizedstring.h>

namespace Marble
{

class Q_DECL_HIDDEN OwncloudSyncBackend::Private {

    public:
        Private( CloudSyncManager* cloudSyncManager );

        QDir m_cacheDir;
        QNetworkAccessManager m_network;
        QNetworkReply *m_routeUploadReply;
        QNetworkReply *m_routeListReply;
        QNetworkReply *m_routeDownloadReply;
        QNetworkReply *m_routeDeleteReply;
    QNetworkReply *m_authReply;

        QVector<RouteItem> m_routeList;

        QString m_routeUploadEndpoint;
        QString m_routeListEndpoint;
        QString m_routeDownloadEndpoint;
        QString m_routeDeleteEndpoint;
        QString m_routePreviewEndpoint;

        CloudSyncManager* m_cloudSyncManager;
        QUrl m_apiUrl;
};

OwncloudSyncBackend::Private::Private( CloudSyncManager* cloudSyncManager ) :
    m_cacheDir(MarbleDirs::localPath() + QLatin1String("/cloudsync/cache/routes/")),
    m_network(),
    m_routeUploadReply(),
    m_routeListReply(),
    m_routeDownloadReply(),
    m_routeDeleteReply(),
    m_authReply(),
    m_routeList(),
    // Route API endpoints
    m_routeUploadEndpoint( QString::fromUtf8("routes/create") ),
    m_routeListEndpoint( QString::fromUtf8("routes") ),
    m_routeDownloadEndpoint( QString::fromUtf8("routes") ),
    m_routeDeleteEndpoint( QString::fromUtf8("routes/delete") ),
    m_routePreviewEndpoint( QString::fromUtf8("routes/preview") ),
    m_cloudSyncManager( cloudSyncManager )
{
}

OwncloudSyncBackend::OwncloudSyncBackend( CloudSyncManager* cloudSyncManager ) :
    d( new Private( cloudSyncManager ) )
{
    connect(d->m_cloudSyncManager, SIGNAL(apiUrlChanged(QUrl)), this, SLOT(validateSettings()));
}

OwncloudSyncBackend::~OwncloudSyncBackend()
{
    delete d;
}

void OwncloudSyncBackend::uploadRoute( const QString &timestamp )
{
    QString word = QString::fromUtf8("----MarbleCloudBoundary");
    QString boundary = QString::fromUtf8( "--%0" ).arg( word );
    QNetworkRequest request( endpointUrl( d->m_routeUploadEndpoint ) );
    request.setHeader( QNetworkRequest::ContentTypeHeader, QString::fromUtf8( "multipart/form-data; boundary=%0" ).arg( word ) );

    QByteArray data;
    data.append( QString( boundary + QString::fromUtf8("\r\n") ).toUtf8() );

    // Timestamp part
    data.append( QByteArray("Content-Disposition: form-data; name=\"timestamp\"") );
    data.append( QByteArray("\r\n\r\n") );
    data.append( QString( timestamp + QString::fromUtf8("\r\n") ).toUtf8() );
    data.append( QString( boundary + QString::fromUtf8("\r\n") ).toUtf8() );

    // Name part
    data.append( QByteArray("Content-Disposition: form-data; name=\"name\"") );
    data.append( QByteArray("\r\n\r\n") );
    data.append( routeName( timestamp ).toUtf8() );
    data.append( QByteArray("\r\n") );
    data.append( QString( boundary + QString::fromUtf8("\r\n") ).toUtf8() );

    QFile kmlFile( d->m_cacheDir.absolutePath() + QString::fromUtf8( "/%0.kml" ).arg( timestamp ) );

    if( !kmlFile.open( QFile::ReadOnly ) ) {
        qCDebug(DIGIKAM_MARBLE_LOG) << "Could not open " << timestamp << ".kml. Either it has not been saved" <<
                    " to cache for upload or another application removed it from there.";
        return;
    }

    GeoDataParser parser(GeoData_KML);
    if (!parser.read(&kmlFile)) {
        qCDebug(DIGIKAM_MARBLE_LOG) << "[OwncloudSyncBackend] KML file" << kmlFile.fileName()
                 << "is broken so I can't fill required properties";
        return;
    }

    GeoDataDocument *root = dynamic_cast<GeoDataDocument*>(parser.releaseDocument());
    if (!root || root->size() < 2) {
        qCDebug(DIGIKAM_MARBLE_LOG) << "[OwncloudSyncBackend] Root document is broken";
        return;
    }

    GeoDataDocument *doc = geodata_cast<GeoDataDocument>(root->child(1));
    if (!doc || doc->size() < 1) {
        qCDebug(DIGIKAM_MARBLE_LOG) << "[OwncloudSyncBackend] Tracking document is broken";
        return;
    }

    GeoDataPlacemark *placemark = geodata_cast<GeoDataPlacemark>(doc->child(0));
    if (!placemark) {
        qCDebug(DIGIKAM_MARBLE_LOG) << "[OwncloudSyncBackend] Placemark is broken";
        return;
    }

    // Duration part
    double duration =
            QTime().secsTo(QTime::fromString(placemark->extendedData().value(QStringLiteral("duration")).value().toString(), Qt::ISODate)) / 60.0;
    qCDebug(DIGIKAM_MARBLE_LOG) << "[Owncloud] Duration on write is" << duration;
    data.append( QByteArray("Content-Disposition: form-data; name=\"duration\"") );
    data.append( QByteArray("\r\n\r\n") );
    data.append( QString::number(duration).toUtf8() );
    data.append( QByteArray("\r\n") );
    data.append( QString( boundary + QString::fromUtf8("\r\n") ).toUtf8() );

    // Distance part
    double distance =
            placemark->extendedData().value(QStringLiteral("length")).value().toDouble();
    qCDebug(DIGIKAM_MARBLE_LOG) << "[Owncloud] Distance on write is" << distance;
    data.append( QByteArray("Content-Disposition: form-data; name=\"distance\"") );
    data.append( QByteArray("\r\n\r\n") );
    data.append( QString::number(distance).toUtf8() );
    data.append( QByteArray("\r\n") );
    data.append( QString( boundary + QString::fromUtf8("\r\n") ).toUtf8() );

    // KML part
    data.append( QString::fromUtf8( "Content-Disposition: form-data; name=\"kml\"; filename=\"%0.kml\"" ).arg( timestamp ).toUtf8() );
    data.append( QByteArray("\r\n") );
    data.append( QByteArray("Content-Type: application/vnd.google-earth.kml+xml") );
    data.append( QByteArray("\r\n\r\n") );

    kmlFile.seek(0); // just to be sure
    data.append( kmlFile.readAll() );
    data.append( QByteArray("\r\n") );
    data.append( QString( boundary + QString::fromUtf8("\r\n") ).toUtf8() );

    kmlFile.close();

    // Preview part
    data.append( QString::fromUtf8( "Content-Disposition: form-data; name=\"preview\"; filename=\"%0.jpg\"" ).arg( timestamp ).toUtf8() );
    data.append( QByteArray("\r\n") );
    data.append( QByteArray("Content-Type: image/jpg") );
    data.append( QByteArray("\r\n\r\n") );

    QByteArray previewBytes;
    QBuffer previewBuffer( &previewBytes );
    QPixmap preview = createPreview( timestamp );
    preview.save( &previewBuffer, "JPG" );

    data.append( previewBytes );
    data.append( QByteArray("\r\n") );
    data.append( QString( boundary + QString::fromUtf8("\r\n") ).toUtf8() );

    d->m_routeUploadReply = d->m_network.post( request, data );
    connect( d->m_routeUploadReply, SIGNAL(uploadProgress(qint64,qint64)), this, SIGNAL(routeUploadProgress(qint64,qint64)) );
}

void OwncloudSyncBackend::downloadRouteList()
{
    QNetworkRequest request( endpointUrl( d->m_routeListEndpoint ) );
    d->m_routeListReply = d->m_network.get( request );
    connect( d->m_routeListReply, SIGNAL(downloadProgress(qint64,qint64)), this, SIGNAL(routeListDownloadProgress(qint64,qint64)) );
    connect( d->m_routeListReply, SIGNAL(finished()), this, SLOT(prepareRouteList()) );
}

void OwncloudSyncBackend::downloadRoute( const QString &timestamp )
{
    QNetworkRequest routeRequest( endpointUrl( d->m_routeDownloadEndpoint, timestamp ) );
    d->m_routeDownloadReply = d->m_network.get( routeRequest );
    connect( d->m_routeDownloadReply, SIGNAL(finished()), this, SLOT(saveDownloadedRoute()) );
    connect( d->m_routeDownloadReply, SIGNAL(downloadProgress(qint64,qint64)), this, SIGNAL(routeDownloadProgress(qint64,qint64)) );
}

void OwncloudSyncBackend::deleteRoute( const QString &timestamp )
{
    QUrl url( endpointUrl( d->m_routeDeleteEndpoint, timestamp ) );
    QNetworkRequest request( url );
    d->m_routeDeleteReply = d->m_network.deleteResource( request );
    connect( d->m_routeDeleteReply, SIGNAL(finished()), this, SIGNAL(routeDeleted()) );
}

QPixmap OwncloudSyncBackend::createPreview( const QString &timestamp ) const
{
    MarbleWidget mapWidget;
    for( RenderPlugin* plugin: mapWidget.renderPlugins() ) {
        plugin->setEnabled( false );
    }

    mapWidget.setProjection( Mercator );
    mapWidget.setMapThemeId(QStringLiteral("earth/openstreetmap/openstreetmap.dgml"));
    mapWidget.resize( 512, 512 );

    RoutingManager* manager = mapWidget.model()->routingManager();
    manager->loadRoute( d->m_cacheDir.absolutePath() + QString::fromUtf8( "/%0.kml" ).arg( timestamp ) );
    GeoDataLatLonBox const bbox = manager->routingModel()->route().bounds();

    if ( !bbox.isEmpty() ) {
        mapWidget.centerOn( bbox );
    }

    QPixmap pixmap = mapWidget.grab();
    QDir( d->m_cacheDir.absolutePath() ).mkpath( QString::fromUtf8("preview") );
    pixmap.save(d->m_cacheDir.absolutePath() + QLatin1String("/preview/") + timestamp + QLatin1String(".jpg"));

    return pixmap;
}

QString OwncloudSyncBackend::routeName( const QString &timestamp ) const
{
    QFile file( d->m_cacheDir.absolutePath() + QString::fromUtf8( "/%0.kml" ).arg( timestamp ) );
    file.open( QFile::ReadOnly );

    GeoDataParser parser( GeoData_KML );
    if( !parser.read( &file ) ) {
        qCDebug(DIGIKAM_MARBLE_LOG) << "Could not read " << timestamp << ".kml. Timestamp will be used as "
                    << "route name because of the problem";
        return timestamp;
    }
    file.close();

    QString routeName;
    GeoDocument *geoDoc = parser.releaseDocument();
    GeoDataDocument *container = dynamic_cast<GeoDataDocument*>( geoDoc );
    if ( container && container->size() > 0 ) {
        GeoDataFolder *folder = container->folderList().at( 0 );
        for ( GeoDataPlacemark *placemark: folder->placemarkList() ) {
            routeName.append( placemark->name() );
            routeName.append( QString::fromUtf8(" - ") );
        }
    }

    return routeName.left( routeName.length() - 3 );
}

void OwncloudSyncBackend::validateSettings()
{
    if( d->m_cloudSyncManager->owncloudServer().size() > 0
        && d->m_cloudSyncManager->owncloudUsername().size() > 0
        && d->m_cloudSyncManager->owncloudPassword().size() > 0 )
    {
        QNetworkRequest request( endpointUrl( d->m_routeListEndpoint ) );
        d->m_authReply = d->m_network.get( request );
        connect( d->m_authReply, SIGNAL(finished()), this, SLOT(checkAuthReply()) );
        connect( d->m_authReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(checkAuthError(QNetworkReply::NetworkError)) );
    } else {
        // no server, make the error field blank
        d->m_cloudSyncManager->setStatus(QString::fromUtf8(""), CloudSyncManager::Success);
    }
}

void OwncloudSyncBackend::checkAuthError(QNetworkReply::NetworkError error)
{
    if ( error == QNetworkReply::HostNotFoundError ) {
        QString const status = i18n( "Server '%1' could not be reached", d->m_cloudSyncManager->owncloudServer() );
        d->m_cloudSyncManager->setStatus( status , CloudSyncManager::Error );
    }
}

void OwncloudSyncBackend::checkAuthReply()
{
    int statusCode = d->m_authReply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();

    if ( statusCode == 0 ) // request was cancelled
        return;

    QString result = QString::fromUtf8(d->m_authReply->readAll());

    if (!result.startsWith(QLatin1Char('{'))) {
        // not a JSON result
        if (result.contains(QLatin1String("http://owncloud.org"))) {
            // an owncloud login page was returned, marble app is not installed
            d->m_cloudSyncManager->setStatus( i18n( "The Marble app is not installed on the ownCloud server" ), CloudSyncManager::Error);
        } else {
            d->m_cloudSyncManager->setStatus( i18n( "The server is not an ownCloud server" ), CloudSyncManager::Error);
        }
    } else if (result == QLatin1String("{\"message\":\"Current user is not logged in\"}") && statusCode == 401) {
        // credentials were incorrect
        d->m_cloudSyncManager->setStatus( i18n( "Username or password are incorrect" ), CloudSyncManager::Error);
    } else if ( result.contains(QString::fromUtf8("\"status\":\"success\"")) && statusCode == 200 ) {
        // credentials were correct
        d->m_cloudSyncManager->setStatus( i18n( "Login successful" ), CloudSyncManager::Success);
    }
}

void OwncloudSyncBackend::cancelUpload()
{
    d->m_routeUploadReply->abort();
}

void OwncloudSyncBackend::prepareRouteList()
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(d->m_routeListReply->readAll());
    QJsonValue dataValue = jsonDoc.object().value(QStringLiteral("data"));

    d->m_routeList.clear();

    if (dataValue.isArray()) {
        QJsonArray dataArray = dataValue.toArray();
        for (int index = 0; index < dataArray.size(); ++index) {
            QJsonObject dataObject = dataArray[index].toObject();

            RouteItem route;
            route.setIdentifier(dataObject.value(QStringLiteral("timestamp")).toString());
            route.setName(dataObject.value(QStringLiteral("name")).toString() );
            route.setDistance(dataObject.value(QStringLiteral("distance")).toString());
            route.setDuration(dataObject.value(QStringLiteral("duration")).toString());
            route.setPreviewUrl( endpointUrl( d->m_routePreviewEndpoint, route.identifier() ) );
            route.setOnCloud( true );

            d->m_routeList.append( route );
        }
    }

    // FIXME Find why an empty item added to the end.
    if( !d->m_routeList.isEmpty() ) {
        d->m_routeList.remove( d->m_routeList.count() - 1 );
    }

    Q_EMIT routeListDownloaded( d->m_routeList );
}

void OwncloudSyncBackend::saveDownloadedRoute()
{
    QString timestamp = QFileInfo( d->m_routeDownloadReply->url().toString() ).fileName();

    bool pathCreated = d->m_cacheDir.mkpath( d->m_cacheDir.absolutePath() );
    if ( !pathCreated ) {
        qCDebug(DIGIKAM_MARBLE_LOG) << "Couldn't create the path " << d->m_cacheDir.absolutePath() <<
                    ". Check if your user has sufficient permissions for this operation.";
    }

    QString kmlFilePath = QString::fromUtf8( "%0/%1.kml").arg( d->m_cacheDir.absolutePath(), timestamp );
    QFile kmlFile( kmlFilePath );
    bool fileOpened = kmlFile.open( QFile::ReadWrite );

    if ( !fileOpened ) {
        qCDebug(DIGIKAM_MARBLE_LOG) << "Failed to open file" << kmlFilePath << " for writing."
                 <<  " Its directory either is missing or is not writable.";
        return;
    }

    kmlFile.write( d->m_routeDownloadReply->readAll() );
    kmlFile.close();

    QString previewPath = QString::fromUtf8( "%0/preview/" ).arg( d->m_cacheDir.absolutePath() );
    bool previewPathCreated = d->m_cacheDir.mkpath( previewPath );
    if ( !previewPathCreated ) {
        qCDebug(DIGIKAM_MARBLE_LOG) << "Couldn't create the path " << previewPath <<
                    ". Check if your user has sufficient permissions for this operation.";
    }

    QString previewFilePath = QString::fromUtf8( "%0/preview/%1.jpg").arg( d->m_cacheDir.absolutePath(), timestamp );
    QFile previewFile( previewFilePath );
    bool previewFileOpened = previewFile.open( QFile::ReadWrite );

    if ( !previewFileOpened ) {
        qCDebug(DIGIKAM_MARBLE_LOG) << "Failed to open file" << previewFilePath << "for writing."
                 <<  " Its directory either is missing or is not writable.";
        return;
    }

    QPixmap preview = createPreview( timestamp );
    preview.save( &previewFile, "JPG" );
    previewFile.close();

    Q_EMIT routeDownloaded();
}

QUrl OwncloudSyncBackend::endpointUrl( const QString &endpoint ) const
{
    const QString endpointUrl = d->m_cloudSyncManager->apiUrl().toString() + QLatin1Char('/') + endpoint;
    return QUrl( endpointUrl );
}

QUrl OwncloudSyncBackend::endpointUrl( const QString &endpoint, const QString &parameter ) const
{
    const QString endpointUrl = d->m_cloudSyncManager->apiUrl().toString() + QLatin1Char('/') + endpoint + QLatin1Char('/') + parameter;
    return QUrl( endpointUrl );
}

void OwncloudSyncBackend::removeFromCache( const QDir &cacheDir, const QString &timestamp )
{
    bool fileRemoved = QFile( QString::fromUtf8( "%0/%1.kml" ).arg( cacheDir.absolutePath(), timestamp ) ).remove();
    bool previewRemoved = QFile( QString::fromUtf8( "%0/preview/%1.jpg" ).arg( cacheDir.absolutePath(), timestamp ) ).remove();
    if ( !fileRemoved || !previewRemoved ) {
        qCDebug(DIGIKAM_MARBLE_LOG) << "Failed to remove locally cached route " << timestamp << ". It might "
                    "have been removed already, or its directory is missing / not writable.";
    }

    Q_EMIT removedFromCache( timestamp );
}

}

#include "moc_OwncloudSyncBackend.cpp"

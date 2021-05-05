/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-05-20
 * Description : a tool to export images to Pinterest web service
 *
 * Copyright (C) 2018      by Tarek Talaat <tarektalaat93 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "ptalker.h"

// Qt includes

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QByteArray>
#include <QList>
#include <QPair>
#include <QFileInfo>
#include <QWidget>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopServices>
#include <QUrlQuery>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QScopedPointer>

// KDE includes

#include <klocalizedstring.h>
#include <kwindowconfig.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_version.h"
#include "wstoolutils.h"
#include "pwindow.h"
#include "pitem.h"
#include "webbrowserdlg.h"
#include "previewloadthread.h"

namespace DigikamGenericPinterestPlugin
{

class Q_DECL_HIDDEN PTalker::Private
{
public:

    enum State
    {
        P_USERNAME = 0,
        P_LISTBOARDS,
        P_CREATEBOARD,
        P_ADDPIN,
        P_ACCESSTOKEN
    };

public:

    explicit Private()
      : parent  (nullptr),
        netMngr (nullptr),
        reply   (nullptr),
        settings(nullptr),
        state   (P_USERNAME),
        browser (nullptr)
    {
        clientId     = QLatin1String("4983380570301022071");
        clientSecret = QLatin1String("2a698db679125930d922a2dfb897e16b668a67c6f614593636e83fc3d8d9b47d");

        authUrl      = QLatin1String("https://api.pinterest.com/oauth/");
        tokenUrl     = QLatin1String("https://api.pinterest.com/v1/oauth/token");
        redirectUrl  = QLatin1String("https://login.live.com/oauth20_desktop.srf");
        scope        = QLatin1String("read_public,write_public");
        serviceName  = QLatin1String("Pinterest");
        serviceKey   = QLatin1String("access_token");
    }

public:

    QString                clientId;
    QString                clientSecret;
    QString                authUrl;
    QString                tokenUrl;
    QString                redirectUrl;
    QString                accessToken;
    QString                scope;
    QString                userName;
    QString                serviceName;
    QString                serviceKey;

    QWidget*               parent;

    QNetworkAccessManager* netMngr;
    QNetworkReply*         reply;

    QSettings*             settings;

    State                  state;

    QMap<QString, QString> urlParametersMap;

    WebBrowserDlg*         browser;
};

PTalker::PTalker(QWidget* const parent)
    : d(new Private)
{
    d->parent   = parent;
    d->netMngr  = new QNetworkAccessManager(this);
    d->settings = WSToolUtils::getOauthSettings(this);

    connect(d->netMngr, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotFinished(QNetworkReply*)));

    connect(this, SIGNAL(pinterestLinkingFailed()),
            this, SLOT(slotLinkingFailed()));

    connect(this, SIGNAL(pinterestLinkingSucceeded()),
            this, SLOT(slotLinkingSucceeded()));
}

PTalker::~PTalker()
{
    if (d->reply)
    {
        d->reply->abort();
    }

    WSToolUtils::removeTemporaryDir("pinterest");

    delete d;
}

void PTalker::link()
{
    emit signalBusy(true);

    QUrl url(d->authUrl);
    QUrlQuery query(url);
    query.addQueryItem(QLatin1String("client_id"),     d->clientId);
    query.addQueryItem(QLatin1String("scope"),         d->scope);
    query.addQueryItem(QLatin1String("redirect_uri"),  d->redirectUrl);
    query.addQueryItem(QLatin1String("response_type"), QLatin1String("code"));
    url.setQuery(query);

    delete d->browser;
    d->browser = new WebBrowserDlg(url, d->parent, true);
    d->browser->setModal(true);

    connect(d->browser, SIGNAL(urlChanged(QUrl)),
            this, SLOT(slotCatchUrl(QUrl)));

    connect(d->browser, SIGNAL(closeView(bool)),
            this, SIGNAL(signalBusy(bool)));

    d->browser->show();
}

void PTalker::unLink()
{
    d->accessToken = QString();

    d->settings->beginGroup(d->serviceName);
    d->settings->remove(QString());
    d->settings->endGroup();

    emit pinterestLinkingSucceeded();
}

void PTalker::slotCatchUrl(const QUrl& url)
{
    d->urlParametersMap = ParseUrlParameters(url.toString());
    QString code        = d->urlParametersMap.value(QLatin1String("code"));
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Received URL from webview in link function: " << url ;

    if (!code.isEmpty())
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "CODE Received";
        d->browser->close();
        getToken(code);
        emit signalBusy(false);
    }
}

void PTalker::getToken(const QString& code)
{
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Code: " << code;
    QUrl url(d->tokenUrl);
    QUrlQuery query(url);
    query.addQueryItem(QLatin1String("grant_type"),    QLatin1String("authorization_code"));
    query.addQueryItem(QLatin1String("client_id"),     d->clientId);
    query.addQueryItem(QLatin1String("client_secret"), d->clientSecret);
    query.addQueryItem(QLatin1String("code"),          code);
    url.setQuery(query);
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Token Request URL:    " << url.toString();

    QNetworkRequest netRequest(url);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
    netRequest.setRawHeader("Accept", "application/json");

    d->reply = d->netMngr->post(netRequest, QByteArray());

    d->state = Private::P_ACCESSTOKEN;
}

QMap<QString, QString> PTalker::ParseUrlParameters(const QString& url)
{
    QMap<QString, QString> urlParameters;

    if (url.indexOf(QLatin1Char('?')) == -1)
    {
        return urlParameters;
    }

    QString tmp           = url.right(url.length()-url.indexOf(QLatin1Char('?')) - 1);
    QStringList paramlist = tmp.split(QLatin1Char('&'));

    for (int i = 0 ; i < paramlist.count() ; ++i)
    {
        QStringList paramarg = paramlist.at(i).split(QLatin1Char('='));

        if (paramarg.count() == 2)
        {
            urlParameters.insert(paramarg.at(0), paramarg.at(1));
        }
    }

    return urlParameters;
}

void PTalker::slotLinkingFailed()
{
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "LINK to Pinterest fail";
    emit signalBusy(false);
}

void PTalker::slotLinkingSucceeded()
{
    if (d->accessToken.isEmpty())
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "UNLINK to Pinterest ok";
        emit signalBusy(false);
        return;
    }

    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "LINK to Pinterest ok";
    writeSettings();
    emit signalLinkingSucceeded();
}

bool PTalker::authenticated()
{
    return (!d->accessToken.isEmpty());
}

void PTalker::cancel()
{
    if (d->reply)
    {
        d->reply->abort();
        d->reply = nullptr;
    }

    emit signalBusy(false);
}

void PTalker::createBoard(QString& boardName)
{
    QUrl url(QLatin1String("https://api.pinterest.com/v1/boards/"));
    QNetworkRequest netRequest(url);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/json"));
    netRequest.setRawHeader("Authorization", QString::fromLatin1("Bearer %1").arg(d->accessToken).toUtf8());

    QByteArray postData = QString::fromUtf8("{\"name\": \"%1\"}").arg(boardName).toUtf8();
/*
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "createBoard:" << postData;
*/
    d->reply = d->netMngr->post(netRequest, postData);

    d->state = Private::P_CREATEBOARD;
    emit signalBusy(true);
}

void PTalker::getUserName()
{
    QUrl url(QLatin1String("https://api.pinterest.com/v1/me/?fields=username"));

    QNetworkRequest netRequest(url);
    netRequest.setRawHeader("Authorization", QString::fromLatin1("Bearer %1").arg(d->accessToken).toUtf8());

    d->reply = d->netMngr->get(netRequest);
    d->state = Private::P_USERNAME;
    emit signalBusy(true);
}

/**
 * Get list of boards by parsing json sent by pinterest
 */
void PTalker::listBoards(const QString& /*path*/)
{
    QUrl url(QLatin1String("https://api.pinterest.com/v1/me/boards/"));;

    QNetworkRequest netRequest(url);
    netRequest.setRawHeader("Authorization", QString::fromLatin1("Bearer %1").arg(d->accessToken).toUtf8());
/*
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/json"));
*/
    d->reply = d->netMngr->get(netRequest);

    d->state = Private::P_LISTBOARDS;
    emit signalBusy(true);
}

bool PTalker::addPin(const QString& imgPath,
                     const QString& uploadBoard,
                     bool rescale,
                     int maxDim,
                     int imageQuality)
{
    if (d->reply)
    {
        d->reply->abort();
        d->reply = nullptr;
    }

    emit signalBusy(true);

    QImage image = PreviewLoadThread::loadHighQualitySynchronously(imgPath).copyQImage();

    if (image.isNull())
    {
        emit signalBusy(false);
        return false;
    }

    QString path = WSToolUtils::makeTemporaryDir("pinterest").filePath(QFileInfo(imgPath)
                                                 .baseName().trimmed() + QLatin1String(".jpg"));

    if (rescale && ((image.width() > maxDim) || (image.height() > maxDim)))
    {
        image = image.scaled(maxDim, maxDim, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    image.save(path, "JPEG", imageQuality);

    QScopedPointer<DMetadata> meta(new DMetadata);

    if (meta->load(imgPath))
    {
        meta->setItemDimensions(image.size());
        meta->setItemOrientation(DMetadata::ORIENTATION_NORMAL);
        meta->setMetadataWritingMode((int)DMetadata::WRITE_TO_FILE_ONLY);
        meta->save(path, true);
    }

    QString boardParam              = d->userName + QLatin1Char('/') + uploadBoard;

    QUrl url(QString::fromLatin1("https://api.pinterest.com/v1/pins/?access_token=%1").arg(d->accessToken));

    QHttpMultiPart* const multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // Board Section

    QHttpPart board;
    QString boardHeader = QLatin1String("form-data; name=\"board\"") ;
    board.setHeader(QNetworkRequest::ContentDispositionHeader, boardHeader);

    QByteArray postData = boardParam.toUtf8();
    board.setBody(postData);
    multiPart->append(board);

    // Note section

    QHttpPart note;
    QString noteHeader = QLatin1String("form-data; name=\"note\"") ;
    note.setHeader(QNetworkRequest::ContentDispositionHeader, noteHeader);

    postData           = QByteArray();

    note.setBody(postData);
    multiPart->append(note);

    // image section

    QFile* const file  = new QFile(imgPath);

    if (!file)
    {
        return false;
    }

    if (!file->open(QIODevice::ReadOnly))
    {
        return false;
    }

    QHttpPart imagePart;
    QString imagePartHeader = QLatin1String("form-data; name=\"image\"; filename=\"") +
                              QFileInfo(imgPath).fileName() + QLatin1Char('"');

    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, imagePartHeader);
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("image/jpeg"));

    imagePart.setBodyDevice(file);
    multiPart->append(imagePart);

    QString content = QLatin1String("multipart/form-data;boundary=") + QString::fromUtf8(multiPart->boundary());
    QNetworkRequest netRequest(url);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, content);

    d->reply = d->netMngr->post(netRequest, multiPart);

    // delete the multiPart and file with the reply

    multiPart->setParent(d->reply);
    d->state = Private::P_ADDPIN;

    return true;
}

void PTalker::slotFinished(QNetworkReply* reply)
{
    if (reply != d->reply)
    {
        return;
    }

    d->reply = nullptr;

    if (reply->error() != QNetworkReply::NoError)
    {
        if (d->state != Private::P_CREATEBOARD)
        {
            emit signalBusy(false);
            QMessageBox::critical(QApplication::activeWindow(),
                                  i18n("Error"), reply->errorString());
/*
            qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Error content: " << QString(reply->readAll());
*/
            reply->deleteLater();
            return;
        }
    }

    QByteArray buffer = reply->readAll();
/*
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "BUFFER" << QString(buffer);
*/
    switch (d->state)
    {
        case Private::P_LISTBOARDS:
            qCDebug(DIGIKAM_WEBSERVICES_LOG) << "In P_LISTBOARDS";
            parseResponseListBoards(buffer);
            break;

        case Private::P_CREATEBOARD:
            qCDebug(DIGIKAM_WEBSERVICES_LOG) << "In P_CREATEBOARD";
            parseResponseCreateBoard(buffer);
            break;

        case Private::P_ADDPIN:
            qCDebug(DIGIKAM_WEBSERVICES_LOG) << "In P_ADDPIN";
            parseResponseAddPin(buffer);
            break;

        case Private::P_USERNAME:
            qCDebug(DIGIKAM_WEBSERVICES_LOG) << "In P_USERNAME";
            parseResponseUserName(buffer);
            break;

        case Private::P_ACCESSTOKEN:
            qCDebug(DIGIKAM_WEBSERVICES_LOG) << "In P_ACCESSTOKEN";
            parseResponseAccessToken(buffer);
            break;

        default:
            break;
    }

    reply->deleteLater();
}

void PTalker::parseResponseAccessToken(const QByteArray& data)
{
    QJsonDocument doc      = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = doc.object();
    d->accessToken         = jsonObject[QLatin1String("access_token")].toString();

    if (!d->accessToken.isEmpty())
    {
        qDebug(DIGIKAM_WEBSERVICES_LOG) << "Access token Received: " << d->accessToken;
        emit pinterestLinkingSucceeded();
    }
    else
    {
        emit pinterestLinkingFailed();
    }

    emit signalBusy(false);
}

void PTalker::parseResponseAddPin(const QByteArray& data)
{
    QJsonDocument doc      = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = doc.object()[QLatin1String("data")].toObject();
    bool success           = jsonObject.contains(QLatin1String("id"));
    emit signalBusy(false);

    if (!success)
    {
        emit signalAddPinFailed(i18n("Failed to upload Pin"));
    }
    else
    {
        emit signalAddPinSucceeded();
    }
}

void PTalker::parseResponseUserName(const QByteArray& data)
{
    QJsonDocument doc      = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = doc.object()[QLatin1String("data")].toObject();
    d->userName            = jsonObject[QLatin1String("username")].toString();

    emit signalBusy(false);
    emit signalSetUserName(d->userName);
}

void PTalker::parseResponseListBoards(const QByteArray& data)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);

    if (err.error != QJsonParseError::NoError)
    {
        emit signalBusy(false);
        emit signalListBoardsFailed(i18n("Failed to list boards"));
        return;
    }

    QJsonObject jsonObject = doc.object();
/*
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Json Listing Boards : " << doc;
*/
    QJsonArray jsonArray   = jsonObject[QLatin1String("data")].toArray();

    QList<QPair<QString, QString> > list;

    foreach (const QJsonValue& value, jsonArray)
    {
        QString boardID;
        QString boardName;
        QJsonObject obj = value.toObject();
        boardID         = obj[QLatin1String("id")].toString();
        boardName       = obj[QLatin1String("name")].toString();

        list.append(qMakePair(boardID, boardName));
    }

    emit signalBusy(false);
    emit signalListBoardsDone(list);
}

void PTalker::parseResponseCreateBoard(const QByteArray& data)
{
    QJsonDocument doc1     = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = doc1.object();
    bool fail              = jsonObject.contains(QLatin1String("error"));

    emit signalBusy(false);

    if (fail)
    {
        QJsonParseError err;
        QJsonDocument doc2 = QJsonDocument::fromJson(data, &err);
        emit signalCreateBoardFailed(jsonObject[QLatin1String("error_summary")].toString());
    }
    else
    {
        emit signalCreateBoardSucceeded();
    }
}

void PTalker::writeSettings()
{
    d->settings->beginGroup(d->serviceName);
    d->settings->setValue(d->serviceKey, d->accessToken);
    d->settings->endGroup();
}

void PTalker::readSettings()
{
    d->settings->beginGroup(d->serviceName);
    d->accessToken = d->settings->value(d->serviceKey).toString();
    d->settings->endGroup();

    if (d->accessToken.isEmpty())
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Linking...";
        link();
    }
    else
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Already Linked";
        emit pinterestLinkingSucceeded();
    }
}

} // namespace DigikamGenericPinterestPlugin

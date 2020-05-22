/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : a tool to export images to VKontakte web service
 *
 * Copyright (C) 2010      by Thomas McGuire <mcguire at kde dot org>
 * Copyright (C) 2011-2015 by Alexander Potashev <aspotashev at gmail dot com>
 * Copyright (C) 2011-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "vkontakte_authenticationdialog.h"
#include "digikam_config.h"

#ifdef HAVE_QWEBENGINE
#   include <QWebEngineView>
#   include <QWebEnginePage>
#   include <QWebEngineProfile>
#   include <QWebEngineCookieStore>
#else
#   include <qwebview.h>
#endif

#include <QMessageBox>
#include <QTimer>
#include <QUrlQuery>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QDialogButtonBox>

#include <KLocalizedString>

#include "vkontakte_util.h"

namespace Vkontakte
{

class Q_DECL_HIDDEN AuthenticationDialog::Private
{
public:
    QString appId;
    Vkontakte::AppPermissions::Value permissions;
    QString displayMode;
    
#ifdef HAVE_QWEBENGINE
    QWebEngineView*    webView;
#else
    QWebView*          webView;
#endif
    
    QProgressBar *progressBar;

    QString error;
    QString errorDescription;
};

AuthenticationDialog::AuthenticationDialog(QWidget *parent)
    : QDialog(parent)
    , d(new Private)
{
    d->displayMode = QStringLiteral("page");

    setWindowTitle(i18nc("@title:window", "Authenticate with VKontakte"));
    setAttribute(Qt::WA_DeleteOnClose, true);

    QWidget *progressWidget = new QWidget(this);
    QHBoxLayout *progressLayout = new QHBoxLayout(progressWidget);
    progressLayout->setContentsMargins(QMargins());
    
#ifdef HAVE_QWEBENGINE
    d->webView = new QWebEngineView(this);
    d->webView->page()->profile()->cookieStore()->deleteAllCookies();
#else
    d->webView = new QWebView(this);
    d->webView->settings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    d->webView->page()->networkAccessManager()->setCookieJar(new QNetworkCookieJar());
#endif
    
    d->progressBar = new QProgressBar(this);
    d->progressBar->setRange(0, 100);
    QLabel *progressLabel = new QLabel(i18n("Loading Page:"), this);
    progressLayout->addWidget(progressLabel);
    progressLayout->addWidget(d->progressBar);

    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(progressWidget);
    layout->addWidget(d->webView);
    layout->addWidget(buttonBox);
    setLayout(layout);

    connect(buttonBox, &QDialogButtonBox::rejected,
            this, &AuthenticationDialog::canceled);

    connect(d->webView, SIGNAL(urlChanged(QUrl)),
            this, SLOT(urlChanged(QUrl)));
  
    connect(d->webView, SIGNAL(loadStarted()),
            progressWidget, SLOT(show()));

    connect(d->webView, SIGNAL(loadFinished(bool)),
            progressWidget, SLOT(hide()));

    connect(d->webView, SIGNAL(loadProgress(int)),
            d->progressBar, SLOT(setValue(int)));

    connect(d->webView, SIGNAL(loadFinished(bool)),
            this, SLOT(loadFinished(bool)));
}

AuthenticationDialog::~AuthenticationDialog()
{
    delete d;
}

void AuthenticationDialog::setAppId(const QString &appId)
{
    d->appId = appId;
}

void Vkontakte::AuthenticationDialog::setPermissions(Vkontakte::AppPermissions::Value permissions)
{
    d->permissions = permissions;
}

// display= {page, popup, touch, wap}
void AuthenticationDialog::setDisplayMode(const QString &displayMode)
{
    d->displayMode = displayMode;
}

void AuthenticationDialog::start()
{
    Q_ASSERT(!d->appId.isEmpty());

    const QString url = QStringLiteral(
        "http://oauth.vk.com/authorize?"
        "client_id=%1&"
        "scope=%2&"
        "redirect_uri=http://oauth.vk.com/blank.html&"
        "display=%3&"
        "response_type=token")
        .arg(d->appId)
        .arg(appPermissionsToStringList(d->permissions).join(QStringLiteral(",")))
        .arg(d->displayMode);
    qDebug() << "Showing" << url;
    d->webView->setUrl(QUrl::fromUserInput(url));
    show();
}

void AuthenticationDialog::showErrorDialog()
{
    hide();
    const QString details = i18n("<b>VKontakte Error Description:</b> %1<br>"
                                 "<b>VKontakte Error:</b> %2<br>", d->errorDescription, d->error);
    QMessageBox::warning(this, i18n("Authentication with VKontakte was not successful."),
                         details, i18nc("@title:window", "Authentication Problem"));
    emit canceled();
    close();
}

void AuthenticationDialog::urlChanged(const QUrl &url)
{
    qDebug() << "Navigating to" << url;
    if (url.host() == QStringLiteral("oauth.vk.com") && url.path() == QStringLiteral("/blank.html"))
    {
        const QUrlQuery query(url);

        d->error = query.queryItemValue(QStringLiteral("error"));
        d->errorDescription = query.queryItemValue(QStringLiteral("error_description")).replace(QLatin1Char('+'), QLatin1Char(' '));
        if (!d->error.isEmpty() || !d->errorDescription.isEmpty())
        {
            QTimer::singleShot(0, this, SLOT(showErrorDialog()));
            return;
        }

        // The URL comes in the form "bla#access_token=bla&expires_in=foo", we need to convert from
        // # to ?
        const QUrl fixedURL = QUrl::fromUserInput(url.toString().replace(QLatin1Char('#'), QLatin1Char('?')));
        const QUrlQuery fixedQuery(fixedURL);
        const QString accessToken = fixedQuery.queryItemValue(QStringLiteral("access_token"));
        const QString tokenExpiresIn = fixedQuery.queryItemValue(QStringLiteral("expires_in")); // TODO: use this for something?
        if (!accessToken.isEmpty())
        {
            emit authenticated(accessToken);
            QTimer::singleShot(0, this, SLOT(close()));
        }
    }
}

void AuthenticationDialog::loadFinished(bool ok)
{
    if (!ok)
    {
        hide();

        QMessageBox::critical(parentWidget(),
                              i18n("There was a network error when trying to authenticate with VKontakte web service."),
                              i18nc("@title:window", "Network Error"));

        emit canceled();
        close();
    }
}

} /* namespace Vkontakte */

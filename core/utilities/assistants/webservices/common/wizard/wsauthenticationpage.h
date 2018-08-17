/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2018-07-02
 * Description : embedded web browser for web service authentication
 *
 * Copyright (C) 2018 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_WS_AUTHENTICATION_PAGE_H
#define DIGIKAM_WS_AUTHENTICATION_PAGE_H

#include "digikam_config.h"

// Qt includes

#include <QByteArray>
#include <QString>
#include <QUrl>
#include <QMap>

#ifdef HAVE_QWEBENGINE
#   include <QWebEngineView>
#   include <QWebEnginePage>
#   include <QWebEngineProfile>
#   include <QWebEngineCookieStore>
#else
#   include <qwebview.h>
#   include <qwebpage.h>
#   include <qwebframe.h>
#   include <QNetworkRequest>
#endif

// Local includes

#include "digikam_export.h"
#include "dwizardpage.h"
#include "dinfointerface.h"
#include "wsauthentication.h"

namespace Digikam
{

#ifdef HAVE_QWEBENGINE
class WSAuthenticationPage : public QWebEnginePage
#else
class WSAuthenticationPage : public QWebPage
#endif // #ifdef HAVE_QWEBENGINE
{
    Q_OBJECT

public:

#ifdef HAVE_QWEBENGINE
    explicit WSAuthenticationPage(QObject* const parent, QWebEngineProfile* profile, QString callbackUrl);
#else
    explicit WSAuthenticationPage(QObject* const parent, QString callbackUrl);
#endif // #ifdef HAVE_QWEBENGINE

    virtual ~WSAuthenticationPage();

    void setCallbackUrl(const QString& url);

#ifdef HAVE_QWEBENGINE
    bool acceptNavigationRequest(const QUrl& url, QWebEnginePage::NavigationType type, bool isMainFrame);
#else
private Q_SLOTS:
    bool slotUrlChanged(const QUrl& url);
#endif // #ifdef HAVE_QWEBENGINE

Q_SIGNALS:

    void callbackCatched(const QString&);

private:

    QString m_callbackUrl;
};

// -------------------------------------------------------------------

#ifdef HAVE_QWEBENGINE
class WSAuthenticationPageView : public QWebEngineView
#else
class WSAuthenticationPageView : public QWebView
#endif
{
    Q_OBJECT

public:

    explicit WSAuthenticationPageView(QWidget* const parent, 
                                      WSAuthentication* const wsAuth,
                                      QString callbackUrl);
    ~WSAuthenticationPageView();

    bool authenticationComplete() const;

private:

    /*
     * Parse url into a hash map with key and value for each parameter
     */
    QMap<QString, QString> parseUrlFragment(const QString& urlFragment);

Q_SIGNALS:

    void signalAuthenticationComplete(bool);

private Q_SLOTS:

    void slotCallbackCatched(const QString& callbackUrl); 
    void slotOpenBrowser(const QUrl& url);
    void slotCloseBrowser();

private:

    WSAuthentication*   m_WSAuthentication;
};

// -------------------------------------------------------------------

class WSAuthenticationWizard : public DWizardPage
{
    Q_OBJECT

public:

    explicit WSAuthenticationWizard(QWizard* const dialog, const QString& title,
                                    const QString& callback = QLatin1String("http://127.1.1.0:8000/"));
    ~WSAuthenticationWizard();

    bool isComplete() const;

    void initializePage();
    bool validatePage();
    void cleanupPage();

public Q_SLOTS:

    void slotAuthenticationComplete(bool isLinked);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_WS_AUTHENTICATION_PAGE_H

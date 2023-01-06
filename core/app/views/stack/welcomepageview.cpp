/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-12-20
 * Description : a widget to display a welcome page
 *               on root album.
 *
 * Copyright (C) 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#include "welcomepageview.h"

// Qt includes

#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <QWidget>
#include <QApplication>
#include <QStandardPaths>

#ifdef HAVE_QWEBENGINE
#   include <QtWebEngineWidgetsVersion>
#endif

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_version.h"
#include "daboutdata.h"
#include "thememanager.h"
#include "webbrowserdlg.h"

namespace Digikam
{

#ifdef HAVE_QWEBENGINE

WelcomePageViewPage::WelcomePageViewPage(QObject* const parent)
    : QWebEnginePage(parent)
{
}

WelcomePageViewPage::~WelcomePageViewPage()
{
}

bool WelcomePageViewPage::acceptNavigationRequest(const QUrl& url, QWebEnginePage::NavigationType type, bool)
{
    if (type == QWebEnginePage::NavigationTypeLinkClicked)
    {
        emit linkClicked(url);

        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------

WelcomePageView::WelcomePageView(QWidget* const parent)
    : QWebEngineView(parent)

#else

WelcomePageView::WelcomePageView(QWidget* const parent)
    : QWebView(parent)

#endif

{
    setFocusPolicy(Qt::WheelFocus);

#ifndef HAVE_QWEBENGINE

    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    setRenderHint(QPainter::TextAntialiasing);

#endif

    setContextMenuPolicy(Qt::NoContextMenu);
    setContentsMargins(QMargins());

#ifdef HAVE_QWEBENGINE

    WelcomePageViewPage* const wpage = new WelcomePageViewPage(this);
    setPage(wpage);

#   if QTWEBENGINEWIDGETS_VERSION >= QT_VERSION_CHECK(5, 7, 0)

    settings()->setAttribute(QWebEngineSettings::WebGLEnabled, false);
    settings()->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, false);

#   endif

#endif

    // ------------------------------------------------------------

#ifdef HAVE_QWEBENGINE

    connect(wpage, SIGNAL(linkClicked(QUrl)),
            this, SLOT(slotUrlOpen(QUrl)));

#else

    connect(this, SIGNAL(linkClicked(QUrl)),
            this, SLOT(slotUrlOpen(QUrl)));

#endif

    connect(ThemeManager::instance(), SIGNAL(signalThemeChanged()),
            this, SLOT(slotThemeChanged()));

    QTimer::singleShot(0, this, SLOT(slotThemeChanged()));
}

WelcomePageView::~WelcomePageView()
{
}

void WelcomePageView::slotUrlOpen(const QUrl& url)
{
    WebBrowserDlg* const browser = new WebBrowserDlg(url, qApp->activeWindow());
    browser->show();
}

QStringList WelcomePageView::featuresTabContent() const
{
    QStringList newFeatures;
    newFeatures << i18n("More than 1300 user-reports closed on bugzilla.");
    newFeatures << i18n("New Neural Network engine based on OpenCV Deep Learning module to detect and recognize faces.");
    newFeatures << i18n("Update Slideshow to plugin with new shuffle mode.");
    newFeatures << i18n("Add new theme Html5Responsive for HtmlGallery plugin.");
    newFeatures << i18n("New plugin ImageMosaicWall to create an image based on a bunch of other photos.");
    newFeatures << i18n("add new batch queue manager tool to fix hot pixels from images.");
    newFeatures << i18n("add new batch queue manager tool to apply texture over images.");
    newFeatures << i18n("Face Scan dialog contents is now simplified and embedded into left sidebar.");
    newFeatures << i18n("Add support for Gimp XCF files >= 2.10.");
    newFeatures << i18n("Add Flatpak bundle support for Linux platforms.");
    newFeatures << i18n("Table-view is now able to show digiKam Tag-Paths properties.");
    newFeatures << i18n("Add option to write geolocation information in file metadata.");
    newFeatures << i18n("Add color label support for advanced metadata settings.");
    newFeatures << i18n("Add Exif, Iptc, and Xmp read support for HEIF images.");
    newFeatures << i18n("Improved support for UNC network paths under Windows.");
    newFeatures << i18n("Improved support for Unicode paths under Windows.");
    newFeatures << i18n("Apple bundle is now relocatable and compatible with macOS BigSur.");
    newFeatures << i18n("New online version checker and downloader to automate upgrade under Windows and macOS.");
    newFeatures << i18n("Better support of astro FITS image format including metadata extraction support to populate the database.");
    newFeatures << i18n("Use last Exiv2 0.27.5 in all bundles with Base Media File metadata read and write support (HEIF, AVIF).");
    newFeatures << i18n("Find duplicates items is now process in parallel using multi-cores.");
    newFeatures << i18n("New tool to export items to iNaturalist web service.");
    newFeatures << i18n("New metadata viewer based on ExifTool.");
    newFeatures << i18n("MacOS package, Windows installer, and AppImage bundle use KF5 5.89 and KAPPS 21.12.0 frameworks.");
    newFeatures << i18n("AppImage Linux bundle now support <a href=\"https://fcitx-im.org/wiki/Fcitx_5\">Platform Input Context FCITX</a>.");
    newFeatures << i18n("Improve RTL translations support.");
    newFeatures << i18n("GUI internationalization and localization updates with more than 50 languages available.");
    newFeatures << i18n("Add OpenWith contextual menu support under MacOS.");
    newFeatures << i18n("Integration of Google Summer of Code project 2021 "
                        "<a href=\"https://community.kde.org/GSoC/2021/StatusReports/PhuocKhanhLE\">Image Quality Sorter</a>.");
    newFeatures << i18n("New tool to share items on the network with a Motion JPEG stream server.");
    newFeatures << i18n("Showfoto has a new left sidebar hosting a folder-view to quickly explore images from local file system.");
    newFeatures << i18n("Update internal RAW engine to last stable Libraw 0.21.1.");
    newFeatures << i18n("New RAW features: "
                        "Phase One/Leaf IIQ-S v2 support "
                        "Canon CR3 filmrolls/RawBurst "
                        "Canon CRM (movie) files "
                        "Tiled bit-packed (and 16-bit unpacked) DNGs "
                        "(non-standard) Deflate-compressed integer DNG files are allowed "
                        "Canon EOS R3, R7 and R10 "
                        "Fujifilm X-H2S, X-T30 II "
                        "OM System OM-1 "
                        "Leica M11 "
                        "Sony A7-IV (ILCE-7M4) "
                        "DJI Mavic 3");

    // Add new features here...

    newFeatures << i18n("...and much more.");

    QString featureItems;

    for (int i = 0 ; i < newFeatures.count() ; ++i)
    {
        featureItems += i18n("<li>%1</li>\n", newFeatures.at(i));
    }

    QString tabHeader  = i18n("New Features");
    QString tabContent =
        i18n("<h3>%1</h3><ul>%2</ul>",
             i18n("Some of the new features in this release of digiKam include (compared to digiKam 6.x):"),
             featureItems);

    return QStringList() << tabHeader << tabContent;
}

QStringList WelcomePageView::aboutTabContent() const
{
    QString tabHeader  = i18n("About");
    QString tabContent =
        i18n("<h3>%1</h3><h3>%2</h3><ul>%3</ul>",
             i18n("digiKam is an open source photo management program designed to import, organize, enhance, search and export your digital images to and from your computer."),
             i18n("Currently, you are in the Album view mode of digiKam. Albums are the places where your files are stored, and are identical to the folders on your hard disk."),
             i18n("<li>%1</li><li>%2</li>",
                  i18n("digiKam has many powerful features which are described in the <a href=\"https://docs.digikam.org/en/index.html\">documentation</a>"),
                  i18n("The <a href=\"https://www.digikam.org\">digiKam homepage</a> provides information about new versions of digiKam.")));

    return QStringList() << tabHeader << tabContent;
}

QByteArray WelcomePageView::fileToString(const QString& aFileName) const
{
    QByteArray   result;
    QFileInfo    info(aFileName);
    unsigned int readLen;
    unsigned int len = info.size();
    QFile        file(aFileName);

    if (aFileName.isEmpty() || (len == 0)   ||
        !info.exists()      || info.isDir() || !info.isReadable() ||
        !file.open(QIODevice::Unbuffered|QIODevice::ReadOnly))
    {
        return QByteArray();
    }

    result.resize(len + 2);
    readLen = file.read(result.data(), len);
    file.close();

    if (result[len-1] != '\n')
    {
        result[len++] = '\n';
        ++readLen;
    }

    result[len] = '\0';

    if (readLen < len)
    {
        return QByteArray();
    }

    return result;
}

void WelcomePageView::slotThemeChanged()
{
    QString appTitle         = i18n("digiKam");
    QString slogan           = DAboutData::digiKamSlogan();
    QString locationHtml     = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("digikam/about/main.html"));
    QString content          = QString::fromUtf8(fileToString(locationHtml));

    content                  = content.arg((layoutDirection() == Qt::RightToLeft) ? QLatin1String("rtl") : QLatin1String("ltr")) // For RTL issue. See bug #376438
                                      .arg(appTitle)
                                      .arg(slogan)
                                      .arg(i18n("Welcome to digiKam %1", QLatin1String(digikam_version)))
                                      .arg(featuresTabContent().value(0))
                                      .arg(aboutTabContent().value(0))
                                      .arg(i18n("Background Image Credits"))
                                      .arg(featuresTabContent().value(1))
                                      .arg(aboutTabContent().value(1))
                                      .arg(i18n("Author:"))
                                      .arg(i18n("Location:"))
                                      .arg(i18n("Date:"))
                                      .arg(i18n("Camera:"))
                                      .arg(i18n("Lens:"))
    ;

    //qCDebug(DIGIKAM_GENERAL_LOG) << content;

    setHtml(content, QUrl::fromLocalFile(locationHtml));
}

} // namespace Digikam

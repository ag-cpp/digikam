/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-07-11
 * Description : shared libraries list dialog
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "libsinfodlg.h"
#include "digikam_config.h"

// Qt includes

#include <QStringList>
#include <QString>
#include <QTreeWidget>
#include <QHeaderView>
#include <QThreadPool>

#ifdef HAVE_QWEBENGINE
#   include <QtWebEngineWidgetsVersion>
#else
#   include <QtWebKitWidgetsVersion>
#endif

// KDE includes

#include <kxmlgui_version.h>
#include <klocalizedstring.h>

// Local includes

#include "drawdecoder.h"
#include "greycstorationfilter.h"
#include "pgfutils.h"
#include "digikam-lcms.h"
#include "metaengine.h"
#include "dngwriter.h"
#include "exiftoolparser.h"

#ifdef HAVE_LENSFUN
#   include "lensfuniface.h"
#endif // HAVE_LENSFUN

#ifdef HAVE_MARBLE
#   include "mapwidget.h"
#endif // HAVE_MARBLE

#ifdef HAVE_IMAGE_MAGICK

// Pragma directives to reduce warnings from ImageMagick header files.
#   if defined(Q_CC_GNU)
#       pragma GCC diagnostic push
#       pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#   endif

#   if defined(Q_CC_CLANG)
#       pragma clang diagnostic push
#       pragma clang diagnostic ignored "-Wkeyword-macro"
#   endif

#   include <Magick++.h>

// Restore warnings

#   if defined(Q_CC_CLANG)
#       pragma clang diagnostic pop
#   endif

#   if defined(Q_CC_GNU)
#       pragma GCC diagnostic pop
#   endif

#endif

// C ANSI includes

#ifndef Q_OS_WIN
extern "C"
{
#endif

#ifdef HAVE_JASPER
#   include <jasper/jas_version.h>
#endif // HAVE_JASPER

#include <png.h>
#include <tiffvers.h>

// Avoid Warnings under Win32
#undef HAVE_STDLIB_H
#undef HAVE_STDDEF_H
#include <jpeglib.h>

#ifndef Q_OS_WIN
}
#endif

namespace Digikam
{

LibsInfoDlg::LibsInfoDlg(QWidget* const parent)
    : InfoDlg(parent)
{
    setWindowTitle(i18nc("@title", "Shared Libraries and Components Information"));

    static const char* CONTEXT         = "@item: Component information, see help->components";
    static const QString SUPPORTED_YES = i18nc("@item: component is supported/available",     "Yes");
    static const QString SUPPORTED_NO  = i18nc("@item: component is not available/supported", "No");

    listView()->setHeaderLabels(QStringList() << QLatin1String("Properties") << QLatin1String("Value")); // Hidden header -> no i18n
    listView()->setSortingEnabled(true);
    listView()->setRootIsDecorated(true);
    listView()->setSelectionMode(QAbstractItemView::SingleSelection);
    listView()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    listView()->setAllColumnsShowFocus(true);
    listView()->setColumnCount(2);
    listView()->header()->setSectionResizeMode(QHeaderView::Stretch);
    listView()->header()->hide();

    // NOTE: by default set a list of common components information used by Showfoto and digiKam.

    m_features = new QTreeWidgetItem(listView(), QStringList() << i18n("Features"));
    listView()->addTopLevelItem(m_features);

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Parallelized demosaicing") <<       checkTriState(DRawDecoder::librawUseGomp()));

#ifdef HAVE_QTXMLPATTERNS
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Rajce support") <<                  SUPPORTED_YES);
#else
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Rajce support") <<                  SUPPORTED_YES);
#endif

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Exiv2 supports XMP metadata") <<    (MetaEngine::supportXmp() ?  SUPPORTED_YES : SUPPORTED_NO));

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Exiv2 supports Base Media") <<      (MetaEngine::supportBmff() ? SUPPORTED_YES : SUPPORTED_NO));

    // ---

    m_libraries = new QTreeWidgetItem(listView(), QStringList() << i18n("Libraries"));
    listView()->addTopLevelItem(m_libraries);


    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "Qt") <<                             QLatin1String(qVersion()));

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "KF5") <<                            QLatin1String(KXMLGUI_VERSION_STRING));

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LibRaw") <<                         DRawDecoder::librawVersion());

#ifdef HAVE_EIGEN3
    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "Eigen") <<                          QLatin1String(EIGEN3_VERSION_STRING));
#else
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Eigen support") <<                  SUPPORTED_NO);
#endif // HAVE_EIGEN3

#ifdef HAVE_QWEBENGINE
    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "Qt WebEngine version") <<           QLatin1String(QTWEBENGINEWIDGETS_VERSION_STR));
#else
    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "Qt WebKit version") <<              QLatin1String(QTWEBKITWIDGETS_VERSION_STR));
#endif

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "Exiv2") <<                          MetaEngine::Exiv2Version());

    ExifToolParser* const parser = new ExifToolParser(this);
    ExifToolParser::ExifToolData parsed;

    if (parser->version())
    {
        parsed            = parser->currentData();
        QString etVersion = parsed.find(QLatin1String("VERSION_STRING")).value()[0].toString();
        new QTreeWidgetItem(m_libraries, QStringList() <<
                            i18nc(CONTEXT, "ExifTool") <<                   etVersion);
    }
    else
    {
        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc(CONTEXT, "ExifTool support") <<           SUPPORTED_NO);
    }

#ifdef HAVE_LENSFUN
    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LensFun") <<                        LensFunIface::lensFunVersion());
#else
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "LensFun support") <<                SUPPORTED_NO);
#endif // HAVE_LENSFUN

#ifdef HAVE_LIBLQR_1
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "LibLqr support") <<                 SUPPORTED_YES);
#else
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "LibLqr support") <<                 SUPPORTED_NO);
#endif // HAVE_LIBLQR_1

#ifdef HAVE_VKONTAKTE
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "VKontakte support") <<              SUPPORTED_YES);
#else
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "VKontakte support") <<              SUPPORTED_NO);
#endif // HAVE_VKONTAKTE

#ifdef HAVE_IMAGE_MAGICK
    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "ImageMagick codecs") <<             QLatin1String(MagickLibVersionText));
#else
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "ImageMagick codecs support") <<     SUPPORTED_NO);
#endif // HAVE_IMAGE_MAGICK

#ifdef HAVE_X265
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "HEIF encoding support") <<          SUPPORTED_YES);
#else
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "HEIF encoding support") <<          SUPPORTED_NO);
#endif // HAVE_X265

    QString tiffver = QLatin1String(TIFFLIB_VERSION_STR);
    tiffver         = tiffver.left(tiffver.indexOf(QLatin1Char('\n')));
    tiffver         = tiffver.section(QLatin1Char(' '), 2, 2);
    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LibTIFF") <<                        tiffver);

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LibPNG") <<                         QLatin1String(PNG_LIBPNG_VER_STRING));

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LibJPEG") <<                        QString::number(JPEG_LIB_VERSION));

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LibCImg") <<                        GreycstorationFilter::cimgVersionString());

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LibLCMS") <<                        QString::number(LCMS_VERSION));

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LibPGF") <<                         PGFUtils::libPGFVersion());

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "XMP SDK") <<                        DNGWriter::xmpSdkVersion());

    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "DNG SDK") <<                        DNGWriter::dngSdkVersion());

#ifdef HAVE_JASPER
    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "LibJasper") <<                      QLatin1String(jas_getversion()));
#else
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "LibJasper support") <<              SUPPORTED_NO);
#endif // HAVE_JASPER

#ifdef HAVE_MARBLE
    new QTreeWidgetItem(m_libraries, QStringList() <<
                        i18nc(CONTEXT, "Marble") <<                         MapWidget::MarbleWidgetVersion());
#else
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(CONTEXT, "Marble support") <<                 SUPPORTED_NO);
#endif // HAVE_MARBLE

    // TODO: add sqlite versions here? Could be useful for debugging sqlite problems..

    int nbcore = QThreadPool::globalInstance()->maxThreadCount();
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18ncp(CONTEXT, "CPU core", "CPU cores", nbcore) << QString::fromLatin1("%1").arg(nbcore));

    // TODO: add free memory reported by kmemoryinfo at startup
}

LibsInfoDlg::~LibsInfoDlg()
{
}

QString LibsInfoDlg::checkTriState(int value) const
{
    switch (value)
    {
        case true:
        {
            return i18nc("@info: tri state", "Yes");
        }

        case false:
        {
            return i18nc("@info: tri state", "No");
        }

        default:
        {
            return i18nc("@info: tri state", "Unknown");
        }
    }
}

} // namespace Digikam

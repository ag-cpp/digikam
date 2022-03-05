/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-07-30
 * Description : digiKam components info dialog.
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

#include "componentsinfodlg.h"

// Qt includes

#include <QApplication>
#include <QString>
#include <QMap>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "applicationsettings.h"
#include "digikam_config.h"
#include "libsinfodlg.h"
#include "dbstatdlg.h"
#include "digikam_opencv.h"

// LibGphoto2 includes

#ifdef HAVE_GPHOTO2

extern "C"
{
#   include <gphoto2-version.h>
}

#endif

// QtAV includes

#ifdef HAVE_MEDIAPLAYER
#    include <QtAV/version.h>

extern "C"
{
#   include <libavformat/version.h>
#   include <libavutil/version.h>
#   include <libavcodec/version.h>
}

#endif

namespace Digikam
{

class ComponentsInfoDlg : public LibsInfoDlg
{

public:

    explicit ComponentsInfoDlg(QWidget* const parent)
        : LibsInfoDlg(parent)
    {
        // Set digiKam specific components info list.

#ifdef HAVE_GPHOTO2

        new QTreeWidgetItem(m_libraries, QStringList() <<
                            i18nc("@item: component info", "LibGphoto2") <<             QLatin1String(gp_library_version(GP_VERSION_SHORT)[0]));

#else

        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc("@item: component info", "LibGphoto2 support") <<     i18nc("@item: component info", "No"));

#endif

#ifdef HAVE_KFILEMETADATA

        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc("@item: component info", "Baloo support") <<          i18nc("@item: component info", "Yes"));

#else

        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc("@item: component info", "Baloo support") <<          i18nc("@item: component info", "No"));

#endif

#ifdef HAVE_AKONADICONTACT

        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc("@item: component info", "AddressBook support") <<    i18nc("@item: component info", "Yes"));

#else

        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc("@item: component info", "AddressBook support") <<    i18nc("@item: component info", "No"));

#endif

#ifdef HAVE_MEDIAPLAYER

        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc("@item: component info", "Media player support") <<   i18nc("@item: component info", "Yes"));

        new QTreeWidgetItem(m_libraries, QStringList() <<
                            i18nc("@item: component info", "LibQtAV") <<                QString::fromLatin1("%1.%2.%3").arg(QTAV_MAJOR).arg(QTAV_MINOR).arg(QTAV_PATCH));

        new QTreeWidgetItem(m_libraries, QStringList() <<
                            i18nc("@item: component info", "LibAVFormat") <<            QString::fromLatin1("%1.%2.%3").arg(LIBAVFORMAT_VERSION_MAJOR).arg(LIBAVFORMAT_VERSION_MINOR).arg(LIBAVFORMAT_VERSION_MICRO));

        new QTreeWidgetItem(m_libraries, QStringList() <<
                            i18nc("@item: component info", "LibAVCodec") <<             QString::fromLatin1("%1.%2.%3").arg(LIBAVCODEC_VERSION_MAJOR).arg(LIBAVCODEC_VERSION_MINOR).arg(LIBAVCODEC_VERSION_MICRO));

        new QTreeWidgetItem(m_libraries, QStringList() <<
                            i18nc("@item: component info", "LibAVUtil") <<              QString::fromLatin1("%1.%2.%3").arg(LIBAVUTIL_VERSION_MAJOR).arg(LIBAVUTIL_VERSION_MINOR).arg(LIBAVUTIL_VERSION_MICRO));

#else

        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc("@item: component info", "Media player support") <<   i18nc("@item: component info", "No"));

#endif

#ifdef HAVE_DBUS

        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc("@item: component info", "DBus support") <<           i18nc("@item: component info", "Yes"));

#else

        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc("@item: component info", "DBus support") <<           i18nc("@item: component info", "No"));

#endif

#ifdef HAVE_PANORAMA

        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc("@item: component info", "Panorama support") <<       i18nc("@item: component info", "Yes"));

#else

        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc("@item: component info", "Panorama support") <<       i18nc("@item: component info", "No"));

#endif

#ifdef HAVE_HTMLGALLERY

        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc("@item: component info", "HTML Gallery support") <<   i18nc("@item: component info", "Yes"));

#else

        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc("@item: component info", "HTML Gallery support") <<   i18nc("@item: component info", "No"));

#endif

#ifdef HAVE_KCALENDAR

        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc("@item: component info", "Calendar support") <<       i18nc("@item: component info", "Yes"));

#else

        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc("@item: component info", "Calendar support") <<       i18nc("@item: component info", "No"));

#endif

        new QTreeWidgetItem(m_libraries, QStringList() <<
                            i18nc("@item: component info", "LibOpenCV") <<              QLatin1String(CV_VERSION));

        // Database Backend information
        // TODO: add sqlite versions here? Could be useful for debugging sqlite problems..
        // Use this sqlite request; https://www.sqlite.org/lang_corefunc.html#sqlite_version

        QString dbBe = ApplicationSettings::instance()->getDbEngineParameters().databaseType;
        new QTreeWidgetItem(m_features, QStringList() <<
                            i18nc("@item: component info", "Database backend") <<       dbBe);

        if (dbBe != QLatin1String("QSQLITE"))
        {
            QString internal = ApplicationSettings::instance()->getDbEngineParameters().internalServer ? i18nc("@item: component info", "Yes")
                                                                                                       : i18nc("@item: component info", "No");
            new QTreeWidgetItem(m_features, QStringList() <<
                                i18nc("@item: component info", "Database internal server") << internal);
        }
    }

    ~ComponentsInfoDlg()
    {
    }
};

// --------------------------------------------------------

void showDigikamComponentsInfo()
{
    ComponentsInfoDlg* const dlg = new ComponentsInfoDlg(qApp->activeWindow());
    dlg->show();
}

void showDigikamDatabaseStat()
{
    DBStatDlg* const dlg = new DBStatDlg(qApp->activeWindow());
    dlg->show();
}

} // namespace Digikam

/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-09-08
 * Description : digiKam debug spaces
 *
 * SPDX-FileCopyrightText: 2014      by Laurent Montel <montel at kde dot org>
 * SPDX-FileCopyrightText: 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * SPDX-FileCopyrightText: 2014-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QLoggingCategory>

// Local includes

#include "digikam_export.h"

DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_GENERAL_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_WIDGETS_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_IOJOB_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_SHOWFOTO_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_WEBSERVICES_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DATABASESERVER_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_IMPORTUI_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_METAENGINE_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_FACESENGINE_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_AUTOTAGSENGINE_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_RAWENGINE_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_GEOIFACE_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_MARBLE_LOG)

// Tools and unit-tests debug space (to use only on core/tests)

DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_TESTS_LOG)

// DPlugin debug spaces

DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DPLUGIN_RAWIMPORT_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DPLUGIN_GENERIC_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DPLUGIN_EDITOR_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DPLUGIN_BQM_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DPLUGIN_LOG)

// Databases debug spaces

DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DATABASE_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DBENGINE_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DBJOB_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_COREDB_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_FACEDB_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_THUMBSDB_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_SIMILARITYDB_LOG)

// DImg debug spaces

DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DIMG_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DIMG_LOG_JPEG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DIMG_LOG_JP2K)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DIMG_LOG_PGF)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DIMG_LOG_PNG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DIMG_LOG_PPM)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DIMG_LOG_TIFF)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DIMG_LOG_RAW)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DIMG_LOG_QIMAGE)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DIMG_LOG_HEIF)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_DIMG_LOG_MAGICK)

// MediaServer debug spaces

DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_MEDIASRV_LOG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_MEDIASRV_LOG_DEBUG)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_MEDIASRV_LOG_INFO)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_MEDIASRV_LOG_WARN)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_MEDIASRV_LOG_CRITICAL)
DIGIKAM_EXPORT Q_DECLARE_LOGGING_CATEGORY(DIGIKAM_MEDIASRV_LOG_FATAL)

/**
 * Static method to tune debug traces at run-time.
 */
DIGIKAM_EXPORT void digikamSetDebugFilterRules(bool on);

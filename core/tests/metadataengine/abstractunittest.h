/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-11-03
 * Description : An abstract digiKam unit-test template.
 *
 * SPDX-FileCopyrightText: 2019-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ABSTRACT_UNIT_TEST_H
#define DIGIKAM_ABSTRACT_UNIT_TEST_H

// Qt includes

#include <QObject>
#include <QDir>
#include <QTest>
#include <QString>

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"
#include "dmetadata.h"
#include "wstoolutils.h"
#include "exiftoolparser.h"

#ifdef HAVE_IMAGE_MAGICK
#   include <Magick++.h>
using namespace Magick;
#endif

using namespace Digikam;

class AbstractUnitTest : public QObject
{
    Q_OBJECT

public:

    AbstractUnitTest(QObject* const parent = nullptr)
        : QObject              (parent),
          m_hasExifTool        (false),
          m_originalImageFolder(QFINDTESTDATA("data/")) ///< Original files come with source code.
    {
    }

protected Q_SLOTS:

    /// Re-implemented from QTest framework

    virtual void initTestCase()
    {

#ifdef HAVE_IMAGE_MAGICK

        qCDebug(DIGIKAM_TESTS_LOG) << "Init ImageMagick";
        InitializeMagick(nullptr);

#endif

        MetaEngine::initializeExiv2();
        qCDebug(DIGIKAM_TESTS_LOG) << "Using Exiv2 Version:" << MetaEngine::Exiv2Version();
        m_tempPath = QString::fromLatin1(QTest::currentAppName());
        m_tempPath.replace(QLatin1String("./"), QString());

        QScopedPointer<ExifToolParser> const parser(new ExifToolParser(nullptr));
        m_hasExifTool = parser->exifToolAvailable();
    }

    /// Re-implemented from QTest framework

    virtual void init()
    {
        m_tempDir = WSToolUtils::makeTemporaryDir(m_tempPath.toLatin1().data());
    }

    /// Re-implemented from QTest framework

    virtual void cleanup()
    {
        WSToolUtils::removeTemporaryDir(m_tempPath.toLatin1().data());

#ifdef HAVE_IMAGE_MAGICK
#   if MagickLibVersion >= 0x693

        qCDebug(DIGIKAM_TESTS_LOG) << "Terminate ImageMagick";
        TerminateMagick();

#   endif
#endif

    }

    /// Re-implemented from QTest framework

    virtual void cleanupTestCase()
    {
    }

protected:

    QString       m_tempPath;               ///< The temporary path to store file to process un unit test.
    QDir          m_tempDir;                ///< Same that previous as QDir object.
    bool          m_hasExifTool;            ///< ExifTool is available in unit test.
    const QString m_originalImageFolder;    ///< The path to original files to process by unit test, and copied to the temporary directory. Original files still in read only.
};

#endif // DIGIKAM_ABSTRACT_UNIT_TEST_H

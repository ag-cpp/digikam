/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface.
 *               Internal private container.
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_META_ENGINE_PRIVATE_H
#define DIGIKAM_META_ENGINE_PRIVATE_H

#include "metaengine.h"

 // C++ includes

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cfloat>
#include <iostream>
#include <iomanip>
#include <string>

// Qt includes

#include <QFile>
#include <QSize>
#include <QLatin1String>
#include <QFileInfo>
#include <QSharedData>
#include <QMutexLocker>

// Exiv2 includes -------------------------------------------------------

// NOTE: All Exiv2 headers must be stay there to not expose external source code to Exiv2 API
//       and reduce Exiv2 dependency to client code.

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

// The pragmas are required to be able to catch exceptions thrown by libexiv2:
// See http://gcc.gnu.org/wiki/Visibility, the section about c++ exceptions.
// They are needed for all libexiv2 versions that do not care about visibility.

#ifdef Q_CC_GNU
#   pragma GCC visibility push(default)
#endif

#include <exiv2/exv_conf.h>
#include <exiv2/error.hpp>
#include <exiv2/image.hpp>
#include <exiv2/jpgimage.hpp>
#include <exiv2/datasets.hpp>
#include <exiv2/tags.hpp>
#include <exiv2/preview.hpp>
#include <exiv2/properties.hpp>
#include <exiv2/types.hpp>
#include <exiv2/exif.hpp>
#include <exiv2/xmpsidecar.hpp>
#include <exiv2/version.hpp>

// Check if Exiv2 support XMP

#ifdef EXV_HAVE_XMP_TOOLKIT
#   define _XMP_SUPPORT_ 1
#endif

#ifndef EXIV2_TEST_VERSION
#    define EXIV2_TEST_VERSION(major,minor,patch) \
         ( EXIV2_VERSION >= EXIV2_MAKE_VERSION(major,minor,patch) )
#endif

#if EXIV2_TEST_VERSION(0,27,99)
#   define AutoPtr UniquePtr
#endif

// With exiv2 > 0.20.0, all makernote header files have been removed to increase binary compatibility.
// See Exiv2 bugzilla entry http://dev.exiv2.org/issues/719
// and wiki topic           http://dev.exiv2.org/boards/3/topics/583

#ifdef Q_CC_GNU
#   pragma GCC visibility pop
#endif

// End of Exiv2 headers ------------------------------------------------------

// Local includes

#include "metaengine_mergehelper.h"

namespace Digikam
{

extern QMutex s_metaEngineMutex;

// --------------------------------------------------------------------------

class Q_DECL_HIDDEN MetaEngine::Private
{
public:

    explicit Private();
    ~Private();

    void copyPrivateData(const Private* const other);

    bool saveToXMPSidecar(const QFileInfo& finfo)                                 const;
    bool saveToFile(const QFileInfo& finfo)                                       const;
    bool saveOperations(const QFileInfo& finfo, Exiv2::Image::AutoPtr image)      const;

    /**
     * Wrapper method to convert a Comments content to a QString.
     */
    QString convertCommentValue(const Exiv2::Exifdatum& exifDatum)                const;

    /**
     * Charset autodetection to convert a string to a QString.
     */
    QString detectEncodingAndDecode(const std::string& value)                     const;

    /**
     * UTF8 autodetection from a string.
     */
    bool isUtf8(const char* const buffer)                                         const;

    int getXMPTagsListFromPrefix(const QString& pf, MetaEngine::TagsMap& tagsMap) const;

    const Exiv2::ExifData& exifMetadata()                                         const;
    const Exiv2::IptcData& iptcMetadata()                                         const;
    const std::string&     itemComments()                                         const;

    Exiv2::ExifData&       exifMetadata();
    Exiv2::IptcData&       iptcMetadata();
    std::string&           itemComments();

#ifdef _XMP_SUPPORT_

    const Exiv2::XmpData&  xmpMetadata()                                          const;
    Exiv2::XmpData&        xmpMetadata();

    void loadSidecarData(Exiv2::Image::AutoPtr xmpsidecar);

#endif

    /**
     * Helper method to decode IPTC tag string contents following characters encoding preset.
     */
    QString extractIptcTagString(const Exiv2::IptcData& iptcData, const Exiv2::Iptcdatum& iptcTag) const;

public:

    /**
     * Generic method to print the Exiv2 C++ Exception error message from 'e'.
     * 'msg' string is printed using qDebug rules.
     */
    static void printExiv2ExceptionError(const QString& msg, Exiv2::AnyError& e);

    /**
     * Generic method to print debug message from Exiv2.
     * 'msg' string is printed using qDebug rules. 'lvl' is the debug level of Exiv2 message.
     */
    static void printExiv2MessageHandler(int lvl, const char* msg);

public:

    bool                                                  writeRawFiles;
    bool                                                  writeDngFiles;
    bool                                                  updateFileTimeStamp;

    bool                                                  useXMPSidecar4Reading;
    bool                                                  useCompatibleFileName;

    /// A mode from #MetadataWritingMode enum.
    int                                                   metadataWritingMode;

    /// XMP, and parts of EXIF/IPTC, were loaded from an XMP sidecar file
    bool                                                  loadedFromSidecar;

    QString                                               filePath;
    QSize                                                 pixelSize;
    QString                                               mimeType;

    QExplicitlySharedDataPointer<MetaEngineData::Private> data;
};

} // namespace Digikam

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif

#endif // DIGIKAM_META_ENGINE_PRIVATE_H

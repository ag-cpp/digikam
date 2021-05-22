/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface.
 *               Comments manipulation methods.
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

// Local includes

#include "metaengine_p.h"
#include "digikam_debug.h"
#include "digikam_config.h"

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace Digikam
{

bool MetaEngine::canWriteComment(const QString& filePath)
{
    QMutexLocker lock(&s_metaEngineMutex);

    try
    {
#if defined Q_OS_WIN && defined EXV_UNICODE_PATH

        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((const wchar_t*)filePath.utf16());

#elif defined Q_OS_WIN

        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(QFile::encodeName(filePath).constData());

#else

        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filePath.toUtf8().constData());

#endif

        Exiv2::AccessMode mode      = image->checkMode(Exiv2::mdComment);

        return ((mode == Exiv2::amWrite) || (mode == Exiv2::amReadWrite));
    }
    catch (Exiv2::AnyError& e)
    {
        qCCritical(DIGIKAM_METAENGINE_LOG) << "Cannot check Comment access mode with Exiv2 (Error #"
                                           << e.code() << ": " << QString::fromStdString(e.what()) << ")";
    }
    catch (...)
    {
        qCCritical(DIGIKAM_METAENGINE_LOG) << "Default exception from Exiv2";
    }

    return false;
}

bool MetaEngine::hasComments() const
{
    return !d->itemComments().empty();
}

bool MetaEngine::clearComments() const
{
    return setComments(QByteArray());
}

QByteArray MetaEngine::getComments() const
{
    return QByteArray(d->itemComments().data(), (int)d->itemComments().size());
}

QString MetaEngine::getCommentsDecoded() const
{
    return d->detectEncodingAndDecode(d->itemComments());
}

bool MetaEngine::setComments(const QByteArray& data) const
{
    d->itemComments() = std::string(data.data(), data.size());

    return true;
}

QString MetaEngine::detectLanguageAlt(const QString& value, QString& lang)
{
    // Ex. from an Xmp tag Xmp.tiff.copyright: "lang="x-default"

    if (value.size() > 6 && value.startsWith(QLatin1String("lang=\"")))
    {
        int pos = value.indexOf(QLatin1String("\""), 6);

        if (pos != -1)
        {
            lang = value.mid(6, pos-6);
            return (value.mid(pos+2));
        }
    }

    lang.clear();

    return value;
}

} // namespace Digikam

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif

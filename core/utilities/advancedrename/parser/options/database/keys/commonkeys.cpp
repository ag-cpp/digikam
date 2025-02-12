/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-05-22
 * Description : common information keys
 *
 * SPDX-FileCopyrightText: 2009-2012 by Andi Clemens <andi dot clemens at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "commonkeys.h"

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "albummanager.h"
#include "coredbinfocontainers.h"
#include "iteminfo.h"
#include "itemcopyright.h"

namespace
{

static const QString KEY_DEFAULTCOMMENT(QLatin1String("DefaultComment"));
static const QString KEY_DIMENSION(QLatin1String("Dimension"));
static const QString KEY_FILESIZE(QLatin1String("FileSize"));
static const QString KEY_FORMAT(QLatin1String("Format"));
static const QString KEY_MEDIATYPE(QLatin1String("MediaType"));
static const QString KEY_RATING(QLatin1String("Rating"));
static const QString KEY_HEIGHT(QLatin1String("Height"));
static const QString KEY_WIDTH(QLatin1String("Width"));
static const QString KEY_ORIENTATION(QLatin1String("Orientation"));
static const QString KEY_COLORDEPTH(QLatin1String("ColorDepth"));
static const QString KEY_COLORMODEL(QLatin1String("ColorModel"));
static const QString KEY_DEFAULTAUTHOR(QLatin1String("DefaultAuthor"));
static const QString KEY_AUTHORS(QLatin1String("Authors"));
static const QString KEY_TITLE(QLatin1String("Title"));
static const QString KEY_TAGSLIST(QLatin1String("TagsList"));
static const QString KEY_TAGSPATHLIST(QLatin1String("TagsPathList"));

}

namespace Digikam
{

CommonKeys::CommonKeys()
    : DbKeysCollection(i18n("Common File Information"))
{
    addId(KEY_DEFAULTCOMMENT, i18n("Default comment of the image"));
    addId(KEY_DEFAULTAUTHOR,  i18n("Default author of the image"));
    addId(KEY_DIMENSION,      i18n("Image dimension"));
    addId(KEY_FILESIZE,       i18n("Image file size"));
    addId(KEY_FORMAT,         i18n("Format of the media file"));
    addId(KEY_MEDIATYPE,      i18n("Type of the media file"));
    addId(KEY_RATING,         i18n("Rating of the media file"));
    addId(KEY_HEIGHT,         i18n("Height of the media file"));
    addId(KEY_WIDTH,          i18n("Width of the media file"));
    addId(KEY_ORIENTATION,    i18n("Image orientation"));
    addId(KEY_COLORDEPTH,     i18n("Color depth (bits per channel)"));
    addId(KEY_COLORMODEL,     i18n("Color model of the image"));
    addId(KEY_AUTHORS,        i18n("A comma separated list of all authors"));
    addId(KEY_TITLE,          i18n("Title of the image"));
    addId(KEY_TAGSLIST,       i18n("A comma separated list of all tags"));
    addId(KEY_TAGSPATHLIST,   i18n("A comma separated list of all tags with path"));
}

QString CommonKeys::getDbValue(const QString& key, ParseSettings& settings)
{
    ItemInfo info = ItemInfo::fromUrl(settings.fileUrl);
    ImageCommonContainer container = info.imageCommonContainer();
    ItemCopyright copyright       = info.imageCopyright();
    QString result;

    if      (key == KEY_DEFAULTCOMMENT)
    {
        result = info.comment().simplified();
    }
    else if (key == KEY_DEFAULTAUTHOR)
    {
        QStringList authors = copyright.author();

        if (!authors.isEmpty())
        {
            result = authors.at(0);
        }
    }
    else if (key == KEY_AUTHORS)
    {
        QStringList authors = copyright.author();

        if (!authors.isEmpty())
        {
            for (const QString& author : std::as_const(authors))
            {
                // cppcheck-suppress useStlAlgorithm
                result += author + QLatin1Char(',');
            }
        }

        if (result.endsWith(QLatin1Char(',')))
        {
            result.chop(1);
        }
    }
    else if (key == KEY_TITLE)
    {
        result = info.title().simplified();
    }
    else if (key == KEY_TAGSLIST)
    {
        QList<int> tagIds = info.tagIds();
        QStringList tags  = AlbumManager::instance()->tagNames(tagIds);
        result            = tags.join(QLatin1String(", "));
    }
    else if (key == KEY_TAGSPATHLIST)
    {
        QList<int> tagIds = info.tagIds();
        QStringList tags  = AlbumManager::instance()->tagPaths(tagIds, false);
        result            = tags.join(QLatin1String(", "));
    }
    else if (key == KEY_DIMENSION)
    {
        QSize dimension = info.dimensions();

        if (dimension.isEmpty() || dimension.isNull() || !dimension.isValid())
        {
            dimension.setWidth(0);
            dimension.setHeight(0);
        }

        result = QString::fromUtf8("%1x%2").arg(dimension.width()).arg(dimension.height());
    }
    else if (key == KEY_HEIGHT)
    {
        int height = container.height;

        if (height < 0)
        {
            height = 0;
        }

        result = QString::number(height);
    }
    else if (key == KEY_WIDTH)
    {
        int width = container.width;

        if (width < 0)
        {
            width = 0;
        }

        result = QString::number(width);
    }
    else if (key == KEY_FILESIZE)
    {
        result = QString::number(info.fileSize());
    }
    else if (key == KEY_FORMAT)
    {
        result = info.format();
    }
    else if (key == KEY_MEDIATYPE)
    {
        switch (info.category())
        {
            case DatabaseItem::UndefinedCategory:
            {
                result = QLatin1String("Undefined");
                break;
            }

            case DatabaseItem::Image:
            {
                result = QLatin1String("Image");
                break;
            }

            case DatabaseItem::Video:
            {
                result = QLatin1String("Video");
                break;
            }

            case DatabaseItem::Audio:
            {
                result = QLatin1String("Audio");
                break;
            }

            case DatabaseItem::Other:
            default:
            {
                result = QLatin1String("Other");
                break;
            }
        }
    }
    else if (key == KEY_RATING)
    {
        result = QString::number(info.rating());
    }
    else if (key == KEY_ORIENTATION)
    {
        result = container.orientation;
    }
    else if (key == KEY_COLORDEPTH)
    {
        result = QString::number(container.colorDepth);
    }
    else if (key == KEY_COLORMODEL)
    {
        result = container.colorModel;
    }

    result.replace(QLatin1Char('/'), QLatin1Char('_'));

    return result;
}

} // namespace Digikam

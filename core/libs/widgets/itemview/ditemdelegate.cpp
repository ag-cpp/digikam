/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-01-16
 * Description : Qt item view for images - common delegate code
 *
 * SPDX-FileCopyrightText: 2002-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText:      2009 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2002-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "ditemdelegate.h"

// C++ includes

#include <cmath>

// Qt includes

#include <QApplication>
#include <QCache>
#include <QPainter>
#include <QLocale>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "itemviewcategorized.h"
#include "thememanager.h"
#include "thumbbardock.h"

namespace Digikam
{

class Q_DECL_HIDDEN DItemDelegate::Private
{
public:

    Private() = default;

    QCache<QString, QPixmap> thumbnailBorderCache;
    QCache<QString, QString> squeezedTextCache;
};

DItemDelegate::DItemDelegate(QObject* const parent)
    : QAbstractItemDelegate(parent),
      d                    (new Private)
{
}

DItemDelegate::~DItemDelegate()
{
    delete d;
}

void DItemDelegate::clearCaches()
{
    d->thumbnailBorderCache.clear();
    d->squeezedTextCache.clear();
}

QPixmap DItemDelegate::thumbnailBorderPixmap(const QSize& pixSize, bool isGrouped) const
{
    const QColor borderColor = QColor(0, 0, 0, 128);
    QString cacheKey         = QString::number(pixSize.width())  + QLatin1Char('-')
                             + QString::number(pixSize.height()) + QLatin1Char('-')
                             + QString::number(isGrouped);
    QPixmap* const cachePix  = d->thumbnailBorderCache.object(cacheKey);

    if (!cachePix)
    {
        const int radius = 3;
        QPixmap pix;
        const int width  = pixSize.width()  + 2*radius;
        const int height = pixSize.height() + 2*radius;

        if (isGrouped)
        {
            pix = ThumbBarDock::generateFuzzyRectForGroup(QSize(width, height),
                                                          borderColor,
                                                          radius);
        }
        else
        {
            pix = ThumbBarDock::generateFuzzyRect(QSize(width, height),
                                                  borderColor,
                                                  radius);
        }

        d->thumbnailBorderCache.insert(cacheKey, new QPixmap(pix));
        return pix;
    }

    return *cachePix;
}

QPixmap DItemDelegate::makeDragPixmap(const QStyleOptionViewItem& option,
                                      const QList<QModelIndex>& indexes,
                                      double displayRatio,
                                      const QPixmap& suggestedPixmap)
{
    int iconSize = qRound(64.0 * displayRatio);
    QPixmap icon = suggestedPixmap;

    if (icon.isNull())
    {
        icon = QPixmap(QIcon::fromTheme(QLatin1String("image-jpeg")).pixmap(32));
    }

    if (qMax(icon.width(), icon.height()) > iconSize)
    {
        icon = icon.scaled(iconSize, iconSize, Qt::KeepAspectRatio,
                                               Qt::SmoothTransformation);
    }

    int w                 = icon.width();
    int h                 = icon.height();
    const int borderWidth = 6;

    QSizeF drawSize       = QSizeF(w, h) / displayRatio;
    QRect  rect(0, 0, w + qRound((double)borderWidth * 2.0 * displayRatio),
                      h + qRound((double)borderWidth * 2.0 * displayRatio));
    QRect  pixmapRect(QPoint(borderWidth, borderWidth), drawSize.toSize());

    QPixmap pix(rect.size());
    pix.setDevicePixelRatio(displayRatio);
    QPainter p(&pix);

/*
    // border
    p.fillRect(0, 0, pix.width()-1, pix.height()-1, QColor(Qt::white));
    p.setPen(QPen(Qt::black, 1));
    p.drawRect(0, 0, pix.width()-1, pix.height()-1);
*/

    QStyleOption opt(option);
    opt.rect = rect;
    qApp->style()->drawPrimitive(QStyle::PE_PanelTipLabel, &opt, &p);

    p.drawPixmap(pixmapRect, icon);

    QFont f(option.font);
    f.setBold(true);
    p.setFont(f);

    if (indexes.size() > 1)
    {
        QRect   textRect;
        QString text;

        QString text2(i18np("1 Image", "%1 Images", indexes.count()));
        QString text1 = QString::number(indexes.count());
        QRect r1      = p.boundingRect(pixmapRect, Qt::AlignLeft | Qt::AlignTop, text1).adjusted(0, 0, 1, 1);
        QRect r2      = p.boundingRect(pixmapRect, Qt::AlignLeft | Qt::AlignTop, text2).adjusted(0, 0, 1, 1);

        if ((r2.width() > pixmapRect.width()) || (r2.height() > pixmapRect.height()))
        {
//            textRect     = r1;
            text         = text1;
            int rectSize = qMax(r1.width(), r1.height());
            textRect     = QRect(0, 0, rectSize, rectSize);
        }
        else
        {
            textRect = QRect(0, 0, r2.width(), r2.height());
            text     = text2;
        }

        textRect.moveLeft((pixmapRect.width() - textRect.width()) / 2 + pixmapRect.x());
        textRect.moveTop((pixmapRect.height() - textRect.height()) * 4 / 5);
        p.fillRect(textRect, QColor(0, 0, 0, 128));
        p.setPen(Qt::white);
        p.drawText(textRect, Qt::AlignCenter, text);
    }

    return pix;
}

QString DItemDelegate::dateToString(const QDateTime& datetime)
{
    return QLocale().toString(datetime, QLocale::ShortFormat);
}

QString DItemDelegate::squeezedTextCached(QPainter* const p, int width, const QString& text) const
{
    QCache<QString, QString>* const cache = &const_cast<DItemDelegate*>(this)->d->squeezedTextCache;

    // We do not need to include the font into cache key, the cache is cleared on font change

    QString cacheKey                      = QString::number(width) + QString::number(qHash(text));
    QString* const cachedString           = cache->object(cacheKey);

    if (cachedString)
    {
        return *cachedString;
    }

    QString result = squeezedText(p->fontMetrics(), width, text);
    cache->insert(cacheKey, new QString(result));

    return result;
}

QString DItemDelegate::squeezedText(const QFontMetrics& fm, int width, const QString& text)
{
    QString fullText(text);
    fullText.replace(QLatin1Char('\n'), QLatin1Char(' '));

    return fm.elidedText(fullText, Qt::ElideRight, width);
}

} // namespace Digikam

#include "moc_ditemdelegate.cpp"

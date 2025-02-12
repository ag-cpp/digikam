/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-07-15
 * Description : Item delegate for image versions list view
 *
 * SPDX-FileCopyrightText: 2010-2011 by Martin Klapetek <martin dot klapetek at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "versionsdelegate.h"

// Qt includes

#include <QApplication>
#include <QPainter>
#include <QPropertyAnimation>
#include <QStyle>
#include <QStyleOptionViewItem>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "itemdelegate.h"
#include "itemhistorygraphmodel.h"
#include "itemversionsmodel.h"
#include "thumbnailloadthread.h"
#include "dcategorydrawer.h"
#include "dlayoutbox.h"
#include "dworkingpixmap.h"

namespace Digikam
{

class Q_DECL_HIDDEN VersionsDelegate::Private
{
public:

    Private() = default;

    const int                     categoryExtraSpacing      = 6;
    const int                     filterItemExtraSpacing    = 4;

    int                           animationState            = 0;
    QPropertyAnimation*           animation                 = nullptr;
    DWorkingPixmap*               workingPixmap             = nullptr;
    DCategoryDrawer*              categoryDrawer            = nullptr;
    int                           thumbnailSize             = 64;

    int                           thumbsWaitingFor          = 0;
    bool                          inSizeHint                = false;

public:

    inline const QWidget* widget(const QStyleOptionViewItem& option)
    {
        if (const QStyleOptionViewItem* v3 = qstyleoption_cast<const QStyleOptionViewItem*>(&option))
        {
            return v3->widget;
        }

        return nullptr;
    }

    inline const QStyle* style(const QStyleOptionViewItem& option)
    {
        const QWidget* w = widget(option);

        return (w ? w->style() : QApplication::style());
    }
};

VersionsDelegate::VersionsDelegate(QObject* const parent)
    : QStyledItemDelegate(parent),
      d                  (new Private)
{
    d->workingPixmap  = new DWorkingPixmap(this);
    d->categoryDrawer = new DCategoryDrawer(nullptr);
    d->animation      = new QPropertyAnimation(this, "animationState", this);
    d->animation->setStartValue(0);
    d->animation->setEndValue(d->workingPixmap->frameCount() - 1);
    d->animation->setDuration(100 * d->workingPixmap->frameCount());
    d->animation->setLoopCount(-1);
}

VersionsDelegate::~VersionsDelegate()
{
    delete d->categoryDrawer;
    delete d;
}

int VersionsDelegate::animationState() const
{
    return d->animationState;
}

void VersionsDelegate::setAnimationState(int animationState)
{
    if (d->animationState == animationState)
    {
        return;
    }

    d->animationState = animationState;
    Q_EMIT animationStateChanged();
}

void VersionsDelegate::setThumbnailSize(int size) const
{
    d->thumbnailSize = size;
}

int VersionsDelegate::thumbnailSize() const
{
    return d->thumbnailSize;
}

void VersionsDelegate::beginPainting()
{
    d->thumbsWaitingFor = 0;
}

void VersionsDelegate::finishPainting()
{
/*
    qCDebug(DIGIKAM_GENERAL_LOG) << "painting finished" << d->thumbsWaitingFor;
*/
    if (d->thumbsWaitingFor)
    {
        d->animation->start();
    }
    else
    {
        d->animation->stop();
    }
}

QSize VersionsDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if      (index.data(ItemHistoryGraphModel::IsImageItemRole).toBool())
    {
        d->inSizeHint = true;
        QSize size    = QStyledItemDelegate::sizeHint(option, index);
        d->inSizeHint = false;

        return size;
    }
    else if (index.data(ItemHistoryGraphModel::IsFilterActionItemRole).toBool())
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size      += QSize(0, d->filterItemExtraSpacing);

        return size;
    }
    else if (index.data(ItemHistoryGraphModel::IsCategoryItemRole).toBool())
    {
        int height = d->categoryDrawer->categoryHeight(index, option) + d->categoryExtraSpacing;
        QSize size = QStyledItemDelegate::sizeHint(option, index);

        return size.expandedTo(QSize(0, height));
    }
    else if (index.data(ItemHistoryGraphModel::IsSeparatorItemRole).toBool())
    {
        //int pm = d->style(option)->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, d->widget(option));
        int pm = d->style(option)->pixelMetric(QStyle::PM_ToolBarSeparatorExtent, nullptr, d->widget(option));
        //int spacing = d->style(option)->pixelMetric(QStyle::PM_LayoutVerticalSpacing, &option);

        return QSize(1, pm);
    }
    else
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }
}

void VersionsDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if      (index.data(ItemHistoryGraphModel::IsCategoryItemRole).toBool())
    {
        QStyleOption opt = option;
        opt.rect.adjust(d->categoryExtraSpacing / 2, d->categoryExtraSpacing / 2, - d->categoryExtraSpacing / 2, 0);

        // purpose of sortRole is unclear, give Qt::DisplayRole

        d->categoryDrawer->drawCategory(index, Qt::DisplayRole, opt, painter);
    }
    else if (index.data(ItemHistoryGraphModel::IsSeparatorItemRole).toBool())
    {
        d->style(option)->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &option, painter, d->widget(option));
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
        return;
/*
        if (index.data(ItemHistoryGraphModel::IsSubjectImageRole).toBool())
        {
            // draw 1px border
            QPen oldPen = painter->pen();
            QPen pen(option.palette.windowText(), 0);
            painter->setPen(pen);
            painter->drawRect(option.rect);
            painter->setPen(oldPen);
        }
*/
    }
}

void VersionsDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const
{
    QStyledItemDelegate::initStyleOption(option, index);

    // Don't show the separator-like focus indicator

    option->state &= ~QStyle::State_HasFocus;

    if (!index.data(ItemHistoryGraphModel::IsImageItemRole).toBool())
    {
        return;
    }
/*
    if (index.data(ItemHistoryGraphModel::IsSubjectImageRole).toBool())
    {
        option->font.setWeight(QFont::Bold);
    }
*/
    option->font.setWeight(QFont::Bold);

    if (QStyleOptionViewItem* const v4 = qstyleoption_cast<QStyleOptionViewItem*>(option))
    {
        v4->features |= QStyleOptionViewItem::HasDecoration;

        if (d->inSizeHint)
        {
            v4->decorationSize = QSize(d->thumbnailSize, d->thumbnailSize);
        }
        else
        {
            QPixmap pix = ItemDelegate::retrieveThumbnailPixmap(index, d->thumbnailSize);

            if (pix.isNull())
            {
                pix = d->workingPixmap->frameAt(d->animationState);
                d->thumbsWaitingFor++;
            }

            v4->icon           = QIcon(pix);
            v4->decorationSize = pix.size();
        }
    }
}

/*
void VersionsDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter);
    //qCDebug(DIGIKAM_GENERAL_LOG) << QApplication::style()->subElementRect(QStyle::SE_ItemViewItemDecoration, &option, 0);

    if (dynamic_cast<const ItemVersionsModel*>(index.model())->paintTree())
    {
        const_cast<QStyleOptionViewItem&>(option).rect.setLeft(option.rect.left() + (index.data(Qt::UserRole).toInt() * 16));
    }

    QRect thumbRect(option.rect.left()+4, option.rect.top()+8, 64, 48);
    painter->setPen(Qt::black);
    painter->drawRect(thumbRect);

    QPixmap thumbnail;

    if (ThumbnailLoadThread::defaultIconViewThread()->find(index.data(Qt::DisplayRole).toString(), thumbnail))
    {
        if (!thumbnail.isNull())
        {
            thumbnail = thumbnail.scaled(64, 48, Qt::KeepAspectRatio);
            const_cast<VersionsDelegate*>(this)->d->thumbsPainted++;
        }
        else
        {
            //if the thumbnail pixmap is null, display an error icon instead
            thumbnail = BarIcon("task-reject");
        }

        if (d->thumbsPainted == index.model()->rowCount())
        {
            // the timer can be stopped after last thumbnail is drawn,
            // but it needs to be delayed a little, so that all thumbs
            // have enough time to get painted correctly
            delayedAnimationTimerStop();

        }
    }
    else
    {
        //when the thumbnail is not loaded yet, start the animation
        d->workingWidget->toggleTimer(true);

        connect(d->workingWidget, SIGNAL(animationStep()),
                dynamic_cast<const ItemVersionsModel*>(index.model()), SLOT(slotAnimationStep()));

        thumbnail = QPixmap::grabWidget(d->workingWidget);
    }

    painter->drawPixmap(thumbRect.left()+(32-(int)(thumbnail.width()/2)), thumbRect.top()+(24-(int)(thumbnail.height()/2)), thumbnail);

    QRect textRect = option.rect;
    textRect.setLeft(textRect.left() + 72);
    QUrl path(index.data(Qt::DisplayRole).toString());

    if (index.row() == 0 && index.model()->rowCount() > 1)
    {
        painter->drawText(textRect, Qt::AlignVCenter, i18n("%1 (Original)", path.fileName()));
    }
    else if (index.row() == 0 && index.model()->rowCount() == 1)
    {
        painter->drawText(textRect, Qt::AlignVCenter, i18n("This is the original image"));
    }
    else
    {
        painter->drawText(textRect, Qt::AlignVCenter, path.fileName());
    }

    painter->restore();
}
*/

} // namespace Digikam

#include "moc_versionsdelegate.cpp"

/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-13
 * Description : a tool to blend bracketed images.
 *
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2015      by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "enfusestack.h"

// Qt includes

#include <QHeaderView>
#include <QPainter>
#include <QFileInfo>
#include <QTimer>
#include <QMenu>
#include <QAction>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dlayoutbox.h"
#include "dworkingpixmap.h"

using namespace Digikam;

namespace DigikamGenericExpoBlendingPlugin
{

class Q_DECL_HIDDEN EnfuseStackItem::Private
{
public:

    Private() = default;

    bool           asValidThumb = false;
    QPixmap        thumb;
    EnfuseSettings settings;
};

EnfuseStackItem::EnfuseStackItem(QTreeWidget* const parent)
    : QTreeWidgetItem(parent),
      d              (new Private)
{
    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    setCheckState(0, Qt::Unchecked);
    setThumbnail(QIcon::fromTheme(QLatin1String("view-preview")).pixmap(treeWidget()->iconSize().width(), QIcon::Disabled));
    d->asValidThumb = false;
}

EnfuseStackItem::~EnfuseStackItem()
{
    delete d;
}

void EnfuseStackItem::setEnfuseSettings(const EnfuseSettings& settings)
{
    d->settings = settings;
    setText(1, d->settings.targetFileName);
    setText(2, d->settings.inputImagesList());
    setToolTip(1, d->settings.asCommentString());
    setToolTip(2, d->settings.inputImagesList().replace(QLatin1String(" ; "), QLatin1String("\n")));
}

EnfuseSettings EnfuseStackItem::enfuseSettings() const
{
    return d->settings;
}

const QUrl& EnfuseStackItem::url() const
{
    return d->settings.previewUrl;
}

void EnfuseStackItem::setProgressAnimation(const QPixmap& pix)
{
    QPixmap overlay = d->thumb;
    QPixmap mask(overlay.size());
    mask.fill(QColor(128, 128, 128, 192));
    QPainter p(&overlay);
    p.drawPixmap(0, 0, mask);
    p.drawPixmap((overlay.width()/2) - (pix.width()/2), (overlay.height()/2) - (pix.height()/2), pix);
    setIcon(0, QIcon(overlay));
}

void EnfuseStackItem::setThumbnail(const QPixmap& pix)
{
    int iconSize = qMax<int>(treeWidget()->iconSize().width(), treeWidget()->iconSize().height());
    QPixmap pixmap(iconSize+2, iconSize+2);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.drawPixmap((pixmap.width()/2) - (pix.width()/2), (pixmap.height()/2) - (pix.height()/2), pix);
    d->thumb = pixmap;
    setIcon(0, QIcon(pixmap));
    d->asValidThumb = true;
}

void EnfuseStackItem::setProcessedIcon(const QIcon& icon)
{
    setIcon(1, icon);
    setIcon(0, QIcon(d->thumb));
}

bool EnfuseStackItem::asValidThumb() const
{
    return d->asValidThumb;
}

bool EnfuseStackItem::isOn() const
{
    return ((checkState(0) == Qt::Checked) ? true : false);
}

void EnfuseStackItem::setOn(bool b)
{
    setCheckState(0, b ? Qt::Checked : Qt::Unchecked);
}

// -------------------------------------------------------------------------

class Q_DECL_HIDDEN EnfuseStackList::Private
{
public:

    Private() = default;

    DSaveSettingsWidget::OutputFormat outputFormat      = DSaveSettingsWidget::OUTPUT_PNG;

    QString                           templateFileName;

    int                               progressCount     = 0;
    QTimer*                           progressTimer     = nullptr;
    DWorkingPixmap*                   progressPix       = nullptr;
    EnfuseStackItem*                  processItem       = nullptr;
};

EnfuseStackList::EnfuseStackList(QWidget* const parent)
    : QTreeWidget(parent),
      d          (new Private)
{
    d->progressTimer = new QTimer(this);
    d->progressPix   = new DWorkingPixmap(this);

    setContextMenuPolicy(Qt::CustomContextMenu);
    setIconSize(QSize(64, 64));
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSortingEnabled(false);
    setRootIsDecorated(false);
    setUniformRowHeights(true);
    setAllColumnsShowFocus(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setColumnCount(3);
    setHeaderHidden(false);
    setDragEnabled(false);
    header()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList labels;
    labels.append( i18nc("@title:column Saving checkbox", "Include during Saving") );
    labels.append( i18nc("@title:column Output file name", "Output") );
    labels.append( i18nc("@title:column Source file names", "Selected Inputs") );
    setHeaderLabels(labels);

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(slotItemClicked(QTreeWidgetItem*)));

    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotContextMenu(QPoint)));

    connect(d->progressTimer, SIGNAL(timeout()),
            this, SLOT(slotProgressTimerDone()));
}

EnfuseStackList::~EnfuseStackList()
{
    delete d;
}

void EnfuseStackList::slotContextMenu(const QPoint& p)
{
    QMenu popmenu(this);

    EnfuseStackItem* const item = dynamic_cast<EnfuseStackItem*>(itemAt(p));

    if (item)
    {
        QAction* const rmItem = new QAction(QIcon::fromTheme(QLatin1String("window-close")), i18nc("@item:inmenu", "Remove item"), this);

        connect(rmItem, SIGNAL(triggered(bool)),
                this, SLOT(slotRemoveItem()));

        popmenu.addAction(rmItem);
        popmenu.addSeparator();
    }

    QAction* const rmAll = new QAction(QIcon::fromTheme(QLatin1String("edit-delete")), i18nc("@item:inmenu", "Clear all"), this);

    connect(rmAll, SIGNAL(triggered(bool)),
            this, SLOT(clear()));

    popmenu.addAction(rmAll);
    popmenu.exec(QCursor::pos());
}

void EnfuseStackList::slotRemoveItem()
{
    EnfuseStackItem* item = dynamic_cast<EnfuseStackItem*>(currentItem());
    delete item;
}

QList<EnfuseSettings> EnfuseStackList::settingsList()
{
    QList<EnfuseSettings> list;
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        EnfuseStackItem* const item = dynamic_cast<EnfuseStackItem*>(*it);

        if (item && item->isOn())
        {
            list.append(item->enfuseSettings());
        }

        ++it;
    }

    return list;
}

void EnfuseStackList::clearSelected()
{
    QList<QTreeWidgetItem*> list;
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        EnfuseStackItem* const item = dynamic_cast<EnfuseStackItem*>(*it);

        if (item && item->isOn())
        {
            list.append(item);
        }

        ++it;
    }

    for (QTreeWidgetItem* const item : std::as_const(list))
    {
        delete item;
    }
}

void EnfuseStackList::setOnItem(const QUrl& url, bool on)
{
    EnfuseStackItem* const item = findItemByUrl(url);

    if (item)
    {
        item->setOn(on);
    }
}

void EnfuseStackList::removeItem(const QUrl& url)
{
    EnfuseStackItem* const item = findItemByUrl(url);
    delete item;
}

void EnfuseStackList::addItem(const QUrl& url, const EnfuseSettings& settings)
{
    if (!url.isValid())
    {
        return;
    }

    // Check if the new item already exist in the list.

    if (!findItemByUrl(url))
    {
        EnfuseSettings enfusePrms = settings;
        QString ext               = DSaveSettingsWidget::extensionForFormat(enfusePrms.outputFormat);
        enfusePrms.previewUrl     = url;

        EnfuseStackItem* const item = new EnfuseStackItem(this);
        item->setEnfuseSettings(enfusePrms);
        item->setOn(true);
        setCurrentItem(item);
        setTemplateFileName(d->outputFormat, d->templateFileName);

        Q_EMIT signalItemClicked(url);
    }
}

void EnfuseStackList::setThumbnail(const QUrl& url, const QImage& img)
{
    if (img.isNull())
    {
        return;
    }

    EnfuseStackItem* const item = findItemByUrl(url);

    if (item && (!item->asValidThumb()))
    {
        item->setThumbnail(QPixmap::fromImage(img.scaled(iconSize().width(), iconSize().height(), Qt::KeepAspectRatio)));
    }
}

void EnfuseStackList::slotItemClicked(QTreeWidgetItem* item)
{
    EnfuseStackItem* const eItem = dynamic_cast<EnfuseStackItem*>(item);

    if (eItem)
    {
        Q_EMIT signalItemClicked(eItem->url());
    }
}

void EnfuseStackList::slotProgressTimerDone()
{
    d->processItem->setProgressAnimation(d->progressPix->frameAt(d->progressCount));
    d->progressCount++;

    if (d->progressCount == 8)
    {
        d->progressCount = 0;
    }

    d->progressTimer->start(300);
}

EnfuseStackItem* EnfuseStackList::findItemByUrl(const QUrl& url)
{
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        EnfuseStackItem* const item = dynamic_cast<EnfuseStackItem*>(*it);

        if (item && (item->url() == url))
        {
            return item;
        }

        ++it;
    }

    return nullptr;
}

void EnfuseStackList::processingItem(const QUrl& url, bool run)
{
    d->processItem = findItemByUrl(url);

    if (d->processItem)
    {
        if (run)
        {
            setCurrentItem(d->processItem, true);
            scrollToItem(d->processItem);
            d->progressTimer->start(300);
        }
        else
        {
            d->progressTimer->stop();
            d->processItem = nullptr;
        }
    }
}

void EnfuseStackList::processedItem(const QUrl& url, bool success)
{
    EnfuseStackItem* const item = findItemByUrl(url);

    if (item)
    {
        item->setProcessedIcon(QIcon::fromTheme(success ? QLatin1String("dialog-ok-apply")
                                                        : QLatin1String("dialog-cancel")));
    }
}

void EnfuseStackList::setTemplateFileName(DSaveSettingsWidget::OutputFormat frm, const QString& string)
{
    d->outputFormat     = frm;
    d->templateFileName = string;
    int count           = 0;

    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        EnfuseStackItem* const item = dynamic_cast<EnfuseStackItem*>(*it);

        if (item)
        {
            QString temp;
            EnfuseSettings settings = item->enfuseSettings();
            QString ext             = DSaveSettingsWidget::extensionForFormat(d->outputFormat);
            settings.outputFormat   = d->outputFormat;
            settings.targetFileName = d->templateFileName + temp.asprintf("-%02i", count+1).append(ext);
            item->setEnfuseSettings(settings);
        }

        ++it;
        count++;
    }
}

} // namespace DigikamGenericExpoBlendingPlugin

#include "moc_enfusestack.cpp"

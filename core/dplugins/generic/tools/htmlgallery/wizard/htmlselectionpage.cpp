/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-04-04
 * Description : a tool to generate HTML image galleries
 *
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "htmlselectionpage.h"

// Qt includes

#include <QIcon>
#include <QPixmap>
#include <QStackedWidget>

// Local includes

#include "htmlwizard.h"
#include "galleryinfo.h"
#include "ditemslist.h"

namespace DigikamGenericHtmlGalleryPlugin
{

class Q_DECL_HIDDEN HTMLSelectionPage::Private
{
public:

    explicit Private(QWizard* const dialog)
      : albumSupport    (false),
        albumSelector   (nullptr),
        imageList       (nullptr),
        stack           (nullptr),
        wizard          (nullptr),
        info            (nullptr),
        iface           (nullptr)
    {
        wizard = dynamic_cast<HTMLWizard*>(dialog);

        if (wizard)
        {
            info  = wizard->galleryInfo();
            iface = info->m_iface;
        }
    }

    bool             albumSupport;
    QWidget*         albumSelector;
    DItemsList*      imageList;
    QStackedWidget*  stack;
    HTMLWizard*      wizard;
    GalleryInfo*     info;
    DInfoInterface*  iface;
};

HTMLSelectionPage::HTMLSelectionPage(QWizard* const dialog, const QString& title)
    : DWizardPage(dialog, title),
      d          (new Private(dialog))
{
    setObjectName(QLatin1String("AlbumSelectorPage"));

    d->stack              = new QStackedWidget(this);
    d->albumSupport       = (d->iface && d->iface->supportAlbums());

    if (d->albumSupport)
    {
        d->albumSelector = d->iface->albumChooser(this);
    }
    else
    {
        d->albumSelector = new QWidget(this);
    }

    d->stack->insertWidget(GalleryInfo::ALBUMS, d->albumSelector);

    d->imageList          = new DItemsList(this);
    d->imageList->setObjectName(QLatin1String("HTMLGalleries ImagesList"));
    d->imageList->setControlButtonsPlacement(DItemsList::ControlButtonsBelow);
    d->stack->insertWidget(GalleryInfo::IMAGES, d->imageList);

    setPageWidget(d->stack);
    setLeftBottomPix(QIcon::fromTheme(QLatin1String("folder-pictures")));

    if (d->albumSupport)
    {
        connect(d->iface, SIGNAL(signalAlbumChooserSelectionChanged()),
                this, SIGNAL(completeChanged()));
    }

    connect(d->imageList, SIGNAL(signalImageListChanged()),
            this, SIGNAL(completeChanged()));
}

HTMLSelectionPage::~HTMLSelectionPage()
{
    delete d;
}


void HTMLSelectionPage::setItemsList(const QList<QUrl>& urls)
{
    d->imageList->slotAddImages(urls);
}

void HTMLSelectionPage::initializePage()
{
    d->imageList->setIface(d->iface);

    if (d->info->m_getOption == GalleryInfo::IMAGES)
    {
        d->imageList->loadImagesFromCurrentSelection();
    }

    d->stack->setCurrentIndex(d->info->m_getOption);
}

bool HTMLSelectionPage::validatePage()
{
    if (d->stack->currentIndex() == GalleryInfo::ALBUMS)
    {
        if (d->albumSupport)
        {
            if (d->iface->albumChooserItems().isEmpty())
                return false;

            d->info->m_albumList = d->iface->albumChooserItems();
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (d->imageList->imageUrls().isEmpty())
        {
            return false;
        }

        d->info->m_imageList = d->imageList->imageUrls();
    }

    return true;
}

bool HTMLSelectionPage::isComplete() const
{
    if (d->stack->currentIndex() == GalleryInfo::ALBUMS)
    {
        if (!d->albumSupport)
        {
            return false;
        }

        return (!d->iface->albumChooserItems().isEmpty());
    }

    return (!d->imageList->imageUrls().isEmpty());
}

} // namespace DigikamGenericHtmlGalleryPlugin

/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-03-27
 * Description : a tool to export items to a local storage
 *
 * Copyright (C) 2006-2009 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2011-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019      by Maik Qualmann <metzpinguin at gmail dot com>
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

#include "fcexportwidget.h"

// Qt includes

#include <QApplication>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QLabel>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "dfileselector.h"
#include "ditemslist.h"
#include "wstoolutils.h"
#include "dlayoutbox.h"
#include "fctask.h"

namespace DigikamGenericFileCopyPlugin
{

class Q_DECL_HIDDEN FCExportWidget::Private
{
public:

    explicit Private()
      : selector(nullptr),
        imageList(nullptr),
        overwrite(nullptr),
        targetButtonGroup(nullptr),
        fileCopyButton(nullptr),
        symLinkButton(nullptr),
        relativeButton(nullptr)
    {
    }

    DFileSelector* selector;
    DItemsList*    imageList;
    QCheckBox*     overwrite;

    QButtonGroup*  targetButtonGroup;
    QRadioButton*  fileCopyButton;
    QRadioButton*  symLinkButton;
    QRadioButton*  relativeButton;

    QUrl           targetUrl;

};

FCExportWidget::FCExportWidget(DInfoInterface* const iface, QWidget* const parent)
    : QWidget(parent),
      d(new Private)
{
    // setup local target selection

    DHBox* const hbox         = new DHBox(this);
    QLabel* const label       = new QLabel(hbox);
    d->selector               = new DFileSelector(hbox);

    QLabel* const targetLabel = new QLabel(i18n("Target File behavior:"), this);
    d->targetButtonGroup      = new QButtonGroup(this);
    d->fileCopyButton         = new QRadioButton(i18n("Copy files"), this);
    d->symLinkButton          = new QRadioButton(i18n("Create symlinks"), this);
    d->relativeButton         = new QRadioButton(i18n("Create relative symlinks"), this);

    d->overwrite              = new QCheckBox(i18n("Overwrite existing items in the target"), this);

    d->targetButtonGroup->addButton(d->fileCopyButton, FCTask::CopyFile);
    d->targetButtonGroup->addButton(d->symLinkButton,  FCTask::FullSymLink);
    d->targetButtonGroup->addButton(d->relativeButton, FCTask::RelativeSymLink);
    d->targetButtonGroup->setExclusive(true);
    d->fileCopyButton->setChecked(true);

    label->setText(i18n("Target location: "));
    d->selector->setFileDlgMode(QFileDialog::Directory);
    d->selector->setFileDlgTitle(i18n("Target Folder"));
    d->selector->setWhatsThis(i18n("Sets the target address to copy the items to."));

    // setup image list
    d->imageList = new DItemsList(this);
    d->imageList->setObjectName(QLatin1String("FCExport ImagesList"));
    d->imageList->setIface(iface);
    d->imageList->loadImagesFromCurrentSelection();
    d->imageList->setAllowRAW(true);
    d->imageList->listView()->setWhatsThis(i18n("This is the list of items to copy "
                                                "to the specified target."));

    // layout dialog
    QVBoxLayout* const layout = new QVBoxLayout(this);

    layout->addWidget(hbox);
    layout->addWidget(targetLabel);
    layout->addWidget(d->fileCopyButton);
    layout->addWidget(d->symLinkButton);
    layout->addWidget(d->relativeButton);
    layout->addWidget(d->overwrite);
    layout->addWidget(d->imageList);
    layout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    layout->setContentsMargins(QMargins());

    // ------------------------------------------------------------------------

    connect(d->selector->lineEdit(), SIGNAL(textEdited(QString)),
            this, SLOT(slotLabelUrlChanged()));

    connect(d->selector, SIGNAL(signalUrlSelected(QUrl)),
            this, SLOT(slotLabelUrlChanged()));
}

FCExportWidget::~FCExportWidget()
{
    delete d;
}

QUrl FCExportWidget::targetUrl() const
{
    return d->targetUrl;
}

void FCExportWidget::setTargetUrl(const QUrl& url)
{
    d->targetUrl = url;
    d->selector->setFileDlgPath(d->targetUrl.toLocalFile());
}

DItemsList* FCExportWidget::imagesList() const
{
    return d->imageList;
}

QCheckBox* FCExportWidget::overwriteBox() const
{
    return d->overwrite;
}

QButtonGroup* FCExportWidget::targetButtonGroup() const
{
    return d->targetButtonGroup;
}

void FCExportWidget::slotLabelUrlChanged()
{
    d->targetUrl = QUrl::fromLocalFile(d->selector->fileDlgPath());

    emit signalTargetUrlChanged(d->targetUrl);
}

} // namespace DigikamGenericFileCopyPlugin

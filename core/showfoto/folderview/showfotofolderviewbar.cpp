/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : a tool bar for Showfoto folder view
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "showfotofolderviewbar.h"

// Qt includes

#include <QDir>
#include <QSlider>
#include <QLayout>
#include <QToolButton>
#include <QPixmap>
#include <QLineEdit>
#include <QActionGroup>
#include <QMenu>
#include <QComboBox>
#include <QUndoView>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "showfotofolderviewiconprovider.h"
#include "showfotofolderviewlist.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoFolderViewBar::Private
{

public:

    explicit Private()
      : previousBtn    (nullptr),
        nextBtn        (nullptr),
        upBtn          (nullptr),
        homeBtn        (nullptr),
        iconSizeSlider (nullptr),
        optionsBtn     (nullptr),
        optionsMenu    (nullptr),
        shortAction    (nullptr),
        detailledAction(nullptr),
        loadBtn        (nullptr),
        pathEdit       (nullptr),
        sidebar        (nullptr),
        undoView       (nullptr)
    {
    }

    QToolButton*               previousBtn;
    QToolButton*               nextBtn;
    QToolButton*               upBtn;
    QToolButton*               homeBtn;
    QSlider*                   iconSizeSlider;
    QToolButton*               optionsBtn;
    QMenu*                     optionsMenu;
    QAction*                   shortAction;
    QAction*                   detailledAction;
    QToolButton*               loadBtn;
    QComboBox*                 pathEdit;
    QList<QAction*>            actionsList;                    ///< used to shared actions with list-view context menu.
    ShowfotoFolderViewSideBar* sidebar;
    QUndoView*                 undoView;
};

ShowfotoFolderViewBar::ShowfotoFolderViewBar(ShowfotoFolderViewSideBar* const sidebar)
    : DVBox(sidebar),
      d    (new Private)
{
    setContentsMargins(QMargins());

    d->sidebar               = sidebar;
    DHBox* const btnBox      = new DHBox(this);
    QAction* btnAction       = nullptr;

    // ---

    btnAction                = new QAction(this);
    btnAction->setObjectName(QLatin1String("GoPrevious"));
    btnAction->setIcon(QIcon::fromTheme(QLatin1String("go-previous")));
    btnAction->setText(i18nc("action", "Go to Previous"));
    btnAction->setToolTip(i18nc("@info", "Go to previous place in folder-view hierarchy"));

    connect(btnAction, SIGNAL(triggered(bool)),
            this, SIGNAL(signalGoPrevious()));

    d->actionsList << btnAction;

    d->previousBtn           = new QToolButton(btnBox);
    d->previousBtn->setDefaultAction(btnAction);
    d->previousBtn->setFocusPolicy(Qt::NoFocus);

    // ---

    btnAction                = new QAction(this);
    btnAction->setObjectName(QLatin1String("GoNext"));
    btnAction->setIcon(QIcon::fromTheme(QLatin1String("go-next")));
    btnAction->setText(i18nc("action", "Go to Next"));
    btnAction->setToolTip(i18nc("@info", "Go to next place in folder-view hierarchy"));

    connect(btnAction, SIGNAL(triggered(bool)),
            this, SIGNAL(signalGoNext()));

    d->actionsList << btnAction;

    d->nextBtn               = new QToolButton(btnBox);
    d->nextBtn->setDefaultAction(btnAction);
    d->nextBtn->setFocusPolicy(Qt::NoFocus);

    // ---

    btnAction                = new QAction(this);
    btnAction->setObjectName(QLatin1String("GoUp"));
    btnAction->setIcon(QIcon::fromTheme(QLatin1String("go-up")));
    btnAction->setText(i18nc("action", "Go Up"));
    btnAction->setToolTip(i18nc("@info", "Go up in folder-view hierarchy"));

    connect(btnAction, SIGNAL(triggered(bool)),
            this, SIGNAL(signalGoUp()));

    d->actionsList << btnAction;

    d->upBtn                 = new QToolButton(btnBox);
    d->upBtn->setDefaultAction(btnAction);
    d->upBtn->setFocusPolicy(Qt::NoFocus);

    // ---

    btnAction                = new QAction(this);
    btnAction->setObjectName(QLatin1String("GoHome"));
    btnAction->setIcon(QIcon::fromTheme(QLatin1String("go-home")));
    btnAction->setText(i18nc("action", "Go Home"));
    btnAction->setToolTip(i18nc("@info", "Go to home directory"));

    connect(btnAction, SIGNAL(triggered(bool)),
            this, SIGNAL(signalGoHome()));

    d->actionsList << btnAction;

    d->homeBtn               = new QToolButton(btnBox);
    d->homeBtn->setDefaultAction(btnAction);
    d->homeBtn->setFocusPolicy(Qt::NoFocus);

    // ---

    d->iconSizeSlider        = new QSlider(Qt::Horizontal, btnBox);
    d->iconSizeSlider->setRange(16, ShowfotoFolderViewIconProvider::maxIconSize());
    d->iconSizeSlider->setSingleStep(10);
    d->iconSizeSlider->setValue(32);
    d->iconSizeSlider->setFocusPolicy(Qt::NoFocus);
    btnBox->setStretchFactor(d->iconSizeSlider, 10);

    connect(d->iconSizeSlider, SIGNAL(valueChanged(int)),
            this, SLOT(slotIconSizeChanged(int)));

    slotIconSizeChanged(32);

    // ---

    d->optionsBtn              = new QToolButton(btnBox);
    d->optionsBtn->setToolTip(i18nc("@info: folder-view", "Folder-View Options"));
    d->optionsBtn->setIcon(QIcon::fromTheme(QLatin1String("configure")));
    d->optionsBtn->setPopupMode(QToolButton::InstantPopup);
    d->optionsBtn->setWhatsThis(i18nc("@info: folder-view", "Selection options to render folder-view."));

    d->optionsMenu             = new QMenu(d->optionsBtn);
    QActionGroup* const optGrp = new QActionGroup(this);

    d->shortAction             = d->optionsMenu->addAction(i18nc("@action:inmenu", "Short View"));
    d->shortAction->setObjectName(QLatin1String("ShortView"));
    d->shortAction->setIcon(QIcon::fromTheme(QLatin1String("view-list-text")));
    d->shortAction->setToolTip(i18nc("@info", "Switch Folder-View to Short Mode"));
    d->shortAction->setCheckable(true);
    optGrp->addAction(d->shortAction);

    d->actionsList << d->shortAction;

    d->detailledAction         = d->optionsMenu->addAction(i18nc("@action:inmenu", "Detailled View"));
    d->detailledAction->setObjectName(QLatin1String("DetailledView"));
    d->detailledAction->setIcon(QIcon::fromTheme(QLatin1String("view-list-details")));
    d->detailledAction->setToolTip(i18nc("@info", "Switch Folder-View to Detailled Mode"));
    d->detailledAction->setCheckable(true);
    optGrp->addAction(d->detailledAction);

    d->actionsList << d->detailledAction;

    optGrp->setExclusive(true);
    d->optionsBtn->setMenu(d->optionsMenu);

    connect(d->optionsMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(slotOptionsChanged(QAction*)));

    // ---

    btnAction                = new QAction(this);
    btnAction->setObjectName(QLatin1String("LoadContents"));
    btnAction->setIcon(QIcon::fromTheme(QLatin1String("media-playlist-normal")));
    btnAction->setText(i18nc("action", "Load Contents"));
    btnAction->setToolTip(i18nc("@info", "Load Contents to Editor"));

    connect(btnAction, SIGNAL(triggered(bool)),
            this, SIGNAL(signalLoadContents()));

    d->actionsList << btnAction;

    d->loadBtn               = new QToolButton(btnBox);
    d->loadBtn->setDefaultAction(btnAction);
    d->loadBtn->setFocusPolicy(Qt::NoFocus);

    connect(d->loadBtn, SIGNAL(clicked()),
            this, SIGNAL(signalLoadContents()));

    // ---

    d->pathEdit              = new QComboBox(this);
    d->pathEdit->setEditable(true);
    d->pathEdit->setDuplicatesEnabled(false);
    d->pathEdit->setWhatsThis(i18nc("@info", "Enter the customized folder-view path or select from history"));
    d->pathEdit->lineEdit()->setPlaceholderText(i18nc("@info: folder-view path edit", "Enter local path here..."));
    d->pathEdit->lineEdit()->setClearButtonEnabled(true);

    connect(d->pathEdit->lineEdit(), SIGNAL(returnPressed()),
            this, SLOT(slotCustomPathChanged()));

    connect(d->pathEdit, SIGNAL(textActivated(QString)),
            this, SIGNAL(signalCustomPathChanged(QString)));
}

ShowfotoFolderViewBar::~ShowfotoFolderViewBar()
{
    delete d;
}

QAction* ShowfotoFolderViewBar::toolBarAction(const QString& name) const
{
    foreach (QAction* const act, d->actionsList)
    {
        if (act && (act->objectName() == name))
        {
            return act;
        }
    }

    return nullptr;
}

void ShowfotoFolderViewBar::setFolderViewMode(int mode)
{
    if (mode == ShowfotoFolderViewList::ShortView)
    {
        d->shortAction->setChecked(true);
    }
    else
    {
        d->detailledAction->setChecked(true);
    }
}

int ShowfotoFolderViewBar::folderViewMode() const
{
    if (d->shortAction->isChecked())
    {
        return ShowfotoFolderViewList::ShortView;
    }

    return ShowfotoFolderViewList::DetailledView;
}

void ShowfotoFolderViewBar::setCurrentPath(const QString& path)
{
    d->pathEdit->lineEdit()->setText(path);

    if (d->pathEdit->findText(path) == -1)
    {
        d->pathEdit->addItem(path);
    }

    QDir dir(path);
    d->upBtn->setEnabled(dir.cdUp());
}

void ShowfotoFolderViewBar::setIconSize(int size)
{
    d->iconSizeSlider->setValue(size);
}

int ShowfotoFolderViewBar::iconSize() const
{
    return d->iconSizeSlider->value();
}

void ShowfotoFolderViewBar::slotCustomPathChanged()
{
    QString path = d->pathEdit->lineEdit()->text();

    if (d->pathEdit->findText(path) == -1)
    {
        d->pathEdit->addItem(path);
    }

    emit signalCustomPathChanged(path);
}

void ShowfotoFolderViewBar::slotPreviousEnabled(bool b)
{
    d->previousBtn->setEnabled(b);
}

void ShowfotoFolderViewBar::slotNextEnabled(bool b)
{
    d->nextBtn->setEnabled(b);
}

void ShowfotoFolderViewBar::slotIconSizeChanged(int size)
{
    d->iconSizeSlider->setToolTip(i18nc("@info", "Icon Size: %1", size));

    emit signalIconSizeChanged(size);
}

void ShowfotoFolderViewBar::slotOptionsChanged(QAction* action)
{
    int mode;

    if      (action == d->shortAction)
    {
        mode = ShowfotoFolderViewList::ShortView;
    }
    else if (action == d->detailledAction)
    {
        mode = ShowfotoFolderViewList::DetailledView;
    }

    emit signalViewModeChanged(mode);
}

} // namespace ShowFoto

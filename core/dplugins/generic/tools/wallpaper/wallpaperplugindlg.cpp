/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-04-02
 * Description : plugin to export image as wallpaper - Settings dialog
 *
 * SPDX-FileCopyrightText: 2019      by Igor Antropov <antropovi at yahoo dot com>
 * SPDX-FileCopyrightText: 2019-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "wallpaperplugindlg.h"

// Qt includes

#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_globals.h"
#include "wallpaperplugin.h"

namespace DigikamGenericWallpaperPlugin
{

class Q_DECL_HIDDEN WallpaperPluginDlg::Private
{
public:

    Private() = default;

    QComboBox* layoutCB = nullptr;
    QWidget*   page     = nullptr;
};

WallpaperPluginDlg::WallpaperPluginDlg(DPlugin* const plugin, QWidget* const parent)
    : DPluginDialog(parent, QLatin1String("WallpaperPluginDlg")),
      d            (new Private)
{
    setPlugin(plugin);
    setWindowIcon(plugin->icon());
    setWindowTitle(i18nc("@title:window", "WallPaper Settings"));

    m_buttons->addButton(QDialogButtonBox::Cancel);
    m_buttons->addButton(QDialogButtonBox::Ok);
    m_buttons->button(QDialogButtonBox::Ok)->setDefault(true);
    d->page                  = new QWidget(this);
    QVBoxLayout* const vbx   = new QVBoxLayout(this);
    vbx->addWidget(d->page);
    vbx->addWidget(m_buttons);
    setLayout(vbx);
    setModal(false);

    // -------------------

    const int spacing       = layoutSpacing();

    QGridLayout* const grid = new QGridLayout(d->page);
    QLabel* const lbl       = new QLabel(i18n("Wallpaper Layout:"), d->page);
    d->layoutCB             = new QComboBox(d->page);
    d->layoutCB->addItem(i18n("Adjusted"),                   WallpaperPlugin::Adjusted);

#ifndef Q_OS_WIN

    d->layoutCB->addItem(i18n("Adjusted with Aspect ratio"), WallpaperPlugin::AdjustedAspectRatio);
    d->layoutCB->addItem(i18n("Adjusted and cropped"),       WallpaperPlugin::AdjustedCropped);

#endif

    d->layoutCB->addItem(i18n("Mosaic"),                     WallpaperPlugin::Mosaic);
    d->layoutCB->addItem(i18n("Centered"),                   WallpaperPlugin::Centered);
    lbl->setBuddy(d->layoutCB);

    grid->addWidget(lbl,         0, 0, 1, 1);
    grid->addWidget(d->layoutCB, 0, 1, 1, 1);
    grid->setSpacing(spacing);

    // -------------------

    connect(m_buttons->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
            this, &WallpaperPluginDlg::reject);

    connect(m_buttons->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &WallpaperPluginDlg::accept);

    // -------------------

    adjustSize();
}

WallpaperPluginDlg::~WallpaperPluginDlg()
{
    delete d;
}

int WallpaperPluginDlg::wallpaperLayout() const
{
    return (d->layoutCB->currentData().toInt());
}

} // namespace DigikamGenericWallpaperPlugin

#include "moc_wallpaperplugindlg.cpp"

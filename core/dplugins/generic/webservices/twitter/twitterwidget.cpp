/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-06-29
 * Description : a tool to export images to Twitter social network
 *
 * Copyright (C) 2018 by Tarek Talaat <tarektalaat93 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "twitterwidget.h"

// Qt includes

#include <QLabel>
#include <QGroupBox>

// Local includes

namespace DigikamGenericTwitterPlugin
{

TwWidget::TwWidget(QWidget* const parent,
                   DInfoInterface* const iface,
                   const QString& toolName)
    : WSSettingsWidget(parent, iface, toolName)
{
    getUploadBox()->hide();
    getSizeBox()->hide();

    /* (Feb 2019)
     * Temporarily hide album box because Twitter API has no functionality for albums
     */
    getAlbumBox()->hide();
}

TwWidget::~TwWidget()
{
}

void TwWidget::updateLabels(const QString& name, const QString& url)
{
    QString web(QLatin1String("https://www.twitter.com/"));

    if (!url.isEmpty())
    {
        web = url;
    }

    getHeaderLbl()->setText(QString::fromLatin1(
        "<b><h2><a href='%1'>"
        "<font color=\"#9ACD32\">Twitter</font>"
        "</a></h2></b>").arg(web));

    if (name.isEmpty())
    {
        getUserNameLabel()->clear();
    }
    else
    {
        getUserNameLabel()->setText(QString::fromLatin1("<b>%1</b>").arg(name));
    }
}

} // namespace DigikamGenericTwitterPlugin

/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-01-31
 * Description : a presentation tool.
 *
 * Copyright (C) 2006-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_PRESENTATION_MNGR_H
#define DIGIKAM_PRESENTATION_MNGR_H

// Qt includes

#include <QObject>
#include <QList>
#include <QUrl>

// Local includes

#include "dplugingeneric.h"
#include "dinfointerface.h"
#include "presentationdlg.h"

using namespace Digikam;

namespace DigikamGenericPresentationPlugin
{

class PresentationContainer;

class PresentationMngr : public QObject
{
    Q_OBJECT

public:

    explicit PresentationMngr(QObject* const parent, DInfoInterface* const iface);
    ~PresentationMngr() override;

    void addFiles(const QList<QUrl>& urls);
    void showConfigDialog();

    void setPlugin(DPlugin* const plugin);

private Q_SLOTS:

    void slotSlideShow();

private:

    DPlugin*               m_plugin;
    PresentationDlg*       m_dialog;
    PresentationContainer* m_sharedData;
};

} // namespace DigikamGenericPresentationPlugin

#endif // DIGIKAM_PRESENTATION_MNGR_H

/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-09
 * Description : a presentation tool.
 *
 * Copyright (C) 2008 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
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

#ifndef DIGIKAM_PRESENTATION_ADV_PAGE_H
#define DIGIKAM_PRESENTATION_ADV_PAGE_H

// Local includes

#include "ui_presentation_advpage.h"

namespace DigikamGenericPresentationPlugin
{

class PresentationContainer;

class PresentationAdvPage : public QWidget,
                            public Ui::PresentationAdvPage
{
    Q_OBJECT

public:

    explicit PresentationAdvPage(QWidget* const parent,
                                 PresentationContainer* const sharedData);
    ~PresentationAdvPage() override;

    void readSettings();
    void saveSettings();

Q_SIGNALS:

    void useMillisecondsToggled();

private Q_SLOTS:

    void slotUseMillisecondsToggled();

private:

    PresentationContainer* m_sharedData;
};

} // namespace DigikamGenericPresentationPlugin

#endif // DIGIKAM_PRESENTATION_ADV_PAGE_H

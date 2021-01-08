/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a tool to generate video slideshow from images.
 *
 * Copyright (C) 2017-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_VIDSLIDE_FINAL_PAGE_H
#define DIGIKAM_VIDSLIDE_FINAL_PAGE_H

// Qt includes

#include <QString>

// Local includes

#include "dwizardpage.h"

using namespace Digikam;

namespace DigikamGenericVideoSlideShowPlugin
{

class VidSlideFinalPage : public DWizardPage
{
    Q_OBJECT

public:

    explicit VidSlideFinalPage(QWizard* const dialog, const QString& title);
    ~VidSlideFinalPage()    override;

    void initializePage()   override;
    bool isComplete() const override;
    void cleanupPage()      override;

private Q_SLOTS:

    void slotProcess();
    void slotDone(bool);
    void slotMessage(const QString&, bool);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericVideoSlideShowPlugin

#endif // DIGIKAM_VIDSLIDE_FINAL_PAGE_H

/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-04-04
 * Description : a tool to generate HTML image galleries
 *
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QString>

// Local includes

#include "dwizardpage.h"

using namespace Digikam;

namespace DigikamGenericHtmlGalleryPlugin
{

class HTMLIntroPage : public DWizardPage
{
    Q_OBJECT

public:

    explicit HTMLIntroPage(QWizard* const dialog, const QString& title);
    ~HTMLIntroPage()      override;

    void initializePage() override;
    bool validatePage()   override;

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace DigikamGenericHtmlGalleryPlugin

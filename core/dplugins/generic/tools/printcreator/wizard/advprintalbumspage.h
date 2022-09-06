/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-06-27
 * Description : a tool to print images
 *
 * Copyright (C) 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ADV_PRINT_ALBUMS_PAGE_H
#define DIGIKAM_ADV_PRINT_ALBUMS_PAGE_H

// Qt includes

#include <QList>
#include <QUrl>
#include <QString>

// Local includes

#include "dwizardpage.h"

using namespace Digikam;

namespace DigikamGenericPrintCreatorPlugin
{

class AdvPrintAlbumsPage : public DWizardPage
{
    Q_OBJECT

public:

    explicit AdvPrintAlbumsPage(QWizard* const dialog, const QString& title);
    ~AdvPrintAlbumsPage()   override;

    bool validatePage()     override;
    bool isComplete() const override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericPrintCreatorPlugin

#endif // DIGIKAM_ADV_PRINT_ALBUMS_PAGE_H

/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a tool to print images
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

#ifndef DIGIKAM_ADV_PRINT_FINAL_PAGE_H
#define DIGIKAM_ADV_PRINT_FINAL_PAGE_H

// Qt includes

#include <QString>
#include <QPainter>
#include <QList>
#include <QRect>
#include <QStringList>

// Local includes

#include "dwizardpage.h"

using namespace Digikam;

namespace DigikamGenericPrintCreatorPlugin
{

class AdvPrintPhoto;
class AdvPrintPhotoSize;
class AdvPrintPhotoPage;

class AdvPrintFinalPage : public DWizardPage
{
    Q_OBJECT

public:

    explicit AdvPrintFinalPage(QWizard* const dialog, const QString& title);
    ~AdvPrintFinalPage() override;

    void initializePage() override;
    bool isComplete() const override;
    void cleanupPage() override;

    bool checkTempPath(const QString& tempPath) const;
    void removeGimpFiles();
    void setPhotoPage(AdvPrintPhotoPage* const photoPage);

private Q_SLOTS:

    void slotProcess();
    void slotPrint(bool);
    void slotDone(bool);
    void slotMessage(const QString&, bool);

private:

    bool print();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericPrintCreatorPlugin

#endif // DIGIKAM_ADV_PRINT_FINAL_PAGE_H

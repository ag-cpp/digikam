/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-04-04
 * Description : a tool to generate jAlbum image galleries
 *
 * SPDX-FileCopyrightText: 2013-2019 by Andrew Goodbody <ajg zero two at elfringham dot co dot uk>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QList>
#include <QUrl>
#include <QString>

// Local includes

#include "dwizardpage.h"

using namespace Digikam;

namespace DigikamGenericJAlbumPlugin
{

class JAlbumSelectionPage : public DWizardPage
{
    Q_OBJECT

public:

    explicit JAlbumSelectionPage(QWizard* const dialog, const QString& title);
    ~JAlbumSelectionPage()  override;

    void initializePage()   override;
    bool validatePage()     override;
    bool isComplete() const override;

    void setItemsList(const QList<QUrl>& urls);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace DigikamGenericJAlbumPlugin

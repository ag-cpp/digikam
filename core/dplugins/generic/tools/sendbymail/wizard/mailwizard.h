/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-06-27
 * Description : a tool to export items by email.
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

#ifndef DIGIKAM_MAIL_WIZARD_H
#define DIGIKAM_MAIL_WIZARD_H

// Qt includes

#include <QList>
#include <QUrl>

// Local includes

#include "dwizarddlg.h"
#include "dinfointerface.h"
#include "mailsettings.h"

using namespace Digikam;

namespace DigikamGenericSendByMailPlugin
{

class MailWizard : public DWizardDlg
{
    Q_OBJECT

public:

    explicit MailWizard(QWidget* const parent, DInfoInterface* const iface = nullptr);
    ~MailWizard() override;

    bool validateCurrentPage()       override;
    int  nextId() const              override;

    DInfoInterface* iface()    const;
    MailSettings*   settings() const;

    void setItemsList(const QList<QUrl>& urls);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericSendByMailPlugin

#endif // DIGIKAM_MAIL_WIZARD_H

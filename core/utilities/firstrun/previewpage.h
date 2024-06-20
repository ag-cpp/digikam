/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-28-04
 * Description : first run assistant dialog
 *
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "dwizardpage.h"

namespace Digikam
{

class PreviewPage : public DWizardPage
{
    Q_OBJECT

public:

    explicit PreviewPage(QWizard* const dlg);
    ~PreviewPage() override;

    void saveSettings();

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam

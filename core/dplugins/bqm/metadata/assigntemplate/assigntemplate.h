/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-03-04
 * Description : assign metadata template batch tool.
 *
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "batchtool.h"
#include "templateselector.h"
#include "templateviewer.h"

using namespace Digikam;

namespace DigikamBqmAssignTemplatePlugin
{

class AssignTemplate : public BatchTool
{
    Q_OBJECT

public:

    explicit AssignTemplate(QObject* const parent = nullptr);
    ~AssignTemplate()                                       override = default;

    BatchToolSettings defaultSettings()                     override;

    BatchTool* clone(QObject* const parent = nullptr) const override;

    void registerSettingsWidget()                           override;

private:

    bool toolOperations()                                   override;

private Q_SLOTS:

    void slotAssignSettings2Widget()                        override;
    void slotSettingsChanged()                              override;

private:

    TemplateSelector* m_templateSelector = nullptr;
    TemplateViewer*   m_templateViewer   = nullptr;
};

} // namespace DigikamBqmAssignTemplatePlugin

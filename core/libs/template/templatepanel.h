/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-07-06
 * Description : metadata template settings panel.
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_TEMPLATE_PANEL_H
#define DIGIKAM_TEMPLATE_PANEL_H

// Qt includes

#include <QTabWidget>

// Local includes

#include "template.h"

namespace Digikam
{

class TemplatePanel : public QTabWidget
{
    Q_OBJECT

public:

    enum TemplateTab
    {
        RIGHTS=0,
        LOCATION,
        CONTACT,
        SUBJECTS
    };

public:

    explicit TemplatePanel(QWidget* const parent = nullptr);
    ~TemplatePanel();

    void     setTemplate(const Template& t);
    Template getTemplate() const;

    void     apply();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_TEMPLATE_PANEL_H

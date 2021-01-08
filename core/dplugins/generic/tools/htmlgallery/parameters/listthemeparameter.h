/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-04-04
 * Description : a tool to generate HTML image galleries
 *
 * Copyright (C) 2006-2010 by Aurelien Gateau <aurelien dot gateau at free dot fr>
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

#ifndef DIGIKAM_LIST_THEME_PARAMETER_H
#define DIGIKAM_LIST_THEME_PARAMETER_H

// Local includes

#include "abstractthemeparameter.h"

namespace DigikamGenericHtmlGalleryPlugin
{

/**
 * A theme parameter to select a value in a list
 */
class ListThemeParameter : public AbstractThemeParameter
{
public:

    explicit ListThemeParameter();
    ~ListThemeParameter() override;

    void     init(const QByteArray& internalName, const KConfigGroup* configGroup) override;
    QWidget* createWidget(QWidget* parent, const QString& value) const override;
    QString  valueFromWidget(QWidget*)                           const override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericHtmlGalleryPlugin

#endif // DIGIKAM_LIST_THEME_PARAMETER_H

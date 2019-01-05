/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-04-04
 * Description : a tool to generate HTML image galleries
 *
 * Copyright (C) 2012-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_JALBUM_CONFIG_H
#define DIGIKAM_JALBUM_CONFIG_H

// Qt includes

#include <QUrl>
#include <QString>

// KDE includes

#include <kconfigskeleton.h>

namespace Digikam
{

class JalbumConfig : public KConfigSkeleton
{
public:

    explicit JalbumConfig();
    virtual ~JalbumConfig();

    void setDestUrl(const QUrl&);
    QUrl destUrl() const;

    void setJarUrl(const QUrl&);
    QUrl jarUrl() const;

    void setImageSelectionTitle(const QString&);
    QString imageSelectionTitle() const;

protected:

    QUrl       m_destUrl;
    QUrl       m_jarUrl;
    QString    m_imageSelectionTitle; // Jalbum title to use for JalbumInfo::ImageGetOption::IMAGES selection.
};

} // namespace Digikam

#endif // DIGIKAM_JALBUM_CONFIG_H

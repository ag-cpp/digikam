/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-02-02
 * Description : a tool to export items to ImageShackSession web service
 *
 * Copyright (C) 2012      by Dodon Victor <dodonvictor at gmail dot com>
 * Copyright (C) 2013-2018 by Caulier Gilles <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_IMAGE_SHACK_SESSION_H
#define DIGIKAM_IMAGE_SHACK_SESSION_H

// Qt includes

#include <QString>

namespace DigikamGenericImageShackPlugin
{

class ImageShackSession
{

public:

    explicit ImageShackSession();
    ~ImageShackSession();

public:

    bool    loggedIn()  const;
    QString username()  const;
    QString email()     const;
    QString password()  const;
    QString credits()   const;
    QString authToken() const;

    void setLoggedIn(bool b);
    void setUsername(const QString& username);
    void setEmail(const QString& email);
    void setPassword(const QString& pass);
    void setCredits(const QString& credits);
    void setAuthToken(const QString& token);

    void readSettings();
    void saveSettings();

    void logOut();

private:

    // Hidden copy constructor and assignment operator.
    ImageShackSession(const ImageShackSession&);
    ImageShackSession& operator=(const ImageShackSession&);

    class Private;
    Private* const d;
};

} // namespace DigikamGenericImageShackPlugin

#endif // DIGIKAM_IMAGE_SHACK_SESSION_H

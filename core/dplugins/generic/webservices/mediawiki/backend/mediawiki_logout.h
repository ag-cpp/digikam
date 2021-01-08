/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-03-22
 * Description : a Iface C++ interface
 *
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
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

#ifndef DIGIKAM_MEDIAWIKI_LOGOUT_H
#define DIGIKAM_MEDIAWIKI_LOGOUT_H

// Local includes

#include "mediawiki_job.h"


namespace MediaWiki
{

class Iface;
class LogoutPrivate;

/**
 * @brief Logout job.
 *
 * Uses for log out a user.
 */
class Logout : public Job
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Logout)

public:

    /**
     * @brief Constructs a Logout job.
     * @param parent the QObject parent
     */
    explicit Logout(Iface& MediaWiki, QObject* const parent = nullptr);

    /**
     * @brief Destroys the Logout job.
     */
    ~Logout() override;

    /**
     * @brief Starts the job asynchronously.
     */
    void start() override;

private Q_SLOTS:

    void doWorkSendRequest();
    void doWorkProcessReply();
};

} // namespace MediaWiki

#endif // DIGIKAM_MEDIAWIKI_LOGOUT_H

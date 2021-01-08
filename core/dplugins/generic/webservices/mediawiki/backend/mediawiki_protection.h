/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-03-22
 * Description : a Iface C++ interface
 *
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011      by Vincent Garcia <xavier dot vincent dot garcia at gmail dot com>
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

#ifndef DIGIKAM_MEDIAWIKI_PROTECTION_H
#define DIGIKAM_MEDIAWIKI_PROTECTION_H

// Qt includes

#include <QString>

namespace MediaWiki
{

/**
 * @brief Protection info job.
 *
 * Represent protection parameters in a page.
 */
class Protection
{

public:

    /**
     * @brief Constructs a protection.
     *
     * You can set parameters of the protection after.
     */
    Protection();

    /**
     * @brief Constructs an protection from an other protection.
     * @param other an other protection
     */
    Protection(const Protection& other);

    /**
     * @brief Destructs a protection.
     */
    ~Protection();

    /**
     * @brief Assigning an protection from an other protection.
     * @param other an other protection
     */
    Protection& operator=(const Protection& other);

    /**
     * @brief Returns true if this instance and other are equal, else false.
     * @param other instance to compare
     * @return true if there are equal, else false
     */
    bool operator==(const Protection& other)    const;

    /**
     * @brief Set the protection type.
     * @param type the protection type
     */
    void setType(const QString& type);

    /**
     * @brief Get the protection type.
     * @return the protection type
     */
    QString type()                              const;

    /**
     * @brief Set the page protection level.
     * @param level the page protection level
     */
    void setLevel(const QString& level);

    /**
     * @brief Get the page protection level.
     * @return the page protection level
     */
    QString level()                             const;

    /**
     * @brief Set the expiry date.
     * @param expiry the expiry date
     */
    void setExpiry(const QString& expiry);

    /**
   ² * @brief Get the expiry date.
     * @return the expiry date
     */
    QString expiry()                            const;

    /**
     * @brief Set the source.
     * @param source the source
     */
    void setSource(const QString& source);

    /**
     * @brief Get the source.
     * @return the source
     */
    QString source()                            const;

private:

    class ProtectionPrivate;
    ProtectionPrivate* const d;
};

} // namespace MediaWiki

#endif // DIGIKAM_MEDIAWIKI_PROTECTION_H

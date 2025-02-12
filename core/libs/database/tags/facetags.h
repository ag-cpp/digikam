/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-08-08
 * Description : Accessing face tags
 *
 * SPDX-FileCopyrightText: 2010-2011 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QList>
#include <QMultiMap>
#include <QRect>
#include <QString>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_DATABASE_EXPORT FaceTags
{

public:

    /**
     * Returns a boolean value indicating whether the given tagId represents a person
     */
    static bool           isPerson(int tagId);

    static bool           isTheUnknownPerson(int tagId);
    static int            unknownPersonTagId();

    static bool           isTheUnconfirmedPerson(int tagId);
    static int            unconfirmedPersonTagId();

    static bool           isTheIgnoredPerson(int tagId);
    static int            ignoredPersonTagId();

    static bool           isSystemPersonTagId(int tagId);

    /**
     * A method to return a list of all person tags in the DB
     */
    static QList<int>     allPersonTags();

    /**
     * A method to return a list of all person tag names in the DB
     */
    static QList<QString> allPersonNames();

    /**
     * A method to return a list of all person tag paths in the DB
     */
    static QList<QString> allPersonPaths();

    /**
     * The suggested parent tag for persons
     */
    static int            personParentTag();

    /**
     * Looks for the given person name under the People tags tree, and returns an ID. Returns 0 if no name found.
     * Per default, fullName is the same as name.
     * As parentId of -1 signals to look for any tag, a valid parentId will limit the search to direct children
     * of this tag. parentId of 0 means top-level tag.
     */
    static int            tagForPerson(const QString& name, int parentId = -1, const QString& fullName = QString());

    /**
     * First, looks for the given person name in person tags, and returns an ID.
     * If not, creates a new tag.
     * Per default, fullName is the same as name.
     */
    static int            getOrCreateTagForPerson(const QString& name, int parentId = -1, const QString& fullName = QString());

    /**
     * Ensure that the given tag is a person tag. If not, it will be converted.
     * Optionally, pass the full name. (tag name is not changed)
     */
    static void           ensureIsPerson(int tagId, const QString& fullName = QString());

    static QString        getNameForRect(qlonglong imageid, const QRect& faceRect);

    /**
     * Use attributes as used by FacesEngine to identify or create a person tag;
     * From the database, produce the identity attributes identifying the corresponding identity.
     */
    static int            getOrCreateTagForIdentity(const QMultiMap<QString, QString>& attributes);
    static QMultiMap<QString, QString> identityAttributes(int tagId);

    /**
     * Map an existing tag to a FacesEngine Identity.
     * Subsequently, the Identity can be retrieved via the identityAttributes().
     */
    static void applyTagIdentityMapping(int tagId, const QMultiMap<QString, QString>& attributes);

    /**
     * Return a person's name for a tag
     */
    static QString        faceNameForTag(int tagId);

    static int            scannedForFacesTagId();
};

} // namespace Digikam

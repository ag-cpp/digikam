/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2021-08-07
 * Description : The access point to Face database.
 *
 * Copyright (C) 2012-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2020-2021 by Nghia Duong    <minhnghiaduong997 at gmail dot com>
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


#ifndef FACIAL_RECOGNITION_WRAPPER_H
#define FACIAL_RECOGNITION_WRAPPER_H

// Qt includes

#include <QList>
#include <QMultiMap>

// Local includes

#include "digikam_export.h"
#include "identity.h"
#include "digikam_opencv.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT FaceDbWrapper
{
public:

    explicit FaceDbWrapper();
    ~FaceDbWrapper();

public:

    /**
     * Checks the integrity and returns true if everything is fine.
     */
    bool integrityCheck();

    /**
     * Shrinks the database.
     */
    void vacuum();

public:

    // --- Identity management -----------------------------------------

    /// NOTE: For the documentation of standard attributes, see identity.h

    /**
     * Returns all identities known to the database
     */
    QList<Identity> allIdentities()                                         const;
    Identity        identity(int id)                                        const;

    /**
     * Finds the first identity with matching attribute - value.
     * Returns a null identity if no match is found or attribute is empty.
     */
    Identity findIdentity(const QString& attribute, const QString& value)   const;

    /**
     * Finds the identity matching the given attributes.
     * Attributes are first checked with knowledge of their meaning.
     * Secondly, all unknown attributes are used.
     * Returns a null Identity if no match is possible or the map is empty.
     */
    Identity findIdentity(const QMultiMap<QString, QString>& attributes)    const;

    /**
     * Adds a new identity with the specified attributes.
     * Please note that a UUID is automatically generated.
     */
    Identity addIdentity(const QMultiMap<QString, QString>& attributes);

    /**
     * This is the debug version of addIdentity, so the identity is only added
     * to identityCache, but not into the recognition database.
     */
    Identity addIdentityDebug(const QMultiMap<QString, QString>& attributes);

    /**
     * Adds or sets, resp., the attributes of an identity.
     */
    void addIdentityAttributes(int id, const QMultiMap<QString, QString>& attributes);
    void addIdentityAttribute(int id, const QString& attribute, const QString& value);
    void setIdentityAttributes(int id, const QMultiMap<QString, QString>& attributes);

    /**
     * Deletes an identity from the database.
     */
    void deleteIdentity(const Identity& identityToBeDeleted);

    /**
     * Deletes a list of identities from the database.
     */
    void deleteIdentities(QList<Identity> identitiesToBeDeleted);

    // --- Faces embedding management ----------------------------------------------------
    /**
     * Save face embedding extracted from face images
     */
    void saveEmbeddings(const QList<cv::Mat>& faceEmbeddings,
                        const QList<int>&     facetagIds,
                        const QString&        context);

    /**
     * Save identities assigned to saved face embeddings
     */
    void assignLabels(const QList<int>& faceEmbeddingIds,
                      const QList<int>& identityIds);

    /**
     * Deletes the training data for all identities,
     * leaving the identities as such in the database.
     */
    void clearAllEmbedding(const QString& trainingContext = QString());

    /**
     * Deletes the training data for the given identity,
     * leaving the identity as such in the database.
     */
    void clearEmbedding(const QList<Identity>& identitiesToClean,
                        const QString& trainingContext = QString());

private:

    // Disable
    FaceDbWrapper(const FaceDbWrapper&)            = delete;
    FaceDbWrapper& operator=(const FaceDbWrapper&) = delete;

private:

    class Private;
    static Private* d;
};

} // namespace Digikam

#endif // FACIAL_RECOGNITION_WRAPPER_H

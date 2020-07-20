/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 02-02-2012
 * Description : Face database interface to train identities.
 *
 * Copyright (C) 2012-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_FACE_DB_H
#define DIGIKAM_FACE_DB_H

// C++ includes

#include <vector>

// Qt includes

#include <QString>
#include <QFile>
#include <QDataStream>
#include <QStandardPaths>

// Local includes

#include "digikam_config.h"
#include "identity.h"
#include "facedbbackend.h"
#include "opencvmatdata.h"

namespace Digikam
{

#ifdef USE_DNN_RECOGNITION_BACKEND

//class DNNFaceModel;

#else

class LBPHFaceModel;

#endif

/*
NOTE: experimental and deprecated

class EigenFaceModel;
class FisherFaceModel;

*/

class FaceDb
{
public:

    explicit FaceDb(FaceDbBackend* const db);
    ~FaceDb();

    BdEngineBackend::QueryState setSetting(const QString& keyword, const QString& value);
    QString setting(const QString& keyword) const;

    /**
     * Returns true if the integrity of the database is preserved.
     */
    bool integrityCheck();

    /**
     * Shrinks the database.
     */
    void vacuum();

public:

    // --- Identity management (facedb_identity.cpp)

    int  addIdentity()                      const;
    int  getNumberOfIdentities()            const;

    void updateIdentity(const Identity& p);
    void deleteIdentity(int id);
    void deleteIdentity(const QString& uuid);
    void clearIdentities();

    QList<Identity> identities()            const;
    QList<int>      identityIds()           const;

#ifdef USE_DNN_RECOGNITION_BACKEND

public:

    // --- OpenCV DNN
/*
    void updateDNNFaceModel(DNNFaceModel& model);
    DNNFaceModel dnnFaceModel(bool debug)   const;
    void clearDNNTraining(const QString& context = QString());
    void clearDNNTraining(const QList<int>& identities, const QString& context = QString());
*/
#else

public:

    // --- OpenCV LBPH

    void updateLBPHFaceModel(LBPHFaceModel& model);
    LBPHFaceModel lbphFaceModel()           const;
    void clearLBPHTraining(const QString& context = QString());
    void clearLBPHTraining(const QList<int>& identities, const QString& context = QString());

#endif

/*
NOTE: experimental and deprecated

public:

    // --- OpenCV EIGEN

    void updateEIGENFaceModel(EigenFaceModel& model, const std::vector<cv::Mat>& images_rgb);
    EigenFaceModel eigenFaceModel()         const;
    void clearEIGENTraining(const QString& context = QString());
    void clearEIGENTraining(const QList<int>& identities, const QString& context = QString());

public:

    // --- OpenCV FISHER

    FisherFaceModel fisherFaceModel()       const;

*/

private:

    // Hidden copy constructor and assignment operator.
    FaceDb(const FaceDb&);
    FaceDb& operator=(const FaceDb&);

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FACE_DB_H

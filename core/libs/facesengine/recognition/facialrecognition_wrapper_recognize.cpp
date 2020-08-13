/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : Recognizing functions of recognition wrapper
 *
 * Copyright (C)      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C)      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * Copyright (C)      2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include "facialrecognition_wrapper_p.h"

#include <QtConcurrent>

namespace Digikam
{

QList<Identity> FacialRecognitionWrapper::recognizeFaces(ImageListProvider* const images)
{
    if (!d || !d->dbAvailable)
    {
        return QList<Identity>();
    }

    QMutexLocker lock(&d->mutex);

    std::function<Identity(QImage&)> recognizeIdentity = [this](QImage& image)
    {
        int id = -1;

        try
        {
            id = d->recognizer->recognize(image);

            qDebug() << "predicted id" << id;
        }
        catch (cv::Exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception:" << e.what();
        }
        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
        }

        if (id == -1)
        {
            return Identity();
        }
        else
        {
            return d->identityCache.value(id);
        }
    };

    QList<QImage> trainImages = images->images();

    // TODO parallelize
    QList<Identity> results;

    for (int i = 0; i < trainImages.size(); ++i)
    {
        results << recognizeIdentity(trainImages[i]);
    }

    return results;
}

QList<Identity> FacialRecognitionWrapper::recognizeFaces(const QList<QImage>& images)
{
    QListImageListProvider provider(images);

    return recognizeFaces(&provider);
}

Identity FacialRecognitionWrapper::recognizeFace(const QImage& image)
{
    QList<Identity> result = recognizeFaces(QList<QImage>() << image);

    if (result.isEmpty())
    {
        return Identity();
    }

    return result.first();
}


} // namespace Digikam

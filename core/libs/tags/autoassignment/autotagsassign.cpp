/* ============================================================
 *
 * This file is a part of digiKam
 * https://www.digikam.org
 *
 * Date        : 2023-09-02
 * Description : Class for autotagging engine
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung TRAN <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "autotagsassign.h"

// C++ includes

#include <vector>

// Qt includes

#include <QtGlobal>
#include <QStandardPaths>
#include <qmath.h>

// Local includes

#include "digikam_debug.h"
#include "dnnyolodetector.h"
#include "dnnresnetdetector.h"

namespace Digikam
{

AutoTagsAssign::AutoTagsAssign(DetectorModel model)
    : m_modelType(model)
{
    switch (m_modelType)
    {
        case DetectorModel::YOLOV5NANO:
        {
            m_inferenceEngine = new DNNYoloDetector(YoloVersions(m_modelType));
            break;
        }

        case DetectorModel::YOLOV5XLARGE:
        {
            m_inferenceEngine = new DNNYoloDetector(YoloVersions(m_modelType));
            break;
        }

        case DetectorModel::RESNET50:
        {
            m_inferenceEngine = new DNNResnetDetector();
            break;
        }

        default:
        {
            qCWarning(DIGIKAM_AUTOTAGSENGINE_LOG) << "Unknow auto-tags model";
            break;
        }
    }
}

AutoTagsAssign::~AutoTagsAssign()
{
    delete m_inferenceEngine;
}

cv::Mat AutoTagsAssign::prepareForDetection(const DImg& inputImage) const
{
    if (inputImage.isNull() || !inputImage.size().isValid())
    {
        return cv::Mat();
    }

    cv::Mat cvImage;

    try
    {
        int type               = inputImage.sixteenBit() ? CV_16UC4 : CV_8UC4;
        cv::Mat cvImageWrapper = cv::Mat(inputImage.height(), inputImage.width(), type, inputImage.bits());

        if (inputImage.hasAlpha())
        {
            cvtColor(cvImageWrapper, cvImage, cv::COLOR_RGBA2BGR);
        }
        else
        {
            cvtColor(cvImageWrapper, cvImage, cv::COLOR_RGB2BGR);
        }

        if (type == CV_16UC4)
        {
            cvImage.convertTo(cvImage, CV_8UC3, 1 / 256.0);
        }
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "Default exception from OpenCV";
    }

    return cvImage;
}

cv::Mat AutoTagsAssign::prepareForDetection(const QImage& inputImage) const
{
    if (inputImage.isNull() || !inputImage.size().isValid())
    {
        return cv::Mat();
    }

    cv::Mat cvImage;
    cv::Mat cvImageWrapper;
    QImage qimage(inputImage);

    try
    {
        switch (qimage.format())
        {
            case QImage::Format_RGB32:
            case QImage::Format_ARGB32:
            case QImage::Format_ARGB32_Premultiplied:
            {
                // I think we can ignore premultiplication when converting to grayscale

                cvImageWrapper = cv::Mat(qimage.height(), qimage.width(), CV_8UC4,
                                         qimage.scanLine(0), qimage.bytesPerLine());
                cvtColor(cvImageWrapper, cvImage, cv::COLOR_RGBA2BGR);
                break;
            }

            default:
            {
                qimage         = qimage.convertToFormat(QImage::Format_RGB888);
                cvImageWrapper = cv::Mat(qimage.height(), qimage.width(), CV_8UC3,
                                         qimage.scanLine(0), qimage.bytesPerLine());
                cvtColor(cvImageWrapper, cvImage, cv::COLOR_RGB2BGR);
                break;
            }
        }
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "Default exception from OpenCV";
    }

    return cvImage;
}

cv::Mat AutoTagsAssign::prepareForDetection(const QString& inputImagePath) const
{
    std::vector<char> buffer;
    QFile file(inputImagePath);
    buffer.resize(file.size());

    if (!file.open(QIODevice::ReadOnly))
    {
        return cv::Mat();
    }

    file.read(buffer.data(), file.size());
    file.close();

    cv::Mat cvImage;

    try
    {
        cvImage = cv::imdecode(std::vector<char>(buffer.begin(), buffer.end()), cv::IMREAD_COLOR);
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "Default exception from OpenCV";
    }

    return cvImage;
}

std::vector<cv::Mat> AutoTagsAssign::prepareForDetection(const QList<DImg>& inputImages, int batchSize) const
{
    std::vector<cv::Mat> result;

    try
    {
        for (const auto& img : inputImages)
        {
            result.push_back(prepareForDetection(img));
        }

        // Add black imgs to fullfill the batch size

        cv::Size inputSize = m_inferenceEngine->getinputImageSize();

        while ((result.size() % batchSize) != 0)
        {
            cv::Mat dummycvImg = cv::Mat::zeros(inputSize.height, inputSize.width, CV_8UC3);
            result.push_back(dummycvImg);
        }
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "Default exception from OpenCV";
    }

    return result;
}

std::vector<cv::Mat> AutoTagsAssign::prepareForDetection(const QList<QString>& inputImagePaths, int batchSize) const
{
    std::vector<cv::Mat> result;

    try
    {
        for (const auto& imgPath : inputImagePaths)
        {
            result.push_back(prepareForDetection(imgPath));
        }

        // Add black imgs to fullfill the batch size

        cv::Size inputSize = m_inferenceEngine->getinputImageSize();

        while ((result.size() % batchSize) != 0)
        {
            cv::Mat dummycvImg = cv::Mat::zeros(inputSize.height, inputSize.width, CV_8UC3);
            result.push_back(dummycvImg);
        }
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "Default exception from OpenCV";
    }

    return result;
}

QList<QString> AutoTagsAssign::generateTagsList(const DImg& image)
{
    QList<QString> result;

    if (image.isNull() || !image.size().isValid())
    {
        return result;
    }

    try
    {
        cv::Mat cvImage = prepareForDetection(image);
        result          = m_inferenceEngine->generateObjects(cvImage);
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "Default exception from OpenCV";
    }

    return result;
}

QList<QString> AutoTagsAssign::generateTagsList(const QImage& image)
{
    QList<QString> result;

    if (image.isNull() || !image.size().isValid())
    {
        return result;
    }

    try
    {
        cv::Mat cvImage = prepareForDetection(image);
        result          = m_inferenceEngine->generateObjects(cvImage);

        return result;
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "Default exception from OpenCV";
    }

    return result;
}

QList<QString> AutoTagsAssign::generateTagsList(const QString& imagePath)
{
    QList<QString> result;

    try
    {
        cv::Mat cvImage = prepareForDetection(imagePath);
        result          = m_inferenceEngine->generateObjects(cvImage);

        return result;
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "Default exception from OpenCV";
    }

    return result;
}

QList<QList<QString>> AutoTagsAssign::generateTagsList(const QList<DImg>& inputImages, int batchSize) const
{
    QList<QList<QString> > result;

    try
    {
        std::vector<cv::Mat> cvImages = prepareForDetection(inputImages, batchSize);
        result                        = m_inferenceEngine->generateObjects(cvImages);

        return result;
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "Default exception from OpenCV";
    }

    return result;
}

QList<QList<QString>> AutoTagsAssign::generateTagsList(const QList<QString>& inputImagePaths, int batchSize) const
{
    QList<QList<QString> > result;

    try
    {
        std::vector<cv::Mat> cvImages = prepareForDetection(inputImagePaths, batchSize);
        result                        = m_inferenceEngine->generateObjects(cvImages);

        return result;
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "Default exception from OpenCV";
    }

    return result;
}

QList<QString> AutoTagsAssign::getPredefinedTagsPath() const
{
    QList<QString> tagsPaths;
    QList<QString> objects = m_inferenceEngine->getPredefinedClasses();
    const QString rootTag  = QLatin1String("auto/");

    for (const auto& obj : objects)
    {
        tagsPaths.append(rootTag + obj);
    }

    return tagsPaths;
}

} // namespace Digikam

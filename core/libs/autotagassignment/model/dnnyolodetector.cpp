/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-08-08
 * Description : Derived class to perform YOLO neural network inference
 *               for face detection (here yolo version 5).
 * 
 * SPDX-FileCopyrightText: 2023 by Quoc Hung TRAN <quochungtran1999 at gmail dot com>
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */


#include "dnnyolodetector.h"

// C++ includes

#include <fstream>

// Qt includes

#include <QStandardPaths>
#include <QFileInfo>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"

namespace Digikam
{

DNNYoloDetector::DNNYoloDetector(YoloVersions modelVersion)
    : DNNBaseDetectorModel(1.0F / 255.0F, cv::Scalar(0.0 ,0.0 ,0.0), cv::Size(640, 640)),
      yoloVersion (modelVersion)    
{
    loadModels();
    predefinedClasses = loadCOCOClass();
}

DNNYoloDetector::~DNNYoloDetector()
{
}

QVector<QString>  DNNYoloDetector::loadCOCOClass()
{
    QVector<QString>  classList;

    // NOTE storing all model definition at the same application path as face engine
    QString appPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                             QLatin1String("digikam/autotagassignment"),
                                             QStandardPaths::LocateDirectory);

    QString cocoClasses = appPath + QLatin1Char('/') + QLatin1String("coco.names");

    std::ifstream ifs(cocoClasses.toStdString());
    std::string line;
    
    while (getline(ifs, line))
    {
        classList.push_back(QString::fromStdString(line));
    }

    qDebug() << "Size of predifined COCO classes" << classList.size();

    return classList;
}


QVector<QString> DNNYoloDetector::getPredefinedClasses() const
{
    return predefinedClasses;
}


bool DNNYoloDetector::loadModels()
{
    QString appPath =  QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                             QLatin1String("digikam/autotagassignment"),
                                             QStandardPaths::LocateDirectory);
        
    QString model;
    
    switch (yoloVersion)
    {
        case (YoloVersions::YOLOV5NANO):
        {
            model = appPath + QLatin1Char('/') + QLatin1String("yolov5n.onnx");  ///< smaller model
            break;
        }

        case (YoloVersions::YOLOV5XLARGE):
        {
            model = appPath + QLatin1Char('/') + QLatin1String("yolov5x.onnx");  ///< bigger model
            break;
        }    
    }

    if (QFileInfo::exists(model))
    {
        try
        {
            net = cv::dnn::readNet(model.toStdString());            
            net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
            net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

            //TODO verify GPU Options here
        }
        catch (cv::Exception& e)
        {
            qCWarning(DIGIKAM_FACEDB_LOG) << "cv::Exception:" << e.what();

            return false;
        }
        catch (...)
        {
           qCWarning(DIGIKAM_FACEDB_LOG) << "Default exception from OpenCV";

           return false;
        }
    }
    else
    {
        qCCritical(DIGIKAM_FACEDB_LOG) << "Cannot found objected detection DNN model" << model;
        return false;
    }
    
    return true;
}


QMap<QString, QVector<QRect>> DNNYoloDetector::detectObjects(const::cv::Mat& inputImage)
{

    if (inputImage.empty())
    {
        qDebug() << "Invalid image given, not detecting objects";
        return {};
    }

    QMap<QString, QVector<QRect>> detectedBoxes;
    
    std::vector<cv::Mat> outs = preprocess(inputImage);
    postprocess(inputImage, outs, detectedBoxes);

    return detectedBoxes;
}


std::vector<cv::Mat> DNNYoloDetector::preprocess(const cv::Mat& inputImage)
{
    cv::Mat inputBlob = cv::dnn::blobFromImage(inputImage, scaleFactor, inputImageSize, meanValToSubtract, true,false);
    std::vector<cv::Mat> outs;

    mutex.lock();
    {
        net.setInput(inputBlob);
        net.forward(outs, getOutputsNames());
    }
    mutex.unlock();

    return outs;
}


void DNNYoloDetector::postprocess(const cv::Mat& inputImage,
                                  const std::vector<cv::Mat>& outs,
                                  QMap <QString, QVector<QRect>>& detectedBoxes) const
{
    std::vector<int>      class_ids;
    std::vector<float>    confidences;
    std::vector<cv::Rect> boxes;

    float x_factor = float(inputImage.cols) / float(inputImageSize.width);
    float y_factor = float(inputImage.rows) / float(inputImageSize.height);

    float *data = (float *)outs[0].data;

    // Calculate the size of the data array and number of outputs
    // NOTE outsput is a cv::Mat vector of [1 x (250200 * 85)] 
    size_t data_size = outs[0].total() * outs[0].channels();
    int rows         = data_size / 85; 

    for (int i = 0; i < rows; ++i)
    {
        float confidence = data[4];
        // Discard bad detections and continue.
        
        if (confidence >= confidenceThreshold)
        {
            float* classes_scores = data + 5;

            // Create a 1x85 Mat and store class scores of 80 classes.
            cv::Mat scores(1, predefinedClasses.size(), CV_32FC1, classes_scores);

            // Perform minMaxLoc and acquire the index of best class score.
            cv::Point class_id;
            double max_class_score;
            cv::minMaxLoc(scores, 0, &max_class_score, 0, &class_id);

            // Continue if the class score is above the threshold.
            if (max_class_score > scoreThreshold)
            {
                // Store class ID and confidence in the pre-defined respective vectors.
                confidences.push_back(confidence);
                class_ids.push_back(class_id.x);
                
                // Center.
                float centerX = data[0];
                float centerY = data[1];
                
                // Box dimension.
                float w    = data[2];
                float h    = data[3];
                
                // Bounding box coordinates.
                int left      = int((centerX - 0.5 * w) * x_factor);
                int top       = int((centerY - 0.5 * h) * y_factor);
                int width     = int(w  * x_factor);
                int height    = int(h  * y_factor);
                
                // Store good detections in the boxes vector.
                boxes.push_back(cv::Rect(left, top, width, height));                    
            }
        }
        // Jump to the next row.
        data += 85;
    }

    // Perform non maximum suppression to eliminate redundant overlapping boxes with lower confidences
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confidenceThreshold, nmsThreshold, indices);

    for (const auto& id : indices)
    {
        cv::Rect bbox = boxes[id];
        QString label = predefinedClasses[class_ids[id]];
        detectedBoxes[label].push_back(QRect(bbox.x, bbox.y, bbox.width, bbox.height));
    }
}

/** Get the names of the output layers
 */
std::vector<cv::String> DNNYoloDetector::getOutputsNames() const
{
    static std::vector<cv::String> names;

    if (names.empty())
    {
        // Get the indices of the output layers, i.e. the layers with unconnected outputs

        std::vector<int> outLayers          = net.getUnconnectedOutLayers();

        // Get the names of all the layers in the network

        std::vector<cv::String> layersNames = net.getLayerNames();

        // Get the names of the output layers in names

        names.resize(outLayers.size());

        for (size_t i = 0 ; i < outLayers.size() ; ++i)
        {
            names[i] = layersNames[outLayers[i] - 1];
        }
    }

    return names;
}
} // namespace Digikam

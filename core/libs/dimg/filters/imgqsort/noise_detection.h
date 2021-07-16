/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 
 * Description : Image Quality Parser - noise detection
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * References  : 
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

#ifndef DIGIKAM_NOISE_DETECTOR_H
#define DIGIKAM_NOISE_DETECTOR_H

// Local includes

#include "dimg.h"
#include "digikam_opencv.h"

namespace Digikam
{

class NoiseDetector 
{
public:
    typedef QList<cv::Mat> Mat3D;
public:

    explicit NoiseDetector(const DImg& image);
    ~NoiseDetector();

    float detect();

private:

    cv::Mat prepareForDetection(const DImg& inputImage) const;
    
    Mat3D   decompose_by_filter(const Mat3D& filters);
    void    calculate_variance_kurtosis(const Mat3D& channels, cv::Mat& variance, cv::Mat& kurtosis);
    float   noise_variance(const cv::Mat& variance, const cv::Mat& kurtosis);
    float   normalize(const float number); // scale result to interval [0 100]

    cv::Mat raw_moment(const NoiseDetector::Mat3D& mat,int ordre);
    cv::Mat pow_mat(const cv::Mat& mat, float ordre);
    float   mean_mat(const cv::Mat& mat);
    
private:

    class Private;
    Private* const d;
};

// class singleton for band pass filter haar
class MatrixFilterHaar : public QObject
{
    Q_OBJECT

public:
    
    explicit MatrixFilterHaar();
    
    ~MatrixFilterHaar() override;
    
    /**
     * @return MatrixFilterHaar global instance
     */
    static MatrixFilterHaar* instance();

    void init();

    NoiseDetector::Mat3D get_data();

private:
    
    NoiseDetector::Mat3D m_data;

    bool m_isInit;
};

} // namespace Digikam

#endif // DIGIKAM_NOISE_DETECTOR_H
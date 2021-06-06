/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 25/08/2013
 * Description : Image Quality Parser
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
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

#ifndef DIGIKAM_IMAGE_QUALITY_PARSER_H
#define DIGIKAM_IMAGE_QUALITY_PARSER_H

// Local includes

#include "dimg.h"
#include "digikam_export.h"
#include "digikam_globals.h"
#include "imagequalitycontainer.h"

#include "digikam_opencv.h"


namespace Digikam
{

class DIGIKAM_EXPORT ImageQualityParser
{
public:

    /** Standard constructor with picklabel container to fill at end of analyze.
     */
    explicit ImageQualityParser(const DImg& image,
                                const ImageQualityContainer& settings,
                                PickLabel* const label);
    ~ImageQualityParser();

    /**
     * Perform quality estimation and fill Pick Label value accordingly.
     */
    void startAnalyse();
    void cancelAnalyse();

private:

    /**
     * Internal method dedicated to convert DImg pixels from integer values to float values.
     * These ones will by used internally by ImageQualityParser through OpenCV API.
     */
    void readImage()                                    const;

    /**
     * @function cannyThreshold
     * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
     */

    // blur detection

    double blurDetector()                               const;

    cv::Mat prepareForDetection(const DImg& inputImage) const;
    
    cv::Mat edgeDetection(const cv::Mat& image)         const;
    cv::Mat defocusDetection(const cv::Mat& edgesMap, const int threshold = 200,const  int sigmaBlur = 5,const int min_abs = 5,const int ordreLog = 10)    const;
    bool    isMotionBlur(const cv::Mat& frag, const int threshold = 10)  const;
    cv::Mat motionBlurDetection(const cv::Mat& edgesMap) const;
    cv::Mat getBlurMap()                                const;

    bool haveFocusMeta()                                const;
    cv::Mat getWeightsMat()                          const;
    
    // noise detection
    double noiseDetector()                              const;
    
    // compresssion detection
    int    compressionDetector()                        const;

    /**
     * Detect under and over exposure amount in image. A pure white mask is computed and
     * a count of pure color pixels is used to evaluate the over-exposition of shot.
     */
    void exposureAmount(double& under, double& over)    const;

private:

    // Disable
    ImageQualityParser(const ImageQualityParser&)            = delete;
    ImageQualityParser& operator=(const ImageQualityParser&) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_IMAGE_QUALITY_PARSER_H

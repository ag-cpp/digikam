/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 25/08/2013
 * Description : Image Quality Calculor
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013-2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
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

#include "imagequalitycalculator.h"

// Local includes 

// Qt includes

#include <QList>


namespace Digikam
{

class Q_DECL_HIDDEN ImageQualityCalculator::Private
{
public:

    explicit Private()
      : deltaWeight(10.o)
    {
    }

    float deltaWeight;

    QList<resultDetection> detectionResults;
};

bool resultLessThan(const ImageQualityCalculator::resultDetection& r1, const ImageQualityCalculator::resultDetection& r2) const
{
    return r1.score < r2.score;
}

ImageQualityCalculator::ImageQualityCalculator()
    : d(new Private)
{
    d->detectionResults = QList<resultDetection>();
}

void ImageQualityCalculator::addDetectionResult(const QString& name, const float score, const float weight) const
{
    resultDetection result;
    
    result.detetionType = name;
    result.weight       = weight;
    result.score        = score;

    d->detectionResults.append(result);
}

void ImageQualityCalculator::normalizeWeight() const
{
    float sum = 0;

    for (const auto result : d->detectionResults)
    {
        sum += result.weight;
    }

    for (auto& result : d->detectionResults)
    {
        result.weight /= sum;
    }
}

float ImageQualityCalculator::calculateQuality() const
{
    sortDetectionResults();

    adjustWeightByQualityLevel();

    normalizeWeight();

    float damage;

    for (const auto result : d->detectionResults)
    {
        damage += result.score * result.weight;
    }

    return (1 - damage)*100;
}

int ImageQualityCalculator::numberDetectors() const
{
    return d->detectionResults.count();
}

void ImageQualityCalculator::sortDetectionResults() const
{
    qSort(d->detectionResults.begin(), d->detectionResults.end(), resultLessThan);
}

void ImageQualityCalculator::adjustWeightByQualityLevel() const
{
    const int nb_detectors = numberDetectors();

    if (!nb_detectors)
    {
        return;
    }

    for (int i = 0; i < nb_detectors ; i++)
    {
        qInfo()<<"coef adjust"<<i - nb_detectors/2;

        d->detectionResults.at(i).weight += d->deltaWeight * (i - nb_detectors/2);

        qInfo()<<"weight after ajdust and score"<<d->detectionResults.at(i).weight << d->detectionResults.at(i).score;
    }
}

}
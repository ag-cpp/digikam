/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-01-03
 * Description : Abstract base classes for all OpenCV based face prediction and recognition models.
 *               Inspired from OpenCV contrib module 'face'
 *               https://github.com/Itseez/opencv_contrib/tree/master/modules/face
 *
 * Copyright (C) 2010      by Aditya Bhatt <adityabhatt at gmail dot com>
 * Copyright (C) 2010      by Alex Jironkin <alexjironkin at gmail dot com>
 * Copyright (c) 2011-2012 by Philipp Wagner <bytefish at gmx dot de>
 * Copyright (C) 2010-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "opencv_face.h"

// C++ includes

#include <iostream>
#include <set>
#include <limits>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

namespace Face
{

static std::pair<int, double> toPair(const StandardCollector::PredictResult& val)
{
    return std::make_pair(val.label, val.distance);
}

static bool pairLess(const std::pair<int, double>& lhs, const std::pair<int, double>& rhs)
{
    return (lhs.second < rhs.second);
}

// ---------------------------------------------------------------------------------------------

StandardCollector::StandardCollector(double thr)
    : threshold(thr)
{
    init(0);
}

void StandardCollector::init(size_t size)
{
    minRes = PredictResult();
    data.clear();
    data.reserve(size);
}

bool StandardCollector::collect(int label, double dist)
{
    if (dist < threshold)
    {
        PredictResult res(label, dist);

        if (res.distance < minRes.distance)
        {
            minRes = res;
        }

        data.push_back(res);
    }

    return true;
}

int StandardCollector::getMinLabel() const
{
    return minRes.label;
}

double StandardCollector::getMinDist() const
{
    return minRes.distance;
}

std::vector<std::pair<int, double> > StandardCollector::getResults(bool sorted) const
{
    std::vector<std::pair<int, double> > res(data.size());
    std::transform(data.begin(), data.end(), res.begin(), &toPair);

    if (sorted)
    {
        std::sort(res.begin(), res.end(), &pairLess);
    }

    return res;
}

std::map<int, double> StandardCollector::getResultsMap() const
{
    std::map<int, double> res;

    for (std::vector<PredictResult>::const_iterator it = data.begin() ;
         it != data.end() ; ++it)
    {
        std::map<int, double>::iterator lbl = res.find(it->label);

        if (lbl == res.end())
        {
            res.insert(toPair(*it));
        }
        else if (it->distance < lbl->second)
        {
            lbl->second = it->distance;
        }
    }

    return res;
}

Ptr<StandardCollector> StandardCollector::create(double threshold)
{
    return makePtr<StandardCollector>(threshold);
}

// ---------------------------------------------------------------------------------------------

std::vector<int> FaceRecognizer::getLabelsByString(const String& str) const
{
    std::vector<int> labels;

    for (std::map<int, String>::const_iterator it = m_labelsInfo.begin() ;
         it != m_labelsInfo.end() ; ++it)
    {
        size_t found = (it->second).find(str);

        if (found != String::npos)
        {
            labels.push_back(it->first);
        }
    }

    return labels;
}

String FaceRecognizer::getLabelInfo(int label) const
{
    std::map<int, String>::const_iterator iter(m_labelsInfo.find(label));

    return ((iter != m_labelsInfo.end()) ? iter->second : String(""));
}

void FaceRecognizer::setLabelInfo(int label, const String& strInfo)
{
    m_labelsInfo[label] = strInfo;
}

void FaceRecognizer::update(InputArrayOfArrays src, InputArray labels)
{
    Q_UNUSED(src);
    Q_UNUSED(labels);

    qCCritical(DIGIKAM_FACESENGINE_LOG) << "This FaceRecognizer does not support updating, "
                                           "you have to use FaceRecognizer::train to update it.";
}

int FaceRecognizer::predict(InputArray src) const
{
    int    lbl = 0;
    double dst = 0.0;

    predict(src, lbl, dst);

    return lbl;
}

void FaceRecognizer::predict(InputArray src,
                             int& label,
                             double& confidence) const
{
    Ptr<StandardCollector> collector = StandardCollector::create(getThreshold());

    predict(src, collector);

    label                            = collector->getMinLabel();
    confidence                       = collector->getMinDist();
}

} // namespace Face

} // namespace Digikam

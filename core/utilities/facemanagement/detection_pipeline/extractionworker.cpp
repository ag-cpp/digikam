/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-07
 * Description : Integrated, multithread face embedding extraction
 *
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2021 by Nghia Duong    <minhnghiaduong997 at gmail dot com>
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

#include "extractionworker.h"

// Local includes
#include "facialrecognition_wrapper.h"

namespace Digikam
{

class Q_DECL_HIDDEN ExtractionWorker::Private 
{
public:

    explicit Private()
    {
    }

    ~Private()
    {
    }

public:

    FacialRecognitionWrapper recognizer;
};

ExtractionWorker::ExtractionWorker()
    : d (new Private())
{
}

ExtractionWorker::~ExtractionWorker()
{
    // TODO wait for finish
    delete d;
}

void ExtractionWorker::process()
{
   
}

void ExtractionWorker::setThreshold(double threshold, bool)
{
    d->recognizer.setParameter(QLatin1String("threshold"), threshold);
}

} // namespace Digikam

/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Face recognition benchmarker
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_RECOGNITION_BENCH_MARKER_H
#define DIGIKAM_RECOGNITION_BENCH_MARKER_H

// Local includes

#include "facepipeline_p.h"

namespace Digikam
{

class Q_DECL_HIDDEN RecognitionBenchmarker : public WorkerObject
{
    Q_OBJECT

public:

    explicit RecognitionBenchmarker(FacePipeline::Private* const dd);
    QString result() const;

public Q_SLOTS:

    void process(FacePipelineExtendedPackage::Ptr package);

Q_SIGNALS:

    void processed(FacePipelineExtendedPackage::Ptr package);

protected:

    class Q_DECL_HIDDEN Statistics
    {
    public:

        Statistics();

    public:

        int knownFaces;
        int correctlyRecognized;
    };

protected:

    QMap<int, Statistics>        results;

    FacePipeline::Private* const d;
    FacialRecognitionWrapper     recognizer;
};

} // namespace Digikam

#endif // DIGIKAM_RECOGNITION_BENCH_MARKER_H

/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-08
 * Description : Node of KD-Tree for vector space partitioning
 *
 * Copyright (C) 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#ifndef KD_NODE_H
#define KD_NODE_H

#include <QVector>
#include <QMap>

#include <vector>

// digiKam include
#include "identity.h"
#include "digikam_opencv.h"

using namespace Digikam;

namespace RecognitionTest
{

class KDNode
{
public:

    explicit KDNode(const cv::Mat& nodePos, const Identity& identity, int splitAxis, int dimension);
    ~KDNode();

public:

    /**
     * @brief insert : insert a new node to sub-tree
     * @param nodePos
     * @param identity
     * @return
     */
    KDNode* insert(const cv::Mat& nodePos, const Identity& identity);

    /**
     * @brief getPosition: return vector
     * @return
     */
    cv::Mat getPosition() const;

    /**
     * @brief getClosestNeighbors : return a list of closest neighbor, limited by maxNbNeighbors and sqRange
     * @param neighborList
     * @param position
     * @param sqRange
     * @param maxNbNeighbors
     * @return
     */
    double getClosestNeighbors(QMap<double, QVector<KDNode*> >& neighborList,
                               const cv::Mat& position,
                               double sqRange,
                               int maxNbNeighbors);
    /**
     * @brief getIdentity
     * @return
     */
    Identity& getIdentity();

private:

    void updateRange(const cv::Mat&);

    KDNode* findParent(const cv::Mat& nodePos);

private:

    class Private;
    Private* d;
};

}

#endif // KD_NODE_H

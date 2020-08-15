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

#include "kd_node.h"

// std include
#include <cfloat>
#include <stdio.h>
// Qt include
#include <QtMath>
#include <QDebug>
#include <QMutex>

#include "dnnfaceextractor.h"

namespace Digikam
{

double sqrDistance(const float* const pos1, const float* const pos2, int dimension)
{
    double sqrDistance = 0;

    for (int i = 0; i < dimension; ++i)
    {
        sqrDistance += pow((pos1[i] - pos2[i]), 2);
    }

    return sqrDistance;
}

class Q_DECL_HIDDEN KDNode::Private
{
public:

    Private(const cv::Mat& nodePos, const int identity, int splitAxis, int dimension)
        : nodeID(-1),
          identity(identity),
          splitAxis(splitAxis),
          nbDimension(dimension),
          position(nodePos.clone()),
          maxRange(nodePos.clone()),
          minRange(nodePos.clone()),
          parent(nullptr),
          left(nullptr),
          right(nullptr)
    {
    }

    ~Private()
    {
        delete left;
        delete right;
    }

public:

    int nodeID;
    int identity;

    int splitAxis;
    int nbDimension;
    cv::Mat position;
    cv::Mat maxRange;
    cv::Mat minRange;
    KDNode* parent;
    KDNode* left;
    KDNode* right;

    QMutex mutex;
};

KDNode::KDNode(const cv::Mat& nodePos,
               const int      identity,
               int            splitAxis,
               int            dimension)
    : d(new Private(nodePos, identity, splitAxis, dimension))
{
    Q_ASSERT(splitAxis < dimension);
    Q_ASSERT(nodePos.rows == 1 && nodePos.cols == dimension && nodePos.type() == CV_32F);
}

KDNode::~KDNode()
{
    delete d;
}

KDNode* KDNode::insert(const cv::Mat& nodePos, const int identity)
{
    if (!(nodePos.rows   == 1              &&
          nodePos.cols   == d->nbDimension &&
          nodePos.type() == CV_32F))
    {
        return nullptr;
    }

    KDNode* parent = findParent(nodePos);

    KDNode* newNode = new KDNode(nodePos, identity,
                                 ((parent->d->splitAxis + 1) % d->nbDimension),
                                 d->nbDimension);
    newNode->d->parent = parent;

    //std::cout << "parent embedding" << parent->getPosition() << std::endl;
    //std::cout << "node embedding" << nodePos << std::endl;

    if (nodePos.at<float>(0, parent->d->splitAxis) >= parent->getPosition().at<float>(0, parent->d->splitAxis))
    {
        parent->d->right = newNode;
    }
    else
    {
        parent->d->left = newNode;
    }

    return newNode;
}

cv::Mat KDNode::getPosition() const
{
    return d->position;
}

int KDNode::getIdentity()
{
    return d->identity;
}

void KDNode::setNodeId(int id)
{
    d->nodeID = id;
}

double KDNode::getClosestNeighbors(QMap<double, QVector<int> >& neighborList,
                                   const cv::Mat&               position,
                                   double                       sqRange,
                                   int                          maxNbNeighbors) const
{
    //d->mutex.lock();
    // add current node to the list
    double sqrdistanceToCurrentNode;
    sqrdistanceToCurrentNode = sqrDistance(position.ptr<float>(), d->position.ptr<float>(), d->nbDimension);

    //d->mutex.unlock();

    if (sqrdistanceToCurrentNode <= sqRange)
    {
        neighborList[sqrdistanceToCurrentNode].append(d->identity);
        // limit the size of the Map to maxNbNeighbors
        int size = 0;

        for (QMap<double, QVector<int> >::const_iterator iter  = neighborList.cbegin();
                                                         iter != neighborList.cend();
                                                       ++iter)
        {
            size += iter.value().size();
        }

        if (size > maxNbNeighbors)
        {
            // Eliminate the farthest neighbor
            QMap<double, QVector<int> >::iterator farthestNodes = (neighborList.end() - 1);

            if (farthestNodes.value().size() == 1)
            {
                neighborList.erase(farthestNodes);
            }
            else
            {
                farthestNodes.value().pop_back();
            }

            // update the searching range
            sqRange = neighborList.lastKey();
        }
    }

    //d->mutex.lock();

    // sub-trees Traversal
    double sqrDistanceleftTree  = 0;

    if (d->left == nullptr)
    {
        sqrDistanceleftTree = DBL_MAX;
    }
    else
    {
        const float* const minRange = d->left->d->minRange.ptr<float>();
        const float* const maxRange = d->left->d->maxRange.ptr<float>();
        const float* const pos      = position.ptr<float>();

        for (int i = 0; i < d->nbDimension; ++i)
        {
            sqrDistanceleftTree += (pow(qMax((minRange[i] - pos[i]), 0.0f), 2) +
                                    pow(qMax((pos[i] - maxRange[i]), 0.0f), 2));
        }
    }

    double sqrDistancerightTree = 0;

    if (d->right == nullptr)
    {
        sqrDistancerightTree = DBL_MAX;
    }
    else
    {
        const float* const minRange = d->right->d->minRange.ptr<float>();
        const float* const maxRange = d->right->d->maxRange.ptr<float>();
        const float* const pos      = position.ptr<float>();

        for (int i = 0; i < d->nbDimension; ++i)
        {
            sqrDistancerightTree += (pow(qMax((minRange[i] - pos[i]), 0.0f), 2) +
                                     pow(qMax((pos[i] - maxRange[i]), 0.0f), 2));
        }
    }
    //d->mutex.unlock();

    // traverse the closest area
    if (sqrDistanceleftTree < sqrDistancerightTree)
    {
        if (sqrDistanceleftTree < sqRange)
        {
            // traverse left Tree
            sqRange = d->left->getClosestNeighbors(neighborList, position, sqRange, maxNbNeighbors);

            if (sqrDistancerightTree < sqRange)
            {
                // traverse right Tree
                sqRange = d->right->getClosestNeighbors(neighborList, position, sqRange, maxNbNeighbors);
            }
        }
    }
    else
    {
        if (sqrDistancerightTree < sqRange)
        {
            // traverse right Tree
            sqRange = d->right->getClosestNeighbors(neighborList, position, sqRange, maxNbNeighbors);

            if (sqrDistanceleftTree < sqRange)
            {
                // traverse left Tree
                sqRange = d->left->getClosestNeighbors(neighborList, position, sqRange, maxNbNeighbors);
            }
        }
    }
/*
    qDebug() << "distance to node" << sqrdistanceToCurrentNode;
    qDebug() << "distance to left tree" << sqrDistanceleftTree;
    qDebug() << "distance to right tree" << sqrDistancerightTree;
*/

    return sqRange;
}

void KDNode::updateRange(const cv::Mat& pos)
{
    if (!(pos.rows   == 1              &&
          pos.cols   == d->nbDimension &&
          pos.type() == CV_32F))
    {
        return;
    }

    float* minRange = d->minRange.ptr<float>();
    float* maxRange = d->maxRange.ptr<float>();
    const float* position = pos.ptr<float>();

    for (int i = 0; i < d->nbDimension; ++i)
    {
        maxRange[i] = std::max(maxRange[i], position[i]);
        minRange[i] = std::min(minRange[i], position[i]);
    }
}

KDNode* KDNode::findParent(const cv::Mat& nodePos)
{
    KDNode* parent = nullptr;
    KDNode* currentNode = this;

    while (currentNode != nullptr)
    {
        currentNode->updateRange(nodePos);

        int split = currentNode->d->splitAxis;
        parent    = currentNode;

        if (nodePos.at<float>(0, split) >= currentNode->d->position.at<float>(0, split))
        {
            currentNode = currentNode->d->right;
        }
        else
        {
            currentNode = currentNode->d->left;
        }
    }

    return parent;
}

}

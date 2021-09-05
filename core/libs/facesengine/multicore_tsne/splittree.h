/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2021-07-04
 * Description : TSNE Quadtree
 *
 * Copyright (C) 2021 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 * Copyright (C) 2012 by Laurens van der Maaten <lvdmaaten at gmail dot com> from Delft University of Technology for the original version
 * Copyright (C) 2016 by Dmitry Ulyanov <dmitry dot ulyanov dot msu at gmail dot com> for the Multicore version
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

#ifndef DIGIKAM_FACESENGINE_SPLITTREE_H
#define DIGIKAM_FACESENGINE_SPLITTREE_H

// C++ includes

#include <cstdlib>
#include <vector>

namespace TSNE
{

static inline float min(float x, float y) { return (x <= y ? x : y); }
static inline float max(float x, float y) { return (x <= y ? y : x); }
static inline float abs_d(float x) { return (x <= 0 ? -x : x); }

class Cell 
{

public:

    ~Cell();
	bool containsPoint(float point[]);
    
public:

    float*  center;
	float*  width;
	int     n_dims;
};

class SplitTree
{
public:
	
	SplitTree(float* inp_data, int N, int no_dims);
	SplitTree(SplitTree* inp_parent, float* inp_data, float* mean_Y, float* width_Y);
	~SplitTree();
	
public:

    void construct(Cell boundary);
	bool insert(int new_index);
	void subdivide();
	void computeNonEdgeForces(int point_index, float theta, float* neg_f, float* sum_Q);

private:
	
	void init(SplitTree* inp_parent, float* inp_data, float* mean_Y, float* width_Y);
	void fill(int N);

private:

    // Fixed constants
	static const int QT_NODE_CAPACITY = 1;

	// Properties of this node in the tree
	int     QT_NO_DIMS;
	bool    is_leaf;
	int     size;
	int     cum_size;

	// Axis-aligned bounding box stored as a center with half-dimensions to represent the boundaries of this quad tree
	Cell    boundary;

	// Indices in this quad tree node, corresponding center-of-mass, and list of all children
	float*  data;
	float*  center_of_mass;
	int     index[QT_NODE_CAPACITY];

	int     num_children;
	std::vector<SplitTree*> children;
};

} // namespace TSNE

#endif // DIGIKAM_FACESENGINE_SPLITTREE_H

/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2021-07-04
 * Description : Implementation of both standard and Barnes-Hut-SNE.
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

#ifndef DIGIKAM_FACESENGINE_TSNE_H
#define DIGIKAM_FACESENGINE_TSNE_H

// C++ includes

#include <cstddef>

namespace TSNE
{

/**
 * tsne_run_float performs t-SNE
 * X -- float matrix of size [N, D]
 * D -- input dimensionality
 * Y -- array to fill with the result of size [N, no_dims]
 * no_dims -- target dimentionality
 */
void tsne_run_float(float* X, int N, int D, float* Y,
                    int no_dims = 2, float perplexity = 30, float theta = .5,
                    int num_threads = 1, int max_iter = 1000, int n_iter_early_exag = 250,
                    int random_state = -1, bool init_from_Y = false, int verbose = 0,
                    float early_exaggeration = 12, float learning_rate = 200,
                    float *final_error = nullptr, int distance = 1);

static inline float sign(float x) { return (x == .0 ? .0 : (x < .0 ? -1.0 : 1.0)); }

class DataPoint;

template <class treeT, float (*dist_fn)( const DataPoint&, const DataPoint&)>
class TSNE
{

public:

    void run(float* X, int N, int D, float* Y,
             int no_dims = 2, float perplexity = 30, float theta = .5,
             int num_threads = 1, int max_iter = 1000, int n_iter_early_exag = 250,
             int random_state = 0, bool init_from_Y = false, int verbose = 0,
             float early_exaggeration = 12, float learning_rate = 200,
             float *final_error = nullptr);

    void symmetrizeMatrix(int** row_P, int** col_P, float** val_P, int N);

private:

    float computeGradient(int* inp_row_P, int* inp_col_P, float* inp_val_P, float* Y, int N, int D, float* dC, float theta, bool eval_error);
    float evaluateError(int* row_P, int* col_P, float* val_P, float* Y, int N, int no_dims, float theta);
    void  zeroMean(float* X, int N, int D);
    void  computeGaussianPerplexity(float* X, int N, int D, int** _row_P, int** _col_P, float** _val_P, float perplexity, int K, int verbose);
    float randn();
};

} // namespace TSNE

#endif // DIGIKAM_FACESENGINE_TSNE_H

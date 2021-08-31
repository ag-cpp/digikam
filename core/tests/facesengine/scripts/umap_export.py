#!/usr/bin/python3

# ============================================================
#
# This file is a part of digiKam project
# https://www.digikam.org
#
# Date        : 2019-08-11
# Description : faces engine test scripts
#
# Copyright (C) 2021 by Nghia Duong <minhnghiaduong997 at gmail dot com>
#
# This program is free software; you can redistribute it
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software Foundation;
# either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# ============================================================ */

import numpy as np

import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import umap.umap_ as umap

sns.set(style='white', context='notebook', rc={'figure.figsize':(14,10)})

faceData = pd.read_csv('facenet_tsne.txt',header=None)

embeddingIndex = set(faceData.columns)
embeddingIndex.remove(0)
faceEmbeddings = faceData[embeddingIndex].values
labels = faceData[0].values

reducer = umap.UMAP(n_neighbors=15, metric='euclidean', min_dist=0.1, n_components=2, transform_seed=42)
reducer.fit(faceEmbeddings)exit()
reducedEmbedding = reducer.transform(faceEmbeddings)

reducedData = pd.DataFrame({'label':labels, 'X1': reducedEmbedding[:, 0], 'X2': reducedEmbedding[:, 1]})
reducedData.to_csv('../data/fix_normalize_umap.txt', index=False, header=False)

plt.scatter(reducedEmbedding[:, 0], reducedEmbedding[:, 1], c=labels, cmap='Spectral', s=5)
plt.gca().set_aspect('equal', 'datalim')
plt.colorbar(boundaries=np.arange(11)-0.5).set_ticks(np.arange(10))
plt.title('UMAP projection of the face embedding from Extended Yale B dataset', fontsize=24);
plt.show()
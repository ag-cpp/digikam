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

from sklearn.manifold import TSNE
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

sns.set(style='white', context='notebook', rc={'figure.figsize':(14,10)})

faceData = pd.read_csv('../data/facenet_blob_data.txt',header=None)

embeddingIndex = set(faceData.columns)
embeddingIndex.remove(0)
faceEmbeddings = faceData[embeddingIndex].values
labels = faceData[0].values

reducedEmbedding = TSNE(n_components=2).fit_transform(faceEmbeddings)

plt.scatter(reducedEmbedding[:, 0], reducedEmbedding[:, 1], c=labels, cmap='Spectral', s=5)
plt.gca().set_aspect('equal', 'datalim')
plt.colorbar(boundaries=np.arange(11)-0.5).set_ticks(np.arange(10))
plt.title('T-SNE projection of the face embedding from Extended Yale B dataset', fontsize=24);
plt.show()

reducedData = pd.DataFrame({'label':labels, 'X1': reducedEmbedding[:, 0], 'X2': reducedEmbedding[:, 1]})
reducedData.to_csv('../data/facenet_blob_tsne_data.txt', index=False, header=False)
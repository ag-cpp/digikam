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
from sklearn.model_selection import train_test_split, cross_val_score
from sklearn import svm

import pandas as pd
import umap.umap_ as umap

faceData = pd.read_csv('../data/face_data.txt',header=None)

embeddingIndex = set(faceData.columns)
embeddingIndex.remove(0)
faceEmbeddings = faceData[embeddingIndex].values
labels = faceData[0].values

for i in range(10,60,5):
    reducer = umap.UMAP(n_neighbors=15, metric='euclidean', min_dist=0.1, n_components=i*2, transform_seed=42)
    reducer.fit(faceEmbeddings)

    reduced_Embedding = reducer.transform(faceEmbeddings)
    reduced_X_train, reduced_X_test, y_train, y_test = train_test_split(reduced_Embedding,
                                                        labels,
                                                        stratify=labels,
                                                        random_state=42)

    print(i*2)
    CC = [0.001,0.01,0.1,1,10,100,1000, 10000]
    scores = np.array([])
    for C in CC:
        svc = svm.SVC(C=C, kernel='rbf', gamma='auto').fit(reduced_X_train, y_train)
        scores = np.append(scores, cross_val_score(svc, reduced_X_train, y_train, cv=5).mean())

    print(CC)
    print(scores)

    C = CC[np.argmax((scores))]
    svc = svm.SVC(C=C, kernel='rbf', gamma='auto').fit(reduced_X_train, y_train)
    print(svc.score(reduced_X_test, y_test))

'''
print("NuSVC")
Nu = [0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 0.7, 0.8]
scores = np.array([])
for n in Nu:
    nusvc = svm.NuSVC(nu=n, kernel='rbf',).fit(reduced_X_train, y_train)
    scores = np.append(scores, cross_val_score(nusvc, reduced_X_train, y_train, cv=5).mean())

print(Nu)
print(scores)
nu = Nu[np.argmax((scores))]

nusvc = svm.NuSVC(nu=nu, kernel='rbf',).fit(reduced_X_train, y_train)
print(nusvc.score(reduced_X_test, y_test))
'''
'''
# split face embedding to train and test sets
X_train, X_test, y_train, y_test = train_test_split(faceEmbeddings,
                                                    labels,
                                                    stratify=labels,
                                                    random_state=42)

reducer = umap.UMAP(n_neighbors=15, metric='euclidean', min_dist=0.1, n_components=28, transform_seed=42)
reducer.fit(X_train, y=y_train)

reduced_X_train = reducer.transform(X_train)
reduced_X_test  = reducer.transform(X_test)

svc = SVC().fit(reduced_X_train, y_train)
knn = KNeighborsClassifier().fit(reduced_X_train, y_train)

print(svc.score(reduced_X_test, y_test), knn.score(reduced_X_test, y_test))

reducedEmbedding = reducer.transform(faceEmbeddings)

plt.scatter(reducedEmbedding[:, 0], reducedEmbedding[:, 1], c=labels, cmap='Spectral', s=5)
plt.gca().set_aspect('equal', 'datalim')
plt.colorbar(boundaries=np.arange(11)-0.5).set_ticks(np.arange(10))
plt.title('UMAP projection of the face embedding', fontsize=24);
plt.show()
'''

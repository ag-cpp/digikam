#
# Copyright (c) 2020, Nghia Duong, <minhnghiaduong997 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

import numpy as np
from sklearn.model_selection import train_test_split, cross_val_score
from sklearn import svm

import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import umap.umap_ as umap

sns.set(style='white', context='notebook', rc={'figure.figsize':(14,10)})

faceData = pd.read_csv('../data/face_data.txt',header=None)

embeddingIndex = set(faceData.columns)
embeddingIndex.remove(0)
faceEmbeddings = faceData[embeddingIndex].values
labels = faceData[0].values

X_train, X_test, y_train, y_test = train_test_split(faceEmbeddings,
                                                    labels,
                                                    stratify=labels,
                                                    random_state=42)

reducer = umap.UMAP(n_neighbors=15, metric='euclidean', min_dist=0.1, n_components=2, transform_seed=42)
reducer.fit(X_train, y=y_train)

reducedEmbedding = reducer.transform(X_test)

plt.scatter(reducedEmbedding[:, 0], reducedEmbedding[:, 1], c=y_test, cmap='Spectral', s=5)
plt.gca().set_aspect('equal', 'datalim')
plt.colorbar(boundaries=np.arange(11)-0.5).set_ticks(np.arange(10))
plt.title('UMAP projection of the face embedding from Extended Yale B dataset', fontsize=24);
plt.show()
'''
X_train, X_test, y_train, y_test = train_test_split(faceEmbeddings,
                                                        labels,
                                                        stratify=labels,
                                                        random_state=42)

reducer = umap.UMAP(n_neighbors=15, metric='euclidean', min_dist=0.1, n_components=2, transform_seed=42)
reducer.fit(X_train, y=y_train)

reduced_X_train = reducer.transform(X_train)
reduced_X_test  = reducer.transform(X_test)

svc = svm.SVC(C=10.0, kernel='rbf',).fit(reduced_X_train, y_train)
nusvc = svm.NuSVC(nu=0.1, kernel='rbf',).fit(reduced_X_train, y_train)

print(svc.score(reduced_X_test, y_test), nusvc.score(reduced_X_test, y_test))
#for i in range(1, 50):
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

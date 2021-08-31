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

from keras_facenet import FaceNet
import pandas as pd
import os

def extractFaces(dataDir):
    labels = []
    faceEmbeddings = []
    embedder = FaceNet()
    print(embedder.metadata['fixed_image_standardization'])
    subDirs = [x[0] for x in os.walk(dataDir)]
    
    for i in range(1, len(subDirs)):
        print("Scan dir " + subDirs[i])
        for images in [x[2] for x in os.walk(subDirs[i])]:
            for image in images:
                data = embedder.extract(subDirs[i] + '/' + image)
                if len(data) > 0:
                    labels.append(i-1)
                    faceEmbeddings.append(data[0]['embedding'])
    
    df = pd.DataFrame(faceEmbeddings)
    df.insert(0, 'label', labels)
    return df

def saveEmbedding(dataDir, targetFile):
    df = extractFaces(dataDir)
    df.to_csv(targetFile, index=False, header=False)

saveEmbedding('/home/minhnghiaduong/Documents/Projects/ExtendedYaleB', '../data/keras.txt')


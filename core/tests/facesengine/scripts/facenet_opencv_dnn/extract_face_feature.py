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
import cv2
import time
from keras_facenet import FaceNet
import pandas as pd
import os

import pdb


face_image_path = './images/yaleB11_P01A+000E-35.pgm'
model_path = './models/graph_final.pb'


# ------------------------------------------------------
def load_model(pb_model_path):
    net = cv2.dnn.readNetFromTensorflow(pb_model_path)
    return net


def prewhiten(x):
    mean = np.mean(x)
    std = np.std(x)
    std_adj = np.maximum(std, 1.0 / np.sqrt(x.size))
    y = np.multiply(np.subtract(x, mean), 1 / std_adj)
    return y

def fix_normalize(x):
    return (np.float32(x) - 127.5) / 127.5

def np_blob(image):
    resized = cv2.resize(image, (160, 160), interpolation=cv2.INTER_LINEAR)
    prewhitened = fix_normalize(resized)
    # HWC -> CHW
    input_face_img = prewhitened.transpose([2, 0, 1])
    # CHW -> NCHW
    input_face_img = np.expand_dims(input_face_img, axis=0)

    return input_face_img
# ------------------------------------------------------
#""" Preprocess """
#image = cv2.imread(face_image_path)
#image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
#dest = cv2.subtract(np.float32(resized), (127.5, 127.5, 127.5, 0)) 
#cv2.multiply(dest, (1/127.5, 1/127.5, 1/127.5, 0), dest)  
#blob = cv2.dnn.blobFromImage(resized, 1/127.5, (160, 160), (127.5, 127.5, 127.5), swapRB=False)
#blob2 = cv2.dnn.blobFromImage(image, 1/127.5, (160, 160), (127.5, 127.5, 127.5), swapRB=False)

# 0.00499203 error, same as keras facenet
def extract(cvNet, image): 
    #input_face_img = np_blob(image)
    input_face_img = cv2.dnn.blobFromImage(image, 1/127.5, (160, 160), (127.5, 127.5, 127.5), swapRB=False)

    """ Forward """
    cvNet.setInput(input_face_img)
    cvOut = cvNet.forward()

    return cvOut


def extractFaces(dataDir):
    """ Load .pb model """
    cvNet = load_model(model_path)

    labels = []
    faceEmbeddings = []
    embedder = FaceNet()
    subDirs = [x[0] for x in os.walk(dataDir)]
    
    for i in range(1, len(subDirs)):
        print("Scan dir " + subDirs[i])
        for images in [x[2] for x in os.walk(subDirs[i])]:
            for image in images:
                detections, crops = embedder.crop(subDirs[i] + '/' + image)
                if not detections:
                    continue
                data = extract(cvNet, crops[0])
                if len(data) > 0:
                    labels.append(i-1)
                    faceEmbeddings.append(data[0])
    
    df = pd.DataFrame(faceEmbeddings)
    df.insert(0, 'label', labels)
    return df

def saveEmbedding(dataDir, targetFile):
    df = extractFaces(dataDir)
    df.to_csv(targetFile, index=False, header=False)

saveEmbedding('/home/minhnghiaduong/Documents/Projects/ExtendedYaleB', 'facenet_blob_data.txt')

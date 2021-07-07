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


# ------------------------------------------------------
#""" Preprocess """
#im = cv2.imread(face_image_path)
#im = cv2.cvtColor(im, cv2.COLOR_BGR2RGB)

def extract(cvNet, image): 
    resized = cv2.resize(image, (160, 160), interpolation=cv2.INTER_LINEAR)
    prewhitened = prewhiten(resized)
    # HWC -> CHW
    input_face_img = prewhitened.transpose([2, 0, 1])
    # CHW -> NCHW
    input_face_img = np.expand_dims(input_face_img, axis=0)

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

saveEmbedding('/home/minhnghiaduong/Documents/Projects/ExtendedYaleB', 'facenet_data.txt')

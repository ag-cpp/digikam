from keras_facenet import FaceNet
import pandas as pd
import os

def extractFaces(dataDir):
    labels = []
    faceEmbeddings = []
    embedder = FaceNet()
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


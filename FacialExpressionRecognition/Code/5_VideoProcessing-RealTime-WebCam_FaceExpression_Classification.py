# -*- coding: utf-8 -*-
"""
Created on Wed Jul 24 03:09:46 2017

@author: Naren Suri
"""

import cv2
import time
import numpy as np
from keras.models import model_from_json

cascadePathToUseHaar = "D:\sem4\ComputerVision\Softwares\opencv\sources\data\haarcascades\haarcascade_frontalface_default.xml"
faceCascade = cv2.CascadeClassifier(cascadePathToUseHaar)

ModelSavedAsJson = open("D:/sem4/ComputerVision/CNN-Proj/Code-FaceRecognition/fer2013/Model/model.json",'r')
CNNmodelForFaceExpresions = model_from_json(ModelSavedAsJson.read())
# loading the weights into the model to reconstruct the complete structire of the convolutional neural network.
CNNmodelForFaceExpresions.load_weights("D:/sem4/ComputerVision/CNN-Proj/Code-FaceRecognition/fer2013/Model/model.h5")
ModelSavedAsJson.close()

emotionsList= {'Angry': 0, 'Disgust': 1, 'Fear': 2, 'Happy': 3, 'Sad': 4, 'Surprise': 5, 'Neutral': 6}
# The list of emotions we have 
emotions = ['Angry', 'Fear', 'Happy', 'Sad', 'Surprise', 'Neutral']


def predictemotions(frame):
    resizeimge = cv2.resize(frame, (48,48), interpolation = cv2.INTER_AREA)
    imageFinal = resizeimge.reshape(1, 1, 48, 48)
    Probailites = CNNmodelForFaceExpresions.predict(imageFinal, batch_size=1)
    indexOfEmotion = np.argmax(np.array(Probailites),axis=0)
    return indexOfEmotion


videocaptureFromWebCam = cv2.VideoCapture(0)

#time.sleep(1)

while True:
    # Capture frame-by-frame
    camerares, Eachframe = videocaptureFromWebCam.read()
    
    grayImage = cv2.cvtColor(Eachframe, cv2.COLOR_BGR2GRAY)
    
    facesDetected = faceCascade.detectMultiScale(grayImage,scaleFactor=2.1,minNeighbors=3,minSize=(30, 30),flags=cv2.cv.CV_HAAR_SCALE_IMAGE)
    
    # Draw a rectangle around the faces
    for (x, y, w, h) in facesDetected:
        cv2.rectangle(Eachframe, (x, y), (x+w, y+h), (0, 255, 0), 2)
    
    # Display the resulting frame
    cv2.imshow('Video', Eachframe)
    
    captuedImage = grayImage[y:y+h, x:x+w]
    expressionPredicted = predictemotions(captuedImage)
    cv2.putText(captuedImage, "Expression :: {expression}".format(expression = emotions[expressionPredicted] ), (10, 20),cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)
    cv2.imshow('Frame Sub-selected for expression feeding', captuedImage)
    

    #time.sleep(1)
    
    
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything is done, release the capture
videocaptureFromWebCam.release()
cv2.destroyAllWindows()
# -*- coding: utf-8 -*-
"""
@author: Naren Suri
"""

from keras.preprocessing.image import ImageDataGenerator
from keras.models import Sequential
from keras.layers import Convolution2D, MaxPooling2D
from keras.layers import Activation, Dropout, Flatten, Dense
from keras.callbacks import EarlyStopping
from sklearn.cross_validation import train_test_split
import numpy as np


# all parameters we need

conv=[(32,3),(64,3),(128,3)]
dense=[64,2]
batchsize=256
epochs=5
dropout=0.5
validation_split=0.2
patience=5


def CNNUsingKeras(XTrainingData, yTrainingData):
    XTrainingData = XTrainingData.astype('float32')

## Now lete use the Keras; it has alredy builtIn neural net implementations; we can directly use them
    CNNFacialModel = Sequential()
    CNNFacialModel.add(Convolution2D(conv[0][0], 3, 3, border_mode='same', activation='relu',input_shape=(1, XTrainingData.shape[2], XTrainingData.shape[3])))
    
    # lets add al the features and conditions to the model that we want to use
    # checking for differernt conditions
    # this process is same for few more conditional filters
    # the below few steps are very same for every one for CNN; and I have referenced the mememoji code.
    
    if (conv[0][1]-1) != 0:
        for i in range(conv[0][1]-1):
            CNNFacialModel.add(Convolution2D(conv[0][0], 3, 3, border_mode='same', activation='relu'))
        CNNFacialModel.add(MaxPooling2D(pool_size=(2, 2)))
    
    if conv[1][1] != 0:
        for i in range(conv[1][1]):
            CNNFacialModel.add(Convolution2D(conv[1][0], 3, 3, border_mode='same', activation='relu'))
        CNNFacialModel.add(MaxPooling2D(pool_size=(2, 2)))

    if conv[2][1] != 0:
        for i in range(conv[2][1]):
            CNNFacialModel.add(Convolution2D(conv[2][0], 3, 3, border_mode='same', activation='relu'))
        CNNFacialModel.add(MaxPooling2D(pool_size=(2, 2)))

    predictionResult = CNNFacialModel.add(Dense(yTrainingData.shape[1], activation='softmax'))
    
    # optimizer:
    model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])

    # set callback:
    callbacks = []
    if patience != 0:
        early_stopping = EarlyStopping(monitor='val_loss', patience=patience, verbose=1)
        callbacks.append(early_stopping)

    modelResult = model.fit(XTrainingData, yTrainingData, nb_epoch = epochs, batch_size=batchsize,validation_split=validation_split, callbacks=callbacks, shuffle=True, verbose=1)

    print modelResult



if __name__ == '__main__':
    # Process data
    
    XTrainingData = "D:/sem4/ComputerVision/CNN-Proj/Code-FaceRecognition/fer2013/DataInNumpyForTrainingAndTesting/XData_Training.npy"
    yTrainingData = "D:/sem4/ComputerVision/CNN-Proj/Code-FaceRecognition/fer2013/DataInNumpyForTrainingAndTesting/yData_Training.npy"
    X = np.load(XTrainingData)
    y = np.load(yTrainingData)
    print 'Data loaded from wjaht we created earlier'

    CNNUsingKeras(XTrainingData, yTrainingData)
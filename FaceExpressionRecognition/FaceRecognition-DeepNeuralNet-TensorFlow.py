# -*- coding: utf-8 -*-
"""
Created on June 25 20:49:29 2017

@author: Naren Suri
"""

import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt
from sklearn.utils import shuffle


###################################################

class EachHiddenLayerUnitsMatrix(object):
    def CreateHiddenLayerUnitsMatrix(self,FromLayerUnitsDimension,toLayersUnits,LayerId):
        self.LayerId = LayerId
        self.FromLayerUnitsDimension = FromLayerUnitsDimension
        self.toLayersUnits = toLayersUnits
        
        # so the above obtained values are being used to create the tandom weigths of the demsnison obained. 
        self.WeightsInCurrentLayer = np.random.randn(FromLayerUnitsDimension, toLayersUnits) / np.sqrt(FromLayerUnitsDimension + toLayersUnits)
        self.biasForCurrentLayer = np.zeros(toLayersUnits)
        
        # the randomly assigned weights are shown aabove:
        # importantly, after data being extracted the tensorflow just creates a graph of all its variables etc. 
        # We have to specifially run it to actually run the logic
        # lets just create the tensorFlow Variables
        self.WeightsInCurrentLayer = tf.Variable(self.WeightsInCurrentLayer.astype(np.float32))
        self.biasForCurrentLayer = tf.Variable(self.biasForCurrentLayer.astype(np.float32))
        self.CollectionOfparametersOfModel = [self.WeightsInCurrentLayer, self.biasForCurrentLayer]
        
        
        
    def forwardPropWithPreviousLayerResults(self,TemporaryFeedForwardResult):
        self.TemporaryFeedForwardResult = TemporaryFeedForwardResult
        # as we did before we will use the tensorflow to do the matrix multiplication to calculate the feed forward for th ecurrent layer        
        self.LayerResult = tf.matmul(self.TemporaryFeedForwardResult, self.WeightsInCurrentLayer) + self.biasForCurrentLayer
        # now after caclulting the feed forward value im usig the RELU ; rectified linear unit
        return tf.nn.relu(self.LayerResult)
        
        
    
class DeepNN_TFlow(object):
    
    def __init__(self,hiddenUnitsInEachLayer):
        
        self.decay=0.999
        self.epochs=400
        self.regularization=10e-3
        self.hiddenUnitsInEachLayer = hiddenUnitsInEachLayer
        self.learningRate=10e-7
        self.momentum=0.99
        self.numberOfBatches=100      

        
     
    def FitTheDNN_TF(self,X_Features,Y_predict,X_Features_Validation,Y_predict_Validation,TotalNumberOfClasses):
        # The number of classes we are trying to predict are
        self.TotalNumberOfClasses = TotalNumberOfClasses
        self.NumberOfImages, self.NuberOfFeatures = X_Features.shape()
        self.ListOfAllHiddenLayers=[]
        
        ## now lets create the matrices of different layers; So the each 2 matrices are later dot producted to get the updated weigths at each unit during the forward propogation
        self.FromLayerUnitsDimension = self.NuberOfFeatures
        self.countOfLayer = 0
        
        # now lets create matrices of each level and store them in a list so that I can use them during the forward and the backward propagatin
        for ALayersUnits in  self.hiddenUnitsInEachLayer:
            ## create objects of each layer and store them in lists as described above
            self.toLayersUnits = ALayersUnits
            self.HiddenLayerCreation = EachHiddenLayerUnitsMatrix()
            self.AhiddenLayerMatrix =  self.HiddenLayerCreation.CreateHiddenLayerUnitsMatrix(self.FromLayerUnitsDimension,self.toLayersUnits,self.countOfLayer)
            self.ListOfAllHiddenLayers.append(self.AhiddenLayerMatrix)
            # update the next current with present tolayer
            self.FromLayerUnitsDimension = self.toLayersUnits
            # increase the layer count for the next layer
            self.countOfLayer = self.countOfLayer+1
            ## With this we have setup the complete layers and assigned them with some default values
            ## all default layers are set and just the final layer to the results classsification is to be done
        
        ## lets create the weights from the final laye to the classificiation or softmax layer
        
        # so the above obtained values are being used to create the tandom weigths of the demsnison obained. 
        self.WeightsInCurrentLayer = np.random.randn(self.FromLayerUnitsDimension, self.TotalNumberOfClasses) / np.sqrt(self.FromLayerUnitsDimension + self.TotalNumberOfClasses)
        self.biasForCurrentLayer = np.zeros(self.TotalNumberOfClasses)
        
        # the randomly assigned weights are shown aabove:
        # importantly, after data being extracted the tensorflow just creates a graph of all its variables etc. 
        # We have to specifially run it to actually run the logic
        # lets just create the tensorFlow Variables
        self.WeightsInCurrentLayer = tf.Variable(self.WeightsInCurrentLayer.astype(np.float32))
        self.biasForCurrentLayer = tf.Variable(self.biasForCurrentLayer.astype(np.float32))
        self.CollectionOfparametersOfModel = [self.WeightsInCurrentLayer, self.biasForCurrentLayer]

        # now for every layer we have created the diffeernt parameters by default and all these parameters keep updated during the forward and backward propagation
        
        # so lets put all the parmeters from all layers and the last softmax layr which wee created seperately into a list; sothat we can use it when ever we need it
        
        for getCurrentLayerParams in self.ListOfAllHiddenLayers:
            self.CollectionOfparametersOfModel = getCurrentLayerParams.CollectionOfparametersOfModel + self.CollectionOfparametersOfModel
            ## this way we are crating them in the right order they are supposed to be in.
            
        ###############################################
        ## With this everything that we need has been setup and running the model is to be done 
            
        ## Now lets create the required tensorFlow tensors to implement our logic
        # in the above code until now we have just prepared the data; the model running is the heavy cmputing task
        # tensorflow runs the model once it creates a plan graph
        
        ## Creating tensors
        self.InputTensor_XTrainData = tf.placeholder(tf.float32, shape=(None, self.NuberOfFeatures), name='XTrainData')
        self.ToBePredictedTensor_Target = tf.placeholder(tf.float32, shape=(None, TotalNumberOfClasses), name='TargetResult')
        
        ## Now lets create the feed forward algorithm
        ## then the bak propagation
        #use the tensorflow built in models to otimize the gradient descent of the backpropagation
        # use the tensorflow to minizethe Error
        
        # lets first do a feed forward propagation
        self.ForwardPropResult = self.forwardPropagation(self.InputTensor_XTrainData,self.ListOfAllHiddenLayers)
        ## so in the above result we got the feedforward odel and all the weights got updated based on the feed forwardd we applied
        # now we have to back propagate and make sure to update weights and biases
        # that is a gradietndecent on the parameters at each layer is done with the values we have in the current iteration
        
        #####################################################
        # lets crete the optimization functions we want as discussed above
        self.l2Losses=[]
        for parametersOfEachLayer in self.CollectionOfparametersOfModel:
            self.l2Losses.append( tf.nn.l2_loss(parametersOfEachLayer))
        self.l2LossCost = self.regularization*sum(self.l2Losses)
        
        ## we wrote the tensor unit for l2Losses
        # lets write the global cost tensor unit; which will be further used to optiize
        
        self.Globalcost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits( logits= self.ForwardPropResult, labels= self.ToBePredictedTensor_Target)) + self.l2LossCost
        
        
        # lets calcualtethe prediction of the results
        self.predictionResult = self.predictThEmotionResults(self.InputTensor_XTrainData)
        self.trainFaceExpressionModel = tf.train.RMSPropOptimizer(self.learningRate, decay= self.decay, momentum=self.momentum).minimize(self.Globalcost)
               
        ### Now lets run things on tensorflow
        costsToPlot = []
        # we shall do the multiple batches of the original data; and run the model on those batches;
        # so the averge perforamnce of the model can be obtained from that
        initAllTensorVariables = tf.global_variables_initializer()
        
        with tf.Session() as session:
            session.run(initAllTensorVariables)
            EachBatchSize = self.NumberOfImages / self.numberOfBatches
            
            for IterI in xrange(self.epochs):
                X_Features,Y_predict = shuffle(X_Features,Y_predict)
                
                ## creating the batches for processing the data                
                for BatchNumber in xrange(EachBatchSize):
                    X_Features_batch = X_Features[BatchNumber*EachBatchSize:(BatchNumber*EachBatchSize+EachBatchSize)]
                    Y_predict_batch = Y_predict[BatchNumber*EachBatchSize:(BatchNumber*EachBatchSize+EachBatchSize)]

                    session.run(self.trainFaceExpressionModel, feed_dict={self.InputTensor_XTrainData: X_Features_batch, self.ToBePredictedTensor_Target: Y_predict_batch})

                    if BatchNumber % 20 == 0:
                        
                        costAtThatIteration = session.run(self.Globalcost, feed_dict={self.InputTensor_XTrainData: X_Features_Validation , self.ToBePredictedTensor_Target: Y_predict_Validation})
                        costsToPlot.append(costAtThatIteration)

                        predictionValue = session.run(self.predictThEmotionResults, feed_dict={self.InputTensor_XTrainData: X_Features_Validation , self.ToBePredictedTensor_Target: Y_predict_Validation})
                        e = np.mean(np.argmax(Y_predict_Validation, axis=1) != predictionValue)
                        
                        print "IterI:", IterI, "BatchNumber:", BatchNumber, "cost:", self.Globalcost, "error rate:", e
                        
        plt.plot(costsToPlot)
        plt.show()
 
    def forwardPropagation(self,InputTensor_XTrainData,ListOfAllHiddenLayers):
        self.ListOfAllHiddenLayers = ListOfAllHiddenLayers
        self.TemporaryFeedForwardResult = self.InputTensor_XTrainData
        ## since there are many layers ; we have to do the forward progation of each layer with its previous forward resullts
        # lets clculate the feed forwward for each layer
        # lets get all the layer objeccts as created above.
        # each layer object has the forward propagation calculation for that layer;
        # we will passs the ther layer to be multiplied with the present layer
        # we use the tensorflow's matrix mulitplication
        for Eachlayer in self.hidden_layers:
            self.TemporaryFeedForwardResult = Eachlayer.forwardPropWithPreviousLayerResults(self.TemporaryFeedForwardResult)
        
        ## for the last layer to the output classification model, Im doing it explicitly myself
        ## here the curent layer would be the final layer as the self object indicates; these are the layer values which i explicitly created for the last layer
        return tf.matmul(self.TemporaryFeedForwardResult, self.WeightsInCurrentLayer) + self.biasForCurrentLayer       
        
        
    def predictThEmotionResults(self, InputTensor_XTrainData):
        # calculate the feedd forward with the current weights
        self.forwardPropResultForPrediction = self.forwardPropagation(self.InputTensor_XTrainData,self.ListOfAllHiddenLayers)
        return tf.argmax(self.forwardPropResultForPrediction, 1)        
 
    
    
    def LoadData():
    # this data is collected from the kaggle competetion - link shared in the intial proposal report
    # https://www.kaggle.com/c/challenges-in-representation-learning-facial-expression-recognition-challenge    
    # implementing the logistic regression, and the data or int pixels are considered as the numpy array with spatial information of the image sacrified
    # The iamges are 48 X 48 pixel
    Header = 1 # Data has the Header
    X_Features = []
    Y_predict =[]
    
    # read the source data file
    #filepath = "C:/Users/nsuri/Downloads/cvData/fer2013.csv"
    #filepath = "D:/sem4/ComputerVision/CNN-Proj/Code-FaceRecognition/fer2013/fer2013.csv"
    filepath = "D:/sem4/ComputerVision/CNN-Proj/Code-FaceRecognition/fer2013/test.csv"
    
    for imageRecord in open(filepath):
        ## The better appraoch to read the data is to create the dictionary reader; so that we can access the each record iteratively with the file pointer and the columns can be read using their column names
        ## However, the below design uses the genral approach.
        if Header == 1:
            Header = 0
            #intentionally  skipping the first row, to avoid processing the header
            
        else:
            Currentfeature=[]
            imageCurrentRow = imageRecord.split(',')
            # get the x feature and the Y prediction values
            features = imageCurrentRow[1].split()
            for f in features:
                Currentfeature.append(float(f))
            X_Features.append(Currentfeature)
            
            #X_Features.append([int(p) for p in imageCurrentRow[1].split()])
            
            pred = imageCurrentRow[0]
            Y_predict.append(float(pred))
            
    # after all the rows of images is collected, convert this is numpy array, that makes our job easy to work on the data
    # normalize the data, that helps in better number ranges from 0 to 1, and would help in predection
            
    X_Features =  np.array(X_Features)  / 255.0
    Y_predict = np.array(Y_predict)
    
    return X_Features,Y_predict
    
    
def LoadValidationData():
    # this data is collected from the kaggle competetion - link shared in our intial proposal report
    # https://www.kaggle.com/c/challenges-in-representation-learning-facial-expression-recognition-challenge    
    # implementing the logistic regression, and the data or int pixels are considered as the numpy array with spatial information of the image sacrified
    # The iamges are 48 X 48 pixel
    Header = 1 # Data has the Header
    X_Features_Validation = []
    Y_predict_Validation =[]
    
    # read the source data file
    filepath = "D:/sem4/ComputerVision/CNN-Proj/Code-FaceRecognition/fer2013/fer2013_publicTest.csv"
    for imageRecord in open(filepath):
        if Header == 1:
            Header = 0
            #intentionally  skipping the first row, to avoid processing the header
            
        else:
            Currentfeature=[]
            imageCurrentRow = imageRecord.split(',')
            # get the x feature and the Y prediction values
            features = imageCurrentRow[1].split()
            for f in features:
                Currentfeature.append(float(f))
            X_Features_Validation.append(Currentfeature)
            
            #X_Features_Validation.append([int(p) for p in imageCurrentRow[1].split()])
            
            pred = imageCurrentRow[0]
            Y_predict_Validation.append(float(pred))
            
    # after all the rows of images is collected, convert this is numpy array, that makes our job easy to work on the data
    # normalize the data, that helps in better number ranges from 0 to 1, and would help in predection
            
    X_Features_Validation =  np.array(X_Features_Validation)  / 255.0
    Y_predict_Validation = np.array(Y_predict_Validation)
    
    return X_Features_Validation,Y_predict_Validation

       
def ClassImbalanace(X_Features,Y_predict):
    # this is used to solve the class imbalance problem, which improves the prediction
    # for example im balancing only the class 1, we can look for all other classes which are less in proportin and we can balance them
    # seperate all the classes with 1 and others
    xNotOfClass1 = X_Features[Y_predict!=1,:]
    yNotOfClass1 = Y_predict[Y_predict!=1]
    
    xOfClass1 = X_Features[Y_predict==1]
    #yOfClass1 = Y_predict[Y_predict==1,:]
    
    # now there are different ways to solve the imbalance problem, either selecting from the same group that is selecting samples from the same set
    # or find the distribution of the data, and try to generate the data from that distribution using the bayesian approach
    # and few other methods
    
    # but im using the repeatitive seelction process.
    XNew = np.repeat(xOfClass1,10,axis=0) ## so we will be creating the duplicate rows; by 10 times to their exiting count
    
    X_Features = np.vstack([xNotOfClass1,XNew])
    Y_predict = np.concatenate((yNotOfClass1,[1]*len(XNew)))
    
    return X_Features,Y_predict

def Calcualte_errorRate(Yvalid, predictedResult):
    # how many of the results were wrongly predicted or 1-correctly predicted
    ErrorVal = 1 - np.mean(Yvalid == predictedResult)
    return ErrorVal
    
def relu(x):
    # condition check return 0 or 1
    # we will pass values only greter than 0 and all others converted to 0
    return x * (x > 0)
    
def softmax(layerResults):
    # look for the proof in stanford material of Machine learning - Dr. Andrew ng
    return np.exp(layerResults)/ np.exp(layerResults).sum(axis=1,keepdims=True)

    
def forwardPropagation(xfeatures,HiddenLayersWeights,BiasForHiddenLayers):
    # Im using the ReLU here, but we can also use the tanh function
    Z_resultsUntilLastLayer = relu(xfeatures.dot(HiddenLayersWeights) + BiasForHiddenLayers)
    return Z_resultsUntilLastLayer 
       
def MainProgramStartsHere():
    # the deep neural net with the tensorflow starts here
    # load the data
    # this data is collected from the kaggle competetion - link shared in the intial proposal report
    X_Features,Y_predict = LoadData()
    # total number of diffeent classes in the data are
    TotalNumberOfClasses = len(set(Y_predict))
    # after the data is loaded, we should solve the class imbalance
    print "Data is loaded"
    ClassImbalanace(X_Features,Y_predict)
    
    ## now lets load the valkidation data for testing the model
    X_Features_Validation,Y_predict_Validation = LoadValidationData()
    
    ## Specifying how many hidden layers we want and how many units in the each layer
    hiddenUnitsInEachLayer= [2000,1000,500]
    
    #### Now lets train the deep neural net
    DeepNN_TFlow(X_Features,Y_predict,X_Features_Validation,Y_predict_Validation,hiddenUnitsInEachLayer,TotalNumberOfClasses)
 
    
if __name__ == '__main__':
    MainProgramStartsHere()
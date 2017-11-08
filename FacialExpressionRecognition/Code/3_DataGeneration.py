import numpy as np
import pandas as pd
import random

## working on data set of kaggle for face expression recognition
# I have already built a deep neural network with the just python and TensorFlow
# now, I will use the CNN for the image analysis, which is definitely a better tool compared to the DNN
# I will use the Theano and the Keras to speed up the CNN creation
# the advantage with the Keras is that we need to write a minimal code for neural network building - In this case a CNN

## so lets try to get all the data we want from the source data files
## and lets save the data in numpy array format so that we can use the same data anywhere we want


## our data set includes 3 types of data; Public, private and train. We can use the above data for trainig and testing the models that I built


# the emotions we have are : 'Angry': 0, 'Disgust': 1, 'Fear': 2, 'Happy': 3, 'Sad': 4, 'Surprise': 5, 'Neutral': 6 as given in the Kaggle
# i have assigned it in the order they gave and the numbers are given to the categorical data to make the calcuations, especially the loss and prediction calculations easy

#######################################
# lets create a dictionary of the emotions; to use them later;
## the data set given contains disgust; but I will be removing disgust and replace them with the angry
emotionsList= {'Angry': 0, 'Disgust': 1, 'Fear': 2, 'Happy': 3, 'Sad': 4, 'Surprise': 5, 'Neutral': 6}
# The list of emotions we have 
emotions = ['Angry', 'Fear', 'Happy', 'Sad', 'Surprise', 'Neutral']


def TotalEmotionCounts(ytrainData, AvailableClasses):

	  ## the data set given contains disgust; but I will be removing disgust and replace them with the angry
    ytrainData.loc[ytrainData == 1] = 0
	  # removing the disgust to process only the other 6 emotions
    AvailableClasses.remove('Disgust')
	  ## using enumeration to get both the value and its index since they are already order as my dictionary
    for EmotionInNumber, classOfEmotion in enumerate(AvailableClasses):
        ytrainData.loc[(ytrainData == emotionsList[classOfEmotion])] = EmotionInNumber
    return ytrainData.values



def ExtractEachRowIntoImage(imageRow):
    size=(48,48)
    pixelIntoArray = np.array(map(int, imageRow.split()))
    return pixelIntoArray.reshape(size)

	

def LoadDataAndProcess(SplitSize,DataTypeToProcess, AvailableClasses):
    
    fileToProcess="D:/sem4/ComputerVision/CNN-Proj/Code-FaceRecognition/fer2013/fer2013.csv"
	  ## the data set given contains disgust; but I will be removing disgust and replace them with the angry
    AvailableClasses.append('Disgust')	
	  # read the data from the Kaggle data file
    SoueceData = pd.read_csv(fileToProcess)
    # now lets select only the data that we want toprocess; lets suppose if I want to process the "PrivateTest" or "PublicTest"; i should use based on that.
    AllDataThatsOnlyWeNeed = [SoueceData.DataTypeToProcess == DataTypeToProcess]
    SoueceData = SoueceData[AllDataThatsOnlyWeNeed]
	  ## so filtered the data as we need
	
    GroupByEmotionType = []
    
    for EachClass in AvailableClasses:
        RecordsTHoseMatchGivenClass = [SoueceData['emotion'] == emotionsList[EachClass]]
        ClasssSoueceData = SoueceData[RecordsTHoseMatchGivenClass]
        GroupByEmotionType.append(ClasssSoueceData)
		
	  ## Now we have all the classes only for those we need the emotions
	  ## not every emotion is included, unless wwe specify. In my case Im specifying it
	## lets put back every thing and get back to the original shape 
    FinalProcessedData = pd.concat(GroupByEmotionType, axis=0)
    NumberOfRecordsToBeSmpled = int(len(FinalProcessedData)*SplitSize)
    IndexOfAllData = FinalProcessedData.index
	  ## randomly wee have seleccted some records based onthe split size we mentioned earlier
    RandomSelectedData = random.sample(IndexOfAllData, NumberOfRecordsToBeSmpled)
    FinalProcessedData = FinalProcessedData.ix[RandomSelectedData]
    ## since all the data of an image is under pixel column as a string lets process each pixel and make them as numbers
	  ## since its an image we shalll create the matrix for each image and put them in a list and reshape using the numpyarray later
    FinalProcessedData['pixels'] = FinalProcessedData.pixels.apply(lambda imageRow: ExtractEachRowIntoImage(imageRow))
	  ####
    AllImagesInmatrixForm = []
    for EachRowData in FinalProcessedData.pixels:
		    AllImagesInmatrixForm.append(EachRowData)
		
    DataInNumpArrayList = np.array(AllImagesInmatrixForm)
    XtrainData = DataInNumpArrayList.reshape(-1, 1, DataInNumpArrayList.shape[1], DataInNumpArrayList.shape[2])
    ytrainData = TotalEmotionCounts(FinalProcessedData.emotion, AvailableClasses)

    return XtrainData, ytrainData

if __name__ == '__main__':
    # makes the numpy arrays ready to use:
    print "Cleaning Data"
    XtrainData, ytrainData = LoadDataAndProcess(SplitSize = 1.0,AvailableClasses= emotions,DataTypeToProcess='PrivateTest')
	  ## saave all the obtained data/fer2013
    folderToSave = "D:/sem4/ComputerVision/CNN-Proj/Code-FaceRecognition/fer2013/"
    np.save(folderToSave + 'XData'+"_Training", XtrainData)
    np.save(folderToSave + 'yData'+"_Training", ytrainData)
	
    print "Data Saved..."



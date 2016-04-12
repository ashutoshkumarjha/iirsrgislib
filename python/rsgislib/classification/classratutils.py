#! /usr/bin/env python

############################################################################
#  classratutils.py
#
#  Copyright 2015 RSGISLib.
#
#  RSGISLib: 'The remote sensing and GIS Software Library'
#
#  RSGISLib is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  RSGISLib is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
#
#
# Purpose:  Provide a set of utilities which combine commands to create
#           useful extra functionality and make it more easily available
#           to be reused.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 28/11/2015
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################

haveGDALPy = True
try:
    import osgeo.gdal as gdal
except ImportError as gdalErr:
    haveGDALPy = False
    
haveNumpy = True
try:
    import numpy
except ImportError as numErr:
    haveNumpy = False

haveRIOSRat = True
try:
    from rios import rat
    from rios import ratapplier
except ImportError as riosRatErr:
    haveRIOSRat = False

haveSKLearnRF = True
try:
    from sklearn.ensemble import RandomForestClassifier
except ImportError as sklearnRFErr:
    haveSKLearnRF = False
    raise Exception("The scikit-learn random forests tools are required for this module could not be imported\n\t" + sklearnRFErr)

haveSKLearnKM = True
try:
    from sklearn.cluster import MiniBatchKMeans
except ImportError as sklearnMBKMErr:
    haveSKLearnKM = False
    raise Exception("The scikit-learn Mini Batch KMeans tools are required for this module could not be imported\n\t" + sklearnMBKMErr)
    

def classifyWithinRAT(clumpsImg, classesIntCol, classesNameCol, variables, classifier=RandomForestClassifier(n_estimators=100, max_features=3, oob_score=True, n_jobs=-1), outColInt="OutClass", outColStr="OutClassName", roiCol=None, roiVal=1, classColours=None):
    """
A function which will perform a classification within the RAT using a classifier from scikit-learn

* clumpsImg is the clumps image on which the classification is to be performed
* classesIntCol is the column with the training data as int values
* classesNameCol is the column with the training data as string class names
* variables is an array of column names which are to be used for the classification
* classifier is an instance of a scikit-learn classifier (e.g., RandomForests which is Default)
* outColInt is the output column name for the int class representation (Default: 'OutClass')
* outColStr is the output column name for the class names column (Default: 'OutClassName')
* roiCol is a column name for a column which specifies the region to be classified. If None ignored (Default: None)
* roiVal is a int value used within the roiCol to select a region to be classified (Default: 1)
* classColours is a python dict using the class name as the key along with arrays of length 3 specifying the RGB colours for the class.


Example::
from sklearn.ensemble import ExtraTreesClassifier
from rsgislib.classification import classratutils

classifier = ExtraTreesClassifier(n_estimators=100, max_features=3, n_jobs=-1, verbose=0)

classColours = dict()
classColours['Forest'] = [0,138,0]
classColours['NonForest'] = [200,200,200]

variables = ['GreenAvg', 'RedAvg', 'NIR1Avg', 'NIR2Avg', 'NDVI']
classifyWithinRAT(clumpsImg, classesIntCol, classesNameCol, variables, classifier=classifier, classColours=classColours)

"""
    # Check gdal is available
    if not haveGDALPy:
        raise Exception("The GDAL python bindings required for this function could not be imported\n\t" + gdalErr)
    # Check numpy is available
    if not haveNumpy:
        raise Exception("The numpy module is required for this function could not be imported\n\t" + numErr)
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception("The RIOS rat tools are required for this function could not be imported\n\t" + riosRatErr)
    # Check scikit-learn RF is available
    if not haveSKLearnRF:
        raise Exception("The scikit-learn random forests tools are required for this function could not be imported\n\t" + sklearnRFErr)
        
    ratDataset = gdal.Open(clumpsImg, gdal.GA_Update)
    numpyVars = []
    for var in variables:
        print("Reading " + var)
        tmpArr = rat.readColumn(ratDataset, var)
        numpyVars.append(tmpArr)
    
    # Read in training classes
    classesInt = rat.readColumn(ratDataset, classesIntCol)
    classesStr = rat.readColumn(ratDataset, classesNameCol)
        
    roi = None
    if not ((roiCol == None) or (roiCol == "")):
        roi = rat.readColumn(ratDataset, roiCol)
    
    # Set up output array
    outLabels = numpy.zeros_like(classesInt, dtype=numpy.int16)
    outClassNames = numpy.empty_like(classesInt, dtype=numpy.dtype('a255'))
    ID = numpy.arange(outLabels.shape[0])
    
    xData = numpy.array(numpyVars)
    xData = xData.transpose()
    xData = numpy.where(numpy.isfinite(xData), xData,0)

    print("Input data size: {} x {}".format(xData.shape[0], xData.shape[1]))
    
    trainingData = xData[numpy.isfinite(xData).all(axis=1)]
    classesInt = classesInt[numpy.isfinite(xData).all(axis=1)]
    classesStr = classesStr[numpy.isfinite(xData).all(axis=1)]
    ID = ID[numpy.isfinite(xData).all(axis=1)]
    
    trainingData = trainingData[classesInt > 0]
    classesStr = classesStr[classesInt > 0]
    classesInt = classesInt[classesInt > 0]
        
    print("Training data size: {} x {}".format(trainingData.shape[0], trainingData.shape[1]))
        
    print('Training Classifier')
    classifier.fit(trainingData, classesInt)
    
    print('Calc Accuracy',)
    accVal = classifier.score(trainingData, classesInt)
    print(' = ', accVal)
    
    if not roi is None:
        xData = xData[roi == roiVal]
        ID = ID[roi == roiVal]
        print("ROI Subsetted data size: {} x {}".format(xData.shape[0], xData.shape[1]))
    
    predClass = classifier.predict(xData)
    
    outLabels[ID] = predClass
    
    print("Writing Columns")
    rat.writeColumn(ratDataset, outColInt, outLabels)
    
    print("Create and Write Output Class Names")
    classNames = numpy.unique(classesStr)
    classes = numpy.zeros_like(classNames, dtype=numpy.int16)
    
  
    i = 0
    classNameIDs = dict()
    for className in classNames:
        classNameStr = str(className.decode())
        if not classNameStr is '':
            classes[i] = classesInt[classesStr == className][0]
            classNameIDs[classNameStr] = classes[i]
            print("Class \'" + classNameStr + "\' has numerical " + str(classes[i]))  
            i = i + 1
    
    outClassNames[...] = ''
    for className in classNameIDs:
        classID = classNameIDs[className]
        outClassNames[outLabels==classID] = className
    
    rat.writeColumn(ratDataset, outColStr, outClassNames)
    
    if not classColours is None:
        print("Set Colours")
        red = rat.readColumn(ratDataset, "Red")
        green = rat.readColumn(ratDataset, "Green")
        blue = rat.readColumn(ratDataset, "Blue")
        
        # Set Background to black
        red[...] = 0
        green[...] = 0
        blue[...] = 0
        
        # Set colours
        for className in classNameIDs:
            print("Colouring class " + className)
            classID = classNameIDs[className]
            colours = classColours[className]
            
            red   = numpy.where(outLabels == classID, colours[0], red)
            green = numpy.where(outLabels == classID, colours[1], green)
            blue  = numpy.where(outLabels == classID, colours[2], blue)
    
        rat.writeColumn(ratDataset, "Red", red)
        rat.writeColumn(ratDataset, "Green", green)
        rat.writeColumn(ratDataset, "Blue", blue)
    
    ratDataset = None




def clusterWithinRAT(clumpsImg, variables, clusterer=MiniBatchKMeans(n_clusters=8, init='k-means++', max_iter=100, batch_size=100), outColInt="OutCluster", roiCol=None, roiVal=1, clrClusters=True, clrSeed=10, addConnectivity=False):
    """
A function which will perform a clustering within the RAT using a clustering algorithm from scikit-learn

* clumpsImg is the clumps image on which the classification is to be performed.
* variables is an array of column names which are to be used for the clustering.
* clusterer is an instance of a scikit-learn clusterer (e.g., MiniBatchKMeans which is Default; Note with 8 clusters).
* outColInt is the output column name identifying the clusters (Default: 'OutCluster').
* roiCol is a column name for a column which specifies the region to be clustered. If None ignored (Default: None).
* roiVal is a int value used within the roiCol to select a region to be clustered (Default: 1).
* clrClusters is a boolean specifying whether the colour table should be updated to correspond to the clusters (Default: True).
* clrSeed is an integer seeding the random generator used to generate the colours (Default=10; if None provided system time used).
* addConnectivity is a boolean which adds a kneighbors_graph to the clusterer (just an option for the AgglomerativeClustering algorithm)


Example::
    
    from rsgislib.classification import classratutils
    from sklearn.cluster import DBSCAN
    
    sklearnClusterer = DBSCAN(eps=1, min_samples=50)
    classratutils.clusterWithinRAT('MangroveClumps.kea', ['MinX', 'MinY'], clusterer=sklearnClusterer, outColInt="OutCluster", roiCol=None, roiVal=1, clrClusters=True, clrSeed=10, addConnectivity=False)

"""
    # Check gdal is available
    if not haveGDALPy:
        raise Exception("The GDAL python bindings required for this function could not be imported\n\t" + gdalErr)
    # Check numpy is available
    if not haveNumpy:
        raise Exception("The numpy module is required for this function could not be imported\n\t" + numErr)
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception("The RIOS rat tools are required for this function could not be imported\n\t" + riosRatErr)
    # Check scikit-learn RF is available
    if not haveSKLearnKM:
        raise Exception("The scikit-learn Mini Batch KMeans tools are required for this function could not be imported\n\t" + sklearnMBKMErr)
        
    ratDataset = gdal.Open(clumpsImg, gdal.GA_Update)
    Histogram = rat.readColumn(ratDataset, 'Histogram')
    numpyVars = []
    for var in variables:
        print("Reading " + var)
        tmpArr = rat.readColumn(ratDataset, var)
        numpyVars.append(tmpArr)
        
    roi = None
    if not ((roiCol == None) or (roiCol == "")):
        roi = rat.readColumn(ratDataset, roiCol)
    
    # Set up output array
    outLabels = numpy.zeros_like(Histogram, dtype=numpy.int16)
    ID = numpy.arange(outLabels.shape[0])
    
    xData = numpy.array(numpyVars)
    xData = xData.transpose()
    ID = ID[numpy.isfinite(xData).all(axis=1)]
    if not roi is None:
        roi = roi[numpy.isfinite(xData).all(axis=1)]
    xData = xData[numpy.isfinite(xData).all(axis=1)]
    
    if not roi is None:
        xData = xData[roi == roiVal]
        ID = ID[roi == roiVal]    
    
    print("Input Data Size: {} x {}".format(xData.shape[0], xData.shape[1]))
    
    if addConnectivity:
        from sklearn.neighbors import kneighbors_graph
        inConnectivity = kneighbors_graph(xData, n_neighbors=10, include_self=False)
        clusterer.set_params(**{'connectivity': inConnectivity})
        
    print('Fit Clusterer')
    outClust = clusterer.fit_predict(xData)    
    
    minClusterID = numpy.min(outClust)
    if minClusterID <= 0:
        minOff = 1 - minClusterID
        outClust = outClust + minOff
    
    outLabels[ID] = outClust
    
    print("Writing Columns")
    rat.writeColumn(ratDataset, outColInt, outLabels)
    
    print("Create and Write Output Class Names")
    clustersIDs = numpy.unique(outClust)
      
    if clrClusters:
        random.seed(clrSeed)
    
        print("Set Colours")
        red = rat.readColumn(ratDataset, "Red")
        green = rat.readColumn(ratDataset, "Green")
        blue = rat.readColumn(ratDataset, "Blue")
        
        # Set Background to black
        red[...] = 0
        green[...] = 0
        blue[...] = 0
        
        # Set colours
        for clusterID in clustersIDs:
            print("Colouring cluster: " + str(clusterID))
            
            red   = numpy.where(outLabels == clusterID, random.randint(0,255), red)
            green = numpy.where(outLabels == clusterID, random.randint(0,255), green)
            blue  = numpy.where(outLabels == clusterID, random.randint(0,255), blue)
    
        rat.writeColumn(ratDataset, "Red", red)
        rat.writeColumn(ratDataset, "Green", green)
        rat.writeColumn(ratDataset, "Blue", blue)

    ratDataset = None



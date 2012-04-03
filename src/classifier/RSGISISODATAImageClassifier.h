 /*
 *  RSGISISODATAImageClassifier.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 18/08/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
 *
 * This file is part of RSGISLib.
 * 
 * RSGISLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSGISLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */



#ifndef RSGISISODATAImageClassifier_H
#define RSGISISODATAImageClassifier_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISImageStatistics.h"

#include "common/RSGISClassificationException.h"

#include "classifier/RSGISClassifier.h"

#include "gdal_priv.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

using namespace rsgis;
using namespace rsgis::img;
using namespace std;

namespace rsgis{ namespace classifier{
	
	
	class RSGISISODATAClassifier
	{
	public:
		RSGISISODATAClassifier(string inputImageFile, bool printinfo);
		void initClusterCentresRandom(unsigned int numClusters)throw(RSGISClassificationException);
		void initClusterCentresKpp(unsigned int numClusters)throw(RSGISClassificationException);
		void calcClusterCentres(double terminalThreshold, unsigned int maxIterations, unsigned int minNumVals, double minDistanceBetweenCentres, double stddevThres, float propOverAvgDist)throw(RSGISClassificationException);
		void generateOutputImage(string outputImageFile)throw(RSGISClassificationException);
		~RSGISISODATAClassifier();
	protected:
		string inputImageFile;
		vector<ClusterCentreISO*> *clusterCentres;
		bool hasInitClusterCentres;
		GDALDataset **datasets;
		unsigned int numDatasets;
		unsigned int numImageBands;
		unsigned int clusterIDVal;
		bool printinfo;
	};
	
	class RSGISISODATACalcPixelClusterCalcImageVal : public RSGISCalcImageValue
	{
	public: 
		RSGISISODATACalcPixelClusterCalcImageVal(int numOutBands, vector<ClusterCentreISO*> *clusterCentres, unsigned int numImageBands);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		vector<ClusterCentreISO*>* getNewClusterCentres();
		void reset(vector<ClusterCentreISO*> *clusterCentres);
		double getAverageDistance();
		~RSGISISODATACalcPixelClusterCalcImageVal();
	protected:
		vector<ClusterCentreISO*> *clusterCentres;
		vector<ClusterCentreISO*> *newClusterCentres;
		unsigned int numImageBands;
		double sumDist;
		unsigned long numVals;
	};
	
	class RSGISISODATACalcPixelClusterStdDevCalcImageVal : public RSGISCalcImageValue
	{
	public: 
		RSGISISODATACalcPixelClusterStdDevCalcImageVal(int numOutBands, vector<ClusterCentreISO*> *clusterCentres, unsigned int numImageBands);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void reset(vector<ClusterCentreISO*> *clusterCentres);
		~RSGISISODATACalcPixelClusterStdDevCalcImageVal();
	protected:
		vector<ClusterCentreISO*> *clusterCentres;
		unsigned int numImageBands;
	};
	
	class RSGISApplyISODATAClassifierCalcImageVal : public RSGISCalcImageValue
	{
	public: 
		RSGISApplyISODATAClassifierCalcImageVal(int numOutBands, vector<ClusterCentreISO*> *clusterCentres);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not Implemented");};
		~RSGISApplyISODATAClassifierCalcImageVal();
	protected:
		vector<ClusterCentreISO*> *clusterCentres;
	};
}}

#endif








/*
 *  RSGISISODATAImageClassifier.cpp
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

#include "RSGISISODATAImageClassifier.h"


namespace rsgis{ namespace classifier{
	
	RSGISISODATAClassifier::RSGISISODATAClassifier(string inputImageFile, bool printinfo): clusterCentres(NULL), hasInitClusterCentres(false), datasets(NULL), numDatasets(0), printinfo(false)
	{
		this->inputImageFile = inputImageFile;
		clusterIDVal = 0;
		this->printinfo = printinfo;
	}
	
	void RSGISISODATAClassifier::initClusterCentresKpp(unsigned int numClusters)throw(RSGISClassificationException)
	{
		throw RSGISClassificationException("Implementation not complete!!");
		RSGISVectors vecUtils;
		
		// Open Image
		GDALAllRegister();		
		try
		{
			this->numDatasets = 1;
			datasets = new GDALDataset*[1];
			cout << this->inputImageFile << endl;
			datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImageFile.c_str(), GA_ReadOnly);
			if(datasets[0] == NULL)
			{
				string message = string("Could not open image ") + this->inputImageFile;
				throw RSGISClassificationException(message.c_str());
			}
		}
		catch(RSGISClassificationException& e)
		{
			throw e;
		}
		
		// Calculate image band stats
		numImageBands = datasets[0]->GetRasterCount();
		
		ImageStats **stats = new ImageStats*[numImageBands];
		for(unsigned int i = 0; i < numImageBands; ++i)
		{
			stats[i] = new ImageStats();
		}
		
		RSGISImageStatistics calcImgStats;
		try 
		{
			calcImgStats.calcImageStatistics(datasets, 1, stats, numImageBands, true);
		}
		catch (RSGISImageCalcException &e) 
		{
			for(unsigned int i = 0; i < numImageBands; ++i)
			{
				delete stats[i];
			}
			delete[] stats;
			
			throw RSGISClassificationException(e.what());
		}
		catch (RSGISImageBandException &e) 
		{
			for(unsigned int i = 0; i < numImageBands; ++i)
			{
				delete stats[i];
			}
			delete[] stats;
			
			throw RSGISClassificationException(e.what());
		}
		
		
		// Allocate K cluster centers
		clusterCentres = new vector<ClusterCentreISO*>();
		ClusterCentreISO *tmpCluster = NULL;
		for(unsigned int i = 0; i < numClusters; ++i)
		{
			tmpCluster = new ClusterCentreISO();
			tmpCluster->classID = clusterIDVal++;
			tmpCluster->data = vecUtils.createVector(numImageBands);
			tmpCluster->stddev = vecUtils.createVector(numImageBands);
			tmpCluster->numVals = 0;
			clusterCentres->push_back(tmpCluster);
		}
		
		
		// Calculate the initial cluster centres.
		
		
		// Print init cluster centres
		vector<ClusterCentreISO*>::iterator iterCentres;
		for(iterCentres = clusterCentres->begin(); iterCentres != clusterCentres->end(); ++iterCentres)
		{
			cout << "Cluster " << (*iterCentres)->classID << ": ";
			for(unsigned int j = 0; j < numImageBands; ++j)
			{
				cout << (*iterCentres)->data->vector[j] << ", ";
			}
			cout << endl;
		}
		
		// Free stats from memory
		for(unsigned int i = 0; i < numImageBands; ++i)
		{
			delete stats[i];
		}
		delete[] stats;
		
		hasInitClusterCentres = true;
	}
	
	void RSGISISODATAClassifier::initClusterCentresRandom(unsigned int numClusters)throw(RSGISClassificationException)
	{
		RSGISVectors vecUtils;
		
		// Open Image
		GDALAllRegister();		
		try
		{
			this->numDatasets = 1;
			datasets = new GDALDataset*[1];
			cout << this->inputImageFile << endl;
			datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImageFile.c_str(), GA_ReadOnly);
			if(datasets[0] == NULL)
			{
				string message = string("Could not open image ") + this->inputImageFile;
				throw RSGISClassificationException(message.c_str());
			}
		}
		catch(RSGISClassificationException& e)
		{
			throw e;
		}
		
		// Calculate image band stats
		numImageBands = datasets[0]->GetRasterCount();
		
		ImageStats **stats = new ImageStats*[numImageBands];
		for(unsigned int i = 0; i < numImageBands; ++i)
		{
			stats[i] = new ImageStats();
		}
		
		RSGISImageStatistics calcImgStats;
		try 
		{
			calcImgStats.calcImageStatistics(datasets, 1, stats, numImageBands, false);
		}
		catch (RSGISImageCalcException &e) 
		{
			for(unsigned int i = 0; i < numImageBands; ++i)
			{
				delete stats[i];
			}
			delete[] stats;
			
			throw RSGISClassificationException(e.what());
		}
		catch (RSGISImageBandException &e) 
		{
			for(unsigned int i = 0; i < numImageBands; ++i)
			{
				delete stats[i];
			}
			delete[] stats;
			
			throw RSGISClassificationException(e.what());
		}
		
		// Allocate K cluster centers
		clusterCentres = new vector<ClusterCentreISO*>();
		ClusterCentreISO *tmpCluster = NULL;
		for(unsigned int i = 0; i < numClusters; ++i)
		{
			tmpCluster = new ClusterCentreISO();
			tmpCluster->classID = clusterIDVal++;
			tmpCluster->data = vecUtils.createVector(numImageBands);
			tmpCluster->stddev = vecUtils.createVector(numImageBands);
			tmpCluster->numVals = 0;
			tmpCluster->avgDist = 0;
			tmpCluster->split = false;
			clusterCentres->push_back(tmpCluster);
		}
		
		// Define Cluster centres
		boost::mt19937 randomGen;
		vector<ClusterCentreISO*>::iterator iterCentres;
		for(unsigned int i = 0; i < numImageBands; ++i)
		{
			boost::uniform_int<> randomDist(stats[i]->min, stats[i]->max);
			boost::variate_generator<boost::mt19937&, boost::uniform_int<> > randomVal(randomGen, randomDist);
			
			for(iterCentres = clusterCentres->begin(); iterCentres != clusterCentres->end(); ++iterCentres)
			{
				(*iterCentres)->data->vector[i] = randomVal();
			}
		}
		
		if(printinfo)
		{
			for(iterCentres = clusterCentres->begin(); iterCentres != clusterCentres->end(); ++iterCentres)
			{
				cout << "Cluster " << (*iterCentres)->classID << ": ";
				for(unsigned int j = 0; j < numImageBands; ++j)
				{
					cout << (*iterCentres)->data->vector[j] << ", ";
				}
				cout << endl;
			}
		}
		
		// Free stats from memory
		for(unsigned int i = 0; i < numImageBands; ++i)
		{
			delete stats[i];
		}
		delete[] stats;
		
		hasInitClusterCentres = true;
	}
	
	void RSGISISODATAClassifier::calcClusterCentres(double terminalThreshold, unsigned int maxIterations, unsigned int minNumVals, double minDistanceBetweenCentres, double stddevThres, float propOverAvgDist) throw(RSGISClassificationException)
	{
		if(hasInitClusterCentres)
		{
			RSGISVectors vecUtils;
			try 
			{
				RSGISISODATACalcPixelClusterCalcImageVal *calcClusterCentre = new RSGISISODATACalcPixelClusterCalcImageVal(0, this->clusterCentres, this->numImageBands);
				RSGISISODATACalcPixelClusterStdDevCalcImageVal *calcClusterStdDevs = new RSGISISODATACalcPixelClusterStdDevCalcImageVal(0, this->clusterCentres, this->numImageBands);
				RSGISCalcImage *calcImageClusterCentres = new RSGISCalcImage(calcClusterCentre, "", true);
				RSGISCalcImage *calcImageClusterStddevs = new RSGISCalcImage(calcClusterStdDevs, "", true);
				
				vector<ClusterCentreISO*> *newClusterCentres = NULL;
				double centreMoveDistanceSum = 0;
				double centreMoveDistance = 0;
				double averageDistance = 0;
				bool continueIterating = true;
				unsigned int iterNum = 0;
				vector<ClusterCentreISO*>::iterator iterCentres;
				vector<ClusterCentreISO*>::iterator iterCentres2;
				vector<ClusterCentreISO*>::iterator iterNewCentres;
				
				while(continueIterating & (iterNum < maxIterations))
				{
					cout << "Iteration " << iterNum << ":\t" << endl;
					centreMoveDistanceSum = 0;
					centreMoveDistance = 0;
					averageDistance = 0;
					
					// Identify new centres
					calcImageClusterCentres->calcImage(datasets, numDatasets);
					newClusterCentres = calcClusterCentre->getNewClusterCentres();					
					// Calculate distance between new and old centres.
					iterNewCentres = newClusterCentres->begin();
					iterCentres = clusterCentres->begin();
					for(unsigned int i = 0; i < clusterCentres->size(); ++i)
					{
						if(printinfo)
						{
							cout << "Cluster " << (*iterNewCentres)->classID << " has " << (*iterNewCentres)->numVals << " associated\n";
						}
						for(unsigned int j = 0; j < this->numImageBands; ++j)
						{
							if((*iterNewCentres)->numVals == 0)
							{
								(*iterNewCentres)->data->vector[j] = (*iterCentres)->data->vector[j];
							}
							else
							{
								(*iterNewCentres)->data->vector[j] = (*iterNewCentres)->data->vector[j] / (*iterNewCentres)->numVals;
							}
						}
						
						(*iterNewCentres)->avgDist = (*iterNewCentres)->avgDist / (*iterNewCentres)->numVals;
						centreMoveDistanceSum += vecUtils.euclideanDistance((*iterNewCentres)->data, (*iterCentres)->data);
						++iterCentres;
						++iterNewCentres;
					}
					centreMoveDistance = centreMoveDistanceSum/clusterCentres->size();
					
					cout << "Distance Moved = " << centreMoveDistance << " from " << newClusterCentres->size() << " clusters." << endl;
					
					if(centreMoveDistance < terminalThreshold)
					{
						continueIterating = false;
					}
					else
					{
						// Calc cluster std devs
						calcClusterStdDevs->reset(newClusterCentres);
						calcImageClusterStddevs->calcImage(datasets, numDatasets);
						for(iterNewCentres = newClusterCentres->begin(); iterNewCentres != newClusterCentres->end(); ++iterNewCentres)
						{
							for(unsigned int i = 0; i < this->numImageBands; ++i)
							{
								(*iterNewCentres)->stddev->vector[i] = sqrt((*iterNewCentres)->stddev->vector[i]/(*iterNewCentres)->numVals);
							}
						}
						
						
						// Replace old cluster centres within new centres
						// Clear current currentCentres
						for(iterCentres = clusterCentres->begin(); iterCentres != clusterCentres->end();)
						{
							vecUtils.freeVector((*iterCentres)->data);
							vecUtils.freeVector((*iterCentres)->stddev);
							delete (*iterCentres);
							clusterCentres->erase(iterCentres);
						}
						// Add new cluster centres
						for(iterNewCentres = newClusterCentres->begin(); iterNewCentres != newClusterCentres->end();)
						{
							clusterCentres->push_back(*iterNewCentres);
							newClusterCentres->erase(iterNewCentres);
						}
						
						
						// Remove clusters with too few values associated with the cluster centre
						for(iterCentres = clusterCentres->begin(); iterCentres != clusterCentres->end(); )
						{
							if((*iterCentres)->numVals < minNumVals)
							{
								vecUtils.freeVector((*iterCentres)->data);
								vecUtils.freeVector((*iterCentres)->stddev);
								delete (*iterCentres);
								clusterCentres->erase(iterCentres);
							}
							else
							{
								++iterCentres;
							}
						}
						
						// Merge cluster who's centres are too close to one another
						double distance = 0;
						for(iterCentres = clusterCentres->begin(); iterCentres != clusterCentres->end(); ++iterCentres)
						{
							if(printinfo)
							{
								cout << "Checking " << (*iterCentres)->classID << endl;
							}
							for(iterCentres2 = clusterCentres->begin(); iterCentres2 != clusterCentres->end(); ++iterCentres2)
							{
								distance = vecUtils.euclideanDistance((*iterCentres2)->data, (*iterCentres)->data);
								if(printinfo)
								{
									cout << "\tAgainst " << (*iterCentres2)->classID << " distance = " << distance << endl;
								}
								if((distance != 0) & (distance < minDistanceBetweenCentres))
								{
									if((*iterCentres)->numVals < (*iterCentres2)->numVals)
									{
										vecUtils.freeVector((*iterCentres)->data);
										vecUtils.freeVector((*iterCentres)->stddev);
										delete (*iterCentres);
										clusterCentres->erase(iterCentres);
									}
									else
									{
										vecUtils.freeVector((*iterCentres2)->data);
										vecUtils.freeVector((*iterCentres2)->stddev);
										delete (*iterCentres2);
										clusterCentres->erase(iterCentres2);
									}
									
									iterCentres = clusterCentres->begin();
									break;
								}
							}
						}
						
						averageDistance = calcClusterCentre->getAverageDistance();
						if(printinfo)
						{
							cout << "Overall Average distance = " << averageDistance << endl;
						}

						for(iterCentres = clusterCentres->begin(); iterCentres != clusterCentres->end(); ++iterCentres)
						{
							(*iterCentres)->split = false;
							if(printinfo)
							{
								cout << "Checking " << (*iterCentres)->classID << " Avg Dist = " << (*iterCentres)->avgDist << endl;
							}

							if((*iterCentres)->avgDist > (averageDistance*propOverAvgDist))
							{
								(*iterCentres)->split = true;
							}
							else
							{
								for(unsigned int i = 0; i < this->numImageBands; ++i)
								{
									if(printinfo)
									{
										cout << "\tStddev band " << i << " = " << (*iterCentres)->stddev->vector[i] << endl;
									}
									if((*iterCentres)->stddev->vector[i] > stddevThres)
									{
										(*iterCentres)->split = true;
									}
								}
							}
						}
						
						ClusterCentreISO *tmpCluster2Split = NULL;
						ClusterCentreISO *tmpClusterNew1 = NULL;
						ClusterCentreISO *tmpClusterNew2 = NULL;
						for(iterCentres = clusterCentres->begin(); iterCentres != clusterCentres->end(); ++iterCentres)
						{
							if((*iterCentres)->split)
							{
								tmpCluster2Split = *iterCentres;
								clusterCentres->erase(iterCentres);
								
								tmpClusterNew1 = new ClusterCentreISO();
								tmpClusterNew1->classID = clusterIDVal++;
								tmpClusterNew1->data = vecUtils.createVector(numImageBands);
								tmpClusterNew1->stddev = vecUtils.createVector(numImageBands);
								tmpClusterNew1->numVals = 0;
								tmpClusterNew1->avgDist = 0;
								tmpClusterNew1->split = false;
								
								tmpClusterNew2 = new ClusterCentreISO();
								tmpClusterNew2->classID = clusterIDVal++;
								tmpClusterNew2->data = vecUtils.createVector(numImageBands);
								tmpClusterNew2->stddev = vecUtils.createVector(numImageBands);
								tmpClusterNew2->numVals = 0;
								tmpClusterNew2->avgDist = 0;
								tmpClusterNew2->split = false;
								
								for(unsigned int i = 0; i < numImageBands; ++i)
								{
									tmpClusterNew1->data->vector[i] = tmpCluster2Split->data->vector[i] + tmpCluster2Split->stddev->vector[i];
									tmpClusterNew2->data->vector[i] = tmpCluster2Split->data->vector[i] - tmpCluster2Split->stddev->vector[i];
								}
								
								vecUtils.freeVector(tmpCluster2Split->data);
								vecUtils.freeVector(tmpCluster2Split->stddev);
								delete tmpCluster2Split;
								
								clusterCentres->push_back(tmpClusterNew1);
								clusterCentres->push_back(tmpClusterNew2);
								
								iterCentres = clusterCentres->begin();
							}
						}
						
						if(printinfo)
						{
							cout << "Num cluster centres in new cluster centres  = " << newClusterCentres->size() << endl;
						}
						
						newClusterCentres = NULL;
						calcClusterCentre->reset(clusterCentres);
					}
										
					
					// Print cluster centres
					if(printinfo)
					{
						for(iterCentres = clusterCentres->begin(); iterCentres != clusterCentres->end(); ++iterCentres)
						{
							cout << "Cluster " << (*iterCentres)->classID << ": ";
							for(unsigned int j = 0; j < numImageBands; ++j)
							{
								cout << (*iterCentres)->data->vector[j] << ", ";
							}
							cout << endl;
						}
					}
					++iterNum;
				}
				
				delete calcClusterCentre;
				delete calcClusterStdDevs;
				delete calcImageClusterCentres;
				delete calcImageClusterStddevs;
			}
			catch (RSGISImageCalcException &e) 
			{
				throw RSGISClassificationException(e.what());
			}
		}
		else
		{
			throw RSGISClassificationException("The cluster centres have not been initialised.");
		}
	}
	
	void RSGISISODATAClassifier::generateOutputImage(string outputImageFile)throw(RSGISClassificationException)
	{
		if(hasInitClusterCentres)
		{
			int i = 0;
			vector<ClusterCentreISO*>::iterator iterCentres;
			for(iterCentres = clusterCentres->begin(); iterCentres != clusterCentres->end(); ++iterCentres)
			{
				(*iterCentres)->classID = i++;
			}
			
			cout << clusterCentres->size() << " clusters have been identified." << endl;
			
			RSGISApplyISODATAClassifierCalcImageVal *applyClass = new RSGISApplyISODATAClassifierCalcImageVal(1, this->clusterCentres);
			RSGISCalcImage *calcImage = new RSGISCalcImage(applyClass, "", true);
			calcImage->calcImage(this->datasets, this->numDatasets, outputImageFile);
			
			delete applyClass;
			delete calcImage;
		}
		else
		{
			throw RSGISClassificationException("The cluster centres have not been initialised.");
		}
	}
	
	RSGISISODATAClassifier::~RSGISISODATAClassifier()
	{
		if(hasInitClusterCentres)
		{
			RSGISVectors vecUtils;
			vector<ClusterCentreISO*>::iterator iterCentres;
			for(iterCentres = clusterCentres->begin(); iterCentres != clusterCentres->end(); )
			{
				vecUtils.freeVector((*iterCentres)->data);
				vecUtils.freeVector((*iterCentres)->stddev);
				delete (*iterCentres);
				clusterCentres->erase(iterCentres);
			}
			delete clusterCentres;
			
			for(unsigned int i = 0; i < numDatasets; ++i)
			{
				GDALClose(datasets[i]);
			}
			delete[] datasets;
		}
	}
	
	
	RSGISISODATACalcPixelClusterCalcImageVal::RSGISISODATACalcPixelClusterCalcImageVal(int numOutBands, vector<ClusterCentreISO*> *clusterCentres, unsigned int numImageBands) : RSGISCalcImageValue(numOutBands)
	{
		this->clusterCentres = clusterCentres;
		this->numImageBands = numImageBands;
		
		RSGISVectors vecUtils;
		newClusterCentres = new vector<ClusterCentreISO*>();
		ClusterCentreISO *tmpCluster = NULL;
		vector<ClusterCentreISO*>::iterator iterCentres;
		for(iterCentres = clusterCentres->begin(); iterCentres != clusterCentres->end(); ++iterCentres)
		{
			tmpCluster = new ClusterCentreISO();
			tmpCluster->classID = (*iterCentres)->classID;
			tmpCluster->data = vecUtils.createVector(numImageBands);
			tmpCluster->stddev = vecUtils.createVector(numImageBands);
			tmpCluster->numVals = 0;
			tmpCluster->avgDist = 0;
			tmpCluster->split = false;
			newClusterCentres->push_back(tmpCluster);
		}
		
		sumDist = 0;
		numVals = 0;
	}
	
	void RSGISISODATACalcPixelClusterCalcImageVal::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		// Identify cluster within which point is associated with
		double minDistance = 0;
		ClusterCentreISO *minClusterCentre = NULL;
		bool first = true;
		double sum = 0;
		double distance = 0;
		vector<ClusterCentreISO*>::iterator iterCentres;
		for(iterCentres = clusterCentres->begin(); iterCentres != clusterCentres->end(); ++iterCentres)
		{
			sum = 0;
			for(int j = 0; j < numBands; ++j)
			{
				sum += (((*iterCentres)->data->vector[j] - bandValues[j])*((*iterCentres)->data->vector[j] - bandValues[j]));
			}
			distance = sum/numBands;
			
			if(first)
			{
				minDistance = distance;
				minClusterCentre = *iterCentres;
				first = false;
			}
			else if(distance < minDistance)
			{
				minDistance = distance;
				minClusterCentre = *iterCentres;
			}
		}
		
		//cout << "Pixel assocaited with cluster " << minClusterCentre->classID << endl;
		
		// add to sum for next centre
		for(iterCentres = newClusterCentres->begin(); iterCentres != newClusterCentres->end(); ++iterCentres)
		{
			if((*iterCentres)->classID == minClusterCentre->classID)
			{
				for(int i = 0; i < numBands; ++i)
				{
					(*iterCentres)->data->vector[i] += bandValues[i];
				}
				(*iterCentres)->numVals += 1;
				(*iterCentres)->avgDist += minDistance;
				break;
			}
		}
		
		sumDist += minDistance;
		++numVals;
	}
	
	vector<ClusterCentreISO*>* RSGISISODATACalcPixelClusterCalcImageVal::getNewClusterCentres()
	{
		return newClusterCentres;
	}
	
	void RSGISISODATACalcPixelClusterCalcImageVal::reset(vector<ClusterCentreISO*> *clusterCentres)
	{
		RSGISVectors vecUtils;
		this->clusterCentres = clusterCentres;

		vector<ClusterCentreISO*>::iterator iterCentres;
		for(iterCentres = newClusterCentres->begin(); iterCentres != newClusterCentres->end(); )
		{
			vecUtils.freeVector((*iterCentres)->data);
			vecUtils.freeVector((*iterCentres)->stddev);
			delete (*iterCentres);
			newClusterCentres->erase(iterCentres);
		}
		delete newClusterCentres;
		
		newClusterCentres = new vector<ClusterCentreISO*>();
		ClusterCentreISO *tmpCluster = NULL;
		for(iterCentres = clusterCentres->begin(); iterCentres != clusterCentres->end(); ++iterCentres)
		{
			tmpCluster = new ClusterCentreISO();
			tmpCluster->classID = (*iterCentres)->classID;
			tmpCluster->data = vecUtils.createVector(numImageBands);
			tmpCluster->stddev = vecUtils.createVector(numImageBands);
			tmpCluster->numVals = 0;
			tmpCluster->avgDist = 0;
			tmpCluster->split = false;
			newClusterCentres->push_back(tmpCluster);
		}
		
		sumDist = 0;
		numVals = 0;
	}
	
	double RSGISISODATACalcPixelClusterCalcImageVal::getAverageDistance()
	{
		return sumDist/numVals;
	}
	
	RSGISISODATACalcPixelClusterCalcImageVal::~RSGISISODATACalcPixelClusterCalcImageVal()
	{
		RSGISVectors vecUtils;
		vector<ClusterCentreISO*>::iterator iterCentres;
		for(iterCentres = newClusterCentres->begin(); iterCentres != newClusterCentres->end(); )
		{
			vecUtils.freeVector((*iterCentres)->data);
			vecUtils.freeVector((*iterCentres)->stddev);
			delete (*iterCentres);
			newClusterCentres->erase(iterCentres);
		}
		delete newClusterCentres;		
	}
	
	RSGISISODATACalcPixelClusterStdDevCalcImageVal::RSGISISODATACalcPixelClusterStdDevCalcImageVal(int numOutBands, vector<ClusterCentreISO*> *clusterCentres, unsigned int numImageBands) : RSGISCalcImageValue(numOutBands)
	{
		this->clusterCentres = clusterCentres;
		this->numImageBands = numImageBands;
	}
	
	void RSGISISODATACalcPixelClusterStdDevCalcImageVal::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		// Identify cluster within which point is associated with
		double minDistance = 0;
		ClusterCentreISO *minClusterCentre = NULL;
		bool first = true;
		double sum = 0;
		double distance = 0;
		vector<ClusterCentreISO*>::iterator iterCentres;
		for(iterCentres = clusterCentres->begin(); iterCentres != clusterCentres->end(); ++iterCentres)
		{
			sum = 0;
			for(int j = 0; j < numBands; ++j)
			{
				sum += (((*iterCentres)->data->vector[j] - bandValues[j])*((*iterCentres)->data->vector[j] - bandValues[j]));
			}
			distance = sum/numBands;
			
			if(first)
			{
				minDistance = distance;
				minClusterCentre = *iterCentres;
				first = false;
			}
			else if(distance < minDistance)
			{
				minDistance = distance;
				minClusterCentre = *iterCentres;
			}
		}
		
		//cout << "Pixel assocaited with cluster " << minClusterCentre->classID << endl;
		
		// add to sum for next centre
		for(int i = 0; i < numBands; ++i)
		{
			minClusterCentre->stddev->vector[i] += (minClusterCentre->data->vector[i] - bandValues[i])*(minClusterCentre->data->vector[i] - bandValues[i]);
		}
	}
	
	void RSGISISODATACalcPixelClusterStdDevCalcImageVal::reset(vector<ClusterCentreISO*> *clusterCentres)
	{
		this->clusterCentres = clusterCentres;
	}
	
	RSGISISODATACalcPixelClusterStdDevCalcImageVal::~RSGISISODATACalcPixelClusterStdDevCalcImageVal()
	{

	}
	
	RSGISApplyISODATAClassifierCalcImageVal::RSGISApplyISODATAClassifierCalcImageVal(int numOutBands,  vector<ClusterCentreISO*> *clusterCentres) : RSGISCalcImageValue(numOutBands)
	{
		this->clusterCentres = clusterCentres;
	}
	
	void RSGISApplyISODATAClassifierCalcImageVal::calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException)
	{
		double minDistance = 0;
		unsigned int minIdx = 0;
		bool first = true;
		double sum = 0;
		double distance = 0;
		vector<ClusterCentreISO*>::iterator iterCentres;
		for(iterCentres = clusterCentres->begin(); iterCentres != clusterCentres->end(); ++iterCentres)
		{
			sum = 0;
			for(int j = 0; j < numBands; ++j)
			{
				sum += (((*iterCentres)->data->vector[j] - bandValues[j])*((*iterCentres)->data->vector[j] - bandValues[j]));
			}
			distance = sum/numBands;
			
			if(first)
			{
				minDistance = distance;
				minIdx = (*iterCentres)->classID;
				first = false;
			}
			else if(distance < minDistance)
			{
				minDistance = distance;
				minIdx = (*iterCentres)->classID;
			}
		}
		output[0] = minIdx;
	}
	
	RSGISApplyISODATAClassifierCalcImageVal::~RSGISApplyISODATAClassifierCalcImageVal()
	{
		
	}
}}


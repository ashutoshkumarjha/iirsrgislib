/*
 *  RSGISPopRATWithStats.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/05/2014.
 *  Copyright 2014 RSGISLib.
 *
 *  RSGISLib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  RSGISLib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "RSGISPopRATWithStats.h"


namespace rsgis{namespace rastergis{

    
    RSGISPopRATWithStats::RSGISPopRATWithStats()
    {
        
    }
    
    void RSGISPopRATWithStats::populateRATWithBasicStats(GDALDataset *inputClumps, GDALDataset *inputValsImage, std::vector<RSGISBandAttStats*> *bandStats, unsigned int ratBand)throw(RSGISAttributeTableException)
    {
        try
        {
            if(ratBand == 0)
            {
                throw rsgis::RSGISAttributeTableException("RAT Band must be greater than zero.");
            }
            if(ratBand > inputClumps->GetRasterCount())
            {
                throw rsgis::RSGISAttributeTableException("RAT Band is larger than the number of bands within the image.");
            }
            RSGISRasterAttUtils attUtils;
            GDALRasterAttributeTable *rat = inputClumps->GetRasterBand(ratBand)->GetDefaultRAT();
            size_t numRows = rat->GetRowCount();
            double maxClumpID = 0.0;
            int nLastProgress = -1;
            inputClumps->GetRasterBand(ratBand)->ComputeStatistics(false, NULL, &maxClumpID, NULL, NULL, RSGISRATStatsTextProgress, &nLastProgress);
            if(maxClumpID > numRows)
            {
                numRows = boost::lexical_cast<size_t>(maxClumpID);
                rat->SetRowCount(numRows);
            }
            
            // Check whether a standard deviation is to be created (requires second iteration of image).
            bool calcMins = false;
            bool calcMaxs = false;
            bool calcMeans = false;
            bool calcStdDevs = false;
            bool calcSums = false;
            
            size_t numFeats2Calc = 0;
            size_t numStdDevs2Calc = 0;
            for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                if(((*iterBands)->calcStdDev) & (!(*iterBands)->calcMean))
                {
                    throw rsgis::RSGISAttributeTableException("If the standard deviation is required to be calculated then the mean must also be calculated.");
                }

                
                if((*iterBands)->calcMin)
                {
                    (*iterBands)->minFieldIdx = attUtils.findColumnIndexOrCreate(rat, (*iterBands)->minField, GFT_Real);
                    calcMins = true;
                    (*iterBands)->minLocalIdx = numFeats2Calc++;
                }
                if((*iterBands)->calcMax)
                {
                    (*iterBands)->maxFieldIdx = attUtils.findColumnIndexOrCreate(rat, (*iterBands)->maxField, GFT_Real);
                    calcMaxs = true;
                    (*iterBands)->maxLocalIdx = numFeats2Calc++;
                }
                if((*iterBands)->calcMean)
                {
                    (*iterBands)->meanFieldIdx = attUtils.findColumnIndexOrCreate(rat, (*iterBands)->meanField, GFT_Real);
                    calcMeans = true;
                    (*iterBands)->meanLocalIdx = numFeats2Calc++;
                }
                if((*iterBands)->calcStdDev)
                {
                    (*iterBands)->stdDevFieldIdx = attUtils.findColumnIndexOrCreate(rat, (*iterBands)->stdDevField, GFT_Real);
                    calcStdDevs = true;
                    (*iterBands)->stdDevLocalIdx = numStdDevs2Calc++;
                }
                if((*iterBands)->calcSum)
                {
                    (*iterBands)->sumFieldIdx = attUtils.findColumnIndexOrCreate(rat, (*iterBands)->sumField, GFT_Real);
                    calcSums = true;
                    (*iterBands)->sumLocalIdx = numFeats2Calc++;
                }
            }
            
            unsigned int histoIdx = attUtils.findColumnIndex(rat, "Histogram");
            
            bool *firstVal = new bool[numRows];
            for(int i = 0; i < numRows; ++i)
            {
                firstVal[i] = true;
            }
            
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = inputClumps;
            datasets[1] = inputValsImage;
            
            double **statsData = new double*[numRows];
            for(unsigned int i = 0; i < numRows; ++i)
            {
                statsData[i] = new double[numFeats2Calc];
                for(unsigned int j = 0; j < numFeats2Calc; ++j)
                {
                    statsData[i][j] = 0.0;
                }
            }
            
            RSGISCalcClusterPxlValueStats *calcImgValStats = new RSGISCalcClusterPxlValueStats(statsData, bandStats, firstVal, ratBand);
            rsgis::img::RSGISCalcImage calcImageStats(calcImgValStats);
            calcImageStats.calcImage(datasets, 1, 1);
            delete calcImgValStats;
            
            std::cout << "Writing Stats (";
            if(calcMins){std::cout << "Min, ";}
            if(calcMaxs){std::cout << "Max, ";}
            if(calcMeans){std::cout << "Mean, ";}
            if(calcSums){std::cout << "Sum";}
            std::cout << ") to Output RAT\n";
            
            size_t numBlocks = floor((double)numRows/(double)RAT_BLOCK_LENGTH);
            size_t rowsRemain = numRows - (numBlocks * RAT_BLOCK_LENGTH);
            
            double *dataBlock = new double[RAT_BLOCK_LENGTH];
            double *histDataBlock = new double[RAT_BLOCK_LENGTH];
            size_t startRow = 0;
            size_t rowID = 0;
            for(size_t i = 0; i < numBlocks; ++i)
            {
                rat->ValuesIO(GF_Read, histoIdx, startRow, RAT_BLOCK_LENGTH, histDataBlock);
                for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                {
                    if((*iterBands)->calcMin)
                    {
                        rowID = startRow;
                        for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                        {
                            if(histDataBlock[j] > 0)
                            {
                                dataBlock[j] = statsData[rowID][(*iterBands)->minLocalIdx];
                            }
                            else
                            {
                                dataBlock[j] = 0.0;
                            }
                            ++rowID;
                        }
                        rat->ValuesIO(GF_Write, (*iterBands)->minFieldIdx, startRow, RAT_BLOCK_LENGTH, dataBlock);
                    }
                    
                    if((*iterBands)->calcMax)
                    {
                        rowID = startRow;
                        for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                        {
                            if(histDataBlock[j] > 0)
                            {
                                dataBlock[j] = statsData[rowID][(*iterBands)->maxLocalIdx];
                            }
                            else
                            {
                                dataBlock[j] = 0.0;
                            }
                            ++rowID;
                        }
                        rat->ValuesIO(GF_Write, (*iterBands)->maxFieldIdx, startRow, RAT_BLOCK_LENGTH, dataBlock);
                    }
                    
                    if((*iterBands)->calcMean)
                    {
                        rowID = startRow;
                        for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                        {
                            if(histDataBlock[j] > 0)
                            {
                                statsData[rowID][(*iterBands)->meanLocalIdx] = statsData[rowID][(*iterBands)->meanLocalIdx] / histDataBlock[j];
                                dataBlock[j] = statsData[rowID][(*iterBands)->meanLocalIdx];
                            }
                            else
                            {
                                dataBlock[j] = 0.0;
                            }
                            ++rowID;
                        }
                        rat->ValuesIO(GF_Write, (*iterBands)->meanFieldIdx, startRow, RAT_BLOCK_LENGTH, dataBlock);
                    }
                    
                    if((*iterBands)->calcSum)
                    {
                        rowID = startRow;
                        for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                        {
                            if(histDataBlock[j] > 0)
                            {
                                dataBlock[j] = statsData[rowID][(*iterBands)->sumLocalIdx];
                            }
                            else
                            {
                                dataBlock[j] = 0.0;
                            }
                            ++rowID;
                        }
                        rat->ValuesIO(GF_Write, (*iterBands)->sumFieldIdx, startRow, RAT_BLOCK_LENGTH, dataBlock);
                    }
                }
                startRow += RAT_BLOCK_LENGTH;
            }
            if(rowsRemain > 0)
            {
                rat->ValuesIO(GF_Read, histoIdx, startRow, rowsRemain, histDataBlock);
                for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                {
                    if((*iterBands)->calcMin)
                    {
                        rowID = startRow;
                        for(size_t j = 0; j < rowsRemain; ++j)
                        {
                            if(histDataBlock[j] > 0)
                            {
                                dataBlock[j] = statsData[rowID][(*iterBands)->minLocalIdx];
                            }
                            else
                            {
                                dataBlock[j] = 0.0;
                            }
                            ++rowID;
                        }
                        rat->ValuesIO(GF_Write, (*iterBands)->minFieldIdx, startRow, rowsRemain, dataBlock);
                    }
                    
                    if((*iterBands)->calcMax)
                    {
                        rowID = startRow;
                        for(size_t j = 0; j < rowsRemain; ++j)
                        {
                            if(histDataBlock[j] > 0)
                            {
                                dataBlock[j] = statsData[rowID][(*iterBands)->maxLocalIdx];
                            }
                            else
                            {
                                dataBlock[j] = 0.0;
                            }
                            ++rowID;
                        }
                        rat->ValuesIO(GF_Write, (*iterBands)->maxFieldIdx, startRow, rowsRemain, dataBlock);
                    }
                    
                    if((*iterBands)->calcMean)
                    {
                        rowID = startRow;
                        for(size_t j = 0; j < rowsRemain; ++j)
                        {
                            if(histDataBlock[j] > 0)
                            {
                                statsData[rowID][(*iterBands)->meanLocalIdx] = statsData[rowID][(*iterBands)->meanLocalIdx] / histDataBlock[j];
                                dataBlock[j] = statsData[rowID][(*iterBands)->meanLocalIdx];
                            }
                            else
                            {
                                dataBlock[j] = 0.0;
                            }
                            ++rowID;
                        }
                        rat->ValuesIO(GF_Write, (*iterBands)->meanFieldIdx, startRow, rowsRemain, dataBlock);
                    }
                    
                    if((*iterBands)->calcSum)
                    {
                        rowID = startRow;
                        for(size_t j = 0; j < rowsRemain; ++j)
                        {
                            if(histDataBlock[j] > 0)
                            {
                                dataBlock[j] = statsData[rowID][(*iterBands)->sumLocalIdx];
                            }
                            else
                            {
                                dataBlock[j] = 0.0;
                            }
                            ++rowID;
                        }
                        rat->ValuesIO(GF_Write, (*iterBands)->sumFieldIdx, startRow, rowsRemain, dataBlock);
                    }
                }
            }
            
            if(calcStdDevs)
            {
                for(int i = 0; i < numRows; ++i)
                {
                    firstVal[i] = true;
                }

                // Get Image Values
                double **stdDevData = new double*[numRows];
                for(unsigned int i = 0; i < numRows; ++i)
                {
                    stdDevData[i] = new double[numStdDevs2Calc];
                    for(unsigned int j = 0; j < numStdDevs2Calc; ++j)
                    {
                        stdDevData[i][j] = 0.0;
                    }
                }
                RSGISCalcClusterPxlValueStdDev *calcImgValStdDev = new RSGISCalcClusterPxlValueStdDev(stdDevData, statsData, bandStats, firstVal, ratBand);
                rsgis::img::RSGISCalcImage calcImageStdDev(calcImgValStdDev);
                calcImageStdDev.calcImage(datasets, 1, 1);
                delete calcImgValStdDev;

                std::cout << "Writing Standard Deviation Stats to Output RAT\n";
                startRow = 0;
                for(size_t i = 0; i < numBlocks; ++i)
                {
                    rat->ValuesIO(GF_Read, histoIdx, startRow, RAT_BLOCK_LENGTH, histDataBlock);
                    for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                    {
                        if((*iterBands)->calcStdDev)
                        {
                            rowID = startRow;
                            for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                            {
                                if(histDataBlock[j] > 0)
                                {
                                    stdDevData[rowID][(*iterBands)->stdDevLocalIdx] = sqrt(stdDevData[rowID][(*iterBands)->stdDevLocalIdx] / histDataBlock[j]);
                                    dataBlock[j] = stdDevData[rowID][(*iterBands)->stdDevLocalIdx];
                                }
                                else
                                {
                                    dataBlock[j] = 0.0;
                                }
                                ++rowID;
                            }
                            rat->ValuesIO(GF_Write, (*iterBands)->stdDevFieldIdx, startRow, RAT_BLOCK_LENGTH, dataBlock);
                        }
                        
                    }
                    startRow += RAT_BLOCK_LENGTH;
                }
                if(rowsRemain > 0)
                {
                    rat->ValuesIO(GF_Read, histoIdx, startRow, rowsRemain, histDataBlock);
                    for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                    {
                        if((*iterBands)->calcStdDev)
                        {
                            rowID = startRow;
                            for(size_t j = 0; j < rowsRemain; ++j)
                            {
                                if(histDataBlock[j] > 0)
                                {
                                    stdDevData[rowID][(*iterBands)->stdDevLocalIdx] = sqrt(stdDevData[rowID][(*iterBands)->stdDevLocalIdx] / histDataBlock[j]);
                                    dataBlock[j] = stdDevData[rowID][(*iterBands)->stdDevLocalIdx];
                                }
                                else
                                {
                                    dataBlock[j] = 0.0;
                                }
                                ++rowID;
                            }
                            rat->ValuesIO(GF_Write, (*iterBands)->stdDevFieldIdx, startRow, rowsRemain, dataBlock);
                        }
                    }
                }
                
                for(unsigned int i = 0; i < numRows; ++i)
                {
                    delete[] stdDevData[i];
                }
                delete[] stdDevData;
            }
            
            for(unsigned int i = 0; i < numRows; ++i)
            {
                delete[] statsData[i];
            }
            delete[] statsData;
            delete[] firstVal;
            
            
            delete[] dataBlock;
            delete[] histDataBlock;
            delete[] datasets;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    void RSGISPopRATWithStats::populateRATWithPercentileStats(GDALDataset *inputClumps, GDALDataset *inputValsImage, unsigned int band, std::vector<RSGISBandAttPercentiles*> *bandStats, unsigned int ratBand, unsigned int numHistBins)throw(RSGISAttributeTableException)
    {
        try
        {
            if(ratBand == 0)
            {
                throw rsgis::RSGISAttributeTableException("RAT Band must be greater than zero.");
            }
            if(ratBand > inputClumps->GetRasterCount())
            {
                throw rsgis::RSGISAttributeTableException("RAT Band is larger than the number of bands within the image.");
            }
            
            if(band == 0)
            {
                throw rsgis::RSGISAttributeTableException("Values image band must be greater than zero.");
            }
            if(band > inputValsImage->GetRasterCount())
            {
                throw rsgis::RSGISAttributeTableException("Values image band is larger than the number of bands within the image.");
            }
            
            RSGISRasterAttUtils attUtils;
            GDALRasterAttributeTable *rat = inputClumps->GetRasterBand(ratBand)->GetDefaultRAT();
            size_t numRows = rat->GetRowCount();
            double maxClumpID = 0.0;
            int nLastProgress = -1;
            inputClumps->GetRasterBand(ratBand)->ComputeStatistics(false, NULL, &maxClumpID, NULL, NULL, RSGISRATStatsTextProgress, &nLastProgress);
            if(maxClumpID > numRows)
            {
                numRows = boost::lexical_cast<size_t>(maxClumpID);
                rat->SetRowCount(numRows);
            }
            double imageValMin = 0.0;
            double imageValMax = 0.0;
            nLastProgress = -1;
            inputValsImage->GetRasterBand(band)->ComputeStatistics(false, &imageValMin, &imageValMax, NULL, NULL, RSGISRATStatsTextProgress, &nLastProgress);
            
            std::cout << "Image Min = " << imageValMin << " Image Max = " << imageValMax << std::endl;
            
            double imageValsRange = imageValMax - imageValMin;
            double binWidth = imageValsRange / ((float)numHistBins);
            std::cout << "Image Range = " << imageValsRange << " Bin Width = " << binWidth << " Number of Bins = " << numHistBins << std::endl;
            
            double *binBounds = new double[numHistBins+1];
            for(unsigned int i = 0; i < numHistBins; ++i)
            {
                binBounds[i] = (imageValMin-binWidth) + (i * binWidth);
            }
            binBounds[numHistBins] = imageValMin + (numHistBins * binWidth);
            
            for(std::vector<rsgis::rastergis::RSGISBandAttPercentiles*>::iterator iterFeat = bandStats->begin(); iterFeat != bandStats->end(); ++iterFeat)
            {
                (*iterFeat)->fieldIdx = attUtils.findColumnIndexOrCreate(rat, (*iterFeat)->fieldName, GFT_Real);
            }
            
            unsigned int histoIdx = attUtils.findColumnIndex(rat, "Histogram");
            
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = inputClumps;
            datasets[1] = inputValsImage;
            
            unsigned int **clumpHistData = new unsigned int*[numRows];
            for(unsigned int i = 0; i < numRows; ++i)
            {
                clumpHistData[i] = new unsigned int[numHistBins];
                for(unsigned int j = 0; j < numHistBins; ++j)
                {
                    clumpHistData[i][j] = 0.0;
                }
            }
            
            RSGISCalcClusterPxlValueHistograms *calcImgValHists = new RSGISCalcClusterPxlValueHistograms(clumpHistData, binBounds, numHistBins, ratBand, band);
            rsgis::img::RSGISCalcImage calcImageStats(calcImgValHists);
            calcImageStats.calcImage(datasets, 1, 1);
            delete calcImgValHists;
            
            std::cout << "Writing Percentile Values to Output RAT\n";
            size_t numBlocks = floor((double)numRows/(double)RAT_BLOCK_LENGTH);
            size_t rowsRemain = numRows - (numBlocks * RAT_BLOCK_LENGTH);
            
            rsgis::math::RSGISMathsUtils mathUtils;
            double *dataBlock = new double[RAT_BLOCK_LENGTH];
            double *histDataBlock = new double[RAT_BLOCK_LENGTH];
            size_t startRow = 0;
            size_t rowID = 0;
            for(size_t i = 0; i < numBlocks; ++i)
            {
                rat->ValuesIO(GF_Read, histoIdx, startRow, RAT_BLOCK_LENGTH, histDataBlock);
                for(std::vector<rsgis::rastergis::RSGISBandAttPercentiles*>::iterator iterFeat = bandStats->begin(); iterFeat != bandStats->end(); ++iterFeat)
                {
                    rowID = startRow;
                    for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                    {
                        if(histDataBlock[j] > 0)
                        {
                            dataBlock[j] = mathUtils.calcPercentile((*iterFeat)->percentile, binBounds, binWidth, numHistBins, clumpHistData[rowID]); // calc percentile from the histogram...
                        }
                        else
                        {
                            dataBlock[j] = 0.0;
                        }
                        ++rowID;
                    }
                    rat->ValuesIO(GF_Write, (*iterFeat)->fieldIdx, startRow, RAT_BLOCK_LENGTH, dataBlock);
                }
                startRow += RAT_BLOCK_LENGTH;
            }
            if(rowsRemain > 0)
            {
                rat->ValuesIO(GF_Read, histoIdx, startRow, rowsRemain, histDataBlock);
                for(std::vector<rsgis::rastergis::RSGISBandAttPercentiles*>::iterator iterFeat = bandStats->begin(); iterFeat != bandStats->end(); ++iterFeat)
                {
                    rowID = startRow;
                    for(size_t j = 0; j < rowsRemain; ++j)
                    {
                        if(histDataBlock[j] > 0)
                        {
                            dataBlock[j] = mathUtils.calcPercentile((*iterFeat)->percentile, binBounds, binWidth, numHistBins, clumpHistData[rowID]); // calc percentile from the histogram...
                        }
                        else
                        {
                            dataBlock[j] = 0.0;
                        }
                        ++rowID;
                    }
                    rat->ValuesIO(GF_Write, (*iterFeat)->fieldIdx, startRow, rowsRemain, dataBlock);
                }
            }
            
            
            for(unsigned int i = 0; i < numRows; ++i)
            {
                delete[] clumpHistData[i];
            }
            delete[] clumpHistData;
            
            
            delete[] dataBlock;
            delete[] histDataBlock;
            delete[] datasets;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }

    void RSGISPopRATWithStats::populateRATWithMeanLitStats(GDALDataset *inputClumps, GDALDataset *inputValsImage, GDALDataset *inputMeanLitImage, unsigned int meanLitBand, std::string meanLitCol, std::string pxlCountCol, std::vector<RSGISBandAttStats*> *bandStats, unsigned int ratBand)throw(RSGISAttributeTableException)
    {
        try
        {
            if(ratBand == 0)
            {
                throw rsgis::RSGISAttributeTableException("RAT Band must be greater than zero.");
            }
            if(ratBand > inputClumps->GetRasterCount())
            {
                throw rsgis::RSGISAttributeTableException("RAT Band is larger than the number of bands within the image.");
            }
            RSGISRasterAttUtils attUtils;
            GDALRasterAttributeTable *rat = inputClumps->GetRasterBand(ratBand)->GetDefaultRAT();
            size_t numRows = rat->GetRowCount();
            double maxClumpID = 0.0;
            int nLastProgress = -1;
            inputClumps->GetRasterBand(ratBand)->ComputeStatistics(false, NULL, &maxClumpID, NULL, NULL, RSGISRATStatsTextProgress, &nLastProgress);
            if(maxClumpID > numRows)
            {
                numRows = boost::lexical_cast<size_t>(maxClumpID);
                rat->SetRowCount(numRows);
            }
            
            // Check whether a standard deviation is to be created (requires second iteration of image).
            bool calcMins = false;
            bool calcMaxs = false;
            bool calcMeans = false;
            bool calcStdDevs = false;
            bool calcSums = false;
            
            size_t numFeats2Calc = 0;
            size_t numStdDevs2Calc = 0;
            for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                if(((*iterBands)->calcStdDev) & (!(*iterBands)->calcMean))
                {
                    throw rsgis::RSGISAttributeTableException("If the standard deviation is required to be calculated then the mean must also be calculated.");
                }
                
                
                if((*iterBands)->calcMin)
                {
                    (*iterBands)->minFieldIdx = attUtils.findColumnIndexOrCreate(rat, (*iterBands)->minField, GFT_Real);
                    calcMins = true;
                    (*iterBands)->minLocalIdx = numFeats2Calc++;
                }
                if((*iterBands)->calcMax)
                {
                    (*iterBands)->maxFieldIdx = attUtils.findColumnIndexOrCreate(rat, (*iterBands)->maxField, GFT_Real);
                    calcMaxs = true;
                    (*iterBands)->maxLocalIdx = numFeats2Calc++;
                }
                if((*iterBands)->calcMean)
                {
                    (*iterBands)->meanFieldIdx = attUtils.findColumnIndexOrCreate(rat, (*iterBands)->meanField, GFT_Real);
                    calcMeans = true;
                    (*iterBands)->meanLocalIdx = numFeats2Calc++;
                }
                if((*iterBands)->calcStdDev)
                {
                    (*iterBands)->stdDevFieldIdx = attUtils.findColumnIndexOrCreate(rat, (*iterBands)->stdDevField, GFT_Real);
                    calcStdDevs = true;
                    (*iterBands)->stdDevLocalIdx = numStdDevs2Calc++;
                }
                if((*iterBands)->calcSum)
                {
                    (*iterBands)->sumFieldIdx = attUtils.findColumnIndexOrCreate(rat, (*iterBands)->sumField, GFT_Real);
                    calcSums = true;
                    (*iterBands)->sumLocalIdx = numFeats2Calc++;
                }
            }
            
            unsigned int pxlCountIdx = attUtils.findColumnIndexOrCreate(rat, pxlCountCol, GFT_Real);
            
            size_t meanLitColNumVals = 0;
            double *meanLitColVals = attUtils.readDoubleColumn(rat, meanLitCol, &meanLitColNumVals);
            if(meanLitColNumVals != numRows)
            {
                throw rsgis::RSGISAttributeTableException("The meanLit column does not have the same number of values as the RAT - werid!");
            }
            
            bool *firstVal = new bool[numRows];
            for(int i = 0; i < numRows; ++i)
            {
                firstVal[i] = true;
            }
            
            GDALDataset **datasets = new GDALDataset*[3];
            datasets[0] = inputClumps;
            datasets[1] = inputValsImage;
            datasets[2] = inputMeanLitImage;
            
            unsigned int meanLitBandArrIdx = (inputValsImage->GetRasterCount() + meanLitBand) - 1;
            
            double *pxlCount = new double[numRows];
            double **statsData = new double*[numRows];
            for(unsigned int i = 0; i < numRows; ++i)
            {
                statsData[i] = new double[numFeats2Calc];
                for(unsigned int j = 0; j < numFeats2Calc; ++j)
                {
                    statsData[i][j] = 0.0;
                }
                pxlCount[i] = 0;
            }
            
            RSGISCalcClusterPxlValueStatsMeanLit *calcImgValStats = new RSGISCalcClusterPxlValueStatsMeanLit(statsData, pxlCount, meanLitColVals, meanLitBandArrIdx, bandStats, firstVal, ratBand);
            rsgis::img::RSGISCalcImage calcImageStats(calcImgValStats);
            calcImageStats.calcImage(datasets, 1, 2);
            delete calcImgValStats;
            
            std::cout << "Writing Stats (";
            if(calcMins){std::cout << "Min, ";}
            if(calcMaxs){std::cout << "Max, ";}
            if(calcMeans){std::cout << "Mean, ";}
            if(calcSums){std::cout << "Sum";}
            std::cout << ") to Output RAT\n";
            
            size_t numBlocks = floor((double)numRows/(double)RAT_BLOCK_LENGTH);
            size_t rowsRemain = numRows - (numBlocks * RAT_BLOCK_LENGTH);
            
            double *dataBlock = new double[RAT_BLOCK_LENGTH];
            size_t startRow = 0;
            size_t rowID = 0;
            for(size_t i = 0; i < numBlocks; ++i)
            {
                for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                {
                    if((*iterBands)->calcMin)
                    {
                        rowID = startRow;
                        for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                        {
                            if(pxlCount[rowID] > 0)
                            {
                                dataBlock[j] = statsData[rowID][(*iterBands)->minLocalIdx];
                            }
                            else
                            {
                                dataBlock[j] = 0.0;
                            }
                            ++rowID;
                        }
                        rat->ValuesIO(GF_Write, (*iterBands)->minFieldIdx, startRow, RAT_BLOCK_LENGTH, dataBlock);
                    }
                    
                    if((*iterBands)->calcMax)
                    {
                        rowID = startRow;
                        for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                        {
                            if(pxlCount[rowID] > 0)
                            {
                                dataBlock[j] = statsData[rowID][(*iterBands)->maxLocalIdx];
                            }
                            else
                            {
                                dataBlock[j] = 0.0;
                            }
                            ++rowID;
                        }
                        rat->ValuesIO(GF_Write, (*iterBands)->maxFieldIdx, startRow, RAT_BLOCK_LENGTH, dataBlock);
                    }
                    
                    if((*iterBands)->calcMean)
                    {
                        rowID = startRow;
                        for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                        {
                            if(pxlCount[rowID] > 0)
                            {
                                statsData[rowID][(*iterBands)->meanLocalIdx] = statsData[rowID][(*iterBands)->meanLocalIdx] / pxlCount[rowID];
                                dataBlock[j] = statsData[rowID][(*iterBands)->meanLocalIdx];
                            }
                            else
                            {
                                dataBlock[j] = 0.0;
                            }
                            ++rowID;
                        }
                        rat->ValuesIO(GF_Write, (*iterBands)->meanFieldIdx, startRow, RAT_BLOCK_LENGTH, dataBlock);
                    }
                    
                    if((*iterBands)->calcSum)
                    {
                        rowID = startRow;
                        for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                        {
                            if(pxlCount[rowID] > 0)
                            {
                                dataBlock[j] = statsData[rowID][(*iterBands)->sumLocalIdx];
                            }
                            else
                            {
                                dataBlock[j] = 0.0;
                            }
                            ++rowID;
                        }
                        rat->ValuesIO(GF_Write, (*iterBands)->sumFieldIdx, startRow, RAT_BLOCK_LENGTH, dataBlock);
                    }
                }
                startRow += RAT_BLOCK_LENGTH;
            }
            if(rowsRemain > 0)
            {
                for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                {
                    if((*iterBands)->calcMin)
                    {
                        rowID = startRow;
                        for(size_t j = 0; j < rowsRemain; ++j)
                        {
                            if(pxlCount[rowID] > 0)
                            {
                                dataBlock[j] = statsData[rowID][(*iterBands)->minLocalIdx];
                            }
                            else
                            {
                                dataBlock[j] = 0.0;
                            }
                            ++rowID;
                        }
                        rat->ValuesIO(GF_Write, (*iterBands)->minFieldIdx, startRow, rowsRemain, dataBlock);
                    }
                    
                    if((*iterBands)->calcMax)
                    {
                        rowID = startRow;
                        for(size_t j = 0; j < rowsRemain; ++j)
                        {
                            if(pxlCount[rowID] > 0)
                            {
                                dataBlock[j] = statsData[rowID][(*iterBands)->maxLocalIdx];
                            }
                            else
                            {
                                dataBlock[j] = 0.0;
                            }
                            ++rowID;
                        }
                        rat->ValuesIO(GF_Write, (*iterBands)->maxFieldIdx, startRow, rowsRemain, dataBlock);
                    }
                    
                    if((*iterBands)->calcMean)
                    {
                        rowID = startRow;
                        for(size_t j = 0; j < rowsRemain; ++j)
                        {
                            if(pxlCount[rowID] > 0)
                            {
                                statsData[rowID][(*iterBands)->meanLocalIdx] = statsData[rowID][(*iterBands)->meanLocalIdx] / pxlCount[rowID];
                                dataBlock[j] = statsData[rowID][(*iterBands)->meanLocalIdx];
                            }
                            else
                            {
                                dataBlock[j] = 0.0;
                            }
                            ++rowID;
                        }
                        rat->ValuesIO(GF_Write, (*iterBands)->meanFieldIdx, startRow, rowsRemain, dataBlock);
                    }
                    
                    if((*iterBands)->calcSum)
                    {
                        rowID = startRow;
                        for(size_t j = 0; j < rowsRemain; ++j)
                        {
                            if(pxlCount[rowID] > 0)
                            {
                                dataBlock[j] = statsData[rowID][(*iterBands)->sumLocalIdx];
                            }
                            else
                            {
                                dataBlock[j] = 0.0;
                            }
                            ++rowID;
                        }
                        rat->ValuesIO(GF_Write, (*iterBands)->sumFieldIdx, startRow, rowsRemain, dataBlock);
                    }
                }
            }
            
            if(calcStdDevs)
            {
                for(int i = 0; i < numRows; ++i)
                {
                    firstVal[i] = true;
                }
                
                // Get Image Values
                double **stdDevData = new double*[numRows];
                for(unsigned int i = 0; i < numRows; ++i)
                {
                    stdDevData[i] = new double[numStdDevs2Calc];
                    for(unsigned int j = 0; j < numStdDevs2Calc; ++j)
                    {
                        stdDevData[i][j] = 0.0;
                    }
                }
                
                RSGISCalcClusterPxlValueStdDevMeanLit *calcImgValStdDev = new RSGISCalcClusterPxlValueStdDevMeanLit(stdDevData, statsData, pxlCount, meanLitColVals, meanLitBandArrIdx, bandStats, firstVal, ratBand);
                rsgis::img::RSGISCalcImage calcImageStdDev(calcImgValStdDev);
                calcImageStdDev.calcImage(datasets, 1, 2);
                delete calcImgValStdDev;
                
                std::cout << "Writing Standard Deviation Stats to Output RAT\n";
                startRow = 0;
                for(size_t i = 0; i < numBlocks; ++i)
                {
                    for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                    {
                        if((*iterBands)->calcStdDev)
                        {
                            rowID = startRow;
                            for(size_t j = 0; j < RAT_BLOCK_LENGTH; ++j)
                            {
                                if(pxlCount[rowID] > 0)
                                {
                                    stdDevData[rowID][(*iterBands)->stdDevLocalIdx] = sqrt(stdDevData[rowID][(*iterBands)->stdDevLocalIdx] / pxlCount[rowID]);
                                    dataBlock[j] = stdDevData[rowID][(*iterBands)->stdDevLocalIdx];
                                }
                                else
                                {
                                    dataBlock[j] = 0.0;
                                }
                                ++rowID;
                            }
                            rat->ValuesIO(GF_Write, (*iterBands)->stdDevFieldIdx, startRow, RAT_BLOCK_LENGTH, dataBlock);
                        }
                        
                    }
                    startRow += RAT_BLOCK_LENGTH;
                }
                if(rowsRemain > 0)
                {
                    for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                    {
                        if((*iterBands)->calcStdDev)
                        {
                            rowID = startRow;
                            for(size_t j = 0; j < rowsRemain; ++j)
                            {
                                if(pxlCount[rowID] > 0)
                                {
                                    stdDevData[rowID][(*iterBands)->stdDevLocalIdx] = sqrt(stdDevData[rowID][(*iterBands)->stdDevLocalIdx] / pxlCount[rowID]);
                                    dataBlock[j] = stdDevData[rowID][(*iterBands)->stdDevLocalIdx];
                                }
                                else
                                {
                                    dataBlock[j] = 0.0;
                                }
                                ++rowID;
                            }
                            rat->ValuesIO(GF_Write, (*iterBands)->stdDevFieldIdx, startRow, rowsRemain, dataBlock);
                        }
                    }
                }
                
                for(unsigned int i = 0; i < numRows; ++i)
                {
                    delete[] stdDevData[i];
                }
                delete[] stdDevData;
            }
            
            rat->ValuesIO(GF_Write, pxlCountIdx, 0, numRows, pxlCount);
            
            for(unsigned int i = 0; i < numRows; ++i)
            {
                delete[] statsData[i];
            }
            delete[] statsData;
            delete[] firstVal;
            
            delete[] meanLitColVals;
            delete[] dataBlock;
            delete[] pxlCount;
            delete[] datasets;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISAttributeTableException(e.what());
        }
    }
    
    
    RSGISPopRATWithStats::~RSGISPopRATWithStats()
    {
        
    }
    
    
    RSGISCalcClusterPxlValueStats::RSGISCalcClusterPxlValueStats(double **statsData, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats, bool *firstVal, unsigned int ratBand) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->statsData = statsData;
        this->bandStats = bandStats;
        this->firstVal = firstVal;
        this->ratBand = ratBand;
    }
    
    void RSGISCalcClusterPxlValueStats::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException)
    {
        if(intBandValues[ratBand-1] > 0)
        {
            unsigned long fid = intBandValues[ratBand-1];

            for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                if((boost::math::isfinite)(floatBandValues[(*iterBands)->band-1]))
                {
                    if((*iterBands)->calcMin)
                    {
                        if(firstVal[fid])
                        {
                            statsData[fid][(*iterBands)->minLocalIdx] = floatBandValues[(*iterBands)->band-1];
                        }
                        else if(floatBandValues[(*iterBands)->band-1] < statsData[fid][(*iterBands)->minLocalIdx])
                        {
                            statsData[fid][(*iterBands)->minLocalIdx] = floatBandValues[(*iterBands)->band-1];
                        }
                    }
                    
                    if((*iterBands)->calcMax)
                    {
                        if(firstVal[fid])
                        {
                            statsData[fid][(*iterBands)->maxLocalIdx] = floatBandValues[(*iterBands)->band-1];
                        }
                        else if(floatBandValues[(*iterBands)->band-1] > statsData[fid][(*iterBands)->maxLocalIdx])
                        {
                            statsData[fid][(*iterBands)->maxLocalIdx] = floatBandValues[(*iterBands)->band-1];
                        }
                    }
                    
                    if((*iterBands)->calcMean)
                    {
                        if(firstVal[fid])
                        {
                            statsData[fid][(*iterBands)->meanLocalIdx] = floatBandValues[(*iterBands)->band-1];
                        }
                        else
                        {
                            statsData[fid][(*iterBands)->meanLocalIdx] += floatBandValues[(*iterBands)->band-1];
                        }
                    }
                    
                    if((*iterBands)->calcSum)
                    {
                        if(firstVal[fid])
                        {
                            statsData[fid][(*iterBands)->sumLocalIdx] = floatBandValues[(*iterBands)->band-1];
                        }
                        else
                        {
                            statsData[fid][(*iterBands)->sumLocalIdx] += floatBandValues[(*iterBands)->band-1];
                        }
                    }
                }
            }
            
            if(firstVal[fid])
            {
                firstVal[fid] = false;
            }
        }
    }
    
    RSGISCalcClusterPxlValueStats::~RSGISCalcClusterPxlValueStats()
    {
        
    }
    
    RSGISCalcClusterPxlValueStdDev::RSGISCalcClusterPxlValueStdDev(double **stdDevData, double **statsData, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats, bool *firstVal, unsigned int ratBand) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->stdDevData = stdDevData;
        this->statsData = statsData;
        this->bandStats = bandStats;
        this->firstVal = firstVal;
        this->ratBand = ratBand;
    }
    
    void RSGISCalcClusterPxlValueStdDev::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException)
    {
        if(intBandValues[ratBand-1] > 0)
        {
            unsigned long fid = intBandValues[ratBand-1];
            for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
            {
                if((boost::math::isfinite)(floatBandValues[(*iterBands)->band-1]))
                {
                    if((*iterBands)->calcStdDev)
                    {
                        double stdDevComp = pow(((double)(floatBandValues[(*iterBands)->band-1] - statsData[fid][(*iterBands)->meanLocalIdx])), 2.0);
                        if(firstVal[fid])
                        {
                            stdDevData[fid][(*iterBands)->stdDevLocalIdx] = stdDevComp;
                        }
                        else
                        {
                            stdDevData[fid][(*iterBands)->stdDevLocalIdx] += stdDevComp;
                        }
                    }
                }
            }
            
            if(firstVal[fid])
            {
                firstVal[fid] = false;
            }
        }
    }
    
    RSGISCalcClusterPxlValueStdDev::~RSGISCalcClusterPxlValueStdDev()
    {
        
    }
    
    
    
    
    RSGISCalcClusterPxlValueHistograms::RSGISCalcClusterPxlValueHistograms(unsigned int **clumpHistData, double *binBounds, unsigned int numBins, unsigned int ratBand, unsigned int imgBand): rsgis::img::RSGISCalcImageValue(0)
    {
        this->clumpHistData = clumpHistData;
        this->binBounds = binBounds;
        this->numBins = numBins;
        this->ratBand = ratBand;
        this->imgBand = imgBand;
    }

    void RSGISCalcClusterPxlValueHistograms::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException)
    {
        if(intBandValues[ratBand-1] > 0)
        {
            size_t fid = boost::lexical_cast<size_t>(intBandValues[ratBand-1]);
            if((boost::math::isfinite)(floatBandValues[imgBand-1]))
            {
                unsigned int binIdx = 0;
                bool foundBinIdx = false;
                for(unsigned int i = 0; i < numBins; ++i)
                {
                    if((floatBandValues[imgBand-1] >= binBounds[i]) & (floatBandValues[imgBand-1] < binBounds[i+1]))
                    {
                        binIdx = i;
                        foundBinIdx = true;
                        break;
                    }
                    else if((i==numBins-1) & ((floatBandValues[imgBand-1] >= binBounds[i])))
                    {
                        binIdx = i;
                        foundBinIdx = true;
                        break;
                    }
                }
                if(!foundBinIdx)
                {
                    std::cout << std::endl;
                    for(unsigned int i = 0; i < numBins; ++i)
                    {
                        std::cout << "Bin [" << i << "] : " << binBounds[i] << " - " << binBounds[i+1] << std::endl;
                    }
                    
                    std::cout << "The pixel value which has caused the problem is " << floatBandValues[imgBand-1] << std::endl;
                    throw rsgis::img::RSGISImageCalcException("The image pixel value was not found within the histogram range specified - either too big or too small.");
                }
                
                ++clumpHistData[fid][binIdx];
            }
        }
    }
		
    RSGISCalcClusterPxlValueHistograms::~RSGISCalcClusterPxlValueHistograms()
    {
        
    }
    
    
    
    RSGISCalcClusterPxlValueStatsMeanLit::RSGISCalcClusterPxlValueStatsMeanLit(double **statsData, double *pxlCount, double *meanLitColVals, unsigned int meanLitBandArrIdx, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats, bool *firstVal, unsigned int ratBand) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->statsData = statsData;
        this->pxlCount = pxlCount;
        this->meanLitColVals = meanLitColVals;
        this->meanLitBandArrIdx = meanLitBandArrIdx;
        this->bandStats = bandStats;
        this->firstVal = firstVal;
        this->ratBand = ratBand;
    }
    
    void RSGISCalcClusterPxlValueStatsMeanLit::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException)
    {
        if(intBandValues[ratBand-1] > 0)
        {
            unsigned long fid = intBandValues[ratBand-1];
            
            if(floatBandValues[meanLitBandArrIdx] >= this->meanLitColVals[fid])
            {
                for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                {
                    if((boost::math::isfinite)(floatBandValues[(*iterBands)->band-1]))
                    {
                        if((*iterBands)->calcMin)
                        {
                            if(firstVal[fid])
                            {
                                statsData[fid][(*iterBands)->minLocalIdx] = floatBandValues[(*iterBands)->band-1];
                            }
                            else if(floatBandValues[(*iterBands)->band-1] < statsData[fid][(*iterBands)->minLocalIdx])
                            {
                                statsData[fid][(*iterBands)->minLocalIdx] = floatBandValues[(*iterBands)->band-1];
                            }
                        }
                        
                        if((*iterBands)->calcMax)
                        {
                            if(firstVal[fid])
                            {
                                statsData[fid][(*iterBands)->maxLocalIdx] = floatBandValues[(*iterBands)->band-1];
                            }
                            else if(floatBandValues[(*iterBands)->band-1] > statsData[fid][(*iterBands)->maxLocalIdx])
                            {
                                statsData[fid][(*iterBands)->maxLocalIdx] = floatBandValues[(*iterBands)->band-1];
                            }
                        }
                        
                        if((*iterBands)->calcMean)
                        {
                            if(firstVal[fid])
                            {
                                statsData[fid][(*iterBands)->meanLocalIdx] = floatBandValues[(*iterBands)->band-1];
                            }
                            else
                            {
                                statsData[fid][(*iterBands)->meanLocalIdx] += floatBandValues[(*iterBands)->band-1];
                            }
                        }
                        
                        if((*iterBands)->calcSum)
                        {
                            if(firstVal[fid])
                            {
                                statsData[fid][(*iterBands)->sumLocalIdx] = floatBandValues[(*iterBands)->band-1];
                            }
                            else
                            {
                                statsData[fid][(*iterBands)->sumLocalIdx] += floatBandValues[(*iterBands)->band-1];
                            }
                        }
                    }
                }
                
                if(firstVal[fid])
                {
                    firstVal[fid] = false;
                }
                ++pxlCount[fid];
            }
        }
    }
    
    RSGISCalcClusterPxlValueStatsMeanLit::~RSGISCalcClusterPxlValueStatsMeanLit()
    {
        
    }
    
    RSGISCalcClusterPxlValueStdDevMeanLit::RSGISCalcClusterPxlValueStdDevMeanLit(double **stdDevData, double **statsData, double *pxlCount, double *meanLitColVals, unsigned int meanLitBandArrIdx, std::vector<rsgis::rastergis::RSGISBandAttStats*> *bandStats, bool *firstVal, unsigned int ratBand) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->stdDevData = stdDevData;
        this->statsData = statsData;
        this->pxlCount = pxlCount;
        this->meanLitColVals = meanLitColVals;
        this->meanLitBandArrIdx = meanLitBandArrIdx;
        this->bandStats = bandStats;
        this->firstVal = firstVal;
        this->ratBand = ratBand;
    }
    
    void RSGISCalcClusterPxlValueStdDevMeanLit::calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException)
    {
        if(intBandValues[ratBand-1] > 0)
        {
            unsigned long fid = intBandValues[ratBand-1];
            if(floatBandValues[meanLitBandArrIdx] >= this->meanLitColVals[fid])
            {
                for(std::vector<rsgis::rastergis::RSGISBandAttStats*>::iterator iterBands = bandStats->begin(); iterBands != bandStats->end(); ++iterBands)
                {
                    if((boost::math::isfinite)(floatBandValues[(*iterBands)->band-1]))
                    {
                        if((*iterBands)->calcStdDev)
                        {
                            double stdDevComp = pow(((double)(floatBandValues[(*iterBands)->band-1] - statsData[fid][(*iterBands)->meanLocalIdx])), 2.0);
                            if(firstVal[fid])
                            {
                                stdDevData[fid][(*iterBands)->stdDevLocalIdx] = stdDevComp;
                            }
                            else
                            {
                                stdDevData[fid][(*iterBands)->stdDevLocalIdx] += stdDevComp;
                            }
                        }
                    }
                }
                
                if(firstVal[fid])
                {
                    firstVal[fid] = false;
                }
            }
        }
    }
    
    RSGISCalcClusterPxlValueStdDevMeanLit::~RSGISCalcClusterPxlValueStdDevMeanLit()
    {
        
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
}}




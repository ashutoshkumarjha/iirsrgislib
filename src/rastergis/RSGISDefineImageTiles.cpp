/*
 *  RSGISDefineImageTiles.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 02/03/2013.
 *  Copyright 2013 RSGISLib.
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

#include "RSGISDefineImageTiles.h"

namespace rsgis{namespace rastergis{
    
    RSGISDefineImageTiles::RSGISDefineImageTiles()
    {
        
    }
    
    void RSGISDefineImageTiles::defineTiles(GDALDataset *inDataset, std::string outputImage, std::string imageFormat, unsigned int tileSizePxl, double validPxlRatio, double nodataValue, bool noDataValDefined)  throw(RSGISImageException, RSGISAttributeTableException)
    {
        try
        {
            // Create the initial tiles.
            std::cout << "Create Initial tiles\n";
            unsigned int numTiles = this->defineImageTilesAsThematicImage(inDataset, outputImage, tileSizePxl, imageFormat);
            
            std::cout << numTiles << " inital tiles have been created\n";
            
            // Populate with stats.
            std::cout << "Populate the image with basics stats\n";
            GDALDataset *outDataset = (GDALDataset *) GDALOpen(outputImage.c_str(), GA_Update);
            if(outDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + outputImage;
                throw RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISPopWithStats popWithStats;
            popWithStats.calcPopStats( outDataset, true, nodataValue, false );
            
            // Calculate the tile valid pixels - only calc if no data value defined.
            if(noDataValDefined)
            {
                std::cout << "Calculate the valid tile pixel counts\n";
                rsgis::rastergis::RSGISCalcClumpStats calcStats;
                calcStats.calcValidPixelCount(outDataset, inDataset, nodataValue, "NumValidPxls");
            }
            
            // Relabel image clusters with zero valid pixels.
            std::cout << "Remove tiles without data.\n";
            
            const GDALRasterAttributeTable *attTableConst = outDataset->GetRasterBand(1)->GetDefaultRAT();
            if(attTableConst == NULL)
            {
                throw RSGISAttributeTableException("Attribute table could not be read.");
            }
            GDALRasterAttributeTable *attTable = new GDALRasterAttributeTable(*attTableConst);
            
            rsgis::rastergis::RSGISRasterAttUtils attUtils;
            unsigned int colRatioIdx = attUtils.findColumnIndex(attTable, "ValidPxlRatio");
            unsigned int colHistoIdx = attUtils.findColumnIndex(attTable, "Histogram");
            unsigned int colValidPxlIdx = attUtils.findColumnIndex(attTable, "NumValidPxls");
            int numRows = attTable->GetRowCount();
            
            double *ratio = new double[numRows];
            unsigned int *histo = new unsigned int[numRows];
            unsigned int *validPxls = new unsigned int[numRows];
            for(int i = 0; i < numRows; ++i)
            {
                ratio[i] = attTable->GetValueAsDouble(i, colRatioIdx);
                histo[i] = attTable->GetValueAsInt(i, colHistoIdx);
                validPxls[i] = attTable->GetValueAsInt(i, colValidPxlIdx);
            }
            
            rsgis::img::RSGISCalcImageValue *calcImageVal = new RSGISRemoveClumps(numRows, ratio);
            rsgis::img::RSGISCalcEditImage calcImage = rsgis::img::RSGISCalcEditImage(calcImageVal);
            calcImage.calcImageUseOut(outDataset);
            delete calcImageVal;
            
            // Find the neighbours of the tiles.
            std::cout << "Find tile neighbours\n";
            std::vector<size_t> *neighbours = new std::vector<size_t>[numRows];
            for(size_t i = 0; i < numRows; ++i)
            {
                neighbours[i].reserve(4);
            }
            
            rsgis::img::RSGISCalcImageValue *calcValNeighbours = new RSGISFindRecNeighbours(numRows, neighbours);
            rsgis::img::RSGISCalcImage calcNeighbours = rsgis::img::RSGISCalcImage(calcValNeighbours);
            calcNeighbours.calcImageWindowData(&outDataset, 1, 3);
            delete calcValNeighbours;
            /*
            for(size_t i = 0; i < numRows; ++i)
            {
                std::cout << i << "\t (" << neighbours[i].size() << "): ";
                for(unsigned int j = 0; j < neighbours[i].size(); ++j)
                {
                    if(j == 0)
                    {
                        std::cout << neighbours[i][j];
                    }
                    else
                    {
                        std::cout << ", " << neighbours[i][j];
                    }
                }
                std::cout << std::endl;
            }
            */
            
            // Merge any tiles below valid pixel threshold.
            std::cout << "Merge tiles\n";
            size_t *cFID = new size_t[numRows];
            for(size_t i = 0; i < numRows; ++i)
            {
                cFID[i] = i;
            }
            bool change = true;
            bool first = true;
            size_t pFID = 0;
            while(change)
            {
                change = false;
                for(size_t i = 1; i < numRows; ++i)
                {
                    if(ratio[i] < validPxlRatio)
                    {
                        if(neighbours[i].size() == 0)
                        {
                            cFID[i] = cFID[i];
                        }
                        else
                        {
                            pFID = cFID[i];
                            float maxVal = 0;
                            size_t maxIdx = cFID[i];
                            first = true;
                            for(std::vector<size_t>::iterator iterN = neighbours[i].begin(); iterN != neighbours[i].end(); ++iterN)
                            {
                                if(first)
                                {
                                    maxVal = ratio[*iterN];
                                    maxIdx = *iterN;
                                    first = false;
                                }
                                else if(ratio[*iterN] > maxVal)
                                {
                                    maxVal = ratio[*iterN];
                                    maxIdx = *iterN;
                                }
                            }
                            for(size_t n = 1; n < numRows; ++n)
                            {
                                if(cFID[n] == pFID)
                                {
                                    cFID[n] = maxIdx;
                                }
                            }
                            cFID[i] = maxIdx;
                            
                            validPxls[maxIdx] += validPxls[i];
                            histo[maxIdx] += histo[i];
                            ratio[maxIdx] = ((double)validPxls[maxIdx])/((double)histo[maxIdx]);
                            
                            for(std::vector<size_t>::iterator iterN = neighbours[i].begin(); iterN != neighbours[i].end(); ++iterN)
                            {
                                if((*iterN) != maxIdx)
                                {
                                    for(unsigned int n = 0; n <= neighbours[(*iterN)].size(); ++n)
                                    {
                                        if(neighbours[(*iterN)][n] == i)
                                        {
                                            neighbours[(*iterN)][n] = maxIdx;
                                        }
                                    }
                                }
                            }
                            
                            for(std::vector<size_t>::iterator iterM = neighbours[maxIdx].begin(); iterM != neighbours[maxIdx].end(); )
                            {
                                if((*iterM) == i)
                                {
                                    neighbours[maxIdx].erase(iterM);
                                }
                                else
                                {
                                    ++iterM;
                                }
                            }
                            ratio[i] = 0;
                            validPxls[i] = 0;
                            histo[i] = 0;
                            
                            for(std::vector<size_t>::iterator iterN = neighbours[i].begin(); iterN != neighbours[i].end(); ++iterN)
                            {
                                if((*iterN) != maxIdx)
                                {
                                    bool addNeighbour = true;
                                    for(std::vector<size_t>::iterator iterM = neighbours[maxIdx].begin(); iterM != neighbours[maxIdx].end(); ++iterM)
                                    {
                                        if((*iterN) == (*iterM))
                                        {
                                            addNeighbour = false;
                                        }
                                    }
                                    if(addNeighbour)
                                    {
                                        neighbours[maxIdx].push_back((*iterN));
                                    }
                                }
                            }
                            
                            neighbours[i].clear();
                            change = true;
                            std::cout << "Merged " << pFID << " with " << maxIdx << std::endl;
                            
                        }
                    }
                    else
                    {
                        cFID[i] = cFID[i];
                    }
                }
            }
            /*
            for(size_t i = 0; i < numRows; ++i)
            {
                std::cout << i <<  " = " << cFID[i] << std::endl;
            }
            */
            /*
            for(size_t i = 0; i < numRows; ++i)
            {
                std::cout << i << "\t (" << neighbours[i].size() << "): ";
                for(unsigned int j = 0; j < neighbours[i].size(); ++j)
                {
                    if(j == 0)
                    {
                        std::cout << neighbours[i][j];
                    }
                    else
                    {
                        std::cout << ", " << neighbours[i][j];
                    }
                }
                std::cout << std::endl;
            }
             */
            
            
                        
            std::cout << "Number Consecutively" << std::endl;
            
            
            size_t numRowsWithData = 1; // Zero is no data.
            for(int i = 0; i < numRows; ++i)
            {
                if(validPxls[i] > 0)
                {
                    ++numRowsWithData;
                }
            }
            
            size_t *compressCFID = new size_t[numRows];
            for(size_t i = 0; i < numRows; ++i)
            {
                compressCFID[i] = 0;
            }
            double *compressRatio = new double[numRowsWithData];
            int *compressHisto = new int[numRowsWithData];
            int *compressValidPixels = new int[numRowsWithData];
            
            size_t cIdx = 1;
            compressRatio[0] = 0;
            compressHisto[0] = 0;
            compressValidPixels[0] = 0;
            
            for(size_t i = 0; i < numRows; ++i)
            {
                if(validPxls[i] > 0)
                {
                    compressCFID[i] = cIdx;
                    for(size_t j = 0; j < numRows; ++j)
                    {
                        if(cFID[j] == cFID[i])
                        {
                            compressCFID[j] = cIdx;
                        }
                    }
                    compressRatio[cIdx] = ratio[i];
                    compressHisto[cIdx] = histo[i];
                    compressValidPixels[cIdx] = validPxls[i];
                    ++cIdx;
                }
            }
            
            
            std::cout << "Relabeling Tiles\n";
            rsgis::img::RSGISCalcImageValue *calcImageValReLabel = new RSGISReLabelClumps(numRows, compressCFID);
            rsgis::img::RSGISCalcEditImage calcImageReLabel = rsgis::img::RSGISCalcEditImage(calcImageValReLabel);
            calcImageReLabel.calcImageUseOut(outDataset);
            delete calcImageValReLabel;
            
            std::cout << "Find Spatial Extent\n";
            double **tileExtent = new double*[numRowsWithData];
            bool *firstVals = new bool[numRowsWithData];
            for(int i = 0; i < numRowsWithData; ++i)
            {
                tileExtent[i] = new double[4];
                for(unsigned int j = 0; j < 4; ++j)
                {
                    tileExtent[i][j] = 0;
                }
                firstVals[i] = true;
            }
            
            rsgis::img::RSGISCalcImageValue *calcValExtent = new RSGISFindClumpExtent(numRows, tileExtent, firstVals);
            rsgis::img::RSGISCalcImage calcExtent = rsgis::img::RSGISCalcImage(calcValExtent);
            calcExtent.calcImageExtent(&outDataset, 1);
            delete calcValExtent;
            
            
            GDALRasterAttributeTable *attTableNew = new GDALRasterAttributeTable();
            attTableNew->SetRowCount(numRowsWithData);
            
            colRatioIdx = attUtils.findColumnIndexOrCreate(attTableNew, "ValidPxlRatio", GFT_Integer);
            colHistoIdx = attUtils.findColumnIndexOrCreate(attTableNew, "Histogram", GFT_Integer);
            colValidPxlIdx = attUtils.findColumnIndexOrCreate(attTableNew, "NumValidPxls", GFT_Integer);
            unsigned int colXMinIdx = attUtils.findColumnIndexOrCreate(attTableNew, "XMIN", GFT_Real);
            unsigned int colXMaxIdx = attUtils.findColumnIndexOrCreate(attTableNew, "XMAX", GFT_Real);
            unsigned int colYMinIdx = attUtils.findColumnIndexOrCreate(attTableNew, "YMIN", GFT_Real);
            unsigned int colYMaxIdx = attUtils.findColumnIndexOrCreate(attTableNew, "YMAX", GFT_Real);
            
            for(int i = 0; i < numRowsWithData; ++i)
            {
                attTableNew->SetValue(i, colRatioIdx, compressRatio[i]);
                attTableNew->SetValue(i, colHistoIdx, compressHisto[i]);
                attTableNew->SetValue(i, colValidPxlIdx, compressValidPixels[i]);
                attTableNew->SetValue(i, colXMinIdx, tileExtent[i][0]);
                attTableNew->SetValue(i, colXMaxIdx, tileExtent[i][1]);
                attTableNew->SetValue(i, colYMinIdx, tileExtent[i][2]);
                attTableNew->SetValue(i, colYMaxIdx, tileExtent[i][3]);
            }
            
            outDataset->GetRasterBand(1)->SetDefaultRAT(attTableNew);
            
            delete[] cFID;
            delete[] ratio;
            delete[] histo;
            delete[] validPxls;
            
            delete[] compressCFID;
            delete[] compressRatio;
            delete[] compressHisto;
            delete[] compressValidPixels;
            
            delete[] neighbours;
            delete[] firstVals;
            for(int i = 0; i < numRowsWithData; ++i)
            {
                delete[] tileExtent[i];
            }
            delete[] tileExtent;
            
            GDALClose(outDataset);
        }
        catch (RSGISImageException &e)
        {
            throw e;
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
        
        
    }
    
    unsigned int RSGISDefineImageTiles::defineImageTilesAsThematicImage(GDALDataset *inputImage, std::string outputImage, unsigned int tileSizePxls, std::string imageFormat) throw(RSGISImageException)
    {
        try
        {
            rsgis::img::RSGISImageUtils imageUtils;
            
            // Create the output image
            std::cout << "Creating output image\n";
            imageUtils.createCopy(inputImage, 1, outputImage, imageFormat, GDT_UInt16);
            
            // Open output image
            GDALDataset *outDataset = (GDALDataset *) GDALOpen(outputImage.c_str(), GA_Update);
            if(outDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + outputImage;
                throw RSGISImageException(message.c_str());
            }
            
            // Set output image to be all zeros.
            std::cout << "Setting output image pixels to be zero.\n";
            imageUtils.zerosUIntGDALDataset(outDataset);
            
            unsigned int xSize = outDataset->GetRasterXSize();
            unsigned int ySize = outDataset->GetRasterYSize();
            
            unsigned int numTilesX = floor(((double)xSize)/((double)tileSizePxls));
            unsigned int numTilesY = floor(((double)ySize)/((double)tileSizePxls));
            
            unsigned int allNumTilesX = numTilesX;
            unsigned int allNumTilesY = numTilesX;
            
            bool extraX = false;
            unsigned int extraXSize = xSize - (numTilesX * tileSizePxls);
            bool extraY = false;
            unsigned int extraYSize = ySize - (numTilesY * tileSizePxls);
            
            if( extraXSize != 0 )
            {
                extraX = true;
                ++allNumTilesX;
            }
            
            if( extraYSize != 0 )
            {
                extraY = true;
                ++allNumTilesY;
            }
            
            unsigned int numTiles = allNumTilesX * allNumTilesY;
            
            GDALRasterBand *rasterBand = outDataset->GetRasterBand(1);
            int xBlockSize = 0;
            int yBlockSize = 0;
            rasterBand->GetBlockSize (&xBlockSize, &yBlockSize);
            
            std::cout << "Block Size: " << yBlockSize << std::endl;
            
            unsigned int *outputData = (unsigned int *) CPLMalloc(sizeof(unsigned int)*xSize*yBlockSize);
            
            int nYBlocks = ySize / yBlockSize;
            int remainRows = ySize - (nYBlocks * yBlockSize);
            int rowOffset = 0;
            
            unsigned int cTile = 0;
            unsigned int rowStartTile = 1;
            unsigned int rowTileCount = 0;
            unsigned int colTileCount = 0;
            
			int feedback = ySize/10;
			int feedbackCounter = 0;
			std::cout << "Started" << std::flush;
			// Loop images to process data
			for(int i = 0; i < nYBlocks; i++)
			{
                for(int m = 0; m < yBlockSize; ++m)
                {
                    if((feedback != 0) && ((((i*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    cTile = rowStartTile;
                    colTileCount = 0;
                    for(int j = 0; j < xSize; j++)
                    {
                        outputData[(m*xSize)+j] = cTile;
                        
                        ++colTileCount;
                        if(colTileCount == tileSizePxls)
                        {
                            ++cTile;
                            colTileCount = 0;
                        }
                    }
                    
                    ++rowTileCount;
                    
                    if(rowTileCount == tileSizePxls)
                    {
                        rowStartTile += allNumTilesX;
                        rowTileCount = 0;
                    }
                }
				
                rowOffset = yBlockSize * i;
                rasterBand->RasterIO(GF_Write, 0, rowOffset, xSize, yBlockSize, outputData, xSize, yBlockSize, GDT_UInt32, 0, 0);
                
			}
            
            if(remainRows > 0)
            {
                for(int m = 0; m < remainRows; ++m)
                {
                    if((feedback != 0) && ((((nYBlocks*yBlockSize)+m) % feedback) == 0))
                    {
                        std::cout << "." << feedbackCounter << "." << std::flush;
                        feedbackCounter = feedbackCounter + 10;
                    }
                    
                    cTile = rowStartTile;
                    colTileCount = 0;
                    for(int j = 0; j < xSize; j++)
                    {
                        outputData[(m*xSize)+j] = cTile;
                        
                        ++colTileCount;
                        if(colTileCount == tileSizePxls)
                        {
                            ++cTile;
                            colTileCount = 0;
                        }
                    }
                    
                    ++rowTileCount;
                    
                    if(rowTileCount == tileSizePxls)
                    {
                        rowStartTile += allNumTilesX;
                        rowTileCount = 0;
                    }
                }
				
				rowOffset = (yBlockSize * nYBlocks);
                rasterBand->RasterIO(GF_Write, 0, rowOffset, xSize, remainRows, outputData, xSize, remainRows, GDT_UInt32, 0, 0);
            }
			std::cout << " Complete.\n";
            
            
            rasterBand->SetMetadataItem("LAYER_TYPE", "thematic");
            
            delete[] outputData;
            
            GDALClose(outDataset);
            
            return numTiles;
        }
        catch (RSGISImageException &e)
        {
            throw e;
        }
        catch (RSGISException &e)
        {
            throw RSGISImageException(e.what());
        }
    }
    
    RSGISDefineImageTiles::~RSGISDefineImageTiles()
    {
        
    }
    
    
    RSGISRemoveClumps::RSGISRemoveClumps(size_t numRows, double *ratioVals) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->numRows = numRows;
        this->ratioVals = ratioVals;
    }
    
    void RSGISRemoveClumps::calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(bandValues[0] > 0)
        {
            size_t fid = boost::lexical_cast<size_t>(bandValues[0]);
            
            if(ratioVals[fid] == 0.0)
            {
                output[0] = 0;
            }
            else
            {
                output[0] = bandValues[0];
            }
        }
        else
        {
            output[0] = bandValues[0];
        }
    }
    
    RSGISRemoveClumps::~RSGISRemoveClumps()
    {
        
    }
    
    
    
    RSGISFindRecNeighbours::RSGISFindRecNeighbours(size_t numRows, std::vector<size_t> *neighbours) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->numRows = numRows;
        this->neighbours = neighbours;
    }
    
    void RSGISFindRecNeighbours::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            int winHsize = ((winSize-1)/2);
            
            if(dataBlock[0][winHsize][winHsize] > 0)
            {
                size_t fid = 0;
                try
                {
                    fid = boost::lexical_cast<size_t>(dataBlock[0][winHsize][winHsize]);
                }
                catch(boost::numeric::negative_overflow& e)
                {
                    std::cout << "dataBlock[0][winHsize][winHsize] = " << dataBlock[0][winHsize][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::positive_overflow& e)
                {
                    std::cout << "dataBlock[0][winHsize][winHsize] = " << dataBlock[0][winHsize][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                catch(boost::numeric::bad_numeric_cast& e)
                {
                    std::cout << "dataBlock[0][winHsize][winHsize] = " << dataBlock[0][winHsize][winHsize] << std::endl;
                    throw rsgis::img::RSGISImageCalcException(e.what());
                }
                
                
                if(neighbours[fid].size() <= 4)
                {
                    size_t fidLeft = 0;
                    try
                    {
                        fidLeft = boost::lexical_cast<size_t>(dataBlock[0][winHsize][winHsize-1]);
                    }
                    catch(boost::numeric::negative_overflow& e)
                    {
                        std::cout << "dataBlock[0][winHsize][winHsize-1] = " << dataBlock[0][winHsize][winHsize-1] << std::endl;
                        throw rsgis::img::RSGISImageCalcException(e.what());
                    }
                    catch(boost::numeric::positive_overflow& e)
                    {
                        std::cout << "dataBlock[0][winHsize][winHsize-1] = " << dataBlock[0][winHsize][winHsize-1] << std::endl;
                        throw rsgis::img::RSGISImageCalcException(e.what());
                    }
                    catch(boost::numeric::bad_numeric_cast& e)
                    {
                        std::cout << "dataBlock[0][winHsize][winHsize-1] = " << dataBlock[0][winHsize][winHsize-1] << std::endl;
                        throw rsgis::img::RSGISImageCalcException(e.what());
                    }
                    
                    if((fidLeft != 0) && (fidLeft != fid))
                    {
                        if(neighbours[fid].size() == 0)
                        {
                            neighbours[fid].push_back(fidLeft);
                        }
                        else
                        {
                            bool found = false;
                            for(std::vector<size_t>::iterator iterVal = neighbours[fid].begin(); iterVal != neighbours[fid].end(); ++iterVal)
                            {
                                if((*iterVal) == fidLeft)
                                {
                                    found = true;
                                    break;
                                }
                            }
                            
                            if(!found)
                            {
                                neighbours[fid].push_back(fidLeft);
                            }
                        }
                    }
                }
                
                if(neighbours[fid].size() <= 4)
                {
                    size_t fidUp = 0;
                    try
                    {
                        fidUp = boost::lexical_cast<size_t>(dataBlock[0][winHsize+1][winHsize]);
                    }
                    catch(boost::numeric::negative_overflow& e)
                    {
                        std::cout << "dataBlock[0][winHsize+1][winHsize] = " << dataBlock[0][winHsize+1][winHsize] << std::endl;
                        throw rsgis::img::RSGISImageCalcException(e.what());
                    }
                    catch(boost::numeric::positive_overflow& e)
                    {
                        std::cout << "dataBlock[0][winHsize+1][winHsize] = " << dataBlock[0][winHsize+1][winHsize] << std::endl;
                        throw rsgis::img::RSGISImageCalcException(e.what());
                    }
                    catch(boost::numeric::bad_numeric_cast& e)
                    {
                        std::cout << "dataBlock[0][winHsize+1][winHsize] = " << dataBlock[0][winHsize+1][winHsize] << std::endl;
                        throw rsgis::img::RSGISImageCalcException(e.what());
                    }
                    
                    if((fidUp != 0) && (fidUp != fid))
                    {
                        if(neighbours[fid].size() == 0)
                        {
                            neighbours[fid].push_back(fidUp);
                        }
                        else
                        {
                            bool found = false;
                            for(std::vector<size_t>::iterator iterVal = neighbours[fid].begin(); iterVal != neighbours[fid].end(); ++iterVal)
                            {
                                if((*iterVal) == fidUp)
                                {
                                    found = true;
                                    break;
                                }
                            }
                            
                            if(!found)
                            {
                                neighbours[fid].push_back(fidUp);
                            }
                        }
                    }
                }
                
                if(neighbours[fid].size() <= 4)
                {
                    size_t fidRight = 0;
                    try
                    {
                        fidRight = boost::lexical_cast<size_t>(dataBlock[0][winHsize][winHsize+1]);
                    }
                    catch(boost::numeric::negative_overflow& e)
                    {
                        std::cout << "dataBlock[0][winHsize][winHsize+1] = " << dataBlock[0][winHsize][winHsize+1] << std::endl;
                        throw rsgis::img::RSGISImageCalcException(e.what());
                    }
                    catch(boost::numeric::positive_overflow& e)
                    {
                        std::cout << "dataBlock[0][winHsize][winHsize+1] = " << dataBlock[0][winHsize][winHsize+1] << std::endl;
                        throw rsgis::img::RSGISImageCalcException(e.what());
                    }
                    catch(boost::numeric::bad_numeric_cast& e)
                    {
                        std::cout << "dataBlock[0][winHsize][winHsize+1] = " << dataBlock[0][winHsize][winHsize+1] << std::endl;
                        throw rsgis::img::RSGISImageCalcException(e.what());
                    }
                    
                    
                    if((fidRight != 0) && (fidRight != fid))
                    {
                        if(neighbours[fid].size() == 0)
                        {
                            neighbours[fid].push_back(fidRight);
                        }
                        else
                        {
                            bool found = false;
                            for(std::vector<size_t>::iterator iterVal = neighbours[fid].begin(); iterVal != neighbours[fid].end(); ++iterVal)
                            {
                                if((*iterVal) == fidRight)
                                {
                                    found = true;
                                    break;
                                }
                            }
                            
                            if(!found)
                            {
                                neighbours[fid].push_back(fidRight);
                            }
                        }
                    }
                }
                
                if(neighbours[fid].size() <= 4)
                {
                    size_t fidDown = 0;
                    try
                    {
                        fidDown = boost::lexical_cast<size_t>(dataBlock[0][winHsize-1][winHsize]);
                    }
                    catch(boost::numeric::negative_overflow& e)
                    {
                        std::cout << "dataBlock[0][winHsize-1][winHsize] = " << dataBlock[0][winHsize-1][winHsize] << std::endl;
                        throw rsgis::img::RSGISImageCalcException(e.what());
                    }
                    catch(boost::numeric::positive_overflow& e)
                    {
                        std::cout << "dataBlock[0][winHsize-1][winHsize] = " << dataBlock[0][winHsize-1][winHsize] << std::endl;
                        throw rsgis::img::RSGISImageCalcException(e.what());
                    }
                    catch(boost::numeric::bad_numeric_cast& e)
                    {
                        std::cout << "dataBlock[0][winHsize-1][winHsize] = " << dataBlock[0][winHsize-1][winHsize] << std::endl;
                        throw rsgis::img::RSGISImageCalcException(e.what());
                    }
                    
                    
                    if((fidDown != 0) && (fidDown != fid))
                    {
                        if(neighbours[fid].size() == 0)
                        {
                            neighbours[fid].push_back(fidDown);
                        }
                        else
                        {
                            bool found = false;
                            for(std::vector<size_t>::iterator iterVal = neighbours[fid].begin(); iterVal != neighbours[fid].end(); ++iterVal)
                            {
                                if((*iterVal) == fidDown)
                                {
                                    found = true;
                                    break;
                                }
                            }
                            
                            if(!found)
                            {
                                neighbours[fid].push_back(fidDown);
                            }
                        }
                    }
                }                
            }
        }
        catch(rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch(std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    RSGISFindRecNeighbours::~RSGISFindRecNeighbours()
    {
        
    }
    
    
    
    RSGISReLabelClumps::RSGISReLabelClumps(size_t numRows, size_t *nFID) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->numRows = numRows;
        this->nFID = nFID;
    }
    
    void RSGISReLabelClumps::calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        if(bandValues[0] > 0)
        {
            size_t fid = boost::lexical_cast<size_t>(bandValues[0]);
            
            output[0] = nFID[fid];
        }
        else
        {
            output[0] = bandValues[0];
        }
    }
    
    RSGISReLabelClumps::~RSGISReLabelClumps()
    {
        
    }
    
    
    

    RSGISFindClumpExtent::RSGISFindClumpExtent(size_t numRows, double **tileExtent, bool *firstVals) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->numRows = numRows;
        this->tileExtent = tileExtent;
        this->firstVals = firstVals;
    }
    
    void RSGISFindClumpExtent::calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException)
    {
        if((bandValues[0] > 0) && (bandValues[0] < numRows))
        {
            size_t fid = boost::lexical_cast<size_t>(bandValues[0]);
            
            if(firstVals[fid])
            {
                tileExtent[fid][0] = extent.getMinX();
                tileExtent[fid][1] = extent.getMaxX();
                tileExtent[fid][2] = extent.getMinY();
                tileExtent[fid][3] = extent.getMaxY();
                firstVals[fid] = false;
            }
            else
            {
                if(extent.getMinX() < tileExtent[fid][0])
                {
                    tileExtent[fid][0] = extent.getMinX();
                }
                
                if(extent.getMaxX() > tileExtent[fid][1])
                {
                    tileExtent[fid][1] = extent.getMaxX();
                }
                
                if(extent.getMinY() < tileExtent[fid][2])
                {
                    tileExtent[fid][2] = extent.getMinY();
                }
                
                if(extent.getMaxY() > tileExtent[fid][3])
                {
                    tileExtent[fid][3] = extent.getMaxY();
                }
            }
            
        }
    }
    
    RSGISFindClumpExtent::~RSGISFindClumpExtent()
    {
        
    }
    
    
    
    
    
    RSGISDefineSegmentsWithinTiles::RSGISDefineSegmentsWithinTiles()
    {
        
    }
    
    void RSGISDefineSegmentsWithinTiles::defineSegmentTilePos(GDALDataset *clumpsDataset, GDALDataset *tileDataset, std::string outColName, unsigned int tileOverlap, unsigned int tileBoundary, unsigned int tileBody) throw(RSGISImageException, RSGISAttributeTableException)
    {
        try
        {
            // Get Attribute table
            const GDALRasterAttributeTable *attTableTmp = clumpsDataset->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *attTable = NULL;
            if(attTableTmp != NULL)
            {
                attTable = new GDALRasterAttributeTable(*attTableTmp);
            }
            else
            {
                attTable = new GDALRasterAttributeTable();
            }
            
            // Make sure it is long enough and extend if required.
            int numRows = attTable->GetRowCount();
            
            double maxVal = 0;
            clumpsDataset->GetRasterBand(1)->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
            
            if(maxVal > numRows)
            {
                attTable->SetRowCount(maxVal+1);
            }
            numRows = attTable->GetRowCount();
            
            RSGISSegTilePos *clumpTilePos = new RSGISSegTilePos[numRows];
            for(size_t i = 0; i < numRows; ++i)
            {
                clumpTilePos[i].overlap = false;
                clumpTilePos[i].boundary = false;
                clumpTilePos[i].body = false;
            }
            
            GDALDataset **datasets = new GDALDataset*[2];
            datasets[0] = clumpsDataset;
            datasets[1] = tileDataset;
            
            RSGISFindClumpPositionsInTile *calcImgValStats = new RSGISFindClumpPositionsInTile(numRows, clumpTilePos, tileOverlap, tileBoundary, tileBody);
            rsgis::img::RSGISCalcImage calcImageStats(calcImgValStats);
            calcImageStats.calcImage(datasets, 2);
            delete calcImgValStats;
            
            delete[] datasets;
            
            RSGISRasterAttUtils attUtils;
            unsigned int colClumpPosIdx = attUtils.findColumnIndexOrCreate(attTable, outColName, GFT_Integer);
            
            for(size_t i = 1; i < numRows; ++i)
            {
                if(clumpTilePos[i].boundary)
                {
                    attTable->SetValue(i, colClumpPosIdx, (int)tileBoundary);
                }
                else if(clumpTilePos[i].overlap & clumpTilePos[i].body)
                {
                    attTable->SetValue(i, colClumpPosIdx, (int)tileBoundary); // Just in case the boundary is not defined.
                }
                else if(clumpTilePos[i].overlap)
                {
                    attTable->SetValue(i, colClumpPosIdx, (int)tileOverlap);
                }
                else if(clumpTilePos[i].body)
                {
                    attTable->SetValue(i, colClumpPosIdx, (int)tileBody);
                }
            }
            
            clumpsDataset->GetRasterBand(1)->SetDefaultRAT(attTable);
            
            delete[] clumpTilePos;
            
        }
        catch (RSGISImageException &e)
        {
            throw e;
        }
        catch(RSGISAttributeTableException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISImageException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
    }
    
    RSGISDefineSegmentsWithinTiles::~RSGISDefineSegmentsWithinTiles()
    {
        
    }
    
    RSGISFindClumpPositionsInTile::RSGISFindClumpPositionsInTile(size_t numRows, RSGISSegTilePos *clumpPos, unsigned int tileOverlap, unsigned int tileBoundary, unsigned int tileBody): rsgis::img::RSGISCalcImageValue(0)
    {
        this->numRows = numRows;
        this->clumpPos = clumpPos;
        this->tileOverlap = tileOverlap;
        this->tileBoundary = tileBoundary;
        this->tileBody = tileBody;
    }
		
    void RSGISFindClumpPositionsInTile::calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException)
    {
        if((bandValues[0] > 0) && (bandValues[0] < numRows))
        {
            size_t fid = boost::lexical_cast<size_t>(bandValues[0]);
            
            if(bandValues[1] == this->tileOverlap)
            {
                this->clumpPos[fid].overlap = true;
            }
            else if(bandValues[1] == this->tileBoundary)
            {
                this->clumpPos[fid].boundary = true;
            }
            else if(bandValues[1] == this->tileBody)
            {
                this->clumpPos[fid].body = true;
            }
        }
    }
    
    RSGISFindClumpPositionsInTile::~RSGISFindClumpPositionsInTile()
    {
        
    }
    
    
    
    
}}



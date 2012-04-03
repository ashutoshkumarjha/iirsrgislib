/*
 *  RSGISImageMorphologyClosing.cpp
 *  RSGIS_LIB
 *
 *  Created by Peter Bunting on 02/03/2012
 *  Copyright 2012 RSGISLib.
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

#include "RSGISMorphologyClosing.h"

namespace rsgis{namespace filter{
    

    RSGISImageMorphologyClosing::RSGISImageMorphologyClosing()
    {
        
    }
    
    void RSGISImageMorphologyClosing::performClosing(GDALDataset *dataset, string outputImage, string tempImage, bool useMemory, Matrix *matrixOperator, unsigned int numIterations) throw(RSGISImageCalcException, RSGISImageBandException)
    {
        try 
        {
            if(matrixOperator->n != matrixOperator->m)
            {
                throw RSGISImageCalcException("Morphological operator must be a square matrix.");
            }
            
            RSGISImageUtils imgUtils;
            GDALDataset *outDataset = NULL;
            GDALDataset *tmpDataset = NULL;
            GDALDataset **tmpGDALDataArray = new GDALDataset*[1];
            
            outDataset = imgUtils.createCopy(dataset, outputImage, "ENVI", GDT_Float32);
            imgUtils.zerosFloatGDALDataset(outDataset);
            
            if(useMemory)
            {
                tmpDataset = imgUtils.createCopy(dataset, tempImage, "MEM", GDT_Float32);
            }
            else
            {
                tmpDataset = imgUtils.createCopy(dataset, tempImage, "ENVI", GDT_Float32);
            }
            imgUtils.zerosFloatGDALDataset(tmpDataset);
            
            
            RSGISCalcImageValue *imgErode = new RSGISMorphologyErode(dataset->GetRasterCount(), matrixOperator);
            RSGISCalcImage calcImageErode(imgErode);
            RSGISCalcImageValue *imgDilate = new RSGISMorphologyDilate(dataset->GetRasterCount(), matrixOperator);
            RSGISCalcImage calcImageDilate(imgDilate);
            
            tmpGDALDataArray[0] = dataset;
            for(unsigned int i = 0; i < numIterations; ++i)
            {
                if(i > 0)
                {
                    tmpGDALDataArray[0] = outDataset;
                }
                
                calcImageDilate.calcImageWindowData(tmpGDALDataArray, 1, tmpDataset, matrixOperator->n);
                tmpGDALDataArray[0] = tmpDataset;
                calcImageErode.calcImageWindowData(tmpGDALDataArray, 1, outDataset, matrixOperator->n);
            }
            
            delete imgErode;
            delete imgDilate;
            
            delete[] tmpGDALDataArray;
            
            GDALClose(outDataset);
            GDALClose(tmpDataset);
        } 
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }
        catch (RSGISImageBandException &e) 
        {
            throw e;
        }
        catch (RSGISImageException &e) 
        {
            throw RSGISImageCalcException(e.what());
        }
    }
    
    RSGISImageMorphologyClosing::~RSGISImageMorphologyClosing()
    {
        
    }
    
    
    
}}




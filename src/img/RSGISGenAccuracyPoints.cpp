/*
 *  RSGISGenAccuracyPoints.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/11/2012.
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

#include "RSGISGenAccuracyPoints.h"

namespace rsgis{namespace img{

    
    RSGISGenAccuracyPoints::RSGISGenAccuracyPoints()
    {
        
    }
    
    void RSGISGenAccuracyPoints::generateRandomPoints(GDALDataset *inputImage, GDALDataset *inputDEM, bool demProvided, std::string outputFile, std::string classColName, unsigned int numPts, unsigned int seed) throw(rsgis::RSGISImageException)
    {
        try
        {
            const GDALRasterAttributeTable *attTableTmp = inputImage->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *attTable = NULL;
            if(attTableTmp != NULL)
            {
                attTable = new GDALRasterAttributeTable(*attTableTmp);
            }
            else
            {
                throw rsgis::RSGISImageException("Attribute table is not present within the image.");
            }
            int numRows = attTable->GetRowCount();
            int numColumns = attTable->GetColumnCount();
            
            if(numRows == 0)
            {
                throw rsgis::RSGISImageException("The attribute table does not have any rows.");
            }
            
            if(numColumns == 0)
            {
                throw rsgis::RSGISImageException("The attribute table does not have any column.");
            }
            
            // Get the column indexes and create output column if not present.
            int inClassColIdx = 0;
            bool inClassColFound = false;
            
            for(int i = 0; i < numColumns; ++i)
            {
                if(!inClassColFound && (std::string(attTable->GetNameOfCol(i)) == classColName))
                {
                    inClassColFound = true;
                    inClassColIdx = i;
                }
            }
            
            if(!inClassColFound)
            {
                throw rsgis::RSGISImageException("Could not find the input class column.");
            }
            
            double tlX = 0;
            double tlY = 0;
            double xRes = 0;
            double yRes = 0;
            unsigned int imgSizeX = 0;
            unsigned int imgSizeY = 0;
            double demTlX = 0;
            double demTlY = 0;
            double demXRes = 0;
            double demYRes = 0;
            unsigned int demSizeX = 0;
            unsigned int demSizeY = 0;
            
            // Get Transformation for Image
            double *trans = new double[6];
            inputImage->GetGeoTransform(trans);
            tlX = trans[0];
            tlY = trans[3];
            xRes = trans[1];
            yRes = trans[5];
            if(yRes < 0)
            {
                yRes = yRes * (-1);
            }
            imgSizeX = inputImage->GetRasterXSize();
            imgSizeY = inputImage->GetRasterYSize();
            
            if(demProvided)
            {
                // Get Transformation for DEM
                inputDEM->GetGeoTransform(trans);
                demTlX = trans[0];
                demTlY = trans[3];
                demXRes = trans[1];
                demYRes = trans[5];
                if(demYRes < 0)
                {
                    demYRes = demYRes * (-1);
                }
                demSizeX = inputDEM->GetRasterXSize();
                demSizeY = inputDEM->GetRasterYSize();
            }
            delete[] trans;
            
            srand(seed);
            
            unsigned int xPxl = 0;
            unsigned int yPxl = 0;
            double eastings = 0;
            double northings = 0;
            float demVal = 0;
            std::string classVal = "";
            
            RSGISAccPoint *tmpAccPt = NULL;
            std::list<RSGISAccPoint*> *accPts = new std::list<RSGISAccPoint*>();
            
            while(accPts->size() < numPts)
            {
                xPxl = rand() % imgSizeX;
                yPxl = rand() % imgSizeY;
                
                eastings = tlX + (((double)xPxl)*xRes);
                northings = tlY - (((double)yPxl)*yRes);
                
                if(demProvided)
                {
                    try
                    {
                        demVal = findPixelVal(inputDEM, 1, eastings, northings, demTlX, demTlY, demXRes, demYRes, demSizeX, demSizeY);
                    }
                    catch (rsgis::RSGISImageException &e)
                    {
                        demVal = -99999;
                    }
                }
                else
                {
                    demVal = 0;
                }
                
                try
                {
                    classVal = findClassVal(inputImage, 1, attTable, inClassColIdx, xPxl, yPxl);
                    
                    tmpAccPt = new RSGISAccPoint();
                    tmpAccPt->ptID = accPts->size()+1;
                    tmpAccPt->eastings = eastings;
                    tmpAccPt->northings = northings;
                    tmpAccPt->elevation = demVal;
                    tmpAccPt->mapClassName = classVal;
                    tmpAccPt->trueClassName = classVal;
                    tmpAccPt->status = -1;
                    tmpAccPt->comment = "";
                    
                    accPts->push_back(tmpAccPt);
                }
                catch (rsgis::RSGISImageException &e)
                {
                    // Ignore as class not found...
                }
                catch (std::exception &e)
                {
                    throw rsgis::RSGISImageException(e.what());
                }
            }           
            
            std::ofstream outFile;
            
            accPts->sort(compareMapClass);
            
            outFile.open(outputFile.c_str());
            if(!outFile.is_open())
            {
                throw rsgis::RSGISImageException("Could not open the output file.");
            }
            outFile.precision(14);
            
            outFile << "# Generated Random Points.\n";
            outFile << "# Input Image: " << inputImage->GetFileList()[0] << std::endl;
            if(demProvided)
            {
                outFile << "# Input DEM: " << inputDEM->GetFileList()[0] << std::endl;
            }
            else
            {
                outFile << "# Not DEM provided.\n";
            }
            outFile << "# Output File: " << outputFile << std::endl;
            outFile << "# Column Name: " << classColName << std::endl;
            outFile << "# Number of Points: " << numPts << std::endl;
            outFile << "# Seed: " << seed << std::endl;
            
            
            for(std::list<RSGISAccPoint*>::iterator iterPts = accPts->begin(); iterPts != accPts->end(); ++iterPts)
            {
                outFile << (*iterPts)->ptID << "\t" << (*iterPts)->eastings << "\t" << (*iterPts)->northings << "\t" << (*iterPts)->elevation << "\t" << (*iterPts)->mapClassName << "\t" << (*iterPts)->trueClassName << "\t" << (*iterPts)->status << "\t" << (*iterPts)->comment << std::endl;
                delete *iterPts;
            }
            delete accPts;
            
            outFile.flush();
            outFile.close();
            
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
    }
    
    
    void RSGISGenAccuracyPoints::generateStratifiedRandomPoints(GDALDataset *inputImage, GDALDataset *inputDEM, bool demProvided, std::string outputFile, std::string classColName, unsigned int numPts, unsigned int seed) throw(rsgis::RSGISImageException)
    {
        try
        {
            const GDALRasterAttributeTable *attTableTmp = inputImage->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *attTable = NULL;
            if(attTableTmp != NULL)
            {
                attTable = new GDALRasterAttributeTable(*attTableTmp);
            }
            else
            {
                throw rsgis::RSGISImageException("Attribute table is not present within the image.");
            }
            int numRows = attTable->GetRowCount();
            int numColumns = attTable->GetColumnCount();
            
            if(numRows == 0)
            {
                throw rsgis::RSGISImageException("The attribute table does not have any rows.");
            }
            
            if(numColumns == 0)
            {
                throw rsgis::RSGISImageException("The attribute table does not have any column.");
            }
            
            // Get the column indexes and create output column if not present.
            int inClassColIdx = 0;
            bool inClassColFound = false;
            
            for(int i = 0; i < numColumns; ++i)
            {
                if(!inClassColFound && (std::string(attTable->GetNameOfCol(i)) == classColName))
                {
                    inClassColFound = true;
                    inClassColIdx = i;
                }
            }
            
            if(!inClassColFound)
            {
                throw rsgis::RSGISImageException("Could not find the input class column.");
            }
            
            double tlX = 0;
            double tlY = 0;
            double xRes = 0;
            double yRes = 0;
            unsigned int imgSizeX = 0;
            unsigned int imgSizeY = 0;
            double demTlX = 0;
            double demTlY = 0;
            double demXRes = 0;
            double demYRes = 0;
            unsigned int demSizeX = 0;
            unsigned int demSizeY = 0;
            
            // Get Transformation for Image
            double *trans = new double[6];
            inputImage->GetGeoTransform(trans);
            tlX = trans[0];
            tlY = trans[3];
            xRes = trans[1];
            yRes = trans[5];
            if(yRes < 0)
            {
                yRes = yRes * (-1);
            }
            imgSizeX = inputImage->GetRasterXSize();
            imgSizeY = inputImage->GetRasterYSize();
            
            if(demProvided)
            {
                // Get Transformation for DEM
                inputDEM->GetGeoTransform(trans);
                demTlX = trans[0];
                demTlY = trans[3];
                demXRes = trans[1];
                demYRes = trans[5];
                if(demYRes < 0)
                {
                    demYRes = demYRes * (-1);
                }
                demSizeX = inputDEM->GetRasterXSize();
                demSizeY = inputDEM->GetRasterYSize();
            }
            delete[] trans;
            
            std::list<std::string> *classes = this->findUniqueClasses(attTable, inClassColIdx);
            
            std::vector<std::vector<RSGISAccPoint*> > *accClassPts = new std::vector<std::vector<RSGISAccPoint*> >();
            
            for(std::list<std::string>::iterator iterClasses = classes->begin(); iterClasses != classes->end(); ++iterClasses)
            {
                std::cout << "Class: \'" <<  *iterClasses << "\'" << std::endl;
                accClassPts->push_back(std::vector<RSGISAccPoint*>());
            }
            
            
            srand(seed);
            
            unsigned int xPxl = 0;
            unsigned int yPxl = 0;
            double eastings = 0;
            double northings = 0;
            float demVal = 0;
            std::string classVal = "";
            bool foundClassName = false;
            
            RSGISAccPoint *tmpAccPt = NULL;
            
            unsigned long totalNumPtsRequired = classes->size() * numPts;
            unsigned long ptsCount = 0;
            size_t idx = 0;
            
            std::cout << "Number of points to be generated: " << totalNumPtsRequired << std::endl;
            
            while(ptsCount < totalNumPtsRequired)
            {
                std::cout << "Completed " << floor((((double)ptsCount)/((double)totalNumPtsRequired))*100) << " %\r" << std::flush;
                
                //std::cout << "ptsCount = " << ptsCount << std::endl;
                
                xPxl = rand() % imgSizeX;
                yPxl = rand() % imgSizeY;
                
                eastings = tlX + (((double)xPxl)*xRes);
                northings = tlY - (((double)yPxl)*yRes);
                
                if(demProvided)
                {
                    try
                    {
                        demVal = findPixelVal(inputDEM, 1, eastings, northings, demTlX, demTlY, demXRes, demYRes, demSizeX, demSizeY);
                    }
                    catch (rsgis::RSGISImageException &e)
                    {
                        demVal = -99999;
                    }
                }
                else
                {
                    demVal = 0;
                }
                
                try
                {
                    classVal = findClassVal(inputImage, 1, attTable, inClassColIdx, xPxl, yPxl);
                    
                    idx = 0;
                    foundClassName = false;
                    for(std::list<std::string>::iterator iterClasses = classes->begin(); iterClasses != classes->end(); ++iterClasses)
                    {
                        if((*iterClasses) == classVal)
                        {
                            foundClassName = true;
                            break;
                        }
                        else
                        {
                            ++idx;
                        }
                    }
                    
                    if(!foundClassName)
                    {
                        rsgis::RSGISImageException("Class name was not found!");
                    }
                    else if(accClassPts->at(idx).size() < numPts)
                    {
                        tmpAccPt = new RSGISAccPoint();
                        tmpAccPt->ptID = ptsCount+1;
                        tmpAccPt->eastings = eastings;
                        tmpAccPt->northings = northings;
                        tmpAccPt->elevation = demVal;
                        tmpAccPt->mapClassName = classVal;
                        tmpAccPt->trueClassName = classVal;
                        tmpAccPt->status = -1;
                        tmpAccPt->comment = "";
                        
                        accClassPts->at(idx).push_back(tmpAccPt);
                        
                        ++ptsCount;
                    }                    
                }
                catch (rsgis::RSGISImageException &e)
                {
                    // Ignore as class not found...
                }
                catch (std::exception &e)
                {
                    throw rsgis::RSGISImageException(e.what());
                }
            }
            
            
            std::ofstream outFile;
                       
            outFile.open(outputFile.c_str());
            if(!outFile.is_open())
            {
                throw rsgis::RSGISImageException("Could not open the output file.");
            }
            outFile.precision(14);
            
            outFile << "# Generated Stratified Random Points.\n";
            outFile << "# Input Image: " << inputImage->GetFileList()[0] << std::endl;
            if(demProvided)
            {
                outFile << "# Input DEM: " << inputDEM->GetFileList()[0] << std::endl;
            }
            else
            {
                outFile << "# Not DEM provided.\n";
            }
            outFile << "# Output File: " << outputFile << std::endl;
            outFile << "# Column Name: " << classColName << std::endl;
            outFile << "# Number of Points Per Class: " << numPts << std::endl;
            outFile << "# Seed: " << seed << std::endl;
            
            for(std::vector<std::vector<RSGISAccPoint*> >::iterator iterPtsVecs = accClassPts->begin(); iterPtsVecs != accClassPts->end(); ++iterPtsVecs)
            {
                for(std::vector<RSGISAccPoint*>::iterator iterPts = (*iterPtsVecs).begin(); iterPts != (*iterPtsVecs).end(); ++iterPts)
                {
                    outFile << (*iterPts)->ptID << "\t" << (*iterPts)->eastings << "\t" << (*iterPts)->northings << "\t" << (*iterPts)->elevation << "\t" << (*iterPts)->mapClassName << "\t" << (*iterPts)->trueClassName << "\t" << (*iterPts)->status << "\t" << (*iterPts)->comment << std::endl;
                    delete *iterPts;
                }
            }
            
            outFile.flush();
            outFile.close();
             
            
            delete classes;
            delete accClassPts;
        }
        catch(rsgis::RSGISImageException &e)
        {
            throw e;
        }
        catch(rsgis::RSGISException &e)
        {
            throw rsgis::RSGISImageException(e.what());
        }
    }
    
    float RSGISGenAccuracyPoints::findPixelVal(GDALDataset *image, unsigned int band, double eastings, double northings, double tlX, double tlY, double xRes, double yRes, unsigned int xSize, unsigned int ySize) throw(rsgis::RSGISImageException)
    {
        double diffX = eastings - tlX;
        double diffY = tlY - northings;
        
        if(diffX < 0)
        {
            throw rsgis::RSGISImageException("Not found within the scene (xMin).");
        }
        
        if(diffY < 0)
        {
            throw rsgis::RSGISImageException("Not found within the scene (yMax).");
        }
        
        unsigned int xPxl = diffX/xRes;
        unsigned int yPxl = diffY/yRes;
        
        if(xPxl >= xSize)
        {
            throw rsgis::RSGISImageException("Not found within the scene (xMax).");
        }
        
        if(yPxl >= ySize)
        {
            throw rsgis::RSGISImageException("Not found within the scene (yMin).");
        }
        
        if((image->GetRasterCount() < band) | (band == 0))
        {
            throw rsgis::RSGISImageException("Band not within image.");
        }
        
        float val = 0.0;
        
        image->GetRasterBand(band)->RasterIO(GF_Read, xPxl, yPxl, 1, 1, &val, 1, 1, GDT_Float32, 0, 0);
        
        
        return val;
    }
    
    std::string RSGISGenAccuracyPoints::findClassVal(GDALDataset *image, unsigned int band, GDALRasterAttributeTable *attTable, unsigned int classNameColIdx, unsigned int xPxl, unsigned int yPxl) throw(rsgis::RSGISImageException)
    {
        if((image->GetRasterCount() < band) | (band == 0))
        {
            throw rsgis::RSGISImageException("Band not within image.");
        }
        
        if(xPxl > image->GetRasterXSize())
        {
            throw rsgis::RSGISImageException("Not found within the scene (xMax).");
        }
        
        if(yPxl > image->GetRasterYSize())
        {
            throw rsgis::RSGISImageException("Not found within the scene (yMin).");
        }
        
        int val = 0;
        
        image->GetRasterBand(band)->RasterIO(GF_Read, xPxl, yPxl, 1, 1, &val, 1, 1, GDT_Int32, 0, 0);
        
        if((val == 0) | (val > attTable->GetRowCount()))
        {
            throw rsgis::RSGISImageException("Row is not available.");
        }
        
        std::string className = std::string(attTable->GetValueAsString(val, classNameColIdx));
        
        return className;
    }
    
    std::list<std::string>* RSGISGenAccuracyPoints::findUniqueClasses(GDALRasterAttributeTable *attTable, unsigned int classNameColIdx) throw(rsgis::RSGISImageException)
    {
        std::list<std::string> *classes = new std::list<std::string>();
        
        std::string className = "";
        bool foundClassName = false;
        for(unsigned long i = 1; i < attTable->GetRowCount(); ++i)
        {
            className = attTable->GetValueAsString(i, classNameColIdx);
            foundClassName = false;
            
            if(classes->empty())
            {
                classes->push_back(className);
            }
            else
            {
                for(std::list<std::string>::iterator iterClasses = classes->begin(); iterClasses != classes->end(); ++iterClasses)
                {
                    if(*iterClasses == className)
                    {
                        foundClassName = true;
                        break;
                    }
                }
                
                if(!foundClassName)
                {
                    classes->push_back(className);
                }
            }
            
        }
        
        classes->sort(rsgis::utils::compareStringNoCase);
        
        return classes;
    }
    
    RSGISGenAccuracyPoints::~RSGISGenAccuracyPoints()
    {
        
    }
    
}}







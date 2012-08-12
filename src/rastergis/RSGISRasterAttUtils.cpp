/*
 *  RSGISRasterAttUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/08/2012.
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

#include "RSGISRasterAttUtils.h"

namespace rsgis{namespace rastergis{
	
    RSGISRasterAttUtils::RSGISRasterAttUtils()
    {
        
    }
    
    void RSGISRasterAttUtils::copyAttColumns(GDALDataset *inImage, GDALDataset *outImage, std::vector<std::string> fields) throw(RSGISAttributeTableException)
    {
        try 
        {
            std::cout << "Import attribute tables to memory.\n";
            const GDALRasterAttributeTable *gdalAttIn = inImage->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *gdalAttOut = NULL;//new GDALRasterAttributeTable(*outImage->GetRasterBand(1)->GetDefaultRAT());
            const GDALRasterAttributeTable *gdalAttOutTmp = outImage->GetRasterBand(1)->GetDefaultRAT();
            
            if((gdalAttOutTmp == NULL) || (gdalAttOutTmp->GetRowCount() == 0))
            {
                gdalAttOut = new GDALRasterAttributeTable();
            }
            else
            {
                gdalAttOut = new GDALRasterAttributeTable(*gdalAttOutTmp);
            }
            
            if(gdalAttIn == NULL)
            {
                rsgis::RSGISAttributeTableException("The input image does not have an attribute table.");
            }
            
            if(gdalAttIn->GetRowCount() > gdalAttOut->GetRowCount())
            {
                gdalAttOut->SetRowCount(gdalAttIn->GetRowCount());
            }
            
            std::cout << "Find field column indexes and created columns were required.\n";
            bool *foundInIdx = new bool[fields.size()];
            int *colInIdxs = new int[fields.size()];
            bool *foundOutIdx = new bool[fields.size()];
            int *colOutIdxs = new int[fields.size()];
            for(size_t i = 0; i < fields.size(); ++i)
            {
                foundInIdx[i] = false;
                colInIdxs[i] = 0;
                foundOutIdx[i] = false;
                colOutIdxs[i] = 0;
            }
            
            if(gdalAttIn->GetRowCount() == 0)
            {
                rsgis::RSGISAttributeTableException("There are no columns in the input attribute table.");
            }
            else
            {
                for(int i = 0; i < gdalAttIn->GetColumnCount(); ++i)
                {
                    for(size_t j = 0; j < fields.size(); ++j)
                    {
                        if(!foundInIdx[j] && (std::string(gdalAttIn->GetNameOfCol(i)) == fields.at(j)))
                        {
                            colInIdxs[j] = i;
                            foundInIdx[j] = true;
                        }
                    }
                }
                
                for(size_t j = 0; j < fields.size(); ++j)
                {
                    if(!foundInIdx[j])
                    {
                        std::string message = std::string("Column ") + fields.at(j) + std::string(" is not within the input attribute table.");
                        throw rsgis::RSGISAttributeTableException(message);
                    }
                }
                
                
                for(int i = 0; i < gdalAttOut->GetColumnCount(); ++i)
                {
                    for(size_t j = 0; j < fields.size(); ++j)
                    {
                        if(!foundOutIdx[j] && (std::string(gdalAttOut->GetNameOfCol(i)) == fields.at(j)))
                        {
                            colOutIdxs[j] = i;
                            foundOutIdx[j] = true;
                        }
                    }
                }
                
                for(size_t j = 0; j < fields.size(); ++j)
                {
                    if(!foundOutIdx[j])
                    {
                        if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
                        {
                            gdalAttOut->CreateColumn(fields.at(j).c_str(), GFT_Integer, GFU_Generic);
                        }
                        else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
                        {
                            gdalAttOut->CreateColumn(fields.at(j).c_str(), GFT_Real, GFU_Generic);
                        }
                        else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_String)
                        {
                            gdalAttOut->CreateColumn(fields.at(j).c_str(), GFT_String, GFU_Generic);
                        }
                        else
                        {
                            throw rsgis::RSGISAttributeTableException("Column data type was not recognised.");
                        }
                        colOutIdxs[j] = gdalAttOut->GetColumnCount()-1;
                        foundOutIdx[j] = true;
                    }
                }
            }
            
            std::cout << "Copying columns to the new attribute table\n";
            for(int i = 0; i < gdalAttIn->GetRowCount(); ++i)
            {
                for(size_t j = 0; j < fields.size(); ++j)
                {
                    if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
                    {
                        gdalAttOut->SetValue(i, colOutIdxs[j], gdalAttIn->GetValueAsInt(i, colInIdxs[j]));
                    }
                    else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
                    {
                        gdalAttOut->SetValue(i, colOutIdxs[j], gdalAttIn->GetValueAsDouble(i, colInIdxs[j]));
                    }
                    else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_String)
                    {
                        gdalAttOut->SetValue(i, colOutIdxs[j], gdalAttIn->GetValueAsString(i, colInIdxs[j]));
                    }
                    else
                    {
                        throw rsgis::RSGISAttributeTableException("Column data type was not recognised.");
                    }
                }
            }
            
            std::cout << "Adding RAT to output file.\n";
            outImage->GetRasterBand(1)->SetDefaultRAT(gdalAttOut);
            
            
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }        
    }
    
    void RSGISRasterAttUtils::copyColourForCats(GDALDataset *catsImage, GDALDataset *classImage, std::string classField) throw(RSGISAttributeTableException)
    {
        try 
        {
            std::cout << "Import attribute tables to memory.\n";
            const GDALRasterAttributeTable *gdalAttIn = catsImage->GetRasterBand(1)->GetDefaultRAT();
            GDALRasterAttributeTable *gdalAttClumps = NULL;//new GDALRasterAttributeTable(*outImage->GetRasterBand(1)->GetDefaultRAT());
            const GDALRasterAttributeTable *gdalAttClasses = classImage->GetRasterBand(1)->GetDefaultRAT();
            
            if((gdalAttIn == NULL) || (gdalAttIn->GetRowCount() == 0))
            {
                throw rsgis::RSGISAttributeTableException("The clumps image does not have an attribute table.");
            }
            else
            {
                gdalAttClumps = new GDALRasterAttributeTable(*gdalAttIn);
            }
            
            if((gdalAttClasses == NULL) || (gdalAttClasses->GetRowCount() == 0))
            {
                throw rsgis::RSGISAttributeTableException("The classes image does not have an attribute table.");
            }
            
            std::cout << "Find field column indexes and created columns were required.\n";
            bool foundClassIdx = false;
            int classIdx = 0;
            bool inRedFound = false;
            int inRedIdx = 0;
            bool inGreenFound = false;
            int inGreenIdx = 0;
            bool inBlueFound = false;
            int inBlueIdx = 0;
            bool inAlphaFound = false;
            int inAlphaIdx = 0;
            
            bool outRedFound = false;
            int outRedIdx = 0;
            bool outGreenFound = false;
            int outGreenIdx = 0;
            bool outBlueFound = false;
            int outBlueIdx = 0;
            bool outAlphaFound = false;
            int outAlphaIdx = 0;
            
            
            if(gdalAttIn->GetRowCount() == 0)
            {
                rsgis::RSGISAttributeTableException("There are no columns in the input attribute table.");
            }
            else
            {
                for(int i = 0; i < gdalAttClasses->GetColumnCount(); ++i)
                {
                    if(!inRedFound && (std::string(gdalAttClasses->GetNameOfCol(i)) == "Red"))
                    {
                        inRedIdx = i;
                        inRedFound = true;
                    }
                    else if(!inGreenFound && (std::string(gdalAttClasses->GetNameOfCol(i)) == "Green"))
                    {
                        inGreenIdx = i;
                        inGreenFound = true;
                    }
                    else if(!inBlueFound && (std::string(gdalAttClasses->GetNameOfCol(i)) == "Blue"))
                    {
                        inBlueIdx = i;
                        inBlueFound = true;
                    }
                    else if(!inAlphaFound && (std::string(gdalAttClasses->GetNameOfCol(i)) == "Alpha"))
                    {
                        inAlphaIdx = i;
                        inAlphaFound = true;
                    }
                }
                
                if(!inRedFound)
                {
                    throw rsgis::RSGISAttributeTableException("A \'Red\' column was not within the input classes table.");
                }
                
                if(!inGreenFound)
                {
                    throw rsgis::RSGISAttributeTableException("A \'Green\' column was not within the input classes table.");
                }
                
                if(!inBlueFound)
                {
                    throw rsgis::RSGISAttributeTableException("A \'Blue\' column was not within the input classes table.");
                }
                
                if(!inAlphaFound)
                {
                    throw rsgis::RSGISAttributeTableException("A \'Alpha\' column was not within the input classes table.");
                }
                
                
                for(int i = 0; i < gdalAttClumps->GetColumnCount(); ++i)
                {
                    if(!foundClassIdx && (std::string(gdalAttClumps->GetNameOfCol(i)) == classField))
                    {
                        classIdx = i;
                        foundClassIdx = true;
                    }
                    else if(!outRedFound && (std::string(gdalAttClumps->GetNameOfCol(i)) == "Red"))
                    {
                        outRedIdx = i;
                        outRedFound = true;
                    }
                    else if(!outGreenFound && (std::string(gdalAttClumps->GetNameOfCol(i)) == "Green"))
                    {
                        outGreenIdx = i;
                        outGreenFound = true;
                    }
                    else if(!outBlueFound && (std::string(gdalAttClumps->GetNameOfCol(i)) == "Blue"))
                    {
                        outBlueIdx = i;
                        outBlueFound = true;
                    }
                    else if(!outAlphaFound && (std::string(gdalAttClumps->GetNameOfCol(i)) == "Alpha"))
                    {
                        outAlphaIdx = i;
                        outAlphaFound = true;
                    }
                } 
                
                if(!foundClassIdx)
                {
                    throw rsgis::RSGISAttributeTableException("The class field column was not within the category table.");
                }
                
                if(!outRedFound)
                {
                    gdalAttClumps->CreateColumn("Red", GFT_Integer, GFU_Red);
                    outRedFound = true;
                    outRedIdx = gdalAttClumps->GetColumnCount()-1;
                }
                
                if(!outGreenFound)
                {
                    gdalAttClumps->CreateColumn("Green", GFT_Integer, GFU_Green);
                    outGreenFound = true;
                    outGreenIdx = gdalAttClumps->GetColumnCount()-1;
                }
                
                if(!outBlueFound)
                {
                    gdalAttClumps->CreateColumn("Blue", GFT_Integer, GFU_Blue);
                    outBlueFound = true;
                    outBlueIdx = gdalAttClumps->GetColumnCount()-1;
                }
                
                if(!outAlphaFound)
                {
                    gdalAttClumps->CreateColumn("Alpha", GFT_Integer, GFU_Alpha);
                    outAlphaFound = true;
                    outAlphaIdx = gdalAttClumps->GetColumnCount()-1;
                }
                
                
                
            }
            
            int classID = 0;
            int redVal = 0;
            int greenVal = 0;
            int blueVal = 0;
            int alphaVal = 0;
            std::cout << "Copying the colours across\n";
            for(int i = 0; i < gdalAttClumps->GetRowCount(); ++i)
            {
                classID = gdalAttClumps->GetValueAsInt(i, classIdx);
                redVal = gdalAttClasses->GetValueAsInt(classID, inRedIdx);
                greenVal = gdalAttClasses->GetValueAsInt(classID, inGreenIdx);
                blueVal = gdalAttClasses->GetValueAsInt(classID, inBlueIdx);
                alphaVal = gdalAttClasses->GetValueAsInt(classID, inAlphaIdx);
                
                gdalAttClumps->SetValue(i, outRedIdx, redVal);
                gdalAttClumps->SetValue(i, outGreenIdx, greenVal);
                gdalAttClumps->SetValue(i, outBlueIdx, blueVal);
                gdalAttClumps->SetValue(i, outAlphaIdx, alphaVal);
            }
                 
            std::cout << "Adding RAT to output file.\n";
            catsImage->GetRasterBand(1)->SetDefaultRAT(gdalAttClumps);
            
            
        }
        catch (RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    void RSGISRasterAttUtils::exportColumns2ASCII(GDALDataset *inImage, std::string outputFile, std::vector<std::string> fields) throw(RSGISAttributeTableException)
    {
        try
        {
            std::cout << "Import attribute table to memory.\n";
            const GDALRasterAttributeTable *gdalAttIn = inImage->GetRasterBand(1)->GetDefaultRAT();
            
            if(gdalAttIn == NULL)
            {
                rsgis::RSGISAttributeTableException("The input image does not have an attribute table.");
            }
            
            std::cout << "Find field column indexes in RAT.\n";
            bool *foundInIdx = new bool[fields.size()];
            int *colInIdxs = new int[fields.size()];
            for(size_t i = 0; i < fields.size(); ++i)
            {
                foundInIdx[i] = false;
                colInIdxs[i] = 0;
            }
            
            if(gdalAttIn->GetRowCount() == 0)
            {
                rsgis::RSGISAttributeTableException("There are no columns in the input attribute table.");
            }
            else
            {
                for(int i = 0; i < gdalAttIn->GetColumnCount(); ++i)
                {
                    for(size_t j = 0; j < fields.size(); ++j)
                    {
                        if(!foundInIdx[j] && (std::string(gdalAttIn->GetNameOfCol(i)) == fields.at(j)))
                        {
                            colInIdxs[j] = i;
                            foundInIdx[j] = true;
                        }
                    }
                }
                
                for(size_t j = 0; j < fields.size(); ++j)
                {
                    if(!foundInIdx[j])
                    {
                        std::string message = std::string("Column ") + fields.at(j) + std::string(" is not within the input attribute table.");
                        throw rsgis::RSGISAttributeTableException(message);
                    }
                }                
            }
            
            std::cout << "Copying columns to the ASCII file.\n";
            std::ofstream outFile;
            outFile.open(outputFile.c_str());
            if(outFile.is_open())
            {
                outFile.precision(12);
                
                for(size_t j = 0; j < fields.size(); ++j)
                {
                    if(j != 0)
                    {
                        outFile << ",";
                    }
                    outFile << gdalAttIn->GetNameOfCol(colInIdxs[j]);
                }
                outFile << std::endl;
                
                size_t numRows = gdalAttIn->GetRowCount();
                unsigned int feedbackStep = numRows/10;
                unsigned int feedback = 0;
                std::cout << "Started." << std::flush;
                for(int i = 0; i < numRows; ++i)
                {
                    if((numRows > 20) && (i % feedbackStep == 0))
                    {
                        std::cout << "." << feedback << "." << std::flush;
                        feedback += 10;
                    }
                    
                    for(size_t j = 0; j < fields.size(); ++j)
                    {
                        if(j != 0)
                        {
                            outFile << ",";
                        }
                        if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Integer)
                        {
                            outFile << gdalAttIn->GetValueAsInt(i, colInIdxs[j]);
                        }
                        else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_Real)
                        {
                            outFile << floor((gdalAttIn->GetValueAsDouble(i, colInIdxs[j]) * 1000)+0.5) / 1000;
                        }
                        else if(gdalAttIn->GetTypeOfCol(colInIdxs[j]) == GFT_String)
                        {
                            outFile << "\"" << gdalAttIn->GetValueAsString(i, colInIdxs[j]) << "\"" ;
                        }
                        else
                        {
                            throw rsgis::RSGISAttributeTableException("Column data type was not recognised.");
                        }                        
                    }
                    outFile << std::endl;
                }
                std::cout << ".Completed\n";
                outFile.flush();
                outFile.close();
            }
            else
            {
                rsgis::RSGISAttributeTableException("Could not open the specified output ASCII file.");
            }
            
            
        }
        catch (rsgis::RSGISAttributeTableException &e)
        {
            throw e;
        }
    }
    
    RSGISRasterAttUtils::~RSGISRasterAttUtils()
    {
        
    }
	
}}




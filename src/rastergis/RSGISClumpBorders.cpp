/*
 *  RSGISClumpBorders.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 20/03/2013.
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

#include "RSGISClumpBorders.h"

namespace rsgis{namespace rastergis{

    RSGISClumpBorders::RSGISClumpBorders()
    {
        
    }
    
    void RSGISClumpBorders::calcClumpBorderLength(GDALDataset *clumpImage, bool includeZeroEdges, std::string colName) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            RSGISRasterAttUtils attUtils;
            const GDALRasterAttributeTable *attTableTmp = clumpImage->GetRasterBand(1)->GetDefaultRAT();
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
            clumpImage->GetRasterBand(1)->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
            
            if(maxVal > numRows)
            {
                attTable->SetRowCount(maxVal+1);
            }
            numRows = attTable->GetRowCount();
            
            double *trans = new double[6];
            clumpImage->GetGeoTransform(trans);
            
            double xRes = trans[1];
            double yRes = trans[5];
            if(yRes < 0)
            {
                yRes = yRes * (-1);
            }
            delete[] trans;
            
            double *borderLenPxls = new double[numRows];
            for(size_t i = 0; i < numRows; ++i)
            {
                borderLenPxls[i] = 0;
            }
            
            rsgis::img::RSGISCalcImageValue *calcBorderLen = new RSGISCalcBorderLenInPixels(borderLenPxls, numRows, xRes, yRes, includeZeroEdges);
            rsgis::img::RSGISCalcImage imgCalc = rsgis::img::RSGISCalcImage(calcBorderLen);
            
            imgCalc.calcImageWindowData(&clumpImage, 1, 3);
            
            
            unsigned int borderLenColIdx = attUtils.findColumnIndexOrCreate(attTable, colName, GFT_Real);
            
            for(size_t i = 0; i < numRows; ++i)
            {
                attTable->SetValue(i, borderLenColIdx, borderLenPxls[i]);
            }
            clumpImage->GetRasterBand(1)->SetDefaultRAT(attTable);
            
            delete[] borderLenPxls;
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    void RSGISClumpBorders::calcClumpRelBorderLen2Class(GDALDataset *clumpImage, bool includeZeroEdges, std::string colName, std::string classColName, std::string className) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            RSGISRasterAttUtils attUtils;
            const GDALRasterAttributeTable *attTableTmp = clumpImage->GetRasterBand(1)->GetDefaultRAT();
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
            clumpImage->GetRasterBand(1)->GetStatistics(false, true, NULL, &maxVal, NULL, NULL);
            
            if(maxVal > numRows)
            {
                attTable->SetRowCount(maxVal+1);
            }
            numRows = attTable->GetRowCount();
            
            double *trans = new double[6];
            clumpImage->GetGeoTransform(trans);
            
            double xRes = trans[1];
            double yRes = trans[5];
            if(yRes < 0)
            {
                yRes = yRes * (-1);
            }
            delete[] trans;
            
            double *borderLen = new double[numRows];
            double *classborderLen = new double[numRows];
            for(size_t i = 0; i < numRows; ++i)
            {
                borderLen[i] = 0;
                classborderLen[i] = 0;
            }
            
            unsigned int classNameColIdx = attUtils.findColumnIndex(attTable, classColName);
            
            rsgis::img::RSGISCalcImageValue *calcBorderLen = new RSGISCalcBorderLenInPixelsWithClass(borderLen, classborderLen, numRows, xRes, yRes, className, classNameColIdx, attTable, includeZeroEdges);
            rsgis::img::RSGISCalcImage imgCalc = rsgis::img::RSGISCalcImage(calcBorderLen);
            
            imgCalc.calcImageWindowData(&clumpImage, 1, 3);
            
            
            unsigned int relBorderLenColIdx = attUtils.findColumnIndexOrCreate(attTable, colName, GFT_Real);
            
            for(size_t i = 0; i < numRows; ++i)
            {
                if(classborderLen[i] == 0)
                {
                    attTable->SetValue(i, relBorderLenColIdx, 0.0);
                }
                else
                {
                    attTable->SetValue(i, relBorderLenColIdx, classborderLen[i]/borderLen[i]);
                }
            }
            clumpImage->GetRasterBand(1)->SetDefaultRAT(attTable);
            
            delete[] borderLen;
            delete[] classborderLen;
        }
        catch (rsgis::img::RSGISImageCalcException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::img::RSGISImageCalcException(e.what());
        }
    }
    
    RSGISClumpBorders::~RSGISClumpBorders()
    {
        
    }



    
    RSGISCalcBorderLenInPixels::RSGISCalcBorderLenInPixels(double *borderLen, size_t numRows, double xRes, double yRes, bool includeZeros) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->borderLen = borderLen;
        this->numRows = numRows;
        this->includeZeros = includeZeros;
        this->xRes = xRes;
        this->yRes = yRes;
    }
    
    void RSGISCalcBorderLenInPixels::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            int winHsize = ((winSize-1)/2);
            
            if((dataBlock[0][winHsize][winHsize] > 0) & (dataBlock[0][winHsize][winHsize] < numRows))
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
                
                if(fidLeft != fid)
                {
                    if(includeZeros)
                    {
                        borderLen[fid] += xRes;
                    }
                    else if(fidLeft != 0)
                    {
                        borderLen[fid] += xRes;
                    }
                }
                
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
                
                if(fidUp != fid)
                {
                    if(includeZeros)
                    {
                        borderLen[fid] += yRes;
                    }
                    else if(fidUp != 0)
                    {
                        borderLen[fid] += yRes;
                    }
                }
                
                
                
                
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
                
                if(fidRight != fid)
                {
                    if(includeZeros)
                    {
                        borderLen[fid] += xRes;
                    }
                    else if(fidRight != 0)
                    {
                        borderLen[fid] += xRes;
                    }
                }
                
                
                
                
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
                
                if(fidDown != fid)
                {
                    if(includeZeros)
                    {
                        borderLen[fid] += yRes;
                    }
                    else if(fidDown != 0)
                    {
                        borderLen[fid] += yRes;
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
    
    RSGISCalcBorderLenInPixels::~RSGISCalcBorderLenInPixels()
    {
        
    }
    
    
    
    
    
    
    
    
    RSGISCalcBorderLenInPixelsWithClass::RSGISCalcBorderLenInPixelsWithClass(double *borderLen, double *classBorderLen, size_t numRows, double xRes, double yRes, std::string className, unsigned int classColIdx, const GDALRasterAttributeTable *attTable, bool includeZeros) : rsgis::img::RSGISCalcImageValue(0)
    {
        this->borderLen = borderLen;
        this->classBorderLen = classBorderLen;
        this->numRows = numRows;
        this->includeZeros = includeZeros;
        this->xRes = xRes;
        this->yRes = yRes;
        this->className = className;
        this->classColIdx = classColIdx;
        this->attTable = attTable;
    }
    
    void RSGISCalcBorderLenInPixelsWithClass::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException)
    {
        try
        {
            int winHsize = ((winSize-1)/2);
            
            if((dataBlock[0][winHsize][winHsize] > 0) & (dataBlock[0][winHsize][winHsize] < numRows))
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
                
                bool fidOfClass = false;
                if(className == attTable->GetValueAsString(fid, classColIdx))
                {
                    fidOfClass = true;
                }
                
                
                
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
                
                if(fidLeft != fid)
                {
                    if(includeZeros)
                    {
                        borderLen[fid] += xRes;
                    }
                    else if(fidLeft != 0)
                    {
                        borderLen[fid] += xRes;
                    }
                    
                    if(!fidOfClass)
                    {
                        if(className == attTable->GetValueAsString(fidLeft, classColIdx))
                        {
                            if(includeZeros)
                            {
                                classBorderLen[fid] += xRes;
                            }
                            else if(fidLeft != 0)
                            {
                                classBorderLen[fid] += xRes;
                            }
                        }
                    }
                }
                
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
                
                if(fidUp != fid)
                {
                    if(includeZeros)
                    {
                        borderLen[fid] += yRes;
                    }
                    else if(fidUp != 0)
                    {
                        borderLen[fid] += yRes;
                    }
                    
                    if(!fidOfClass)
                    {
                        if(className == attTable->GetValueAsString(fidUp, classColIdx))
                        {
                            if(includeZeros)
                            {
                                classBorderLen[fid] += yRes;
                            }
                            else if(fidLeft != 0)
                            {
                                classBorderLen[fid] += yRes;
                            }
                        }
                    }
                }
                
                
                
                
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
                
                if(fidRight != fid)
                {
                    if(includeZeros)
                    {
                        borderLen[fid] += xRes;
                    }
                    else if(fidRight != 0)
                    {
                        borderLen[fid] += xRes;
                    }
                    
                    if(!fidOfClass)
                    {
                        if(className == attTable->GetValueAsString(fidRight, classColIdx))
                        {
                            if(includeZeros)
                            {
                                classBorderLen[fid] += xRes;
                            }
                            else if(fidLeft != 0)
                            {
                                classBorderLen[fid] += xRes;
                            }
                        }
                    }
                }
                
                
                
                
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
                
                if(fidDown != fid)
                {
                    if(includeZeros)
                    {
                        borderLen[fid] += yRes;
                    }
                    else if(fidDown != 0)
                    {
                        borderLen[fid] += yRes;
                    }
                    
                    if(!fidOfClass)
                    {
                        if(className == attTable->GetValueAsString(fidDown, classColIdx))
                        {
                            if(includeZeros)
                            {
                                classBorderLen[fid] += yRes;
                            }
                            else if(fidLeft != 0)
                            {
                                classBorderLen[fid] += yRes;
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
    
    RSGISCalcBorderLenInPixelsWithClass::~RSGISCalcBorderLenInPixelsWithClass()
    {
        
    }


}}

/*
 *  RSGISCmdImageUtils.cpp
 *
 *
 *  Created by Pete Bunting on 03/05/2013.
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

#include "RSGISCmdImageUtils.h"
#include "RSGISCmdParent.h"

#include "geos/geom/Envelope.h"

#include "common/RSGISImageException.h"

#include "img/RSGISBandMath.h"
#include "img/RSGISImageMaths.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISCopyImage.h"
#include "img/RSGISStretchImage.h"
#include "img/RSGISMaskImage.h"
#include "img/RSGISImageMosaic.h"
#include "img/RSGISPopWithStats.h"


namespace rsgis{ namespace cmds {

    void executeStretchImage(std::string inputImage, std::string outputImage, bool saveOutStats, std::string outStatsFile, bool ignoreZeros, bool onePassSD, std::string gdalFormat, RSGISLibDataType outDataType, RSGISStretches stretchType, float stretchParam)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();

            GDALDataset *inDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }

            rsgis::img::RSGISStretchImage stretchImg = rsgis::img::RSGISStretchImage(inDataset, outputImage, saveOutStats, outStatsFile, ignoreZeros, onePassSD, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            if(stretchType == linearMinMax)
            {
                stretchImg.executeLinearMinMaxStretch();
            }
            else if(stretchType == linearPercent)
            {
                stretchImg.executeLinearPercentStretch(stretchParam);
            }
            else if(stretchType == linearStdDev)
            {
                stretchImg.executeLinearStdDevStretch(stretchParam);
            }
            else if(stretchType == histogram)
            {
                stretchImg.executeHistogramStretch();
            }
            else if(stretchType == exponential)
            {
                stretchImg.executeExponentialStretch();
            }
            else if(stretchType == logarithmic)
            {
                stretchImg.executeLogrithmicStretch();
            }
            else if(stretchType == powerLaw)
            {
                stretchImg.executePowerLawStretch(stretchParam);
            }
            else
            {
                throw RSGISException("Stretch is not recognised.");
            }

            GDALClose(inDataset);
            GDALDestroyDriverManager();
        }
        catch(RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeStretchImageWithStats(std::string inputImage, std::string outputImage, std::string inStatsFile, std::string gdalFormat, RSGISLibDataType outDataType, RSGISStretches stretchType, float stretchParam)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();

            GDALDataset *inDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }

            rsgis::img::RSGISStretchImageWithStats stretchImg = rsgis::img::RSGISStretchImageWithStats(inDataset, outputImage, inStatsFile, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
            if(stretchType == linearMinMax)
            {
                stretchImg.executeLinearMinMaxStretch();
            }
            else if(stretchType == histogram)
            {
                stretchImg.executeHistogramStretch();
            }
            else if(stretchType == exponential)
            {
                stretchImg.executeExponentialStretch();
            }
            else if(stretchType == logarithmic)
            {
                stretchImg.executeLogrithmicStretch();
            }
            else if(stretchType == powerLaw)
            {
                stretchImg.executePowerLawStretch(stretchParam);
            }
            else
            {
                throw RSGISException("Stretch is not recognised.");
            }

            GDALClose(inDataset);
            GDALDestroyDriverManager();
        }
        catch(RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeMaskImage(std::string inputImage, std::string imageMask, std::string outputImage, std::string gdalFormat, RSGISLibDataType outDataType, float outValue, float maskValue)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }

            GDALDataset *mask = (GDALDataset *) GDALOpenShared(imageMask.c_str(), GA_ReadOnly);
            if(mask == NULL)
            {
                std::string message = std::string("Could not open image ") + imageMask;
                throw RSGISImageException(message.c_str());
            }

            rsgis::img::RSGISMaskImage maskImage =  rsgis::img::RSGISMaskImage();
            maskImage.maskImage(dataset, mask, outputImage, gdalFormat, RSGIS_to_GDAL_Type(outDataType), outValue, maskValue);

            GDALClose(dataset);
            GDALClose(mask);
            GDALDestroyDriverManager();
        }
        catch(RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeCreateTiles(std::string inputImage, std::string outputImageBase, float width, float height, float tileOverlap, bool offsetTiling, std::string gdalFormat, RSGISLibDataType outDataType, std::string outFileExtension, std::vector<std::string> *outFileNames)throw(RSGISCmdException)
    {
        GDALAllRegister();
        OGRRegisterAll();

        GDALDataset **dataset = NULL;

        rsgis::img::RSGISImageUtils imgUtils;
        rsgis::img::RSGISCopyImage *copyImage = NULL;
        rsgis::img::RSGISCalcImage *calcImage = NULL;

        int numImageBands = 0;
        std::string outputFilePath;

        try
        {
            // Open Image
            dataset = new GDALDataset*[1];
            //cout << this->inputImage << endl;
            dataset[0] = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
            if(dataset[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISCmdException(message.c_str());
            }
            numImageBands = dataset[0]->GetRasterCount();
            std::cout << "Raster Band Count = " << numImageBands << std::endl;

            // Set up envlopes for image tiles
            std::vector<geos::geom::Envelope*> *tileEnvelopes = new std::vector<geos::geom::Envelope*>;

            int numDS = 1;
            double *gdalTransform = new double[6];
            int **dsOffsets = new int*[numDS];
            for(int i = 0; i < numDS; i++)
            {
                dsOffsets[i] = new int[2];
            }
            int imgHeight = 0;
            int imgWidth = 0;

            imgUtils.getImageOverlap(dataset, numDS, dsOffsets, &imgWidth, &imgHeight, gdalTransform);

            double pixelXRes = gdalTransform[1];
            double pixelYRes = gdalTransform[5];

            // Get absolute minimum and maximum values from image
            double imageMinX = gdalTransform[0];
            double imageMaxY = gdalTransform[3];
            double imageMaxX = imageMinX + (imgWidth * pixelXRes);
            double imageMinY = imageMaxY + (imgHeight * pixelYRes);

            // Get minimum and maximum images to use for tile grid
            double minX = imageMinX;
            double maxX = imageMaxX;
            double minY = imageMinY;
            double maxY = imageMaxY;

            if(offsetTiling)
            {
                minX -= (width * pixelXRes)/2;
                maxX += (width * pixelXRes)/2;
                if((height * pixelYRes) > 0)
                {
                    minY -= (height * pixelYRes)/2;
                    maxY += (height * pixelYRes)/2;
                }
                else
                {
                    minY += (height * pixelYRes)/2;
                    maxY -= (height * pixelYRes)/2;
                }

            }

            double tileWidthMapUnits = width * pixelXRes;
            double tileHeightMapUnits = height*pixelYRes;
            if(tileHeightMapUnits < 0){tileHeightMapUnits = tileHeightMapUnits*-1;}  // Max y resolution positive (makes things simpler)
            double tileXOverlapMapUnits = tileOverlap * pixelXRes;
            double tileYOverlapMapUnits = tileOverlap * abs(pixelYRes);

            double xStart = 0;
            double yStart = 0;
            double yEnd = 0;
            double xEnd = 0;
            double xStartOverlap = 0;
            double yStartOverlap = 0;
            double xEndOverlap = 0;
            double yEndOverlap = 0;

            std::cout << "Tile Width: " << width << " pixels (" <<  tileWidthMapUnits << " map units)" << std::endl;
            std::cout << "Tile Height: " << height << " pixels (" <<  tileHeightMapUnits << " map units)" << std::endl;
            std::cout << "Tile Overlap: " << tileOverlap << " pixels (" <<  tileXOverlapMapUnits << " map units)" << std::endl;
            if(offsetTiling)
            {
                std::cout << "Tiling is offset by half a tile.\n";
            }

            // Start at top left corner and work down (minX, maxY)
            for(xStart = minX; xStart < maxX; xStart+=tileWidthMapUnits)
            {
                xEnd = xStart + tileWidthMapUnits;
                xStartOverlap = xStart - tileXOverlapMapUnits;
                xEndOverlap = xEnd + tileXOverlapMapUnits;

                if(xStartOverlap < imageMinX) // Check tile will fit within image
                {
                    xStartOverlap = imageMinX;
                }
                if(xEndOverlap > imageMaxX) // Check tile will fit within image
                {
                    xEndOverlap = imageMaxX;
                }

                if((xEndOverlap > imageMinX) && (xStartOverlap < xEndOverlap)) // Check x extent is within image (min and max), don't run if not
                {
                    for(yStart = maxY; yStart > minY; yStart-=tileHeightMapUnits)
                    {
                        yEnd = yStart - tileHeightMapUnits;

                        yStartOverlap = yStart + tileYOverlapMapUnits;
                        yEndOverlap = yEnd - tileYOverlapMapUnits;

                        if(yStartOverlap > imageMaxY) // Check tile will fit within image
                        {
                            yStartOverlap = imageMaxY+(0.5*abs(pixelYRes));
                        }
                        if(yEndOverlap < imageMinY) // Check tile will fit within image
                        {
                            yEndOverlap = imageMinY+(0.5*abs(pixelYRes));
                        }
                        if((yStartOverlap > imageMinY) && (yStartOverlap > yEndOverlap)) // Check y extent is within image (min and max), don't run if not
                        {
                            tileEnvelopes->push_back(new geos::geom::Envelope(xStartOverlap, xEndOverlap, yStartOverlap, yEndOverlap));
                        }
                    }
                }
            }

            copyImage = new rsgis::img::RSGISCopyImage(numImageBands);
            calcImage = new rsgis::img::RSGISCalcImage(copyImage, "", true);

            for(unsigned int i = 0; i < tileEnvelopes->size(); ++i)
            {
                std::cout << "Tile " << i+1 << "/" << tileEnvelopes->size() << std::endl;
                outputFilePath = outputImageBase + "_tile" + boost::lexical_cast<std::string>(i) + "." + outFileExtension;
                try
                {
                    calcImage->calcImageInEnv(dataset, 1, outputFilePath, tileEnvelopes->at(i), false, NULL, gdalFormat, RSGIS_to_GDAL_Type(outDataType));
                    // Save out file name to vector
                    if(outFileNames != NULL){outFileNames->push_back(outputFilePath);}
                }
                catch (rsgis::img::RSGISImageBandException e)
                {
                    throw RSGISCmdException(e.what());
                }
            }

            GDALClose(dataset[0]);
            delete[] dataset;
            GDALDestroyDriverManager();
            delete calcImage;
            delete copyImage;
        }
        catch(rsgis::RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executePopulateImgStats(std::string inputImage, bool useIgnoreVal, float nodataValue, bool calcImgPyramids)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }

            rsgis::img::RSGISPopWithStats popWithStats;
            popWithStats.calcPopStats( inDataset, useIgnoreVal, nodataValue, calcImgPyramids );


            GDALClose(inDataset);
            GDALDestroyDriverManager();
        }
        catch(rsgis::RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }

    void executeImageMosaic(std::string *inputImages, int numDS, std::string outputImage, float background, float skipVal, unsigned int skipBand, unsigned int overlapBehaviour, std::string format, RSGISLibDataType outDataType) throw(RSGISCmdException)
    {
        try
        {
            rsgis::img::RSGISImageMosaic *mosaic = NULL;
            mosaic = new rsgis::img::RSGISImageMosaic();
            // Projection hardcoded to from image (to simplify interface)
            mosaic->mosaicSkipVals(inputImages, numDS, outputImage, background, skipVal, true, "", skipBand, overlapBehaviour, format, RSGIS_to_GDAL_Type(outDataType));
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    
    void executeAssignProj(std::string inputImage, std::string wktStr, bool readWKTFromFile, std::string wktFile)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            if(readWKTFromFile)
            {
                rsgis::utils::RSGISTextUtils textUtils;
                wktStr = textUtils.readFileToString(wktFile);
            }
            
            inDataset->SetProjection(wktStr.c_str());
            
            GDALClose(inDataset);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeAssignSpatialInfo(std::string inputImage, double xTL, double yTL, double xRes, double yRes, double xRot, double yRot, bool xTLDef, bool yTLDef, bool xResDef, bool yResDef, bool xRotDef, bool yRotDef)throw(RSGISCmdException)
    {
        try
        {
            std::cout.precision(12);
            GDALAllRegister();
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            double *trans = new double[6];
            inDataset->GetGeoTransform(trans);
            
            if(xTLDef)
            {
                trans[0] = xTL;
            }
            if(yTLDef)
            {
                trans[3] = yTL;
            }
            if(xResDef)
            {
                trans[1] = xRes;
            }
            if(yResDef)
            {
                trans[5] = yRes;
            }
            if(xRotDef)
            {
                trans[2] = xRot;
            }
            if(yRotDef)
            {
                trans[4] = yRot;
            }
            
            std::cout << "TL: [" << trans[0] << "," << trans[3] << "]" << std::endl;
            std::cout << "RES: [" << trans[1] << "," << trans[5] << "]" << std::endl;
            std::cout << "ROT: [" << trans[2] << "," << trans[4] << "]" << std::endl;
            
            inDataset->SetGeoTransform(trans);
            
            GDALClose(inDataset);
            delete trans;
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeCopyProj(std::string inputImage, std::string refImageFile)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *refDataset = (GDALDataset *) GDALOpen(refImageFile.c_str(), GA_ReadOnly);
            if(refDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + refImageFile;
                throw RSGISImageException(message.c_str());
            }
            
            std::string wktString = std::string(refDataset->GetProjectionRef());
            
            GDALClose(refDataset);
            
            executeAssignProj(inputImage, wktString);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeCopyProjSpatial(std::string inputImage, std::string refImageFile)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *refDataset = (GDALDataset *) GDALOpen(refImageFile.c_str(), GA_ReadOnly);
            if(refDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + refImageFile;
                throw RSGISImageException(message.c_str());
            }
            
            std::string wktString = std::string(refDataset->GetProjectionRef());
            
            double *trans = new double[6];
            refDataset->GetGeoTransform(trans);
            
            GDALClose(refDataset);
            
            
            GDALDataset *inDataset = NULL;
            inDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_Update);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISImageException(message.c_str());
            }
            
            inDataset->SetGeoTransform(trans);
            inDataset->SetProjection(wktString.c_str());
            
            delete[] trans;
            GDALClose(inDataset);
        }
        catch (RSGISImageException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }


}}


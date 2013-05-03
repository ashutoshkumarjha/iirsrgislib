/*
 *  RSGISCmdSegmentation.cpp
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

#include "RSGISCmdSegmentation.h"
#include "RSGISCmdParent.h"

#include "common/RSGISImageException.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"
#include "img/RSGISStretchImage.h"

#include "segmentation/RSGISLabelPixelsUsingClusters.h"
#include "segmentation/RSGISEliminateSinglePixels.h"
#include "segmentation/RSGISClumpPxls.h"
#include "segmentation/RSGISEliminateSmallClumps.h"


namespace rsgis{ namespace cmds {
    
    void executeLabelPixelsFromClusterCentres(std::string inputImage, std::string outputImage, std::string clusterCentresFile, bool ignoreZeros, std::string imageFormat)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset **datasets = new GDALDataset*[1];
            datasets[0] = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(datasets[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::segment::RSGISLabelPixelsUsingClusters labelPixels;
            labelPixels.labelPixelsUsingClusters(datasets, 1, outputImage, clusterCentresFile, ignoreZeros, imageFormat, true, "");
            
            // Tidy up
            GDALClose(datasets[0]);
            GDALDestroyDriverManager();
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeEliminateSinglePixels(std::string inputImage, std::string clumpsImage, std::string outputImage, std::string tempImage, std::string imageFormat, bool processInMemory, bool ignoreZeros)throw(RSGISCmdException)
    {
        try
        {
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALAllRegister();
            GDALDataset *spectralDataset = NULL;
            spectralDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(spectralDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            GDALDataset *clumpsDataset = NULL;
            clumpsDataset = (GDALDataset *) GDALOpen(clumpsImage.c_str(), GA_ReadOnly);
            if(clumpsDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            GDALDataset *pixelMaskDataset = NULL;
            if(processInMemory)
            {
                pixelMaskDataset = imgUtils.createCopy(clumpsDataset, 1, "", "MEM", GDT_Byte , true, "");
            }
            else
            {
                pixelMaskDataset = imgUtils.createCopy(clumpsDataset, 1, tempImage, imageFormat, GDT_Byte, true, "");
            }
            
            
            std::cout << "Eliminating Individual Pixels\n";
            rsgis::segment::RSGISEliminateSinglePixels eliminate;
            eliminate.eliminateBlocks(spectralDataset, clumpsDataset, pixelMaskDataset, outputImage, 0, ignoreZeros, true, "", imageFormat);
            
            clumpsDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            // Tidy up
            GDALClose(spectralDataset);
            GDALClose(clumpsDataset);
            GDALClose(pixelMaskDataset);
            GDALDestroyDriverManager();
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::cmds::RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::cmds::RSGISCmdException(e.what());
        }
    }
    
    void executeClump(std::string inputImage, std::string outputImage, std::string imageFormat, bool processInMemory, bool noDataValProvided, float noDataVal) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALDataset *catagoryDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(processInMemory)
            {
                std::cout << "Processing in Memory\n";
                catagoryDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_UInt32, true, "");
                imgUtils.copyUIntGDALDataset(inDataset, catagoryDataset);
                resultDataset = imgUtils.createCopy(inDataset, 1, "", "MEM", GDT_UInt32, true, "");
            }
            else
            {
                std::cout << "Processing using Disk\n";
                catagoryDataset = inDataset;
                resultDataset = imgUtils.createCopy(inDataset, 1, outputImage, imageFormat, GDT_UInt32, true, "");
            }
            
            std::cout << "Performing Clump\n";
            rsgis::segment::RSGISClumpPxls clumpImg;
            clumpImg.performClump(catagoryDataset, resultDataset, noDataValProvided, noDataVal);
            
            if(processInMemory)
            {
                std::cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inDataset, 1, outputImage, imageFormat, GDT_UInt32, true, "");
                imgUtils.copyUIntGDALDataset(resultDataset, outDataset);
                GDALClose(outDataset);
                GDALClose(catagoryDataset);
            }
            
            resultDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            // Tidy up
            GDALClose(inDataset);
            GDALClose(resultDataset);
            GDALDestroyDriverManager();
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::cmds::RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::cmds::RSGISCmdException(e.what());
        }
    }
    
    void executeRMSmallClumpsStepwise(std::string inputImage, std::string clumpsImage, std::string outputImage, std::string imageFormat, bool stretchStatsAvail, std::string stretchStatsFile, bool storeMean, bool processInMemory, unsigned int minClumpSize, float specThreshold)throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            GDALDataset *inClumpDataset = (GDALDataset *) GDALOpen(clumpsImage.c_str(), GA_ReadOnly);
            if(inClumpDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            
            std::vector<rsgis::img::BandSpecThresholdStats> *bandStretchStats = NULL;
            if(stretchStatsAvail)
            {
                bandStretchStats = rsgis::img::RSGISStretchImage::readBandSpecThresholds(stretchStatsFile);
            }
            
            GDALDataset *spectralDataset = NULL;
            GDALDataset *clumpsDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(processInMemory)
            {
                std::cout << "Processing in Memory\n";
                spectralDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_Float32, true, "");
                imgUtils.copyFloat32GDALDataset(inDataset, spectralDataset);
                clumpsDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, true, "");
                imgUtils.copyUIntGDALDataset(inClumpDataset, clumpsDataset);
                resultDataset = imgUtils.createCopy(inClumpDataset, "", "MEM", GDT_UInt32, true, "");
            }
            else
            {
                std::cout << "Processing using Disk\n";
                spectralDataset = inDataset;
                clumpsDataset = inClumpDataset;
                resultDataset = imgUtils.createCopy(inClumpDataset, outputImage, imageFormat, GDT_UInt32, true, "");
            }
            imgUtils.copyUIntGDALDataset(clumpsDataset, resultDataset);
            
            std::cout << "Eliminant Clumps\n";
            rsgis::segment::RSGISEliminateSmallClumps eliminate;
            if(storeMean)
            {
                eliminate.stepwiseEliminateSmallClumps(spectralDataset, resultDataset, minClumpSize, specThreshold, bandStretchStats, stretchStatsAvail);
            }
            else
            {
                eliminate.stepwiseEliminateSmallClumpsNoMean(spectralDataset, resultDataset, minClumpSize, specThreshold, bandStretchStats, stretchStatsAvail);
            }
            
            if(processInMemory)
            {
                std::cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inClumpDataset, outputImage, imageFormat, GDT_UInt32, true, "");
                imgUtils.copyUIntGDALDataset(resultDataset, outDataset);
                GDALClose(outDataset);
                GDALClose(spectralDataset);
                GDALClose(clumpsDataset);
            }
            
            if(stretchStatsAvail)
            {
                delete bandStretchStats;
            }
            
            resultDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            // Tidy up
            GDALClose(inDataset);
            GDALClose(inClumpDataset);
            GDALClose(resultDataset);
            GDALDestroyDriverManager();
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::cmds::RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::cmds::RSGISCmdException(e.what());
        }
    }
    
    void executeRelabelClumps(std::string inputImage, std::string outputImage, std::string imageFormat, bool processInMemory) throw(RSGISCmdException)
    {
        try
        {
            GDALAllRegister();
            GDALDataset *inDataset = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALDataset *catagoryDataset = NULL;
            GDALDataset *resultDataset = NULL;
            
            if(processInMemory)
            {
                std::cout << "Processing in Memory\n";
                catagoryDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_UInt32, true, "");
                imgUtils.copyUIntGDALDataset(inDataset, catagoryDataset);
                resultDataset = imgUtils.createCopy(inDataset, 1, "", "MEM", GDT_UInt32, true, "");
            }
            else
            {
                std::cout << "Processing using Disk\n";
                catagoryDataset = inDataset;
                resultDataset = imgUtils.createCopy(inDataset, 1, outputImage, imageFormat, GDT_UInt32, true, "");
            }
            
            std::cout << "Performing relabel\n";
            rsgis::segment::RSGISRelabelClumps relabelImg;
            //relabelImg.relabelClumps(catagoryDataset, resultDataset);
            relabelImg.relabelClumpsCalcImg(catagoryDataset, resultDataset);
            
            if(processInMemory)
            {
                std::cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inDataset, 1, outputImage, imageFormat, GDT_UInt32, true, "");
                imgUtils.copyUIntGDALDataset(resultDataset, outDataset);
                GDALClose(outDataset);
                GDALClose(catagoryDataset);
            }
            
            resultDataset->GetRasterBand(1)->SetMetadataItem("LAYER_TYPE", "thematic");
            
            // Tidy up
            GDALClose(inDataset);
            GDALClose(resultDataset);
            GDALDestroyDriverManager();
        }
        catch (rsgis::RSGISException &e)
        {
            throw rsgis::cmds::RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw rsgis::cmds::RSGISCmdException(e.what());
        }
    }
    
}}


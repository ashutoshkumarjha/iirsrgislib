/*
 *  RSGISCmdClassification.cpp
 *
 *
 *  Created by Pete Bunting on 18/11/2013.
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

#include "RSGISCmdClassification.h"
#include "RSGISCmdParent.h"

#include "common/RSGISException.h"
#include "common/RSGISImageException.h"

#include "img/RSGISImageUtils.h"

#include "classifier/RSGISRATClassificationUtils.h"

namespace rsgis{ namespace cmds {
    
    void executeCollapseRAT2Class(std::string clumpsImage, std::string outputImage, std::string outImageFormat, std::string classColumn)throw(RSGISCmdException)
    {
        try
        {
            rsgis::img::RSGISImageUtils imgUtils;
            
            std::cout << "Opening an image\n";
            GDALAllRegister();
            GDALDataset *imageDataset = NULL;
            imageDataset = (GDALDataset *) GDALOpen(clumpsImage.c_str(), GA_ReadOnly);
            if(imageDataset == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            rsgis::classifier::RSGISCollapseSegmentsClassification collapseSegments;
            collapseSegments.collapseClassification(imageDataset, classColumn, outputImage, outImageFormat);
            
            // Tidy up
            GDALClose(imageDataset);
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
    void executeGenerate3BandFromColourTable(std::string clumpsImage, std::string outputImage, std::string outImageFormat)throw(RSGISCmdException)
    {
        try
        {
            std::cout << "Openning input image.\n";
            rsgis::img::RSGISImageUtils imgUtils;
            
            GDALAllRegister();
            GDALDataset **imageDataset = new GDALDataset*[1];
            imageDataset[0] = (GDALDataset *) GDALOpen(clumpsImage.c_str(), GA_ReadOnly);
            if(imageDataset[0] == NULL)
            {
                std::string message = std::string("Could not open image ") + clumpsImage;
                throw rsgis::RSGISImageException(message.c_str());
            }
            
            std::cout << "Reading colour table\n";
            GDALColorTable *clrTab = imageDataset[0]->GetRasterBand(1)->GetColorTable();
            /*
             for(int i = 0; i < clrTab->GetColorEntryCount(); ++i)
             {
             const GDALColorEntry *clr = clrTab->GetColorEntry(i);
             std::cout << i << ": [" <<  clr->c1 << "," << clr->c2 << "," << clr->c3 << "]\n";
             }
             */
            std::string *bandNames = new std::string[3];
            bandNames[0] = "Red";
            bandNames[1] = "Green";
            bandNames[2] = "Blue";
            
            std::cout << "Applying to the image\n";
            rsgis::classifier::RSGISColourImageFromClassRAT *clrAsRGB = new rsgis::classifier::RSGISColourImageFromClassRAT(clrTab);
            rsgis::img::RSGISCalcImage calcImg = rsgis::img::RSGISCalcImage(clrAsRGB, "", true);
            calcImg.calcImage(imageDataset, 1, outputImage, true, bandNames, outImageFormat, GDT_Byte);
            delete[] bandNames;
            
            // Tidy up
            GDALClose(imageDataset[0]);
            delete[] imageDataset;
            GDALDestroyDriverManager();
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }

}}


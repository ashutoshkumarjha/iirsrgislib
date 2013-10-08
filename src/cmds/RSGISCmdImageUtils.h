/*
 *  RSGISCmdImageUtils.h
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

#ifndef RSGISCmdImageUtils_H
#define RSGISCmdImageUtils_H

#include <iostream>
#include <string>
#include <vector>

#include "common/RSGISCommons.h"
#include "RSGISCmdException.h"

namespace rsgis{ namespace cmds {
    
    enum RSGISStretches
    {
        linearMinMax,
        linearPercent,
        linearStdDev,
        histogram,
        exponential,
        logarithmic,
        powerLaw
    };
    
    /** Function to run the stretch image command */
    void executeStretchImage(std::string inputImage, std::string outputImage, bool saveOutStats, std::string outStatsFile, bool ignoreZeros, bool onePassSD, std::string gdalFormat, RSGISLibDataType outDataType, RSGISStretches stretchType, float stretchParam)throw(RSGISCmdException);
    /** Function to run the stretch image command with predefined stretch parameters*/
    void executeStretchImageWithStats(std::string inputImage, std::string outputImage, std::string inStatsFile, std::string gdalFormat, RSGISLibDataType outDataType, RSGISStretches stretchType, float stretchParam)throw(RSGISCmdException);
    /** Function to run the mask image command */
    void executeMaskImage(std::string inputImage, std::string imageMask, std::string outputImage, std::string gdalFormat, RSGISLibDataType outDataType, float outValue, float maskValue)throw(RSGISCmdException);
    /** A function to split an image into image tiles.
        An overlap between tiles may be specified.
        Optionally the tiles may be offset from the image boundries by half a pixel, useful for creating two overlapping lots of tiles.
        The filenames for each tile are passed back as a vector.
     */
    void executeCreateTiles(std::string inputImage, std::string outputImageBase, float width, float height, float tileOverlap, bool offsetTiling, std::string gdalFormat, RSGISLibDataType outDataType, std::string outFileExtension, std::vector<std::string> *outFileNames = NULL)throw(RSGISCmdException);
    /** A function to run the populate statistics command */
    void executePopulateImgStats(std::string inputImage, bool useIgnoreVal, float nodataValue, bool calcImgPyramids)throw(RSGISCmdException);
    /** A function to mosaic a set of input images 
        Pixels with a value of 'skipValue' in band 'skipBand' are excluded (all bands).
        Where pixels overlap:
        - The pixel is overwritten by the next image (overlapBehaviour=0)
        - The minimum value is taken (overlapBehaviour=1)
        - The maximum behaviour is taken (overlapBehaviour=1)
     */
    void executeImageMosaic(std::string *inputImages, int numDS, std::string outputImage, float background, float skipVal, unsigned int skipBand, unsigned int overlapBehaviour, std::string format, RSGISLibDataType outDataType) throw(RSGISCmdException);
    
    /** A function to assign the projection on an image file */
    void executeAssignProj(std::string inputImage, std::string wktStr, bool readWKTFromFile=false, std::string wktFile="")throw(RSGISCmdException);
    
    /** A function to assign the spatial information on an image file */
    void executeAssignSpatialInfo(std::string inputImage, double xTL, double yTL, double xRes, double yRes, double xRot, double yRot, bool xTLDef, bool yTLDef, bool xResDef, bool yResDef, bool xRotDef, bool yRotDef)throw(RSGISCmdException);
    
    /** A function to copy the projection from one file to another (i.e., similar to executeAssignProj) */
    void executeCopyProj(std::string inputImage, std::string refImageFile)throw(RSGISCmdException);
    
    /** A function to copy the projection and spaital info from one file to another (i.e., similar to executeAssignProj and executeAssignSpatialInfo combined) */
    void executeCopyProjSpatial(std::string inputImage, std::string refImageFile)throw(RSGISCmdException);
    
    /** A function to stack image bands into a single output image */
    void executeStackImageBands(std::string *imageFiles, std::string *imageBandNames, int numImages, std::string outputImage, bool skipPixels, float skipValue, float noDataValue, std::string gdalFormat, RSGISLibDataType outDataType, bool replaceBandNames)throw(RSGISCmdException);
    
}}


#endif


/*
 *  RSGISImageUtils.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 23/04/2008.
 *  Copyright 2008 RSGISLib.
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

#ifndef RSGISImageUtils_H
#define RSGISImageUtils_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <list>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "img/RSGISImageBandException.h"

#include "math/RSGISMathsUtils.h"

#include "common/RSGISImageException.h"
#include "common/RSGISOutputStreamException.h"

#include "geos/geom/Envelope.h"
#include "geos/geom/Polygon.h"

#include <boost/cstdint.hpp>

namespace rsgis 
{
	namespace img
	{
        struct ImagePixelValuePt
        {
            geos::geom::Coordinate *pt;
            uint_fast32_t imgX;
            uint_fast32_t imgY;
            float value;
        };
        
        struct PxlLoc
        {
            PxlLoc(){};
            PxlLoc( unsigned int xPos,  unsigned int yPos)
            {
                this->xPos = xPos;
                this->yPos = yPos;
            };
            unsigned int xPos;
            unsigned int yPos;
        };
        
        struct ImgClump
        {
            ImgClump(unsigned long clumpID)
            {
                this->clumpID = clumpID;
                this->active = true;
            };
            unsigned long clumpID;
            std::vector<PxlLoc> *pxls;
            float *sumVals;
            float *meanVals;
            bool active;
        };
        
        struct ImgClumpSum
        {
            ImgClumpSum(unsigned int clumpID)
            {
                this->clumpID = clumpID;
                this->active = true;
            };
            unsigned int clumpID;
            std::vector<PxlLoc> *pxls;
            float *sumVals;
            bool active;
        };
        
        struct ImgClumpMean
        {
            ImgClumpMean(unsigned long clumpID)
            {
                this->clumpID = clumpID;
                this->numPxls = 0;
            };
            unsigned long clumpID;
            unsigned int numPxls;
            float *sumVals;
            float *meanVals;
        };
        
        struct ImgClumpRG
        {
            ImgClumpRG(unsigned long clumpID)
            {
                this->clumpID = clumpID;
                this->active = true;
                this->seedVal = 0;
            };
            unsigned long clumpID;
            std::vector<PxlLoc> *pxls;
            float *sumVals;
            float *meanVals;
            bool active;
            std::list<unsigned long> neighbours;
            unsigned long seedVal;
        };
        
		class RSGISImageUtils
			{
			public:
				RSGISImageUtils();
				void getImageOverlap(GDALDataset **datasets, int numDS, int **dsOffsets, int *width, int *height, double *gdalTransform) throw(RSGISImageBandException);
                void getImageOverlap(GDALDataset **datasets, int numDS, int **dsOffsets, int *width, int *height, double *gdalTransform, int *maxBlockX, int *maxBlockY) throw(RSGISImageBandException);
				void getImageOverlap(GDALDataset **datasets, int numDS, int **dsOffsets, int *width, int *height, double *gdalTransform, geos::geom::Envelope *env) throw(RSGISImageBandException);
                void getImageOverlapCut2Env(GDALDataset **datasets, int numDS,  int **dsOffsets, int *width, int *height, double *gdalTransform, geos::geom::Envelope *env) throw(RSGISImageBandException);
				void getImageOverlap(GDALDataset **datasets, int numDS, int *width, int *height, geos::geom::Envelope *env) throw(RSGISImageBandException);
				void getImagesExtent(GDALDataset **datasets, int numDS, int *width, int *height, double *gdalTransform) throw(RSGISImageBandException);
                void getImagesExtent(std::string *inputImages, int numDS, int *width, int *height, double *gdalTransform) throw(RSGISImageBandException);
				void exportImageToTextCol(GDALDataset *image, int band, std::string outputText)throw(RSGISImageBandException, RSGISOutputStreamException);
				GDALDataset* createBlankImage(std::string imageFile, double *transformation, int xSize, int ySize, int numBands, std::string projection, float value, std::string gdalFormat="ENVI") throw(RSGISImageException, RSGISImageBandException);
                GDALDataset* createBlankImage(std::string imageFile, double *transformation, int xSize, int ySize, int numBands, std::string projection, float value, std::vector<std::string> bandNames, std::string gdalFormat="ENVI") throw(RSGISImageException, RSGISImageBandException);
				GDALDataset* createBlankImage(std::string imageFile, geos::geom::Envelope extent, double resolution, int numBands, std::string projection, float value, std::string gdalFormat="ENVI") throw(RSGISImageException, RSGISImageBandException);
				void exportImageBands(std::string imageFile, std::string outputFilebase, std::string format) throw(RSGISImageException, RSGISImageBandException);
				void exportImageStack(std::string *inputImages, std::string *outputImages, std::string outputFormat, int numImages)  throw(RSGISImageException, RSGISImageBandException);
				void exportImageStackWithMask(std::string *inputImages, std::string *outputImages, std::string imageMask, std::string outputFormat, int numImages, float maskValue)  throw(RSGISImageException, RSGISImageBandException);
				void convertImageFileFormat(std::string inputImage, std::string outputImage, std::string outputImageFormat, bool projFromImage=false, std::string wktProjStr="");
				float** getImageDataBlock(GDALDataset *dataset, int *dsOffsets, unsigned int width, unsigned int height, unsigned int *numVals);
				void copyImageRemoveSpatialReference(std::string inputImage, std::string outputImage)throw(RSGISImageException);
				void copyImageDefiningSpatialReference(std::string inputImage, std::string outputImage, std::string proj, double tlX, double tlY, float xRes, float yRes)throw(RSGISImageException);
                void createImageSlices(GDALDataset *dataset, std::string outputImageBase) throw(RSGISImageException);
                void copyFloatGDALDataset(GDALDataset *inData, GDALDataset *outData) throw(RSGISImageException);
                void copyIntGDALDataset(GDALDataset *inData, GDALDataset *outData) throw(RSGISImageException);
                void copyUIntGDALDataset(GDALDataset *inData, GDALDataset *outData) throw(RSGISImageException);
                void copyFloat32GDALDataset(GDALDataset *inData, GDALDataset *outData) throw(RSGISImageException);
                void copyByteGDALDataset(GDALDataset *inData, GDALDataset *outData) throw(RSGISImageException);
                void zerosUIntGDALDataset(GDALDataset *data) throw(RSGISImageException);
                void zerosFloatGDALDataset(GDALDataset *data) throw(RSGISImageException);
                void zerosByteGDALDataset(GDALDataset *data) throw(RSGISImageException);
                void assignValGDALDataset(GDALDataset *data, float value) throw(RSGISImageException);
                GDALDataset* createCopy(GDALDataset *inData, std::string outputFilePath, std::string outputFormat, GDALDataType eType, bool useImgProj=true, std::string proj="")throw(RSGISImageException);
                GDALDataset* createCopy(GDALDataset *inData, unsigned int numBands, std::string outputFilePath, std::string outputFormat, GDALDataType eType, bool useImgProj=true, std::string proj="")throw(RSGISImageException);
                void createKMLText(std::string inputImage, std::string outKMLFile) throw(RSGISImageBandException);
                ~RSGISImageUtils();
			};
	}
}

#endif


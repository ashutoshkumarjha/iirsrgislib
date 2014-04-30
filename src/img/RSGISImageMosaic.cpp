/*
 *  RSGISImageMosaic.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 07/11/2008.
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
 *
 *  Modified by Dan Clewley on 21/11/2010
 *  Added 'mosaicSkipVals' and 'mosaicSkipThresh'
 *  to skip values in input image
 *
 *  Modified by Dan Clewley on 27/05/2013
 *  Changed to block read / write
 *  Added ability to take minimum / maximum pixel in overlapping regions
 */

#include "RSGISImageMosaic.h"

namespace rsgis{namespace img{

	RSGISImageMosaic::RSGISImageMosaic()
	{

	}

	void RSGISImageMosaic::mosaic(std::string *inputImages, int numDS, std::string outputImage, float background, bool projFromImage, std::string proj, std::string format, GDALDataType imgDataType) throw(RSGISImageException)
	{
		RSGISImageUtils imgUtils;
        rsgis::math::RSGISMathsUtils mathsUtils;
        GDALAllRegister();
        GDALDataset *dataset = NULL;
        GDALRasterBand *imgBand = NULL;
		int width;
		int height;
		double *transformation = new double[6];
		double *imgTransform = new double[6];
		int numberBands = 0;
		int tileXSize = 0;
		int tileYSize = 0;
		double xDiff = 0;
		double yDiff = 0;
		int xStart = 0;
		int yStart = 0;
		std::string projection = proj;
		GDALDataset *outputDataset = NULL;
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		float **inputData = NULL;

        std::vector<std::string> bandnames;

		try
		{
			for(int i = 0; i < numDS; i++)
			{
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }

				if(i == 0)
				{
					numberBands = dataset->GetRasterCount();
                    for(int j = 0; j < numberBands; ++j)
                    {
                        imgBand = dataset->GetRasterBand(j+1);
                        bandnames.push_back(std::string(imgBand->GetDescription()));
                    }
                    if(projFromImage)
                    {
                        projection = std::string(dataset->GetProjectionRef());
                    }
				}
				else
				{
					if(dataset->GetRasterCount() != numberBands)
					{
						throw RSGISImageBandException("All input images need to have the same number of bands (" + mathsUtils.doubletostring(numberBands) + ").\n"
                                    + inputImages[i] + " has " + mathsUtils.doubletostring(dataset->GetRasterCount()) );
					}
				}
                GDALClose(dataset);
			}

			imgUtils.getImagesExtent(inputImages, numDS, &width, &height, transformation);

            // Create blank image
			std::cout << "Create new image [" << width << "," << height << "] with projection: \n" << projection << std::endl;

			outputDataset = imgUtils.createBlankImage(outputImage, transformation, width, height, numberBands, projection, background, bandnames, format, imgDataType);

			// COPY IMAGE DATA INTO THE BLANK IMAGE

            //Get Image Output Bands
            outputRasterBands = new GDALRasterBand*[numberBands];
            for(int i = 0; i < numberBands; i++)
            {
                outputRasterBands[i] = outputDataset->GetRasterBand(i+1);
            }

            int xBlockSize = 0;
            int yBlockSize = 0;
            outputRasterBands[0]->GetBlockSize (&xBlockSize, &yBlockSize);

            std::cout << "Max. block size: " << yBlockSize << std::endl;

			std::cout << "Started (total " << numDS << ") ." << std::flush;

            for(int ds = 0; ds < numDS; ds++)
			{
				std::cout << "." << ds+1 << "." << std::flush;

                dataset = (GDALDataset *) GDALOpenShared(inputImages[ds].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[ds];
                    throw RSGISImageException(message.c_str());
                }

                inputRasterBands = new GDALRasterBand*[numberBands];
                for(int i = 0; i < numberBands; i++)
                {
                    inputRasterBands[i] = dataset->GetRasterBand(i+1);
                }

				dataset->GetGeoTransform(imgTransform);
				tileXSize = dataset->GetRasterXSize();
				tileYSize = dataset->GetRasterYSize();

				xDiff = imgTransform[0] - transformation[0];
				yDiff = transformation[3] - imgTransform[3];

				xStart = floor(xDiff/transformation[1]);
				yStart = floor(yDiff/transformation[1]);

                // Allocate memory
                inputData = new float*[numberBands];
                for(int i = 0; i < numberBands; i++)
                {
                    inputData[i] = (float *) CPLMalloc(sizeof(float)*(tileXSize*yBlockSize));
                }

                int nYBlocks = tileYSize / yBlockSize;
                int remainRows = tileYSize - (nYBlocks * yBlockSize);
                int rowOffset = 0;

                for(int i = 0; i < nYBlocks; i++)
                {
                    //std::cout << i << " of " << nYBlocks << std::endl;

                    for(int n = 0; n < numberBands; n++)
                    {
                        rowOffset = yBlockSize * i;
                        inputRasterBands[n]->RasterIO(GF_Read, 0, rowOffset, tileXSize, yBlockSize, inputData[n], tileXSize, yBlockSize, GDT_Float32, 0, 0);
                        outputRasterBands[n]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, yBlockSize, inputData[n], tileXSize, yBlockSize, GDT_Float32, 0, 0);
                    }

                }

                if(remainRows > 0)
                {
                    for(int n = 0; n < numberBands; n++)
                    {
                        rowOffset = yBlockSize * nYBlocks;
                        inputRasterBands[n]->RasterIO(GF_Read, 0, rowOffset, tileXSize, remainRows, inputData[n], tileXSize, remainRows, GDT_Float32, 0, 0);
                        outputRasterBands[n]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, remainRows, inputData[n], tileXSize, remainRows, GDT_Float32, 0, 0);
                    }
                }

                // Tidy up
                if(inputData != NULL)
                {
                    for(int i = 0; i < numberBands; i++)
                    {
                        if(inputData[i] != NULL)
                        {
                            delete[] inputData[i];
                        }
                    }
                    delete[] inputData;
                }
                if(inputRasterBands != NULL)
                {
                    delete[] inputRasterBands;
                }

                GDALClose(dataset);
			}
			std::cout << ".complete\n";

		}
		catch(RSGISImageBandException e)
		{
            if(inputData != NULL)
            {
                for(int i = 0; i < numberBands; i++)
                {
                    if(inputData[i] != NULL)
                    {
                        delete[] inputData[i];
                    }
                }
                delete[] inputData;
            }
            if(inputRasterBands != NULL)
            {
                delete[] inputRasterBands;
            }
            if(outputRasterBands != NULL)
            {
                delete[] outputRasterBands;
            }
			if(transformation != NULL)
			{
				delete[] transformation;
			}
			if(imgTransform != NULL)
			{
				delete[] imgTransform;
			}
			throw e;
		}

        // Tidy
        if(outputRasterBands != NULL)
        {
            delete[] outputRasterBands;
        }

		if(transformation != NULL)
		{
			delete[] transformation;
		}
		if(imgTransform != NULL)
		{
			delete[] imgTransform;
		}
		GDALClose(outputDataset);
        //GDALDestroyDriverManager();
	}

	void RSGISImageMosaic::mosaicSkipVals(std::string *inputImages, int numDS, std::string outputImage, float background, float skipVal, bool projFromImage, std::string proj, unsigned int skipBand, unsigned int overlapBehaviour, std::string format, GDALDataType imgDataType) throw(RSGISImageException)
	{
		RSGISImageUtils imgUtils;
		rsgis::math::RSGISMathsUtils mathsUtils;

        GDALAllRegister();
        GDALDataset *dataset = NULL;
        GDALRasterBand *imgBand = NULL;
		int width;
		int height;
		double *transformation = new double[6];
		double *imgTransform = new double[6];
		int numberBands = 0;
		int tileXSize = 0;
		int tileYSize = 0;
		double xDiff = 0;
		double yDiff = 0;
		int xStart = 0;
		int yStart = 0;
		std::string projection = proj;
		GDALDataset *outputDataset = NULL;
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		float **inputData = NULL;
		float **outputData = NULL;
		bool pixelsChanged;

        std::vector<std::string> bandnames;

		try
		{
			for(int i = 0; i < numDS; i++)
			{
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }

				if(i == 0)
				{
					numberBands = dataset->GetRasterCount();
                    for(int j = 0; j < numberBands; ++j)
                    {
                        imgBand = dataset->GetRasterBand(j+1);
                        bandnames.push_back(std::string(imgBand->GetDescription()));
                    }
                    if(projFromImage)
                    {
                        projection = std::string(dataset->GetProjectionRef());
                    }
				}
				else
				{
					if(dataset->GetRasterCount() != numberBands)
					{
						throw RSGISImageBandException("All input images need to have the same number of bands (" + mathsUtils.doubletostring(numberBands) + ").\n"
                                    + inputImages[i] + " has " + mathsUtils.doubletostring(dataset->GetRasterCount()) );
					}
				}
                GDALClose(dataset);
			}

			imgUtils.getImagesExtent(inputImages, numDS, &width, &height, transformation);

            // Create blank image
			std::cout << "Create new image [" << width << "," << height << "] with projection: \n" << projection << std::endl;

			outputDataset = imgUtils.createBlankImage(outputImage, transformation, width, height, numberBands, projection, background, bandnames, format, imgDataType);

			// COPY IMAGE DATA INTO THE BLANK IMAGE

            //Get Image Output Bands
            outputRasterBands = new GDALRasterBand*[numberBands];
            for(int i = 0; i < numberBands; i++)
            {
                outputRasterBands[i] = outputDataset->GetRasterBand(i+1);
            }

            int xBlockSize = 0;
            int yBlockSize = 0;
            outputRasterBands[0]->GetBlockSize (&xBlockSize, &yBlockSize);

            std::cout << "Max. block size: " << yBlockSize << std::endl;

			std::cout << "Started (total " << numDS << ") ." << std::flush;

            for(int ds = 0; ds < numDS; ds++)
			{
				std::cout << "." << ds+1 << "." << std::flush;

                dataset = (GDALDataset *) GDALOpenShared(inputImages[ds].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[ds];
                    throw RSGISImageException(message.c_str());
                }

                inputRasterBands = new GDALRasterBand*[numberBands];
                for(int i = 0; i < numberBands; i++)
                {
                    inputRasterBands[i] = dataset->GetRasterBand(i+1);
                }

				dataset->GetGeoTransform(imgTransform);
				tileXSize = dataset->GetRasterXSize();
				tileYSize = dataset->GetRasterYSize();

				xDiff = imgTransform[0] - transformation[0];
				yDiff = transformation[3] - imgTransform[3];

				xStart = floor(xDiff/transformation[1]);
				yStart = floor(yDiff/transformation[1]);

                // Allocate memory
                inputData = new float*[numberBands];
                outputData = new float*[numberBands];
                for(int i = 0; i < numberBands; i++)
                {
                    inputData[i] = (float *) CPLMalloc(sizeof(float)*(tileXSize*yBlockSize));
                    outputData[i] = (float *) CPLMalloc(sizeof(float)*(tileXSize*yBlockSize));
                }

                int nYBlocks = tileYSize / yBlockSize;
                int remainRows = tileYSize - (nYBlocks * yBlockSize);
                int rowOffset = 0;

                for(int i = 0; i < nYBlocks; i++)
                {
                    // Read input and output data
                    for(int n = 0; n < numberBands; n++)
                    {
                        rowOffset = yBlockSize * i;
                        // Read input data
                        inputRasterBands[n]->RasterIO(GF_Read, 0, rowOffset, tileXSize, yBlockSize, inputData[n], tileXSize, yBlockSize, GDT_Float32, 0, 0);
                        // Read output data
                        outputRasterBands[n]->RasterIO(GF_Read, xStart, (yStart + rowOffset), tileXSize, yBlockSize, outputData[n], tileXSize, yBlockSize, GDT_Float32, 0, 0);
                    }

                    pixelsChanged = false; // Keep track if there are changes that need writing out.

                    for(int m = 0; m < yBlockSize; ++m)
                    {
                        for(int j = 0; j < tileXSize; j++)
                        {
                            // Check for skip value
                            if(inputData[skipBand][(m*tileXSize)+j] != skipVal)
                            {
                                // Check if behaviour is defined for overlap and not the first image
                                if( (overlapBehaviour > 0) && (ds > 0) )
                                {
                                    // Check if pixel is background value (no data has been written yet)
                                    if( outputData[skipBand][(m*tileXSize)+j] == background )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                    // If data has been written - check if new value is less (min overlap behaviour)
                                    else if( (overlapBehaviour == 1) &&  (inputData[skipBand][(m*tileXSize)+j] < outputData[skipBand][(m*tileXSize)+j]) )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                    // If data has been written - check if new value is greater (max overlap behaviour)
                                    else if( (overlapBehaviour == 2) &&  (inputData[skipBand][(m*tileXSize)+j] > outputData[skipBand][(m*tileXSize)+j]) )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                }
                                // If no overlap behaviour defined, write out,
                                else
                                {
                                    for(int n = 0; n < numberBands; n++)
                                    {
                                        outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                    }
                                    pixelsChanged = true;
                                }


                            }
                        }
                    }

                    // Write out data to mosaic
                    if(pixelsChanged)
                    {
                        for(int n = 0; n < numberBands; n++)
                        {
                            outputRasterBands[n]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, yBlockSize, outputData[n], tileXSize, yBlockSize, GDT_Float32, 0, 0);
                        }
                    }

                }

                if(remainRows > 0)
                {
                    for(int n = 0; n < numberBands; n++)
                    {
                        rowOffset = yBlockSize * nYBlocks;
                        // Read input data
                        inputRasterBands[n]->RasterIO(GF_Read, 0, rowOffset, tileXSize, remainRows, inputData[n], tileXSize, remainRows, GDT_Float32, 0, 0);
                        // Read output data
                        outputRasterBands[n]->RasterIO(GF_Read, xStart, (yStart + rowOffset), tileXSize, remainRows, outputData[n], tileXSize, remainRows, GDT_Float32, 0, 0);
                    }

                    pixelsChanged = false; // Keep track if there are changes that need writing out.

                    for(int m = 0; m < remainRows; ++m)
                    {
                        for(int j = 0; j < tileXSize; j++)
                        {
                            // Check for skip value
                            if(inputData[skipBand][(m*tileXSize)+j] != skipVal)
                            {
                                // Check if behaviour is defined for overlap and not the first image
                                if( (overlapBehaviour > 0) && (ds > 0) )
                                {
                                    // Check if pixel is background value (no data has been written yet)
                                    if( outputData[skipBand][(m*tileXSize)+j] == background )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                    // If data has been written - check if new value is less (min overlap behaviour)
                                    else if( (overlapBehaviour == 1) &&  (inputData[skipBand][(m*tileXSize)+j] < outputData[skipBand][(m*tileXSize)+j]) )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                    // If data has been written - check if new value is greater (max overlap behaviour)
                                    else if( (overlapBehaviour == 2) &&  (inputData[skipBand][(m*tileXSize)+j] > outputData[skipBand][(m*tileXSize)+j]) )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                }
                                // If no overlap behaviour defined, write out,
                                else
                                {
                                    for(int n = 0; n < numberBands; n++)
                                    {
                                        outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                    }
                                    pixelsChanged = true;
                                }


                            }
                        }
                    }

                    // Write out data
                    if(pixelsChanged)
                    {
                        for(int n = 0; n < numberBands; n++)
                        {
                            outputRasterBands[n]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, remainRows, outputData[n], tileXSize, remainRows, GDT_Float32, 0, 0);
                        }
                    }

                }

                // Tidy up
                if(inputData != NULL)
                {
                    for(int i = 0; i < numberBands; i++)
                    {
                        if(inputData[i] != NULL)
                        {
                            delete[] inputData[i];
                        }
                    }
                    delete[] inputData;
                }
                if(outputData != NULL)
                {
                    for(int i = 0; i < numberBands; i++)
                    {
                        if(outputData[i] != NULL)
                        {
                            delete[] outputData[i];
                        }
                    }
                    delete[] outputData;
                }
                if(inputRasterBands != NULL)
                {
                    delete[] inputRasterBands;
                }

                GDALClose(dataset);
			}
			std::cout << ".complete\n";

		}
		catch(RSGISImageBandException e)
		{
            if(inputData != NULL)
            {
                for(int i = 0; i < numberBands; i++)
                {
                    if(inputData[i] != NULL)
                    {
                        delete[] inputData[i];
                    }
                }
                delete[] inputData;
            }
            if(outputData != NULL)
            {
                for(int i = 0; i < numberBands; i++)
                {
                    if(outputData[i] != NULL)
                    {
                        delete[] outputData[i];
                    }
                }
                delete[] outputData;
            }
            if(inputRasterBands != NULL)
            {
                delete[] inputRasterBands;
            }
            if(outputRasterBands != NULL)
            {
                delete[] outputRasterBands;
            }
			if(transformation != NULL)
			{
				delete[] transformation;
			}
			if(imgTransform != NULL)
			{
				delete[] imgTransform;
			}
			throw e;
		}

        // Tidy
        if(outputRasterBands != NULL)
        {
            delete[] outputRasterBands;
        }

		if(transformation != NULL)
		{
			delete[] transformation;
		}
		if(imgTransform != NULL)
		{
			delete[] imgTransform;
		}
		GDALClose(outputDataset);
        //GDALDestroyDriverManager();
	}

	void RSGISImageMosaic::mosaicSkipThresh(std::string *inputImages, int numDS, std::string outputImage, float background, float skipLowerThresh, float skipUpperThresh, bool projFromImage, std::string proj, unsigned int threshBand, unsigned int overlapBehaviour, std::string format, GDALDataType imgDataType) throw(RSGISImageException)
	{
		RSGISImageUtils imgUtils;
        rsgis::math::RSGISMathsUtils mathsUtils;
        GDALAllRegister();
        GDALDataset *dataset = NULL;
        GDALRasterBand *imgBand = NULL;
		int width;
		int height;
		double *transformation = new double[6];
		double *imgTransform = new double[6];
		int numberBands = 0;
		int tileXSize = 0;
		int tileYSize = 0;
		double xDiff = 0;
		double yDiff = 0;
		int xStart = 0;
		int yStart = 0;
		std::string projection = proj;
		GDALDataset *outputDataset = NULL;
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		float **inputData = NULL;
		float **outputData = NULL;
		bool pixelsChanged;

        std::vector<std::string> bandnames;

		try
		{
			for(int i = 0; i < numDS; i++)
			{
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }

				if(i == 0)
				{
					numberBands = dataset->GetRasterCount();
                    for(int j = 0; j < numberBands; ++j)
                    {
                        imgBand = dataset->GetRasterBand(j+1);
                        bandnames.push_back(std::string(imgBand->GetDescription()));
                    }
                    if(projFromImage)
                    {
                        projection = std::string(dataset->GetProjectionRef());
                    }
				}
				else
				{
					if(dataset->GetRasterCount() != numberBands)
					{
						throw RSGISImageBandException("All input images need to have the same number of bands (" + mathsUtils.doubletostring(numberBands) + ").\n"
                                    + inputImages[i] + " has " + mathsUtils.doubletostring(dataset->GetRasterCount()) );
					}
				}
                GDALClose(dataset);
			}

			imgUtils.getImagesExtent(inputImages, numDS, &width, &height, transformation);

            // Create blank image
			std::cout << "Create new image [" << width << "," << height << "] with projection: \n" << projection << std::endl;

			outputDataset = imgUtils.createBlankImage(outputImage, transformation, width, height, numberBands, projection, background, bandnames, format, imgDataType);

			// COPY IMAGE DATA INTO THE BLANK IMAGE

            //Get Image Output Bands
            outputRasterBands = new GDALRasterBand*[numberBands];
            for(int i = 0; i < numberBands; i++)
            {
                outputRasterBands[i] = outputDataset->GetRasterBand(i+1);
            }

            int xBlockSize = 0;
            int yBlockSize = 0;
            outputRasterBands[0]->GetBlockSize (&xBlockSize, &yBlockSize);

            std::cout << "Max. block size: " << yBlockSize << std::endl;

			std::cout << "Started (total " << numDS << ") ." << std::flush;

            for(int ds = 0; ds < numDS; ds++)
			{
				std::cout << "." << ds+1 << "." << std::flush;

                dataset = (GDALDataset *) GDALOpenShared(inputImages[ds].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[ds];
                    throw RSGISImageException(message.c_str());
                }

                inputRasterBands = new GDALRasterBand*[numberBands];
                for(int i = 0; i < numberBands; i++)
                {
                    inputRasterBands[i] = dataset->GetRasterBand(i+1);
                }

				dataset->GetGeoTransform(imgTransform);
				tileXSize = dataset->GetRasterXSize();
				tileYSize = dataset->GetRasterYSize();

				xDiff = imgTransform[0] - transformation[0];
				yDiff = transformation[3] - imgTransform[3];

				xStart = floor(xDiff/transformation[1]);
				yStart = floor(yDiff/transformation[1]);

                // Allocate memory
                inputData = new float*[numberBands];
                outputData = new float*[numberBands];
                for(int i = 0; i < numberBands; i++)
                {
                    inputData[i] = (float *) CPLMalloc(sizeof(float)*(tileXSize*yBlockSize));
                    outputData[i] = (float *) CPLMalloc(sizeof(float)*(tileXSize*yBlockSize));
                }

                int nYBlocks = tileYSize / yBlockSize;
                int remainRows = tileYSize - (nYBlocks * yBlockSize);
                int rowOffset = 0;

                for(int i = 0; i < nYBlocks; i++)
                {
                    // Read input and output data
                    for(int n = 0; n < numberBands; n++)
                    {
                        rowOffset = yBlockSize * i;
                        // Read input data
                        inputRasterBands[n]->RasterIO(GF_Read, 0, rowOffset, tileXSize, yBlockSize, inputData[n], tileXSize, yBlockSize, GDT_Float32, 0, 0);
                        // Read output data
                        outputRasterBands[n]->RasterIO(GF_Read, xStart, (yStart + rowOffset), tileXSize, yBlockSize, outputData[n], tileXSize, yBlockSize, GDT_Float32, 0, 0);
                    }

                    pixelsChanged = false; // Keep track if there are changes that need writing out.

                    for(int m = 0; m < yBlockSize; ++m)
                    {
                        for(int j = 0; j < tileXSize; j++)
                        {
                            // Check value is between upper and lower limits
                            if((inputData[threshBand][(m*tileXSize)+j] > skipLowerThresh) && (inputData[threshBand][(m*tileXSize)+j] < skipUpperThresh))
                            {
                                // Check if behaviour is defined for overlap and not the first image
                                if( (overlapBehaviour > 0) && (ds > 0) )
                                {
                                    // Check if pixel is background value (no data has been written yet)
                                    if( outputData[threshBand][(m*tileXSize)+j] == background )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                    // If data has been written - check if new value is less (min overlap behaviour)
                                    else if( (overlapBehaviour == 1) &&  (inputData[threshBand][(m*tileXSize)+j] < outputData[threshBand][(m*tileXSize)+j]) )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                    // If data has been written - check if new value is greater (max overlap behaviour)
                                    else if( (overlapBehaviour == 2) &&  (inputData[threshBand][(m*tileXSize)+j] > outputData[threshBand][(m*tileXSize)+j]) )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                }
                                // If no overlap behaviour defined, write out,
                                else
                                {
                                    for(int n = 0; n < numberBands; n++)
                                    {
                                        outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                    }
                                    pixelsChanged = true;
                                }


                            }
                        }
                    }

                    // Write out data to mosaic
                    if(pixelsChanged)
                    {
                        for(int n = 0; n < numberBands; n++)
                        {
                            outputRasterBands[n]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, yBlockSize, outputData[n], tileXSize, yBlockSize, GDT_Float32, 0, 0);
                        }
                    }

                }

                if(remainRows > 0)
                {
                    for(int n = 0; n < numberBands; n++)
                    {
                        rowOffset = yBlockSize * nYBlocks;
                        // Read input data
                        inputRasterBands[n]->RasterIO(GF_Read, 0, rowOffset, tileXSize, remainRows, inputData[n], tileXSize, remainRows, GDT_Float32, 0, 0);
                        // Read output data
                        outputRasterBands[n]->RasterIO(GF_Read, xStart, (yStart + rowOffset), tileXSize, remainRows, outputData[n], tileXSize, remainRows, GDT_Float32, 0, 0);
                    }

                    pixelsChanged = false; // Keep track if there are changes that need writing out.

                    for(int m = 0; m < remainRows; ++m)
                    {
                        for(int j = 0; j < tileXSize; j++)
                        {
                            // Check value is between upper and lower limits
                            if((inputData[threshBand][(m*tileXSize)+j] > skipLowerThresh) && (inputData[threshBand][(m*tileXSize)+j] < skipUpperThresh))
                            {
                                // Check if behaviour is defined for overlap and not the first image
                                if( (overlapBehaviour > 0) && (ds > 0) )
                                {
                                    // Check if pixel is background value (no data has been written yet)
                                    if( outputData[threshBand][(m*tileXSize)+j] == background )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                    // If data has been written - check if new value is less (min overlap behaviour)
                                    else if( (overlapBehaviour == 1) &&  (inputData[threshBand][(m*tileXSize)+j] < outputData[threshBand][(m*tileXSize)+j]) )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                    // If data has been written - check if new value is greater (max overlap behaviour)
                                    else if( (overlapBehaviour == 2) &&  (inputData[threshBand][(m*tileXSize)+j] > outputData[threshBand][(m*tileXSize)+j]) )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                }
                                // If no overlap behaviour defined, write out,
                                else
                                {
                                    for(int n = 0; n < numberBands; n++)
                                    {
                                        outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                    }
                                    pixelsChanged = true;
                                }


                            }
                        }
                    }

                    // Write out data
                    if(pixelsChanged)
                    {
                        for(int n = 0; n < numberBands; n++)
                        {
                            outputRasterBands[n]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, remainRows, outputData[n], tileXSize, remainRows, GDT_Float32, 0, 0);
                        }
                    }

                }

                // Tidy up
                if(inputData != NULL)
                {
                    for(int i = 0; i < numberBands; i++)
                    {
                        if(inputData[i] != NULL)
                        {
                            delete[] inputData[i];
                        }
                    }
                    delete[] inputData;
                }
                if(outputData != NULL)
                {
                    for(int i = 0; i < numberBands; i++)
                    {
                        if(outputData[i] != NULL)
                        {
                            delete[] outputData[i];
                        }
                    }
                    delete[] outputData;
                }
                if(inputRasterBands != NULL)
                {
                    delete[] inputRasterBands;
                }

                GDALClose(dataset);
			}
			std::cout << ".complete\n";

		}
		catch(RSGISImageBandException e)
		{
            if(inputData != NULL)
            {
                for(int i = 0; i < numberBands; i++)
                {
                    if(inputData[i] != NULL)
                    {
                        delete[] inputData[i];
                    }
                }
                delete[] inputData;
            }
            if(outputData != NULL)
            {
                for(int i = 0; i < numberBands; i++)
                {
                    if(outputData[i] != NULL)
                    {
                        delete[] outputData[i];
                    }
                }
                delete[] outputData;
            }
            if(inputRasterBands != NULL)
            {
                delete[] inputRasterBands;
            }
            if(outputRasterBands != NULL)
            {
                delete[] outputRasterBands;
            }
			if(transformation != NULL)
			{
				delete[] transformation;
			}
			if(imgTransform != NULL)
			{
				delete[] imgTransform;
			}
			throw e;
		}

        // Tidy
        if(outputRasterBands != NULL)
        {
            delete[] outputRasterBands;
        }

		if(transformation != NULL)
		{
			delete[] transformation;
		}
		if(imgTransform != NULL)
		{
			delete[] imgTransform;
		}
		GDALClose(outputDataset);
        //GDALDestroyDriverManager();
	}

	void RSGISImageMosaic::includeDatasets(GDALDataset *baseImage, std::string *inputImages, int numDS, std::vector<int> bands, bool bandsDefined) throw(RSGISImageException)
	{
		RSGISImageUtils imgUtils;
        GDALDataset *dataset = NULL;
		int width;
		int height;

		double *transformation = new double[6];
		double *imgTransform = new double[6];
		double *baseTransform = new double[6];
		int numberBands = 0;
		int tileXSize = 0;
		int tileYSize = 0;
		double xDiff = 0;
		double yDiff = 0;
		int xStart = 0;
		int yStart = 0;
		std::string projection;

		GDALRasterBand *inputBand = NULL;
		GDALRasterBand **outputBand = NULL;
		float *imgData = NULL;

		try
		{
			numberBands = baseImage->GetRasterCount();
            for(int i = 0; i < numDS; i++)
			{
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }

                if(!bandsDefined)
                {
                    if(dataset->GetRasterCount() != numberBands)
                    {
                        throw RSGISImageBandException("All input images need to have the same number of bands.");
                    }
                }
                else
                {
                    for(std::vector<int>::iterator iterBands = bands.begin(); iterBands != bands.end(); ++iterBands)
                    {
                        if(((*iterBands) <= 0) | ((*iterBands) > dataset->GetRasterCount()))
                        {
                            std::cerr << "Band = " << *iterBands << std::endl;
                            std::string message = std::string("Band is not within the input dataset ") + inputImages[i];
                            throw RSGISImageException(message.c_str());
                        }
                    }
                }
                GDALClose(dataset);
			}

            if(bandsDefined)
            {
                if(numberBands < bands.size())
                {
                    throw RSGISImageException("The base image does not have enough image bands for the output data specificed.");
                }
            }

            projection = std::string(baseImage->GetProjectionRef());
			imgUtils.getImagesExtent(inputImages, numDS, &width, &height, transformation);

			baseImage->GetGeoTransform(baseTransform);

			double baseExtentX = baseTransform[0] + (baseImage->GetRasterXSize() * baseTransform[1]);
			double baseExtentY = baseTransform[3] + (baseImage->GetRasterYSize() * baseTransform[5]);
			double imgExtentX = transformation[0] + (width * transformation[1]);
			double imgExtentY = transformation[3] + (height * transformation[5]);

			/*std::cout << "Transformation[0] = " << transformation[0] << std::endl;
			std::cout << "Transformation[1] = " << transformation[1] << std::endl;
			std::cout << "Transformation[2] = " << transformation[2] << std::endl;
			std::cout << "Transformation[3] = " << transformation[3] << std::endl;
			std::cout << "Transformation[4] = " << transformation[4] << std::endl;
			std::cout << "Transformation[5] = " << transformation[5] << std::endl;

			std::cout << "baseTransform[0] = " << baseTransform[0] << std::endl;
			std::cout << "baseTransform[1] = " << baseTransform[1] << std::endl;
			std::cout << "baseTransform[2] = " << baseTransform[2] << std::endl;
			std::cout << "baseTransform[3] = " << baseTransform[3] << std::endl;
			std::cout << "baseTransform[4] = " << baseTransform[4] << std::endl;
			std::cout << "baseTransform[5] = " << baseTransform[5] << std::endl;

			std::cout << "baseImage->GetRasterXSize() = " << baseImage->GetRasterXSize() << std::endl;
			std::cout << "baseImage->GetRasterYSize() = " << baseImage->GetRasterYSize() << std::endl;

			std::cout << "height = " << height << std::endl;
			std::cout << "Width = " << width << std::endl;

			std::cout << "baseExtentX = " << baseExtentX << std::endl;
			std::cout << "baseExtentY = " << baseExtentY << std::endl;
			std::cout << "imgExtentX = " << imgExtentX << std::endl;
			std::cout << "imgExtentY = " << imgExtentY << std::endl;*/

			// Check datasets fit within the base image.
			if(transformation[0] < baseTransform[0])
			{
				throw RSGISImageException("Images do not fit within the base image (Eastings Min)");
			}
			if(transformation[3] > baseTransform[3])
			{
				throw RSGISImageException("Images do not fit within the base image (Northings Max)");
			}
			if(imgExtentX > baseExtentX)
			{
				throw RSGISImageException("Images do not fit within the base image (Eastings Max)");
			}
			if(imgExtentY < baseExtentY)
			{
				throw RSGISImageException("Images do not fit within the base image (Northings Min)");
			}

			height = baseImage->GetRasterYSize();
			width = baseImage->GetRasterXSize();

            outputBand = new GDALRasterBand*[numberBands];
            for(int i = 0; i < numberBands; i++)
            {
                outputBand[i] = baseImage->GetRasterBand(i+1);
            }

            int xBlockSize = 0;
            int yBlockSize = 0;
            outputBand[0]->GetBlockSize (&xBlockSize, &yBlockSize);

            std::cout << "Max. block size: " << yBlockSize << std::endl;

			std::cout << "Started (total " << numDS << ") ." << std::flush;

			for(int i = 0; i < numDS; i++)
			{
				std::cout << "." << i << "." << std::flush;
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }

				dataset->GetGeoTransform(imgTransform);
				tileXSize = dataset->GetRasterXSize();
				tileYSize = dataset->GetRasterYSize();

				xDiff = imgTransform[0] - baseTransform[0];
				yDiff = baseTransform[3] - imgTransform[3];

				xStart = floor(xDiff/baseTransform[1]);
				yStart = floor(yDiff/baseTransform[1]);
				imgData = (float *) CPLMalloc(sizeof(float)*(tileXSize*yBlockSize));

                int nYBlocks = tileYSize / yBlockSize;
                int remainRows = tileYSize - (nYBlocks * yBlockSize);
                int rowOffset = 0;

                for(int i = 0; i < nYBlocks; i++)
                {
                    rowOffset = yBlockSize * i;
                    if(bandsDefined)
                    {
                        int nBand = 1;
                        for(std::vector<int>::iterator iterBands = bands.begin(); iterBands != bands.end(); ++iterBands)
                        {
                            if(((*iterBands) <= 0) | ((*iterBands) > dataset->GetRasterCount()))
                            {
                                throw RSGISImageException("Band is not within the input dataset.");
                            }
                            inputBand = dataset->GetRasterBand(*iterBands);

                            inputBand->RasterIO(GF_Read, 0, rowOffset, tileXSize, yBlockSize, imgData, tileXSize, yBlockSize, GDT_Float32, 0, 0);
                            outputBand[nBand-1]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, yBlockSize, imgData, tileXSize, yBlockSize, GDT_Float32, 0, 0);

                            ++nBand;
                        }
                    }
                    else
                    {
                        if(numberBands != dataset->GetRasterCount())
                        {
                            throw RSGISImageException("The number of bands in the input datasets and base dataset need to be the same.");
                        }

                        for(int n = 1; n <= numberBands; n++)
                        {
                            inputBand = dataset->GetRasterBand(n);

                            inputBand->RasterIO(GF_Read, 0, rowOffset, tileXSize, yBlockSize, imgData, tileXSize, yBlockSize, GDT_Float32, 0, 0);
                            outputBand[n-1]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, yBlockSize, imgData, tileXSize, yBlockSize, GDT_Float32, 0, 0);

                        }
                    }
                }
                if(remainRows > 0)
                {
                    rowOffset = yBlockSize * nYBlocks;
                    if(bandsDefined)
                    {
                        int nBand = 1;
                        for(std::vector<int>::iterator iterBands = bands.begin(); iterBands != bands.end(); ++iterBands)
                        {
                            if(((*iterBands) <= 0) | ((*iterBands) > dataset->GetRasterCount()))
                            {
                                throw RSGISImageException("Band is not within the input dataset.");
                            }
                            inputBand = dataset->GetRasterBand(*iterBands);

                            inputBand->RasterIO(GF_Read, 0, rowOffset, tileXSize, remainRows, imgData, tileXSize, remainRows, GDT_Float32, 0, 0);
                            outputBand[nBand-1]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, remainRows, imgData, tileXSize, remainRows, GDT_Float32, 0, 0);

                            ++nBand;
                        }
                    }
                    else
                    {
                        if(numberBands != dataset->GetRasterCount())
                        {
                            throw RSGISImageException("The number of bands in the input datasets and base dataset need to be the same.");
                        }

                        for(int n = 1; n <= numberBands; n++)
                        {
                            inputBand = dataset->GetRasterBand(n);

                            inputBand->RasterIO(GF_Read, 0, rowOffset, tileXSize, remainRows, imgData, tileXSize, remainRows, GDT_Float32, 0, 0);
                            outputBand[n-1]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, remainRows, imgData, tileXSize, remainRows, GDT_Float32, 0, 0);
                        }
                    }
                }
				delete imgData;
                GDALClose(dataset);
			}
			std::cout << ".complete\n";
		}
		catch(RSGISImageBandException e)
		{
			if(imgData != NULL)
			{
				delete imgData;
			}
			if(transformation != NULL)
			{
				delete[] transformation;
			}
			if(imgTransform != NULL)
			{
				delete[] imgTransform;
			}
			throw e;
		}

		if(transformation != NULL)
		{
			delete[] transformation;
		}
		if(imgTransform != NULL)
		{
			delete[] imgTransform;
		}
	}

    void RSGISImageMosaic::orderInImagesValidData(std::vector<std::string> images, std::vector<std::string> *orderedImages, float noDataValue) throw(RSGISImageException)
    {
        try
        {
            RSGISImageValidDataMetric *validPxlCountObj = new RSGISImageValidDataMetric();
            RSGISCountValidPixels *calcImageValidPxlCount = new RSGISCountValidPixels(validPxlCountObj, noDataValue);
			RSGISCalcImage calcImg = RSGISCalcImage(calcImageValidPxlCount, "", true);

            std::list<RSGISImageValidDataMetric> validDataImageMetrics;
            for(std::vector<std::string>::iterator iterImage = images.begin(); iterImage != images.end(); ++iterImage)
            {
                // Calculate Valid Pixel Count.
                validPxlCountObj->totalNumPxls = 0;
                validPxlCountObj->validPxlCount = 0;
                validPxlCountObj->noDataPxlCount = 0;
                calcImageValidPxlCount->resetValidPxlsObj();

                std::cout << "Processing Image: " << (*iterImage) << std::endl;
                GDALDataset *dataset = (GDALDataset *) GDALOpen((*iterImage).c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + (*iterImage);
                    throw rsgis::RSGISImageException(message.c_str());
                }

                try
                {
                    calcImg.calcImage(&dataset, 1);

                    RSGISImageValidDataMetric imgDataMetric;
                    imgDataMetric.imageFile = (*iterImage);
                    imgDataMetric.totalNumPxls = validPxlCountObj->totalNumPxls;
                    imgDataMetric.validPxlCount = validPxlCountObj->validPxlCount;
                    imgDataMetric.noDataPxlCount = validPxlCountObj->noDataPxlCount;
                    imgDataMetric.validPxlFunc = ((double)validPxlCountObj->validPxlCount) / ((double)validPxlCountObj->totalNumPxls);

                    validDataImageMetrics.push_back(imgDataMetric);

                    GDALClose(dataset);
                }
                catch (rsgis::RSGISException &e)
                {
                    delete validPxlCountObj;
                    delete calcImageValidPxlCount;
                    GDALClose(dataset);
                    throw RSGISImageException(e.what());
                }
                catch (std::exception &e)
                {
                    delete validPxlCountObj;
                    delete calcImageValidPxlCount;
                    GDALClose(dataset);
                    throw RSGISImageException(e.what());
                }
            }

            validDataImageMetrics.sort(compare_ImageValidPxlCounts);

            orderedImages->clear();
            for(std::list<RSGISImageValidDataMetric>::iterator iterImage = validDataImageMetrics.begin(); iterImage != validDataImageMetrics.end(); ++iterImage)
            {
                orderedImages->push_back((*iterImage).imageFile);
            }

            delete validPxlCountObj;
            delete calcImageValidPxlCount;
        }
        catch (RSGISImageException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISImageException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
    }

	RSGISImageMosaic::~RSGISImageMosaic()
	{

	}


    RSGISCountValidPixels::RSGISCountValidPixels(RSGISImageValidDataMetric *validPxlsObj, float noDataVal):RSGISCalcImageValue(0)
    {
        this->validPxlsObj = validPxlsObj;
        this->noDataVal = noDataVal;
    }

    void RSGISCountValidPixels::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
    {
        bool pxlIsNoData = true;
        for(int i = 0; i < numBands; ++i)
        {
            if(bandValues[i] != noDataVal)
            {
                pxlIsNoData = false;
                break;
            }
        }

        if(pxlIsNoData)
        {
            ++this->validPxlsObj->noDataPxlCount;
        }
        else
        {
            ++this->validPxlsObj->validPxlCount;
        }

        ++this->validPxlsObj->totalNumPxls;
    }

    void RSGISCountValidPixels::resetValidPxlsObj()
    {
        this->validPxlsObj->noDataPxlCount = 0;
        this->validPxlsObj->validPxlCount = 0;
        this->validPxlsObj->totalNumPxls = 0;
    }

    RSGISCountValidPixels::~RSGISCountValidPixels()
    {

    }



}}


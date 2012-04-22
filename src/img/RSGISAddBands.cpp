/*
 *  RSGISAddBands.cpp
 *  
 *	RSGIS IMG 
 *
 *	A class to stack image bands to create a new file
 *
 *  Created by Pete Bunting on 04/02/2008.
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

#include "RSGISAddBands.h"

namespace rsgis{namespace img{

img::RSGISAddBands::RSGISAddBands()
{
	
}

void img::RSGISAddBands::addBandToFile(GDALDataset *input, GDALDataset *toAdd, string *outputFile, int band) throw(RSGISImageBandException)
{
	double *inputTrans = new double[6];
	input->GetGeoTransform(inputTrans);
	double *toAddTrans = new double[6];
	toAdd->GetGeoTransform(toAddTrans);
	
	std::cout << "Image Pixel resolutions: X: " <<  inputTrans[1] << " == " << toAddTrans[1] << " Y: " <<  inputTrans[5] << " == " << toAddTrans[5] << std::endl;
	
	if( ((int)inputTrans[1]) != ((int)toAddTrans[1]))
	{
		throw RSGISImageBandException("X Pixel resolutions do not match.");
	}
	
	if( ((int)inputTrans[5]) != ((int)toAddTrans[5]))
	{
		throw RSGISImageBandException("Y Pixel resolutions do not match.");
	}
	
	int pixelXRes = (int)inputTrans[1];
	int pixelYRes = (int)inputTrans[5];
	
	int inputXSize = input->GetRasterXSize();
	int inputYSize = input->GetRasterYSize();
	int toAddXSize = toAdd->GetRasterXSize();
	int toAddYSize = toAdd->GetRasterYSize();
	
	cout << "input image: [" << inputXSize << "," << inputYSize << "] To Add Image: [" << toAddXSize << "," << toAddYSize << "]\n";
	
	double *inputDimensions = new double[4];
	inputDimensions[0] = inputTrans[0];
	inputDimensions[1] = inputTrans[3];
	inputDimensions[2] = inputTrans[0] + (inputXSize * pixelXRes);
	inputDimensions[3] = inputTrans[3] + (inputYSize * pixelYRes);
	double *toAddDimensions = new double[4];
	toAddDimensions[0] = toAddTrans[0];
	toAddDimensions[1] = toAddTrans[3];
	toAddDimensions[2] = toAddTrans[0] + (toAddXSize * pixelXRes);
	toAddDimensions[3] = toAddTrans[3] + (toAddYSize * pixelYRes);
	
	cout << "Input Image: [" << inputDimensions[0] << "," << inputDimensions[1] << "][" << inputDimensions[2] << "," << inputDimensions[3] << "]\n";
	cout << "Image to Add Image: [" << toAddDimensions[0] << "," << toAddDimensions[1] << "][" << toAddDimensions[2] << "," << toAddDimensions[3] << "]\n";
	
	if(inputDimensions[0] < toAddDimensions[0])
	{
		throw RSGISImageBandException("Input image xMin too small.");
	}
	
	if(inputDimensions[1] > toAddDimensions[1])
	{
		throw RSGISImageBandException("Input image yMax too large.");
	}
	
	if(inputDimensions[2] > toAddDimensions[2])
	{
		throw RSGISImageBandException("Input image xMax too large.");
	}
	
	if(inputDimensions[3] < toAddDimensions[3])
	{
		throw RSGISImageBandException("Input image yMin too small.");
	}
	
	cout << "Input Image fits within the image to be added will proceed...\n";
	
	double xDiff = inputDimensions[0] - toAddDimensions[0];
	double yDiff = toAddDimensions[1] - inputDimensions[1];
	
	int xStart = ((int)(xDiff/pixelXRes));
	int yStart = ((int)(yDiff/pixelXRes));
	
	cout << "Start [" << xStart << "," << yStart << "]\n";
	
	GDALDriver *poDriver;
	poDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
	if(poDriver == NULL)
	{
		throw RSGISImageBandException("Driver does not exists!");
	}
	
	int numBands = input->GetRasterCount()+1;
	
	// Create new file. 
	GDALDataset *outputImage = NULL;
	outputImage = poDriver->Create(outputFile->c_str(), inputXSize, inputYSize, numBands, GDT_Float32, NULL);
	outputImage->SetGeoTransform(inputTrans);
	//std::cout << "Projection: " << input->GetProjectionRef() << std::endl;
	outputImage->SetProjection(input->GetProjectionRef());

	float *imgData = (float *) CPLMalloc(sizeof(float)*inputXSize);
	GDALRasterBand *outputBand = NULL;
	GDALRasterBand *inputBand = NULL;
	
	for(int n = 1; n < numBands; n++)
	{
		outputBand = outputImage->GetRasterBand(n);
		inputBand = input->GetRasterBand(n);
		std::cout << "Processing band " << n << " of " << numBands-1 << std::endl;
		for(int i = 0; i < inputYSize; i++)
		{
			inputBand->RasterIO(GF_Read, 0, i, inputXSize, 1, imgData, inputXSize, 1, GDT_Float32, 0, 0);
			outputBand->RasterIO(GF_Write, 0, i, inputXSize, 1, imgData, inputXSize, 1, GDT_Float32, 0, 0);
		}
	}
	
	cout << "Adding new bands data\n";
	
	GDALRasterBand *toAddBand = toAdd->GetRasterBand(band);
	outputBand = outputImage->GetRasterBand(numBands);
	
	for(int i = 0; i < inputYSize; i++)
	{
		toAddBand->RasterIO(GF_Read, xStart, (yStart + i), inputXSize, 1, imgData, inputXSize, 1, GDT_Float32, 0, 0);
		outputBand->RasterIO(GF_Write, 0, i, inputXSize, 1, imgData, inputXSize, 1, GDT_Float32, 0, 0);
	}
	
	delete imgData;
	GDALClose(outputImage);
}

void RSGISAddBands::addMultipleBands(GDALDataset *input, GDALDataset **toAdd, string *outputFile, int *band, int numAddBands) throw(RSGISImageBandException)
{
	double *toAddTrans = NULL;
	double *inputDimensions = NULL;
	double *toAddDimensions = NULL;
	float *imgData = NULL;
	
	int pixelXRes = 0;
	int pixelYRes = 0;
	int inputXSize = 0;
	int inputYSize = 0;
	int toAddXSize = 0;
	int toAddYSize = 0;
	int xStart = 0;
	int yStart = 0;
	int numBands = 0;
	int baseBands = 0;
	double xDiff = 0;
	double yDiff = 0;
	
	GDALRasterBand *outputBand = NULL;
	GDALRasterBand *inputBand = NULL;
	GDALRasterBand *toAddBand = NULL;
	GDALDataset *outputImage = NULL;
	
	double *inputTrans = new double[6];
	input->GetGeoTransform(inputTrans);
	
	cout << "Checking input files ...\n";
	
	for(int i = 0; i < numAddBands; i++)
	{
		toAddTrans = new double[6];
		toAdd[i]->GetGeoTransform(toAddTrans);
		
		if( ((int)inputTrans[1]) != ((int)toAddTrans[1]))
		{
			char buffer [255];
			sprintf(buffer, "X Pixel resolutions do not match. Base %f and additional %f", inputTrans[1], toAddTrans[1]);
			throw RSGISImageBandException(buffer);
		}
		
		if( ((int)inputTrans[5]) != ((int)toAddTrans[5]))
		{
			char buffer [255];
			sprintf(buffer, "Y Pixel resolutions do not match. Base %f and additional %f", inputTrans[5], toAddTrans[5]);
			throw RSGISImageBandException(buffer);
		}
		
		pixelXRes = (int)inputTrans[1];
		pixelYRes = (int)inputTrans[5];
		
		inputXSize = input->GetRasterXSize();
		inputYSize = input->GetRasterYSize();
		toAddXSize = toAdd[i]->GetRasterXSize();
		toAddYSize = toAdd[i]->GetRasterYSize();
		
		//std::cout << "input image: [" << inputXSize << "," << inputYSize << "] To Add Image: [" << toAddXSize << "," << toAddYSize << "]\n";
		
		inputDimensions = new double[4];
		inputDimensions[0] = inputTrans[0];
		inputDimensions[1] = inputTrans[3];
		inputDimensions[2] = inputTrans[0] + (inputXSize * pixelXRes);
		inputDimensions[3] = inputTrans[3] + (inputYSize * pixelYRes);
		toAddDimensions = new double[4];
		toAddDimensions[0] = toAddTrans[0];
		toAddDimensions[1] = toAddTrans[3];
		toAddDimensions[2] = toAddTrans[0] + (toAddXSize * pixelXRes);
		toAddDimensions[3] = toAddTrans[3] + (toAddYSize * pixelYRes);
		
		//std::cout << "Input Image: [" << inputDimensions[0] << "," << inputDimensions[1] << "][" << inputDimensions[2] << "," << inputDimensions[3] << "]\n";
		//std::cout << "Image to Add Image: [" << toAddDimensions[0] << "," << toAddDimensions[1] << "][" << toAddDimensions[2] << "," << toAddDimensions[3] << "]\n";
		
		if(inputDimensions[0] < toAddDimensions[0])
		{
			char buffer [255];
			sprintf(buffer, "Input image xMin too small. Base[xMin] = %f Input[xMin] = %f", inputDimensions[0], toAddDimensions[0]);
			throw RSGISImageBandException(buffer);
		}
		
		if(inputDimensions[1] > toAddDimensions[1])
		{
			char buffer [255];
			sprintf(buffer, "Input image yMax too large. Base[yMax] = %f Input[yMax] = %f", inputDimensions[1], toAddDimensions[1]);
			throw RSGISImageBandException(buffer);
		}
		
		if(inputDimensions[2] > toAddDimensions[2])
		{
			char buffer [255];
			sprintf(buffer, "Input image xMax too large. Base[xMax] = %f Input[xMax] = %f", inputDimensions[2], toAddDimensions[2]);
			throw RSGISImageBandException(buffer);
		}
		
		if(inputDimensions[3] < toAddDimensions[3])
		{
			char buffer [255];
			sprintf(buffer, "Input image yMin too small. Base[yMin] = %f Input[yMin] = %f", inputDimensions[3], toAddDimensions[3]);
			throw RSGISImageBandException(buffer);
		}
		
	}
	
	cout << "All files have successfully been checked so stacking can proceed.\n";
	
	numBands = input->GetRasterCount()+numAddBands;
	baseBands = input->GetRasterCount();
	pixelXRes = (int)inputTrans[1];
	pixelYRes = (int)inputTrans[5];
	inputXSize = input->GetRasterXSize();
	inputYSize = input->GetRasterYSize();
	inputDimensions = new double[4];
	inputDimensions[0] = inputTrans[0];
	inputDimensions[1] = inputTrans[3];
	inputDimensions[2] = inputTrans[0] + (inputXSize * pixelXRes);
	inputDimensions[3] = inputTrans[3] + (inputYSize * pixelYRes);
	
	GDALDriver *poDriver;
	poDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
	if(poDriver == NULL)
	{
		throw RSGISImageBandException("ENVI Driver does not exists!");
	}
	
	// Create new file. 
	outputImage = poDriver->Create(outputFile->c_str(), inputXSize, inputYSize, numBands, GDT_Float32, NULL);
	outputImage->SetGeoTransform(inputTrans);
	//std::cout << "Projection: " << input->GetProjectionRef() << std::endl;
	outputImage->SetProjection(input->GetProjectionRef());
	
	imgData = (float *) CPLMalloc(sizeof(float)*inputXSize);
	for(int n = 1; n <= baseBands; n++)
	{
		outputBand = outputImage->GetRasterBand(n);
		inputBand = input->GetRasterBand(n);
		std::cout << "Processing band " << n << " of " << baseBands << std::endl;
		for(int i = 0; i < inputYSize; i++)
		{
			inputBand->RasterIO(GF_Read, 0, i, inputXSize, 1, imgData, inputXSize, 1, GDT_Float32, 0, 0);
			outputBand->RasterIO(GF_Write, 0, i, inputXSize, 1, imgData, inputXSize, 1, GDT_Float32, 0, 0);
		}
	}
	
	for(int i = 0; i < numAddBands; i++)
	{
		toAdd[i]->GetGeoTransform(toAddTrans);
		
		toAddXSize = toAdd[i]->GetRasterXSize();
		toAddYSize = toAdd[i]->GetRasterYSize();
		
		toAddDimensions = new double[4];
		toAddDimensions[0] = toAddTrans[0];
		toAddDimensions[1] = toAddTrans[3];
		toAddDimensions[2] = toAddTrans[0] + (toAddXSize * pixelXRes);
		toAddDimensions[3] = toAddTrans[3] + (toAddYSize * pixelYRes);
		
		xDiff = inputDimensions[0] - toAddDimensions[0];
		yDiff = toAddDimensions[1] - inputDimensions[1];
		
		xStart = ((int)(xDiff/pixelXRes));
		yStart = ((int)(yDiff/pixelXRes));
		
		std::cout << "Adding addtional band " << i << " of " << numAddBands << std::endl;
		toAddBand = toAdd[i]->GetRasterBand(band[i]);
		cout << "Output band: " <<  baseBands+i+1 << endl;
		outputBand = outputImage->GetRasterBand(baseBands+i+1);
		
		for(int n = 0; n < inputYSize; n++)
		{
			toAddBand->RasterIO(GF_Read, xStart, (yStart + n), inputXSize, 1, imgData, inputXSize, 1, GDT_Float32, 0, 0);
			outputBand->RasterIO(GF_Write, 0, n, inputXSize, 1, imgData, inputXSize, 1, GDT_Float32, 0, 0);
		}
	}
	
	delete imgData;
	GDALClose(outputImage);
}
	
	void RSGISAddBands::stackImages(GDALDataset **datasets, int numDS, string outputImage, string *imageBandNames) throw(RSGISImageBandException)
	{
		bool useBandNames = false;
		if(imageBandNames != NULL)
		{	
			useBandNames = true;
		}
		
		GDALAllRegister();
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numDS];
		for(int i = 0; i < numDS; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int **bandOffsets = NULL;
		int height = 0;
		int width = 0;
		int numInBands = 0;
		
		float **inputData = NULL;
		
		GDALDataset *outputImageDS = NULL;
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		GDALDriver *gdalDriver = NULL;
		
		try
		{
			// Find image overlap
			imgUtils.getImageOverlap(datasets, numDS, dsOffsets, &width, &height, gdalTranslation);
			
			// Count number of image bands
			for(int i = 0; i < numDS; i++)
			{
				numInBands += datasets[i]->GetRasterCount();
			}
			
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
			if(gdalDriver == NULL)
			{
				throw RSGISImageBandException("ENVI driver does not exists..");
			}
			cout << "New image width = " << width << " height = " << height << endl;
			outputImageDS = gdalDriver->Create(outputImage.c_str(), width, height, numInBands, GDT_Float32, NULL);
			outputImageDS->SetGeoTransform(gdalTranslation);
			outputImageDS->SetProjection(datasets[0]->GetProjectionRef());
			
			// Get Image Input Bands
			bandOffsets = new int*[numInBands];
            string *bandNames = new string[numInBands];
			inputRasterBands = new GDALRasterBand*[numInBands];
			int counter = 0;
			for(int i = 0; i < numDS; i++)
			{
				for(int j = 0; j < datasets[i]->GetRasterCount(); j++)
				{
					inputRasterBands[counter] = datasets[i]->GetRasterBand(j+1);
                    if (useBandNames){bandNames[counter] = imageBandNames[i];}
					bandOffsets[counter] = new int[2];
					bandOffsets[counter][0] = dsOffsets[i][0];
					bandOffsets[counter][1] = dsOffsets[i][1];
					//cout << counter << ") dataset " << i << " band " << j << " offset [" << bandOffsets[counter][0] << "," << bandOffsets[counter][1] << "]\n";
					counter++;
				}
			}
			
			//Get Image Output Bands
			outputRasterBands = new GDALRasterBand*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				outputRasterBands[i] = outputImageDS->GetRasterBand(i+1);
				if (useBandNames) // Set band name (if being used)
				{
					outputRasterBands[i]->SetDescription(bandNames[i].c_str());
				}
			}
			
			// Allocate memory
			inputData = new float*[numInBands];
			for(int i = 0; i < numInBands; i++)
			{
				inputData[i] = (float *) CPLMalloc(sizeof(float)*width);
			}
			
			int feedback = height/10;
			int feedbackCounter = 0;
			cout << "Started" << flush;
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{
				//cout << i << " of " << height << endl;
				
				if((i % feedback) == 0)
				{
					cout << ".." << feedbackCounter << ".." << flush;
					feedbackCounter = feedbackCounter + 10;
				}
				
				for(int n = 0; n < numInBands; n++)
				{
					inputRasterBands[n]->RasterIO(GF_Read, bandOffsets[n][0], (bandOffsets[n][1]+i), width, 1, inputData[n], width, 1, GDT_Float32, 0, 0);
				}
				
				for(int n = 0; n < numInBands; n++)
				{
					outputRasterBands[n]->RasterIO(GF_Write, 0, i, width, 1, inputData[n], width, 1, GDT_Float32, 0, 0);
				}
			}
			cout << " Complete.\n";
		}
		catch(RSGISImageBandException e)
		{
			//GDALClose(outputImageDS);
			
			if(gdalTranslation != NULL)
			{
				delete[] gdalTranslation;
			}
			
			if(dsOffsets != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					if(dsOffsets[i] != NULL)
					{
						delete[] dsOffsets[i];
					}
				} 
				delete[] dsOffsets;
			}
			
			if(inputData != NULL)
			{
				for(int i = 0; i < numInBands; i++)
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
			throw e;
		}
		
		GDALClose(outputImageDS);
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		
		if(dsOffsets != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				if(dsOffsets[i] != NULL)
				{
					delete[] dsOffsets[i];
				}
			} 
			delete[] dsOffsets;
		}
		
		if(inputData != NULL)
		{
			for(int i = 0; i < numInBands; i++)
			{
				if(inputData[i] != NULL)
				{
					CPLFree(inputData[i]);
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
	}

RSGISAddBands::~RSGISAddBands()
{
	
}

}} // rsgis::img

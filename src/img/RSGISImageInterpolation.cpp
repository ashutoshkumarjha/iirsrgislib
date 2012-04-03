/*
 *  RSGISImageInterpolation.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/05/2008.
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

#include "RSGISImageInterpolation.h"

namespace rsgis{namespace img{
	
	RSGISImageInterpolation::RSGISImageInterpolation(RSGISInterpolator *interpolator)
	{
		this->interpolator = interpolator;
	}
	
	void RSGISImageInterpolation::interpolateNewImage(GDALDataset *data,
															  double outputXResolution, 
															  double outputYResolution, 
															  string filename) throw(RSGISFileException, RSGISImageException)
	{
		// Image Data Stores.
		float *scanline0 = NULL;
		float *scanline1 = NULL;
		float *scanline2 = NULL;
		double *transformation = NULL;
		float *newLine = NULL;
		double *pixels = NULL;
		GDALDriver *poDriver = NULL;
		GDALRasterBand *outputRasterBand = NULL;
		GDALRasterBand *rasterband = NULL;
		try
		{
			GDALDataset *output;
			/********************** Calculate Scaling *************************/
			transformation = new double[6];
			data->GetGeoTransform(transformation);	
			
			int dataXSize = data->GetRasterXSize();
			int dataYSize = data->GetRasterYSize();
			
			double inputXResolution = transformation[1];
			double inputYResolution = transformation[5];
			
			/*if(inputXResolution < 0)
			{
				inputXResolution = inputXResolution * (-1);
			}
			
			if(inputYResolution < 0)
			{
				inputYResolution = inputYResolution * (-1);
			}*/
			
			double xScale = inputXResolution/outputXResolution;
			double yScale = inputYResolution/outputYResolution;
			
			int xSize = static_cast<int>(dataXSize*xScale);  //mathUtils.round(dataXSize*xScale);
			int ySize = static_cast<int>(dataYSize*yScale); //mathUtils.round(dataYSize*yScale);;
			int bands = data->GetRasterCount();
			
			cout << "size [" << xSize << "," << ySize << "]\n";
			
			transformation[1] = outputXResolution;
			transformation[5] = outputYResolution;
			/*******************************************************************/
			
			/************ Output Image with the New registration *******************/   
			poDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
			if(poDriver == NULL)
			{
				throw RSGISImageException("Could not find ENVI driver");
			}
			
			output = poDriver->Create(filename.c_str(), xSize, ySize, bands, GDT_Float32, poDriver->GetMetadata());
			output->SetGeoTransform(transformation);
			output->SetProjection(data->GetProjectionRef());
			
			
			scanline0 = (float *) CPLMalloc(sizeof(float)*dataXSize);
			scanline1 = (float *) CPLMalloc(sizeof(float)*dataXSize);
			scanline2 = (float *) CPLMalloc(sizeof(float)*dataXSize);
			
			newLine = (float *)CPLMalloc(sizeof(float)*xSize);
			pixels = new double[9];
			
			int column = 0;
			int row = 0;
			double xShift = 0;
			double yShift = 0;
						
			for(int n = 1; n <= bands; n++)
			{
				cout << "Interpolating band "  << n << ".." << endl;
				outputRasterBand = output->GetRasterBand(n);
				rasterband = data->GetRasterBand(n);
				
				int feedback = ySize/10;
				int feedbackCounter = 0;
				cout << "Started " << flush;
				for( int i = 0; i < ySize; i++)
				{
					if((ySize > 10) && (i % feedback) == 0)
					{
						cout << ".." << feedbackCounter << ".." << flush;
						feedbackCounter = feedbackCounter + 10;
					}
					
					
					yShift = this->findFloatingPointComponent(((i*outputYResolution)/inputYResolution),
															  &row);
					if(row == 0)
					{
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row, 
											 dataXSize, 
											 1, 
											 scanline0, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row, 
											 dataXSize, 
											 1, 
											 scanline1, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row+1, 
											 dataXSize, 
											 1, 
											 scanline2, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
					}
					else if(row == (dataYSize-1))
					{
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row-1, 
											 dataXSize, 
											 1, 
											 scanline0, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row, 
											 dataXSize, 
											 1, 
											 scanline1, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row, 
											 dataXSize, 
											 1, 
											 scanline2, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
					}
					else
					{
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row-1, 
											 dataXSize, 
											 1, 
											 scanline0, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row, 
											 dataXSize, 
											 1, 
											 scanline1, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
						rasterband->RasterIO(GF_Read, 
											 0, 
											 row+1, 
											 dataXSize, 
											 1, 
											 scanline2, 
											 dataXSize, 
											 1, 
											 GDT_Float32, 
											 0, 
											 0);
					}
					for(int j = 0; j < xSize; j++)
					{
						xShift = this->findFloatingPointComponent(((j*outputXResolution)/inputXResolution), 
																  &column);
						if(column == 0)
						{
							//Column 1
							pixels[0] = scanline0[column];
							pixels[3] = scanline1[column];
							pixels[6] = scanline2[column];
							//Column 2
							pixels[1] = scanline0[column];
							pixels[4] = scanline1[column];
							pixels[7] = scanline2[column];
							//Column 1
							pixels[2] = scanline0[column+1];
							pixels[5] = scanline1[column+1];
							pixels[8] = scanline2[column+1];
						}
						else if(column == (dataXSize-1))
						{
							//Column 1
							pixels[0] = scanline0[column-1];
							pixels[3] = scanline1[column-1];
							pixels[6] = scanline2[column-1];
							//Column 2
							pixels[1] = scanline0[column];
							pixels[4] = scanline1[column];
							pixels[7] = scanline2[column];
							//Column 1
							pixels[2] = scanline0[column];
							pixels[5] = scanline1[column];
							pixels[8] = scanline2[column];
						}
						else
						{
							//Column 1
							pixels[0] = scanline0[column-1];
							pixels[3] = scanline1[column-1];
							pixels[6] = scanline2[column-1];
							//Column 2
							pixels[1] = scanline0[column];
							pixels[4] = scanline1[column];
							pixels[7] = scanline2[column];
							//Column 1
							pixels[2] = scanline0[column+1];
							pixels[5] = scanline1[column+1];
							pixels[8] = scanline2[column+1];
						}
						newLine[j] = interpolator->interpolate(xShift,yShift,pixels);
					}
					outputRasterBand->RasterIO(GF_Write, 0, i, xSize, 1, newLine, xSize, 1, GDT_Float32, 0, 0);
				}
				cout << " Completed\n";
			}
			GDALClose(output);
			cout << "Interpolation complete\n";
		}
		catch(RSGISFileException e)
		{
			if( transformation != NULL )
			{
				delete transformation;
			}
			if(newLine != NULL)
			{
				delete newLine;
			}
			if( pixels != NULL )
			{
				delete pixels;
			}
			if( scanline0 != NULL )
			{
				delete scanline0;
			}
			if( scanline1 != NULL )
			{
				delete scanline1;
			}
			if( scanline2 != NULL )
			{
				delete scanline2;
			}
			throw e;
		}
		catch(RSGISImageException e)
		{
			if( transformation != NULL )
			{
				delete transformation;
			}
			if(newLine != NULL)
			{
				delete newLine;
			}
			if( pixels != NULL )
			{
				delete pixels;
			}
			if( scanline0 != NULL )
			{
				delete scanline0;
			}
			if( scanline1 != NULL )
			{
				delete scanline1;
			}
			if( scanline2 != NULL )
			{
				delete scanline2;
			}
			throw e;
		}
		
		if( transformation != NULL )
		{
			delete transformation;
		}
		if(newLine != NULL)
		{
			delete newLine;
		}
		if( pixels != NULL )
		{
			delete pixels;
		}
		if( scanline0 != NULL )
		{
			delete scanline0;
		}
		if( scanline1 != NULL )
		{
			delete scanline1;
		}
		if( scanline2 != NULL )
		{
			delete scanline2;
		}
	}
	
	void RSGISImageInterpolation::interpolateNewImage(GDALDataset *data,
															  double outputXResolution, 
															  double outputYResolution, 
															  string filename,
															  int band) throw(RSGISFileException, RSGISImageException)
	{
		// Image Data Stores.
		float *scanline0 = NULL;
		float *scanline1 = NULL;
		float *scanline2 = NULL;
		double *transformation = NULL;
		float *newLine = NULL;
		double *pixels = NULL;
		GDALDriver *poDriver = NULL;
		GDALRasterBand *outputRasterBand = NULL;
		GDALRasterBand *rasterband = NULL;
		try
		{
			GDALDataset *output;
			/********************** Calculate Scaling *************************/
			transformation = new double[6];
			data->GetGeoTransform(transformation);	
			
			int dataXSize = data->GetRasterXSize();
			int dataYSize = data->GetRasterYSize();
			
			double inputXResolution = transformation[1];
			if(inputXResolution < 0)
			{
				inputXResolution = inputXResolution * (-1);
			}
			double inputYResolution = transformation[5];
			if(inputYResolution < 0)
			{
				inputYResolution = inputYResolution * (-1);
			}
			
			double xScale = inputXResolution/outputXResolution;
			double yScale = inputYResolution/outputYResolution;
			
			int xSize = static_cast<int>(dataXSize*xScale);  //mathUtils.round(dataXSize*xScale);
			int ySize = static_cast<int>(dataYSize*yScale); //mathUtils.round(dataYSize*yScale);;
			
			transformation[1] = outputXResolution;
			transformation[5] = outputYResolution;
			/*******************************************************************/
			
			/************ Output Image with the New registration *******************/   
			poDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
			if(poDriver == NULL)
			{
				throw RSGISImageException("Could not find ENVI driver");
			}
			
			output = poDriver->Create(filename.c_str(), xSize, ySize, 1, GDT_Float32, poDriver->GetMetadata());
			output->SetGeoTransform(transformation);
			output->SetProjection(data->GetProjectionRef());
			
			
			scanline0 = (float *) CPLMalloc(sizeof(float)*dataXSize);
			scanline1 = (float *) CPLMalloc(sizeof(float)*dataXSize);
			scanline2 = (float *) CPLMalloc(sizeof(float)*dataXSize);
			
			newLine = (float *)CPLMalloc(sizeof(float)*xSize);
			pixels = new double[9];
			
			int column = 0;
			int row = 0;
			double xShift = 0;
			double yShift = 0;
			
			outputRasterBand = output->GetRasterBand(1);
			rasterband = data->GetRasterBand(band);
			
			
			int feedback = ySize/10;
			int feedbackCounter = 0;
			cout << "Started Interpolating";
			
			for( int i = 0; i < ySize; i++)
			{
				if((ySize > 10) && (i % feedback) == 0)
				{
					cout << ".." << feedbackCounter << "..";
					feedbackCounter = feedbackCounter + 10;
				}
				yShift = this->findFloatingPointComponent(((i*outputYResolution)/inputYResolution),
														  &row);
				if(row == 0)
				{
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row, 
										 dataXSize, 
										 1, 
										 scanline0, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row, 
										 dataXSize, 
										 1, 
										 scanline1, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row+1, 
										 dataXSize, 
										 1, 
										 scanline2, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
				}
				else if(row == (dataYSize-1))
				{
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row-1, 
										 dataXSize, 
										 1, 
										 scanline0, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row, 
										 dataXSize, 
										 1, 
										 scanline1, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row, 
										 dataXSize, 
										 1, 
										 scanline2, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
				}
				else
				{
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row-1, 
										 dataXSize, 
										 1, 
										 scanline0, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row, 
										 dataXSize, 
										 1, 
										 scanline1, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
					rasterband->RasterIO(GF_Read, 
										 0, 
										 row+1, 
										 dataXSize, 
										 1, 
										 scanline2, 
										 dataXSize, 
										 1, 
										 GDT_Float32, 
										 0, 
										 0);
				}
				for(int j = 0; j < xSize; j++)
				{
					xShift = this->findFloatingPointComponent(((j*outputXResolution)/inputXResolution), 
															  &column);
					if(column == 0)
					{
						//Column 1
						pixels[0] = scanline0[column];
						pixels[3] = scanline1[column];
						pixels[6] = scanline2[column];
						//Column 2
						pixels[1] = scanline0[column];
						pixels[4] = scanline1[column];
						pixels[7] = scanline2[column];
						//Column 1
						pixels[2] = scanline0[column+1];
						pixels[5] = scanline1[column+1];
						pixels[8] = scanline2[column+1];
					}
					else if(column == (dataXSize-1))
					{
						//Column 1
						pixels[0] = scanline0[column-1];
						pixels[3] = scanline1[column-1];
						pixels[6] = scanline2[column-1];
						//Column 2
						pixels[1] = scanline0[column];
						pixels[4] = scanline1[column];
						pixels[7] = scanline2[column];
						//Column 1
						pixels[2] = scanline0[column];
						pixels[5] = scanline1[column];
						pixels[8] = scanline2[column];
					}
					else
					{
						//Column 1
						pixels[0] = scanline0[column-1];
						pixels[3] = scanline1[column-1];
						pixels[6] = scanline2[column-1];
						//Column 2
						pixels[1] = scanline0[column];
						pixels[4] = scanline1[column];
						pixels[7] = scanline2[column];
						//Column 1
						pixels[2] = scanline0[column+1];
						pixels[5] = scanline1[column+1];
						pixels[8] = scanline2[column+1];
					}
					newLine[j] = interpolator->interpolate(xShift,yShift,pixels);
				}
				outputRasterBand->RasterIO(GF_Write, 0, i, xSize, 1, newLine, xSize, 1, GDT_Float32, 0, 0);
			}
			GDALClose(output);
			cout << ".. Complete." << endl;
		}
		catch(RSGISFileException e)
		{
			if( transformation != NULL )
			{
				delete transformation;
			}
			if(newLine != NULL)
			{
				delete newLine;
			}
			if( pixels != NULL )
			{
				delete pixels;
			}
			if( scanline0 != NULL )
			{
				delete scanline0;
			}
			if( scanline1 != NULL )
			{
				delete scanline1;
			}
			if( scanline2 != NULL )
			{
				delete scanline2;
			}
			throw e;
		}
		catch(RSGISImageException e)
		{
			if( transformation != NULL )
			{
				delete transformation;
			}
			if(newLine != NULL)
			{
				delete newLine;
			}
			if( pixels != NULL )
			{
				delete pixels;
			}
			if( scanline0 != NULL )
			{
				delete scanline0;
			}
			if( scanline1 != NULL )
			{
				delete scanline1;
			}
			if( scanline2 != NULL )
			{
				delete scanline2;
			}
			throw e;
		}
		
		if( transformation != NULL )
		{
			delete transformation;
		}
		if(newLine != NULL)
		{
			delete newLine;
		}
		if( pixels != NULL )
		{
			delete pixels;
		}
		if( scanline0 != NULL )
		{
			delete scanline0;
		}
		if( scanline1 != NULL )
		{
			delete scanline1;
		}
		if( scanline2 != NULL )
		{
			delete scanline2;
		}

	}
	
	double RSGISImageInterpolation::findFloatingPointComponent(double floatingPointNum, int *integer)
	{
		//std::cout << "Starting find floating point\n";
		*integer = 0;
		bool negative = false;
		if(floatingPointNum < 0)
		{
			floatingPointNum = floatingPointNum * (-1);
			negative = true;
		}
		
		int reduction = 100000000;
		
		while(floatingPointNum > 1)
		{
			while(floatingPointNum > reduction & reduction != 0)
			{
				//std::cout << "floatingPointNum = " << floatingPointNum << " reduction = " << reduction << std::endl;
				floatingPointNum = floatingPointNum - reduction;
				*integer = *integer + reduction;
			}
			//std::cout << "Adjust reduction: floatingPointNum = " << floatingPointNum << std::endl;
			reduction = reduction / 10;
		}
		if(floatingPointNum > -0.000000001 & floatingPointNum < 0.000000001)
		{
			floatingPointNum = 0;
		}
		if(floatingPointNum > 0.999999999 & floatingPointNum < 1.000000001)
		{
			floatingPointNum = 0;
			*integer = *integer + 1;
		}
		
		if( negative )
		{
			*integer = (*integer) * (-1);
		}
		//std::cout << "finished floating point\n";
		return floatingPointNum;
	}
	
	void RSGISImageInterpolation::findOutputResolution(GDALDataset *dataset, float scale, int *outResolutionX, int *outResolutionY)
	{
		double *transformation = new double[6];
		dataset->GetGeoTransform(transformation);	
		int inResolutionX = transformation[1];
		int inResolutionY = transformation[5];
		
		*outResolutionX = ceil(inResolutionX * scale);
		*outResolutionY = ceil(inResolutionY * scale);
		delete[] transformation;
	}
	
	RSGISImageInterpolation::~RSGISImageInterpolation()
	{
		
	}
	
}}

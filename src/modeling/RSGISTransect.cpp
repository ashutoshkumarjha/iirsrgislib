/*
 *  RSGISTransect.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 17/11/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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

#include "RSGISTransect.h"

namespace rsgis{ namespace modeling  {
	RSGISTransect::RSGISTransect(unsigned int transectWidth, unsigned int transectLenght, unsigned int transectHeight, double transectRes)
	{
		this->transectLenght = transectLenght;
		this->transectWidth = transectWidth;
		this->transectHeight = transectHeight;
		this->transectRes = transectRes;
		
		// Allocate memory for the array
		this->transectData = new char**[transectWidth];
		for(unsigned int i = 0; i < transectWidth; ++i)
		{
			transectData[i] = new char*[transectLenght];
			for(unsigned int j = 0; j < transectLenght; ++j)
			{
				transectData[i][j] = new char[transectHeight];
				for(unsigned int k = 0; k < transectHeight; ++k)
				{
					transectData[i][j][k] = 0;
				}
			}
		}
		
		// TO DO: Add check to ensure array will fit into memory.
	}
	void RSGISTransect::setZero()
	{
		// Set all values of the array to zero
		for(unsigned int i = 0; i < transectWidth; ++i)
		{
			for(unsigned int j = 0; j < transectLenght; ++j)
			{
				for(unsigned int k = 0; k < transectHeight; ++k)
				{
					transectData[i][j][k] = 0;
				}
			}
		}
	}
	void RSGISTransect::setVal(char newVal)
	{
		// Set all values of the array to newVal
		for(unsigned int i = 0; i < transectWidth; ++i)
		{
			for(unsigned int j = 0; j < transectLenght; ++j)
			{
				for(unsigned int k = 0; k < transectHeight; ++k)
				{
					transectData[i][j][k] = newVal;
				}
			}
		}
	}
	unsigned int RSGISTransect::getWidth()
	{
		// Returns transect width (x) in voxels
		return this->transectWidth;
	}
	unsigned int RSGISTransect::getLenth()
	{
		// Retruns transect lenght (y) in voxels
		return this->transectLenght;;
	}
	unsigned int RSGISTransect::getHeight()
	{
		// Returns transet height (z)
		return this->transectHeight ;
	}
	double RSGISTransect::getRes()
	{
		// Return transect resolution (m) in voxels
		return this->transectRes;
	}
	char RSGISTransect::getValue(unsigned int xCord, unsigned int yCord, unsigned int zCord)
	{
		// Get value from array - throws exception if values outside array are reffernced
		if(xCord >= transectWidth)
		{
			throw RSGISModelingException("Width greater than transect max!");
		}
		if(yCord >= transectLenght)
		{
			throw RSGISModelingException("Lenght greater than transect max!");
		}
		if(zCord >= transectHeight)
		{
			throw RSGISModelingException("Height greater than transect max!");
		}
		
		char transectVal = transectData[xCord][yCord][this->transectHeight - zCord];
		return transectVal;
		
	}
	void RSGISTransect::setValue(unsigned int xCord, unsigned int yCord, unsigned int zCord, char transectVal)
	{
		// Set array value - throws exception if values outside array are reffernced
		/*if(xCord > transectWidth)
		{
			throw RSGISModelingException("Width greater than transect max!");
		}
		if(yCord > transectLenght)
		{
			throw RSGISModelingException("Length greater than transect max!");
		}
		if(zCord > transectHeight)
		{
			throw RSGISModelingException("Height greater than transect max!");
		}*/
		
		// Only sets value if within transect, ignore is outside.
		
		if(xCord > 0 and yCord > 0 and zCord > 0)
		{
			if(xCord < transectWidth and yCord < transectLenght and zCord < transectHeight) 
			{
				transectData[xCord][yCord][this->transectHeight - zCord] = transectVal;
			}
		}
	}
	unsigned int RSGISTransect::countPoints()
	{
		unsigned int nPoints = 0;
		
		for(unsigned int i = 0; i < transectWidth; ++i)
		{
			for(unsigned int j = 0; j < transectLenght; ++j)
			{
				for(unsigned int k = 0; k < transectHeight; ++k)
				{
					unsigned int melement = transectData[i][j][k];
					if(melement > 0)
					{
						nPoints++;
					}
				}
			}
		}
		
		return nPoints;
	}
	void RSGISTransect::exportImage(string outFileName)
	{
		// Generate image - Produces an ENVI file displaying a flattened view of the canopy
		RSGISMatrices matrixUtils;
		Matrix *imageMatrix = matrixUtils.createMatrix(transectLenght, transectWidth);
		
		int mIndex = 0;
		int mElement = 0;
		
		for(unsigned int i = 0; i < transectLenght; i++)
		{
			for(unsigned int j = 0; j < transectWidth; j++)
			{
				for(unsigned int k = 0; k < transectHeight; k++)
				{
					mElement = transectData[j][i][k];
					if(mElement != 0)
					{
						imageMatrix->matrix[mIndex] = mElement;
						mIndex++;
						break;
					}
					else if(k == transectHeight - 1)
					{
						imageMatrix->matrix[mIndex] = 0;
						mIndex++;
					}
					
				}
			}
		}
		
		matrixUtils.exportAsImage(imageMatrix, outFileName);
		delete imageMatrix;
	}
	void RSGISTransect::exportSPD(string outFileName)
	{
		
	}
	void RSGISTransect::exportPlot(string outFileName, unsigned int numPts)
	{
		double *xCoord = new double[numPts];
		double *yCoord = new double[numPts];
		double *zCoord = new double[numPts];
		double *col = new double[numPts];
		
		unsigned int pIndex = 0;
		
		for(unsigned int i = 0; i < transectWidth; i++)
		{
			for(unsigned int j = 0; j < transectLenght; j++)
			{
				for(unsigned int k = 0; k < transectHeight; k++)
				{
					unsigned int mElement = transectData[i][j][k];
					if(mElement != 0)
					{
						xCoord[pIndex] = i;
						yCoord[pIndex] = j;
						zCoord[pIndex] = this->transectHeight - k;
						col[pIndex] = mElement;
						pIndex++;
					}
				}
			}
		}
		
		RSGISExportForPlotting::getInstance()->export3DColourScatter(outFileName, xCoord, yCoord, zCoord, col, numPts);
		
		cout << "Saved " << numPts << " points to: " << outFileName << endl;

		delete[] xCoord;
		delete[] yCoord;
		delete[] zCoord;
		delete[] col;
		
	}
	
	RSGISTransect::~RSGISTransect()
	{
		for(unsigned int i = 0; i < transectWidth; ++i)
		{
			for(unsigned int j = 0; j < transectLenght; ++j)
			{
				delete[] transectData[i][j];
			}
			delete[] transectData[i];
		}
		delete[] transectData;
	}
}}


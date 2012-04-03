/*
 *  RSGISPixelInPoly.cpp
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 05/03/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
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

#include "RSGISPixelInPoly.h"

namespace rsgis{namespace img {
	
	RSGISPixelInPoly::RSGISPixelInPoly(pixelInPolyOption method)
	{
		this->method = method;
		this->polyOGRPoly = NULL;
	}
	bool RSGISPixelInPoly::findPixelInPoly(OGRGeometry *poly, OGRPolygon *pixel) throw(RSGISVectorException)
	{
		bool returnVal = false;
		
		// Check geometry
		if (poly->IsEmpty()) 
		{
			cout << "Polygon is empty" << endl;
		}
		if (pixel->IsEmpty())
		{
			cout << "Pixel is empty" << endl;
		}
		
		if(method == polyContainsPixel) // Polygon completely contains pixel
		{						
			returnVal = poly->Contains(pixel);
		}
		else if(method == polyContainsPixelCenter) // Pixel center is within the polygon
		{
			OGRPoint *centerPoint = NULL;
			centerPoint = new OGRPoint(0,0);
			pixel->Centroid(centerPoint);
			returnVal = poly->Contains(centerPoint);
			delete centerPoint;
		}
		else if(method == polyOverlapsPixel) // Polygon overlaps the pixel
		{
			returnVal = poly->Overlaps(pixel);
		}
		else if(method == polyOverlapsOrContainsPixel) // Polygon overlaps or contains the pixel
		{
			if (poly->Overlaps(pixel) or poly->Contains(pixel))
			{
				returnVal = true;
			}
			else 
			{
				returnVal = false;
			}
		}
		else if(method == pixelContainsPoly) // Pixel contains the polygon
		{
			returnVal = pixel->Contains(poly);
		}
		else if(method == pixelContainsPoly) // Pixel contains the polygon
		{
			returnVal = pixel->Contains(poly);
		}
		else if(method == pixelContainsPolyCenter)
		{
			this->polyOGRPoly = (OGRPolygon*) poly->clone();
			OGRPoint *centerPoint = NULL;
			centerPoint = new OGRPoint(0,0);
			polyOGRPoly->Centroid(centerPoint);
			returnVal = poly->Contains(centerPoint);
			delete centerPoint;
		}
		else if(method == envelope)
		{
			returnVal = true;
		}
		else if(method == adaptive) // The method is chosen based on relative areas of pixel and polygon.
		{
			int factor = 1;
			
			this->polyOGRPoly = (OGRPolygon*) poly->clone();
			
			double pixelArea = pixel->get_Area();
			double polyArea = polyOGRPoly->get_Area();
			
			if (polyArea > (pixelArea * factor))
			{
				//cout << "Check pixel contains poly" << endl;
				returnVal = pixel->Contains(poly);
			}
			else if (polyArea < (pixelArea / factor))
			{
				//cout << "Check poly contains pixel" << endl;
				returnVal = pixel->Within(poly);
			}
			else
			{
				//cout << "Check pixel contains poly center" << endl;
				this->polyOGRPoly = (OGRPolygon*) poly->clone();
				OGRPoint *centerPoint = NULL;
				centerPoint = new OGRPoint(0,0);
				polyOGRPoly->Centroid(centerPoint);
				returnVal = poly->Contains(centerPoint);
				delete centerPoint;
			}
			
		}
		else if(method == pixelAreaInPoly)
		{
			throw RSGISVectorException("The option 'pixelAreaInPoly' can not be used here");		
		}
		else 
		{
			throw RSGISVectorException("Method for determining pixel in polygon was not recognised");
		}
		
		return returnVal;
	}
	
	RSGISPixelInPoly::~RSGISPixelInPoly()
	{
		delete polyOGRPoly;
	}
}}


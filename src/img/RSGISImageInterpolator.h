/*
 *  RSGISImageInterpolator.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 15/05/2008.
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

#ifndef RSGISImageInterpolator_H
#define RSGISImageInterpolator_H

#include <iostream>
#include <string>

#include "common/RSGISImageException.h"

using namespace std;
using namespace rsgis;

namespace rsgis{namespace img{
	
	class RSGISInterpolator
		{
		public:
			RSGISInterpolator();
			virtual double interpolate(double xShift, double yShift, double *pixels) throw(RSGISImageException)=0;
			virtual ~RSGISInterpolator();
		};
	
	class RSGISCubicInterpolator : public RSGISInterpolator
		{
		public:
			RSGISCubicInterpolator();
			double interpolate(double xShift, double yShift, double *pixels) throw(RSGISImageException);
		protected:
			double estimateNewValueFromCurve(double *pixels, double shift);
		};

	class RSGISBilinearAreaInterpolator : public RSGISInterpolator
		{
		public:
			RSGISBilinearAreaInterpolator();
			double interpolate(double xShift, double yShift, double *pixels) throw(RSGISImageException);
		};
	
	class RSGISBilinearPointInterpolator : public RSGISInterpolator
		{
		public:
			RSGISBilinearPointInterpolator();
			double interpolate(double xShift, double yShift, double *pixels) throw(RSGISImageException);
		};
	
	class RSGISNearestNeighbourInterpolator : public RSGISInterpolator
		{
		public:
			RSGISNearestNeighbourInterpolator();
			double interpolate(double xShift, double yShift, double *pixels) throw(RSGISImageException);
		protected:
			int findIndexOfMax(double *arr, int size);
		};
	
	class RSGISTriangulationInterpolator : public RSGISInterpolator
		{
		public:
			RSGISTriangulationInterpolator();
			double interpolate(double xShift, double yShift, double *pixels) throw(RSGISImageException);
		protected:
			double triangle(double xShift, double yShift, double *pixels, bool triangulation);
		};
	
}}

#endif


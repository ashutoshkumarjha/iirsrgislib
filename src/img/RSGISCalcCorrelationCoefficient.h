/*
 *  RSGISCalcCorrelationCoefficient.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 22/07/2008.
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

#ifndef RSGISCalcCorrelationCoefficient_H
#define RSGISCalcCorrelationCoefficient_H

#include <iostream>
#include <string>
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageSingleValue.h"
#include "img/RSGISCalcImageSingle.h"

using namespace std;

namespace rsgis{namespace img{
	
	class RSGISCalcCC : public RSGISCalcImageSingleValue
		{
		public:
			RSGISCalcCC(int numOutputValues);
			void calcImageValue(float *bandValuesImageA, float *bandValuesImageB, int numBands, int bandA, int bandB) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValuesImageA, int numBands, int band) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValuesImageA, int numBands, Envelope *extent) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValuesImage, double interceptArea, int numBands, Polygon *poly, Point *pt) throw(RSGISImageCalcException);
			double* getOutputValues() throw(RSGISImageCalcException);
			void reset();
			void setBandA(int band);
			void setBandB(int band);
			int getBandA();
			int getBandB();
		private:
			int n;
			double ab;
			double a;
			double b;
			double aSQ;
			double bSQ;
		};
}}

#endif



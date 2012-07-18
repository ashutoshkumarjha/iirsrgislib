/*
 *  RSGISReplaceValuesLessThanGivenValue.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 09/06/2010.
 *  Copyright 2010 RSGISLib.
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

#ifndef RSGISReplaceValuesLessThanGivenValue_H
#define RSGISReplaceValuesLessThanGivenValue_H

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"

namespace rsgis{namespace img{
	
	/***
	 *
	 * This class replace all values below a threshold with
	 * a new value passed by the user. (e.g., replace all values
	 * less than zero with zero).
	 *
	 */
	
	class RSGISReplaceValuesLessThanGivenValue : public RSGISCalcImageValue
	{
	public: 
		RSGISReplaceValuesLessThanGivenValue(int numberOutBands, float threshold, float value);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException);
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
		~RSGISReplaceValuesLessThanGivenValue();
	private:
		float threshold;
		float value;
	};
	
}}

#endif





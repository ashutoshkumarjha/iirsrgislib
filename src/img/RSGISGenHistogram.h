/*
 *  RSGISGenHistogram.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 16/07/2012.
 *  Copyright 2012 RSGISLib. All rights reserved.
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

#ifndef RSGISGenHistogram_H
#define RSGISGenHistogram_H

#include <iostream>
#include <math.h>
#include <limits>

#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"

namespace rsgis { namespace img {
	
    class RSGISGenHistogram
    {
    public:
        RSGISGenHistogram();
        void genHistogram(GDALDataset **datasets, unsigned int numDS, std::string outputFile, unsigned int imgBand, double imgMin, double imgMax, float maskValue, float binWidth)throw(RSGISImageCalcException);
        ~RSGISGenHistogram();
    };
	
	class RSGISGenHistogramCalcVal : public RSGISCalcImageValue
	{
	public:
		RSGISGenHistogramCalcVal(unsigned int *bins, float *binRanges, unsigned int band, unsigned int numBins, float maskValue, float binWidth);
		void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
		void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
		void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};	
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
		bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};															
		~RSGISGenHistogramCalcVal();
    protected:
        unsigned int *bins;
        float *binRanges;
        unsigned int band;
        float maskValue;
        float binWidth;
        unsigned int numBins;
	};
	
}}

#endif

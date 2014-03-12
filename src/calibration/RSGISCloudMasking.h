/*
 *  RSGISCloudMasking.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 24/02/2014.
 *  Copyright 2014 RSGISLib. All rights reserved.
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

#ifndef RSGISCloudMasking_h
#define RSGISCloudMasking_h

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"

#include "common/RSGISException.h"
#include "img/RSGISImageCalcException.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISCalcImage.h"

namespace rsgis{namespace calib{
    
	class RSGISLandsatFMaskPass1CloudMasking : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISLandsatFMaskPass1CloudMasking(unsigned int scaleFactor);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISLandsatFMaskPass1CloudMasking();
    protected:
        unsigned int scaleFactor;
        unsigned int blueIdx;
        unsigned int greenIdx;
        unsigned int redIdx;
        unsigned int nirIdx;
        unsigned int swir1Idx;
        unsigned int swir2Idx;
        unsigned int therm1Idx;
        unsigned int therm2Idx;
    };
    
    class RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking(unsigned int scaleFactor, unsigned int numThermalBands, double water82ndThres, double land82ndThres, double land17thThres);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISLandsatFMaskPass2ClearSkyCloudProbCloudMasking();
    protected:
        unsigned int scaleFactor;
        unsigned int numThermalBands;
        unsigned int pass1Idx;
        unsigned int blueIdx;
        unsigned int greenIdx;
        unsigned int redIdx;
        unsigned int nirIdx;
        unsigned int swir1Idx;
        unsigned int swir2Idx;
        unsigned int therm1Idx;
        unsigned int therm2Idx;
        unsigned int blueSatIdx;
        unsigned int greenSatIdx;
        unsigned int redSatIdx;
        unsigned int nirSatIdx;
        unsigned int swir1SatIdx;
        unsigned int swir2SatIdx;
        unsigned int therm1SatIdx;
        unsigned int therm2SatIdx;
        double water82ndThres;
        double land82ndThres;
        double land17thThres;
    };
    
    class RSGISLandsatFMaskPass2CloudMasking : public rsgis::img::RSGISCalcImageValue
    {
    public:
        RSGISLandsatFMaskPass2CloudMasking(unsigned int scaleFactor, unsigned int numThermalBands, double water82ndThres, double land82ndThres, double land17thThres, double landCloudProb82ndThres);
        void calcImageValue(float *bandValues, int numBands, float *output) throw(rsgis::img::RSGISImageCalcException);
        void calcImageValue(float *bandValues, int numBands) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(long *intBandValues, unsigned int numIntVals, float *floatBandValues, unsigned int numfloatVals, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implemented");};
        void calcImageValue(float *bandValues, int numBands, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float *bandValues, int numBands, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, geos::geom::Envelope extent) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("No implemented");};
        bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(rsgis::img::RSGISImageCalcException){throw rsgis::img::RSGISImageCalcException("Not implmented.");};
        ~RSGISLandsatFMaskPass2CloudMasking();
    protected:
        unsigned int scaleFactor;
        unsigned int numThermalBands;
        unsigned int pass1Idx;
        unsigned int blueIdx;
        unsigned int greenIdx;
        unsigned int redIdx;
        unsigned int nirIdx;
        unsigned int swir1Idx;
        unsigned int swir2Idx;
        unsigned int therm1Idx;
        unsigned int therm2Idx;
        unsigned int blueSatIdx;
        unsigned int greenSatIdx;
        unsigned int redSatIdx;
        unsigned int nirSatIdx;
        unsigned int swir1SatIdx;
        unsigned int swir2SatIdx;
        unsigned int therm1SatIdx;
        unsigned int therm2SatIdx;
        double water82ndThres;
        double land82ndThres;
        double land17thThres;
        double landCloudProb82ndThres;
    };
    
    
}}

#endif

/*
 *  RSGISRATCalc.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 4/05/2014.
 *  Copyright 2014 RSGISLib.
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

#ifndef RSGISRATCalc_H
#define RSGISRATCalc_H

#include <iostream>
#include <string>
#include <math.h>

#include "gdal_priv.h"

#include "common/RSGISAttributeTableException.h"

#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISRATCalcValue.h"


namespace rsgis{namespace rastergis{
    
    class RSGISRATCalc
    {
    public:
        RSGISRATCalc(RSGISRATCalcValue *ratCalcVal);
        virtual void calcRATValues(GDALRasterAttributeTable *gdalRAT, std::vector<unsigned int> inRealColIdx, std::vector<unsigned int> inIntColIdx, std::vector<unsigned int> inStrColIdx, std::vector<unsigned int> outRealColIdx, std::vector<unsigned int> outIntColIdx, std::vector<unsigned int> outStrColIdx) throw(RSGISAttributeTableException);
        virtual ~RSGISRATCalc();
    protected:
        RSGISRATCalcValue *ratCalcVal;
    };
    
}}

#endif


/*
 *  RSGISSelectClumps.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 13/09/2013.
 *  Copyright 2013 RSGISLib.
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

#ifndef RSGISSelectClumps_H
#define RSGISSelectClumps_H

#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <algorithm>

#include "common/RSGISAttributeTableException.h"

#include "img/RSGISImageCalcException.h"
#include "img/RSGISImageUtils.h"

#include "rastergis/RSGISRasterAttUtils.h"
#include "rastergis/RSGISRATCalcValue.h"
#include "rastergis/RSGISRATCalc.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"

namespace rsgis{namespace rastergis{
    
    enum RSGISSelectMethods
    {
        noMethod = 0,
        minMethod = 1,
        maxMethod = 2,
        meanMethod = 3,
    };
    
    class RSGISSelectClumpsOnGrid
    {
    public:
        RSGISSelectClumpsOnGrid();
        void selectClumpsOnGrid(GDALDataset *clumpsDataset, std::string inSelectField, std::string outSelectField, std::string eastingsField, std::string northingsField, std::string metricField, unsigned int rows, unsigned int cols, RSGISSelectMethods method)throw(rsgis::RSGISAttributeTableException);
        ~RSGISSelectClumpsOnGrid();
    };
    
    
    class RSGISCalcTileStats : public RSGISRATCalcValue
    {
    public:
        RSGISCalcTileStats(unsigned int numRows, unsigned int numCols, double *selectVal, unsigned int *selectIdx, std::vector<unsigned int> **tileIdxs, OGREnvelope **tilesEnvs, bool *first, RSGISSelectMethods method);
        void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException);
        ~RSGISCalcTileStats();
    private:
        unsigned int numRows;
        unsigned int numCols;
        double *selectVal;
        unsigned int *selectIdx;
        std::vector<unsigned int> **tileIdxs;
        OGREnvelope **tilesEnvs;
        bool *first;
        RSGISSelectMethods method;
    };
    
    class RSGISSelectClumpClosest2TileMean : public RSGISRATCalcValue
    {
    public:
        RSGISSelectClumpClosest2TileMean(unsigned int numRows, unsigned int numCols, double *selectVal, double *selectDistVal, unsigned int *selectIdx, std::vector<unsigned int> **tileIdxs, OGREnvelope **tilesEnvs, bool *first);
        void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException);
        ~RSGISSelectClumpClosest2TileMean();
    private:
        unsigned int numRows;
        unsigned int numCols;
        double *selectVal;
        double *selectDistVal;
        unsigned int *selectIdx;
        std::vector<unsigned int> **tileIdxs;
        OGREnvelope **tilesEnvs;
        bool *first;
    };
    
    
    class RSGISWriteSelectedClumpsColumn : public RSGISRATCalcValue
    {
    public:
        RSGISWriteSelectedClumpsColumn(unsigned int *selectIdx, unsigned int numIdxes);
        void calcRATValue(size_t fid, double *inRealCols, unsigned int numInRealCols, int *inIntCols, unsigned int numInIntCols, std::string *inStringCols, unsigned int numInStringCols, double *outRealCols, unsigned int numOutRealCols, int *outIntCols, unsigned int numOutIntCols, std::string *outStringCols, unsigned int numOutStringCols) throw(RSGISAttributeTableException);
        ~RSGISWriteSelectedClumpsColumn();
    private:
        unsigned int *selectIdx;
        unsigned int numIdxes;
    };
    
    
}}

#endif



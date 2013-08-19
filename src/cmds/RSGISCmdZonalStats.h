/*
 *  RSGISCmdZonalStats.h
 *
 *
 *  Created by Dan Clewley on 08/08/2013.
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

#ifndef RSGISCmdZonalStats_H
#define RSGISCmdZonalStats_H

#include <iostream>
#include <string>
#include <vector>

#include "common/RSGISCommons.h"
#include "RSGISCmdException.h"

namespace rsgis{ namespace cmds {

    struct RSGISBandAttZonalStatsCmds
    {
        float minThreshold;
        float maxThreshold;
        bool calcCount;
        bool calcMin;
        bool calcMax;
        bool calcMean;
        bool calcStdDev;
        bool calcMode;
        bool calcSum;
    };

    /** Function to extract pixel value for points and save as a shapefile or CSV */
    void executePointValue(std::string inputImage, std::string inputVecPolys, std::string outputStatsFile, bool outputToText = false, bool force = false, bool useBandNames = true);
    /** Function to extract statistics for pixels falling within a polygon */
    void executePixelStats(std::string inputImage, std::string inputVecPolys, std::string outputStatsFile, RSGISBandAttZonalStatsCmds *calcStats, std::string inputRasPolys = "", bool outputToText = false, bool force = false, bool useBandNames = true, bool ignoreProjection = false, int pixelInPolyMethodInt = 1);
    /** Function to extract pixel values for each polygon and save to a seperate text file */
    void executePixelVals2txt(std::string inputImage, std::string inputVecPolys, std::string outputTextBase, std::string polyAttribute, std::string outtxtform = "csv", bool ignoreProjection = false, int pixelInPolyMethodInt = 1);
}}


#endif


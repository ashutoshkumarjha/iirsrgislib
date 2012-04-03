/*
 *  RSGISVectorZonalStats.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 22/09/2009.
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

#ifndef RSGISVectorZonalStats_H
#define RSGISVectorZonalStats_H

#include <iostream>
#include <string>
#include <list>

#include "ogrsf_frmts.h"

#include "gdal_priv.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISVectorOutputException.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "vec/RSGISVectorUtils.h"

#include "math/RSGISMathsUtils.h"

#include "geos/geom/Envelope.h"

using namespace std;
using namespace geos::geom;
using namespace rsgis::math;

namespace rsgis{namespace vec{
	
	class RSGISVectorZonalStats : public RSGISProcessOGRFeature
	{
	public:
		RSGISVectorZonalStats(GDALDataset *image);
		virtual void processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException);
		virtual void processFeature(OGRFeature *feature, Envelope *env, long fid) throw(RSGISVectorException);
		virtual void createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
		float* getPixelColumns(int xPxl, int yPxl);
		virtual ~RSGISVectorZonalStats();
	private:
		GDALDataset *image;
		GDALRasterBand **bands;
		int numImgBands;
		Envelope *imageExtent;
		double imgRes;
		float *pxlValues;
	};
}}

#endif




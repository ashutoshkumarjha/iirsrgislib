/*
 *  RSGISRasterizeVector.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 19/08/2009.
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

#ifndef RSGISRasterizeVector_H
#define RSGISRasterizeVector_H

#include <iostream>
#include <string>
#include <list>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"
#include "common/RSGISImageException.h"

#include "vec/RSGISVectorUtils.h"
#include "img/RSGISPixelInPoly.h"

#include "geos/geom/Envelope.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/PrecisionModel.h"

using namespace std;
using namespace rsgis;
using namespace rsgis::img;
using namespace geos::geom;

namespace rsgis{namespace vec{
		
	class RSGISRasterizeVector
		{
		public:
			RSGISRasterizeVector();
			GDALDataset* createDataset(GDALDriver *gdalDriver, OGRLayer *layer, string filename, float resolution, float constVal) throw(RSGISImageException);
			GDALDataset* createDataset(GDALDriver *gdalDriver, vector<Polygon*> *polys, string filename, float resolution, OGRSpatialReference *spatialRef, float constVal) throw(RSGISImageException);
			void rasterizeLayer(OGRLayer *layer, GDALDataset *image, string attribute, pixelInPolyOption method) throw(RSGISVectorException);
			void rasterizeLayer(Geometry *geom, GDALDataset *image, bool useFID, float constVal, pixelInPolyOption method) throw(RSGISVectorException);
			int editPixels(GDALDataset *image, float pixelValue, Envelope *env, OGRGeometry *geom) throw(RSGISImageException);
			//void editPixels(GDALDataset *image, float pixelValue, Envelope *env, Geometry *geom) throw(RSGISImageException);
			~RSGISRasterizeVector();
		private:
			pixelInPolyOption method;
		};
}}

#endif




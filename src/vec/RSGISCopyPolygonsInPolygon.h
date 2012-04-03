 /*
 *  RSGISCopyPolygonsInPolygon.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 26/02/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
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

#ifndef RSGISCopyPolygonsInPolygon_H
#define RSGISCopyPolygonsInPolygon_H

#include <iostream>
#include <string>
#include <list>

#include "ogrsf_frmts.h"

#include "common/RSGISVectorException.h"

#include "vec/RSGISVectorUtils.h"


using namespace std;
using namespace rsgis;
using namespace geos::geom;

namespace rsgis{namespace vec{
	
	class RSGISCopyPolygonsInPolygon
	{
	public:
		RSGISCopyPolygonsInPolygon();
		long unsigned copyPolygonsInPoly(OGRLayer *input, OGRLayer *output, OGRGeometry *coverPolygon)throw(RSGISVectorException);
		void copyFeatureDefn(OGRLayer *outputSHPLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException);
		void copyFeatureData(OGRFeature *inFeature, OGRFeature *outFeature, OGRFeatureDefn *inFeatureDefn, OGRFeatureDefn *outFeatureDefn);
		~RSGISCopyPolygonsInPolygon();
	};
	
}}

#endif



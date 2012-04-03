/*
 *  RSGISClusteredClassificationPolygon.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 10/08/2009.
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

#ifndef RSGISClusteredClassificationPolygon_H
#define RSGISClusteredClassificationPolygon_H

#include <iostream>
#include <string>

#include "vec/RSGISPolygonData.h"
#include "vec/RSGISVectorUtils.h"

#include "math/RSGISMathsUtils.h"

using namespace std;
using namespace geos::geom;
using namespace geos;
using namespace rsgis::math;

namespace rsgis{namespace vec{
	
	class RSGISClusteredClassificationPolygon : public RSGISPolygonData
		{
		public:
			RSGISClusteredClassificationPolygon();
			RSGISClusteredClassificationPolygon(RSGISClusteredClassificationPolygon *data);
			virtual void readAttribtues(OGRFeature *feature, OGRFeatureDefn *featDefn);
			virtual void createLayerDefinition(OGRLayer *outputSHPLayer)throw(RSGISVectorOutputException);
			virtual void populateFeature(OGRFeature *feature, OGRFeatureDefn *featDefn);
			void setArea(float area);
			float getArea();
			void setPolysArea(float polysArea);
			float getPolysArea();
			void setProPolysArea(float proPolysArea);
			float getProPolysArea();
			void setProTotalAr(float proTotalAr);
			float getProTotalAr();
			void setEdgeThreshold(float edgeThreshold);
			float getEdgeThreshold();
			void setClusterClass(string name);
			string getClusterClass();
			~RSGISClusteredClassificationPolygon();
		protected:
			float area;
			float polysArea;
			float proPolysArea;
			float proTotalArea;
			float edgeThreshold;
			string clusterClass;
		};
}}

#endif


/*
 *  RSGISGEOSFactoryGenerator.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 04/09/2009.
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

#include "RSGISGEOSFactoryGenerator.h"


namespace rsgis{namespace utils{
	
	RSGISGEOSFactoryGenerator* RSGISGEOSFactoryGenerator::instance = NULL;
	
	RSGISGEOSFactoryGenerator::RSGISGEOSFactoryGenerator()
	{
		this->pm = new PrecisionModel();
		this->geomFactory = new GeometryFactory(this->pm);
		instance = NULL;
	}
	
	RSGISGEOSFactoryGenerator* RSGISGEOSFactoryGenerator::getInstance()
	{
		if(instance == NULL)
		{
			instance = new RSGISGEOSFactoryGenerator();
		}
		return instance;
	}
	
	GeometryFactory* RSGISGEOSFactoryGenerator::getFactory()
	{
		return this->geomFactory;
	}
	
	PrecisionModel* RSGISGEOSFactoryGenerator::getPrecision()
	{
		return this->pm;
	}
	
	RSGISGEOSFactoryGenerator::~RSGISGEOSFactoryGenerator()
	{
		delete geomFactory;
		delete pm;
		instance = NULL;
	}	
}}



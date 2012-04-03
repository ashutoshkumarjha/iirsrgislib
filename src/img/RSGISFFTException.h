/*
 *  RSGISFFTException.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/12/2008.
 *  Copyright 2008 RSGISLib.
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

#ifndef RSGISFFTException_H
#define RSGISFFTException_H

#include "common/RSGISImageException.h"

using namespace std;
using namespace rsgis;

namespace rsgis 
{
	namespace img
	{
		class RSGISFFTException : public RSGISImageException
			{
			public:
				RSGISFFTException();
				RSGISFFTException(const char* message);
			};
	}
}

#endif


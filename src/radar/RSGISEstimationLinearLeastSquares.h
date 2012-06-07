 /*
 *  RSGISEstimationLinearLeastSquares.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 17/05/2010.
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

#ifndef RSGISEstimationLinearLeastSquares_H
#define RSGISEstimationLinearLeastSquares_H

#include <math.h>
#include <iostream>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include "math/RSGISMatrices.h"
#include "math/RSGISVectors.h"
#include "math/RSGISLinearFit.h"
#include "radar/RSGISEstimationOptimiser.h"
#include "math/RSGISFunctions.h"

namespace rsgis {namespace radar{
    
    using namespace std;
    using namespace rsgis::math;
    
	class RSGISEstimationLinearLeastSquares : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationLinearLeastSquares(gsl_matrix *coefficients);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		virtual estOptimizerType getOptimiserType(){return unknown;}; 
		virtual void printOptimiser(){cout << "Linear Least Squares" << endl;};
		~RSGISEstimationLinearLeastSquares();
	private:
		gsl_matrix *a;
		gsl_vector *offsets;
	};
	
	class RSGISEstimationLinearize : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationLinearize(gsl_matrix *coefficients);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		virtual estOptimizerType getOptimiserType(){return unknown;}; 
		virtual void printOptimiser(){cout << "Linearize and solve using linear Least Squares" << endl;};
		~RSGISEstimationLinearize();
	private:
		gsl_matrix *a;
		gsl_vector *offsets;
	};
	
	class RSGISEstimationLinearizeWithPriors : public RSGISEstimationOptimiser
	{
	public:
		RSGISEstimationLinearizeWithPriors(gsl_matrix *coefficients,
										   gsl_matrix *covMatrixP, 
										   gsl_matrix *covMatrixD);
		int minimise(gsl_vector *inData, gsl_vector *initialPar, gsl_vector *outParError);
		virtual void modifyAPriori(gsl_vector *newAPrioriPar){};
		virtual estOptimizerType getOptimiserType(){return unknown;}; 
		virtual void printOptimiser(){cout << "Linearize and solve using linear Least Squares (with Priors)" << endl;};
		~RSGISEstimationLinearizeWithPriors();
	private:
		gsl_matrix *a;
		gsl_vector *offsets;
		gsl_matrix *invCovMatrixP; 
		gsl_matrix *invCovMatrixD;
	};
}}

#endif


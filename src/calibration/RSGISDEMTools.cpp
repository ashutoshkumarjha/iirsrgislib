/*
 *  RSGISDEMTools.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/08/2011.
 *  Copyright 2011 RSGISLib. All rights reserved.
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

#include "RSGISDEMTools.h"


namespace rsgis{namespace calib{
    

    RSGISCalcSlope::RSGISCalcSlope(int numberOutBands, unsigned int band, float ewRes, float nsRes, int outType) : RSGISCalcImageValue(numberOutBands)
    {
        this->band = band;
        this->ewRes = ewRes;
        this->nsRes = nsRes;
        this->outType = outType;
    }
    void RSGISCalcSlope::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
    {
        if(winSize != 3)
        {
            throw RSGISImageCalcException("Window size must be equal to 3 for the calculate of slope.");
        }
        
        if(!(band < numBands))
        {
            throw RSGISImageCalcException("Specified image band is not within the image.");
        }

        const double radiansToDegrees = 180.0 / M_PI;

        double dx, dy, slopeRad;
        
        dx = ((dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]) - 
              (dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]))/ewRes;
        
        dy = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
              (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;

        slopeRad = atan(sqrt((dx * dx) + (dy * dy))/8);

        if(outType == 0)
        {
            output[0] = (slopeRad * radiansToDegrees);
        }
        else
        {
            output[0] = slopeRad;
        }
    }
    
    RSGISCalcSlope::~RSGISCalcSlope()
    {
        
    }

    RSGISCalcAspect::RSGISCalcAspect(int numberOutBands, unsigned int band, float ewRes, float nsRes) : RSGISCalcImageValue(numberOutBands)
    {
        this->band = band;
        this->ewRes = ewRes;
        this->nsRes = nsRes;
    }
		
    void RSGISCalcAspect::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
    {
        if(winSize != 3)
        {
            throw RSGISImageCalcException("Window size must be equal to 3 for the calculate of slope.");
        }
        
        if(!(band < numBands))
        {
            throw RSGISImageCalcException("Specified image band is not within the image.");
        }
        
        const double radiansToDegrees = 180.0 / M_PI;
        
        double dx, dy, aspect;
        
        dx = ((dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]) - 
              (dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]))/ewRes;
        
        dy = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
              (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
        
        aspect = atan2(-dx, dy)*radiansToDegrees;
                
        if (dx == 0 && dy == 0)
        {
            /* Flat area */
            aspect = numeric_limits<double>::signaling_NaN();
        }
        
        if (aspect < 0)
        {
            aspect += 360.0;
        }
        
        if (aspect == 360.0)
        {
            aspect = 0.0;
        }
        
        output[0] = aspect;
    }

    RSGISCalcAspect::~RSGISCalcAspect()
    {
        
    }
    
    
    
    

    RSGISCalcSlopeAspect::RSGISCalcSlopeAspect(int numberOutBands, unsigned int band, float ewRes, float nsRes) : RSGISCalcImageValue(numberOutBands)
    {
        this->band = band;
        this->ewRes = ewRes;
        this->nsRes = nsRes;
    }
    void RSGISCalcSlopeAspect::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
    {
        if(winSize != 3)
        {
            throw RSGISImageCalcException("Window size must be equal to 3 for the calculate of slope.");
        }
        
        if(!(band < numBands))
        {
            throw RSGISImageCalcException("Specified image band is not within the image.");
        }
        
        const double radiansToDegrees = 180.0 / M_PI;
        
        double dxSlope, dySlope, slopeRad;
        
        dxSlope = ((dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]) - 
              (dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]))/ewRes;
        
        dySlope = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
              (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
        
        slopeRad = atan(sqrt((dxSlope * dxSlope) + (dySlope * dySlope))/8);
        
        output[0] = (slopeRad * radiansToDegrees);
        
        double dxAspect, dyAspect, aspect;
        
        dxAspect = ((dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]) - 
              (dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]))/ewRes;
        
        dyAspect = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
              (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
        
        aspect = atan2(-dxAspect, dyAspect)*radiansToDegrees;
        
        if (dxAspect == 0 && dyAspect == 0)
        {
            /* Flat area */
            aspect = numeric_limits<double>::signaling_NaN();
        }
        
        if (aspect < 0)
        {
            aspect += 360.0;
        }
        
        if (aspect == 360.0)
        {
            aspect = 0.0;
        }
        
        output[1] = aspect;
    }
    
    RSGISCalcSlopeAspect::~RSGISCalcSlopeAspect()
    {
        
    }
    
    
    
    
    RSGISCalcHillShade::RSGISCalcHillShade(int numberOutBands, unsigned int band, float ewRes, float nsRes, float sunZenith, float sunAzimuth) : RSGISCalcImageValue(numberOutBands)
    {
        this->band = band;
        this->ewRes = ewRes;
        this->nsRes = nsRes;
        this->sunZenith = sunZenith;
        this->sunAzimuth = sunAzimuth;
    }
    
    void RSGISCalcHillShade::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
    {
        if(winSize != 3)
        {
            throw RSGISImageCalcException("Window size must be equal to 3 for the calculate of slope.");
        }
        
        if(!(band < numBands))
        {
            throw RSGISImageCalcException("Specified image band is not within the image.");
        }
        
        const double degreesToRadians = M_PI / 180.0;
        
        double dx, dy, aspect;
        
        dx = ((dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2])-
                (dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]))/(ewRes*8);
        
        dy = ((dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2])-
              (dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]))/(nsRes*8);       
        
        double xx_plus_yy = dx * dx + dy * dy;
        
        // aspect...
        aspect = atan2(dy,dx);
        
        // shade value
                
        double cang = (sin(sunZenith * degreesToRadians) -
                cos(sunZenith * degreesToRadians) * sqrt(xx_plus_yy) *
                sin(aspect - (sunAzimuth-M_PI/2 * degreesToRadians))) /
                sqrt(1 + 1 * xx_plus_yy);
        
        if (cang <= 0.0)
        {
            cang = 1.0;
        }
        else
        {
            cang = 1.0 + (254.0 * cang);
        }
        
        output[0] = cang;
    }
    
    RSGISCalcHillShade::~RSGISCalcHillShade()
    {
        
    }
    
    
    
    
    
    RSGISCalcShadowBinaryMask::RSGISCalcShadowBinaryMask(int numberOutBands, GDALDataset *inputImage, unsigned int band, float ewRes, float nsRes, float sunZenith, float sunAzimuth, float maxElevHeight) : RSGISCalcImageValue(numberOutBands)
    {
        this->band = band;
        this->ewRes = ewRes;
        this->nsRes = nsRes;
        this->sunZenith = sunZenith;
        this->sunAzimuth = sunAzimuth;
        this->inputImage = inputImage;
        this->maxElevHeight = maxElevHeight;
        
        this->demWidth = inputImage->GetRasterXSize() * ewRes;
        this->demHeight = inputImage->GetRasterYSize() * nsRes;
        
        this->sunRange = sqrt((demWidth * demWidth) + (demHeight * demHeight))*2;
        
    }
		
    void RSGISCalcShadowBinaryMask::calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException)
    {
        float outputValue = 1;
        
        try 
        {
            const double degreesToRadians = M_PI / 180.0;
            
            // Location of active point.
            double x = extent.getMinX() + (extent.getMaxX() - extent.getMinX())/2;
            double y = extent.getMinY() + (extent.getMaxY() - extent.getMinY())/2;
            double z = bandValues[band-1];
            
            // Location of the sun.
            double sunX = x + (sunRange * sin(sunZenith * degreesToRadians) * cos(sunAzimuth * degreesToRadians));
            double sunY = y + (sunRange * sin(sunZenith * degreesToRadians) * sin(sunAzimuth * degreesToRadians));
            double sunZ = z + (sunRange * cos(sunZenith * degreesToRadians));
            
            // Create Ray Line
            Coordinate pxlPt;
            pxlPt.x = x;
            pxlPt.y = y;
            pxlPt.z = z;
            
            Coordinate sunPt;
            sunPt.x = sunX;
            sunPt.y = sunY;
            sunPt.z = sunZ;
            
            RSGISExtractImagePixelsOnLine extractPixels;
            vector<ImagePixelValuePt*> *imagePxlPts = extractPixels.getImagePixelValues(inputImage, band, &pxlPt, (sunAzimuth * degreesToRadians), (sunZenith * degreesToRadians), maxElevHeight);
            /*
            cout << "Point, " << x << "," << y << "," << z << endl;
            cout << "Sun, " << sunX << "," << sunY << "," << sunZ << endl;
            cout << "Resolution, " << ewRes << "," << nsRes << endl;
            
            cout << imagePxlPts->size() << " extracted pixels\n";
            
            for(vector<ImagePixelValuePt*>::iterator iterPxls = imagePxlPts->begin(); iterPxls != imagePxlPts->end(); ++iterPxls)
            {
                cout << (*iterPxls)->pt->x << "," << (*iterPxls)->pt->y << "," << (*iterPxls)->pt->z << "," << (*iterPxls)->imgX << "," << (*iterPxls)->imgY << "," << (*iterPxls)->value << "\n";
            }
            cout << endl << endl;
            */
            
            // Check whether pixel intersects with ray.
            for(vector<ImagePixelValuePt*>::iterator iterPxls = imagePxlPts->begin(); iterPxls != imagePxlPts->end(); ++iterPxls)
            {
                if((*iterPxls)->pt->z < (*iterPxls)->value)
                {
                    outputValue = 0;
                    break;
                }
            }
            
            // Clean up memory..
            for(vector<ImagePixelValuePt*>::iterator iterPxls = imagePxlPts->begin(); iterPxls != imagePxlPts->end(); )
            {
                delete (*iterPxls)->pt;
                delete (*iterPxls);
                iterPxls = imagePxlPts->erase(iterPxls);
            }
            delete imagePxlPts;
        } 
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }
        
        //if shadow then outputValue = 0;
        
        output[0] = outputValue;
    }
    
    RSGISCalcShadowBinaryMask::~RSGISCalcShadowBinaryMask()
    {
        
    }
    
    
    
    

    RSGISCalcRayIncidentAngle::RSGISCalcRayIncidentAngle(int numberOutBands, unsigned int band, float ewRes, float nsRes, float sunZenith, float sunAzimuth) : RSGISCalcImageValue(numberOutBands)
    {
        this->band = band;
        this->ewRes = ewRes;
        this->nsRes = nsRes;
        this->sunZenith = sunZenith;
        this->sunAzimuth = sunAzimuth;
    }
		
    void RSGISCalcRayIncidentAngle::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
    {
        float outputValue = 0;
        
        const double degreesToRadians = M_PI / 180.0;
        
        try 
        {
            if(winSize != 3)
            {
                throw RSGISImageCalcException("Window size must be equal to 3 for the calculate of slope.");
            }
            
            if(!(band < numBands))
            {
                throw RSGISImageCalcException("Specified image band is not within the image.");
            }
            
            const double radiansToDegrees = 180.0 / M_PI;
            
            double dxSlope, dySlope, slopeRad, slope;
            
            dxSlope = ((dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]) - 
                       (dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]))/ewRes;
            
            dySlope = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
                       (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
            
            slopeRad = atan(sqrt((dxSlope * dxSlope) + (dySlope * dySlope))/8);
            
            slope = (slopeRad * radiansToDegrees);
            
            double dxAspect, dyAspect, aspect;
            
            dxAspect = ((dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]) - 
                        (dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]))/ewRes;
            
            dyAspect = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
                        (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
            
            aspect = atan2(-dxAspect, dyAspect)*radiansToDegrees;
            
            if (dxAspect == 0 && dyAspect == 0)
            {
                /* Flat area */
                aspect = numeric_limits<double>::signaling_NaN();
            }
            
            if (aspect < 0)
            {
                aspect += 360.0;
            }
            
            if (aspect == 360.0)
            {
                aspect = 0.0;
            }
            
            // UNIT VECTOR FOR SURFACE
            double pA = sin(slope*degreesToRadians) * cos(aspect*degreesToRadians);
            double pB = sin(slope*degreesToRadians) * sin(aspect*degreesToRadians);
            double pC = cos(slope*degreesToRadians);
            
            //cout << "Plane: " << pA << ", " << pB << ", " << pC << endl;
            
            // UNIT VECTOR FOR INCIDENT RAY
            double rA = sin((sunZenith)*degreesToRadians) * cos(sunAzimuth*degreesToRadians);
            double rB = sin((sunZenith)*degreesToRadians) * sin(sunAzimuth*degreesToRadians);
            double rC = cos((sunZenith)*degreesToRadians);
            
            //cout << "Ray: " << rA << ", " << rB << ", " << rC << endl;
            
            //cout << "output value (radians) = " << acos((pA*rA)+(pB*rB)+(pC*rC)) << endl;
            
            outputValue = acos((pA*rA)+(pB*rB)+(pC*rC)) * radiansToDegrees;
            
            //cout << "output value = " << outputValue << endl << endl;
            
            if(isnan(outputValue))
            {
                outputValue = sunZenith;
            }
        } 
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }

        output[0] = outputValue;
    }
    
    RSGISCalcRayIncidentAngle::~RSGISCalcRayIncidentAngle()
    {
        
	}
    
    
    
    RSGISCalcRayExitanceAngle::RSGISCalcRayExitanceAngle(int numberOutBands, unsigned int band, float ewRes, float nsRes, float viewZenith, float viewAzimuth) : RSGISCalcImageValue(numberOutBands)
    {
        this->band = band;
        this->ewRes = ewRes;
        this->nsRes = nsRes;
        this->viewZenith = viewZenith;
        this->viewAzimuth = viewAzimuth;
    }
		
    void RSGISCalcRayExitanceAngle::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
    {
        float outputValue = 0;
        
        const double degreesToRadians = M_PI / 180.0;
        
        try 
        {
            if(winSize != 3)
            {
                throw RSGISImageCalcException("Window size must be equal to 3 for the calculate of slope.");
            }
            
            if(!(band < numBands))
            {
                throw RSGISImageCalcException("Specified image band is not within the image.");
            }
            
            const double radiansToDegrees = 180.0 / M_PI;
            
            double dxSlope, dySlope, slopeRad, slope;
            
            dxSlope = ((dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]) - 
                       (dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]))/ewRes;
            
            dySlope = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
                       (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
            
            slopeRad = atan(sqrt((dxSlope * dxSlope) + (dySlope * dySlope))/8);
            
            slope = (slopeRad * radiansToDegrees);
            
            double dxAspect, dyAspect, aspect;
            
            dxAspect = ((dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]) - 
                        (dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]))/ewRes;
            
            dyAspect = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
                        (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
            
            aspect = atan2(-dxAspect, dyAspect)*radiansToDegrees;
            
            if (dxAspect == 0 && dyAspect == 0)
            {
                /* Flat area */
                aspect = numeric_limits<double>::signaling_NaN();
            }
            
            if (aspect < 0)
            {
                aspect += 360.0;
            }
            
            if (aspect == 360.0)
            {
                aspect = 0.0;
            }
            
            // UNIT VECTOR FOR SURFACE
            double pA = sin(slope*degreesToRadians) * cos(aspect*degreesToRadians);
            double pB = sin(slope*degreesToRadians) * sin(aspect*degreesToRadians);
            double pC = cos(slope*degreesToRadians);
            
            //cout << "Plane: " << pA << ", " << pB << ", " << pC << endl;
            
            // UNIT VECTOR FOR EXITANCE RAY
            double rA = sin(viewZenith*degreesToRadians) * cos(viewAzimuth*degreesToRadians);
            double rB = sin(viewZenith*degreesToRadians) * sin(viewAzimuth*degreesToRadians);
            double rC = cos(viewZenith*degreesToRadians);
            
            //cout << "Ray: " << rA << ", " << rB << ", " << rC << endl;
            
            //cout << "output value (radians) = " << acos((pA*rA)+(pB*rB)+(pC*rC)) << endl;
            
            outputValue = acos((pA*rA)+(pB*rB)+(pC*rC)) * radiansToDegrees;
            
            //cout << "output value = " << incidenceAngle << endl << endl;
            
            if(isnan(outputValue))
            {
                outputValue = 0;
            }
        } 
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }
        
        output[0] = outputValue;
    }
		
    RSGISCalcRayExitanceAngle::~RSGISCalcRayExitanceAngle()
    {
        
    }

    
    
    
    RSGISCalcRayIncidentAndExitanceAngles::RSGISCalcRayIncidentAndExitanceAngles(int numberOutBands, unsigned int band, float ewRes, float nsRes, float sunZenith, float sunAzimuth, float viewZenith, float viewAzimuth) : RSGISCalcImageValue(numberOutBands)
    {
        this->band = band;
        this->ewRes = ewRes;
        this->nsRes = nsRes;
        this->sunZenith = sunZenith;
        this->sunAzimuth = sunAzimuth;
        this->viewZenith = viewZenith;
        this->viewAzimuth = viewAzimuth;
    }
		
    void RSGISCalcRayIncidentAndExitanceAngles::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
    {
        float incidenceAngle = 0;
        float existanceAngle = 0;
        
        const double degreesToRadians = M_PI / 180.0;
        
        try 
        {
            if(winSize != 3)
            {
                throw RSGISImageCalcException("Window size must be equal to 3 for the calculate of slope.");
            }
            
            if(!(band < numBands))
            {
                throw RSGISImageCalcException("Specified image band is not within the image.");
            }
            
            const double radiansToDegrees = 180.0 / M_PI;
            
            double dxSlope, dySlope, slopeRad, slope;
            
            dxSlope = ((dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]) - 
                       (dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]))/ewRes;
            
            dySlope = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
                       (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
            
            slopeRad = atan(sqrt((dxSlope * dxSlope) + (dySlope * dySlope))/8);
            
            slope = (slopeRad * radiansToDegrees);
            
            double dxAspect, dyAspect, aspect;
            
            dxAspect = ((dataBlock[band][0][2] + dataBlock[band][1][2] + dataBlock[band][1][2] + dataBlock[band][2][2]) - 
                        (dataBlock[band][0][0] + dataBlock[band][1][0] + dataBlock[band][1][0] + dataBlock[band][2][0]))/ewRes;
            
            dyAspect = ((dataBlock[band][2][0] + dataBlock[band][2][1] + dataBlock[band][2][1] + dataBlock[band][2][2]) - 
                        (dataBlock[band][0][0] + dataBlock[band][0][1] + dataBlock[band][0][1] + dataBlock[band][0][2]))/nsRes;
            
            aspect = atan2(-dxAspect, dyAspect)*radiansToDegrees;
            
            if (dxAspect == 0 && dyAspect == 0)
            {
                /* Flat area */
                aspect = numeric_limits<double>::signaling_NaN();
            }
            
            if (aspect < 0)
            {
                aspect += 360.0;
            }
            
            if (aspect == 360.0)
            {
                aspect = 0.0;
            }
            
            // UNIT VECTOR FOR SURFACE
            double pA = sin(slope*degreesToRadians) * cos(aspect*degreesToRadians);
            double pB = sin(slope*degreesToRadians) * sin(aspect*degreesToRadians);
            double pC = cos(slope*degreesToRadians);
            
            //cout << "Plane: " << pA << ", " << pB << ", " << pC << endl;
            
            // UNIT VECTOR FOR INCIDENT RAY
            double rA = sin((sunZenith)*degreesToRadians) * cos(sunAzimuth*degreesToRadians);
            double rB = sin((sunZenith)*degreesToRadians) * sin(sunAzimuth*degreesToRadians);
            double rC = cos((sunZenith)*degreesToRadians);
            
            incidenceAngle = acos((pA*rA)+(pB*rB)+(pC*rC)) * radiansToDegrees;
            
            if(isnan(incidenceAngle))
            {
                incidenceAngle = sunZenith;
            }
            
            // UNIT VECTOR FOR EXITANCE RAY
            rA = sin(viewZenith*degreesToRadians) * cos(viewAzimuth*degreesToRadians);
            rB = sin(viewZenith*degreesToRadians) * sin(viewAzimuth*degreesToRadians);
            rC = cos(viewZenith*degreesToRadians);
            
            existanceAngle = acos((pA*rA)+(pB*rB)+(pC*rC)) * radiansToDegrees;
            
            if(isnan(existanceAngle))
            {
                existanceAngle = 0;
            }            
        } 
        catch (RSGISImageCalcException &e) 
        {
            throw e;
        }
        
        output[0] = incidenceAngle;
        output[1] = existanceAngle;
    }
    
    RSGISCalcRayIncidentAndExitanceAngles::~RSGISCalcRayIncidentAndExitanceAngles()
    {
        
    }
	
}}



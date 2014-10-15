/*
 *  RSGISCmdVectorUtils.cpp
 *
 *
 *  Created by Pete Bunting on 03/05/2013.
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

#include <boost/filesystem.hpp>

#include "RSGISCmdVectorUtils.h"
#include "RSGISCmdParent.h"

#include "common/RSGISVectorException.h"
#include "common/RSGISException.h"

#include "vec/RSGISProcessVector.h"
#include "vec/RSGISProcessOGRFeature.h"
#include "vec/RSGISProcessGeometry.h"
#include "vec/RSGISGenerateConvexHullGroups.h"
#include "vec/RSGISVectorUtils.h"
#include "vec/RSGISProcessFeatureCopyVector.h"
#include "vec/RSGISVectorAttributeFindReplace.h"
#include "vec/RSGISVectorBuffer.h"
#include "vec/RSGISCalcPolygonArea.h"
#include "vec/RSGISCopyPolygonsInPolygon.h"
#include "vec/RSGISPopulateFeatsElev.h"
#include "vec/RSGISGetOGRGeometries.h"
#include "vec/RSGISVectorMaths.h"

#include "utils/RSGISTextUtils.h"
#include "utils/RSGISFileUtils.h"

#include "geos/geom/Coordinate.h"

namespace rsgis{ namespace cmds {
    
    void executeGenerateConvexHullsGroups(std::string inputFile, std::string outputVector, std::string outVecProj, bool force, unsigned int eastingsColIdx, unsigned int northingsColIdx, unsigned int attributeColIdx)throw(RSGISCmdException)
    {
        try
        {
            rsgis::utils::RSGISTextUtils textUtils;
            std::string wktProj = textUtils.readFileToString(outVecProj);
            
            rsgis::vec::RSGISGenerateConvexHullGroups genConvexGrps;
            
            std::vector<std::pair<std::string,std::vector<geos::geom::Coordinate>* >* > *coordGrps = NULL;
            
            coordGrps = genConvexGrps.getCoordinateGroups(inputFile, eastingsColIdx, northingsColIdx, attributeColIdx);
            
            /*
             for(std::vector<std::pair<std::string,std::vector<geos::geom::Coordinate>* >* >::iterator iterGrps = coordGrps->begin(); iterGrps != coordGrps->end(); ++iterGrps)
             {
             std::cout << (*iterGrps)->first << std::endl;
             std::cout << "\t";
             for(std::vector<geos::geom::Coordinate>::iterator iterCoords = (*iterGrps)->second->begin(); iterCoords != (*iterGrps)->second->end(); ++iterCoords)
             {
             std::cout << "[" << (*iterCoords).x << "," << (*iterCoords).y << "]";
             }
             std::cout << std::endl;
             }
             */
            
            std::cout << "Creating Polygons\n";
            genConvexGrps.createPolygonsAsShapefile(coordGrps, outputVector, wktProj, force);
            
            for(std::vector<std::pair<std::string,std::vector<geos::geom::Coordinate>* >* >::iterator iterGrps = coordGrps->begin(); iterGrps != coordGrps->end(); ++iterGrps)
            {
                delete (*iterGrps);
                delete (*iterGrps)->second;
            }
            delete coordGrps;
        }
            
        catch(rsgis::RSGISVectorException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeRemoveAttributes(std::string inputVector, std::string outputVector, bool force)throw(RSGISCmdException)
    {
        try
        {
            
            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).c_str();
            outputVector = boost::filesystem::absolute(outputVector).c_str();
            
            OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputVector);
            
            OGRDataSource *inputSHPDS = NULL;
            OGRLayer *inputSHPLayer = NULL;
            OGRSFDriver *shpFiledriver = NULL;
            OGRDataSource *outputSHPDS = NULL;
            OGRLayer *outputSHPLayer = NULL;
            OGRSpatialReference* inputSpatialRef = NULL;
            
            rsgis::vec::RSGISProcessVector *processVector = NULL;
            rsgis::vec::RSGISProcessOGRFeature *processFeature = NULL;
            
            std::string outputDIR = "";
            
            outputDIR = fileUtils.getFileDirectoryPath(outputVector);
            
            if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
            {
                if(force)
                {
                    vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
                }
                else
                {
                    throw RSGISException("Shapefile already exists, either delete or select force.");
                }
            }
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = OGRSFDriverRegistrar::Open(inputVector.c_str(), FALSE);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVector;
                throw rsgis::RSGISFileException(message.c_str());
            }
            inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw rsgis::RSGISFileException(message.c_str());
            }
            inputSpatialRef = inputSHPLayer->GetSpatialRef();
            
            // Get Geometry Type.
            OGRFeature *feature = inputSHPLayer->GetFeature(0);
            OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
            
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            const char *pszDriverName = "ESRI Shapefile";
            shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->CreateDataSource(outputVector.c_str(), NULL);
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVector;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            
            processFeature = new rsgis::vec::RSGISProcessFeatureCopyVector();
            processVector = new rsgis::vec::RSGISProcessVector(processFeature);
            
            processVector->processVectors(inputSHPLayer, outputSHPLayer, false, false, false);
            
            OGRDataSource::DestroyDataSource(inputSHPDS);
            OGRDataSource::DestroyDataSource(outputSHPDS);
            
            delete processVector;
            delete processFeature;
            
            //OGRCleanupAll();
            
        }
        catch(rsgis::RSGISVectorException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeBufferVector(std::string inputVector, std::string outputVector, float bufferDist, bool force) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).c_str();
            outputVector = boost::filesystem::absolute(outputVector).c_str();
            
            OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputVector);
            
            OGRDataSource *inputSHPDS = NULL;
            OGRLayer *inputSHPLayer = NULL;
            OGRSFDriver *shpFiledriver = NULL;
            OGRDataSource *outputSHPDS = NULL;
            OGRLayer *outputSHPLayer = NULL;
            OGRSpatialReference* inputSpatialRef = NULL;
            OGRFeatureDefn *inFeatureDefn = NULL;
            
            rsgis::vec::RSGISProcessGeometry *processVector = NULL;
            rsgis::vec::RSGISProcessOGRGeometry *processGeom = NULL;
            
            std::string outputDIR = "";
            
            outputDIR = fileUtils.getFileDirectoryPath(outputVector);
            
            if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
            {
                if(force)
                {
                    vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
                }
                else
                {
                    throw RSGISException("Shapefile already exists, either delete or select force.");
                }
            }
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = OGRSFDriverRegistrar::Open(inputVector.c_str(), FALSE);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVector;
                throw RSGISFileException(message.c_str());
            }
            inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            inputSpatialRef = inputSHPLayer->GetSpatialRef();
            inFeatureDefn = inputSHPLayer->GetLayerDefn();
            
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            const char *pszDriverName = "ESRI Shapefile";
            shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->CreateDataSource(outputVector.c_str(), NULL);
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVector;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, wkbPolygon, NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            
            processGeom = new rsgis::vec::RSGISVectorBuffer(bufferDist);
            processVector = new rsgis::vec::RSGISProcessGeometry(processGeom);
            
            processVector->processGeometryPolygonOutput(inputSHPLayer, outputSHPLayer, true, false);
            
            OGRDataSource::DestroyDataSource(inputSHPDS);
            OGRDataSource::DestroyDataSource(outputSHPDS);
            
            delete processVector;
            delete processGeom;
            
            //OGRCleanupAll();
        }
        catch(rsgis::RSGISVectorException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void  executePrintPolyGeom(std::string inputVector) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).c_str();
            
            OGRRegisterAll();
            
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            
            OGRDataSource *inputSHPDS = NULL;
            OGRLayer *inputSHPLayer = NULL;
            
            //rsgis::vec::RSGISVectorIO *vecIO = NULL;
            //rsgis::vec::RSGISPolygonData **polygons = NULL;
            //int numFeatures = 0;
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = OGRSFDriverRegistrar::Open(inputVector.c_str(), FALSE);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVector;
                throw RSGISFileException(message.c_str());
            }
            inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            
            rsgis::vec::RSGISProcessOGRFeature *processFeature = new rsgis::vec::RSGISPrintGeometryToConsole();
            rsgis::vec::RSGISProcessVector *processVector = new rsgis::vec::RSGISProcessVector(processFeature);
            
            processVector->processVectorsNoOutput(inputSHPLayer, true);
            
            delete processVector;
            delete processFeature;

            
            /*
            numFeatures = inputSHPLayer->GetFeatureCount(true);
            
            std::cout << "Shapefile has " << numFeatures << " features\n";
            vecIO = new rsgis::vec::RSGISVectorIO();
            polygons = new rsgis::vec::RSGISPolygonData*[numFeatures];
            for(int i = 0; i < numFeatures; i++)
            {
                polygons[i] = new rsgis::vec::RSGISEmptyPolygon();
            }
            vecIO->readPolygons(inputSHPLayer, polygons, numFeatures);
            
            std::cout.precision(8);
            
            for(int i = 0; i < numFeatures; i++)
            {
                std::cout << "Polygon " << i << ":\t" << polygons[i]->getGeometry()->toString() << std::endl;
            }
            
            if(vecIO != NULL)
            {
                delete vecIO;
            }
            if(polygons != NULL)
            {
                for(int i = 0; i < numFeatures; i++)
                {
                    delete polygons[i];
                }
                delete polygons;
            }
             */
            
            OGRDataSource::DestroyDataSource(inputSHPDS);
            
            //OGRCleanupAll();
            
        }
        catch(rsgis::RSGISVectorException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void  executeFindReplaceText(std::string inputVector, std::string attribute, std::string find, std::string replace) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).c_str();
            
            
            OGRRegisterAll();
            
            rsgis::vec::RSGISVectorUtils vecUtils;
            rsgis::vec::RSGISProcessVector *processVector = NULL;
            rsgis::vec::RSGISProcessOGRFeature *processFeature = NULL;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            
            OGRDataSource *inputSHPDS = NULL;
            OGRLayer *inputSHPLayer = NULL;
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = OGRSFDriverRegistrar::Open(inputVector.c_str(), TRUE, NULL);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVector;
                throw RSGISFileException(message.c_str());
            }
            inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            
            processFeature = new rsgis::vec::RSGISVectorAttributeFindReplace(attribute, find, replace);
            processVector = new rsgis::vec::RSGISProcessVector(processFeature);
            
            processVector->processVectors(inputSHPLayer, false);
            
            OGRDataSource::DestroyDataSource(inputSHPDS);
            delete processVector;
            delete processFeature;
            
            //OGRCleanupAll();
            
        }
        catch(rsgis::RSGISVectorException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
    
    void executeCalcPolyArea(std::string inputVector, std::string outputVector, bool force) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).c_str();
            outputVector = boost::filesystem::absolute(outputVector).c_str();
            
            
            OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputVector);
            
            OGRDataSource *inputSHPDS = NULL;
            OGRLayer *inputSHPLayer = NULL;
            OGRSFDriver *shpFiledriver = NULL;
            OGRDataSource *outputSHPDS = NULL;
            OGRLayer *outputSHPLayer = NULL;
            OGRSpatialReference* inputSpatialRef = NULL;
            OGRFeatureDefn *inFeatureDefn = NULL;
            
            rsgis::vec::RSGISProcessVector *processVector = NULL;
            rsgis::vec::RSGISProcessOGRFeature *processFeature = NULL;
            
            std::string outputDIR = "";
            
            outputDIR = fileUtils.getFileDirectoryPath(outputVector);
            
            if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
            {
                if(force)
                {
                    vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
                }
                else
                {
                    throw RSGISException("Shapefile already exists, either delete or select force.");
                }
            }
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = OGRSFDriverRegistrar::Open(inputVector.c_str(), FALSE);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVector;
                throw RSGISFileException(message.c_str());
            }
            inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            inputSpatialRef = inputSHPLayer->GetSpatialRef();
            inFeatureDefn = inputSHPLayer->GetLayerDefn();
            
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            const char *pszDriverName = "ESRI Shapefile";
            shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->CreateDataSource(outputVector.c_str(), NULL);
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVector;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, inFeatureDefn->GetGeomType(), NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            
            processFeature = new rsgis::vec::RSGISCalcPolygonArea();
            processVector = new rsgis::vec::RSGISProcessVector(processFeature);
            
            processVector->processVectors(inputSHPLayer, outputSHPLayer, true, false, false);
            
            OGRDataSource::DestroyDataSource(inputSHPDS);
            OGRDataSource::DestroyDataSource(outputSHPDS);
            
            delete processVector;
            delete processFeature;
            
            //OGRCleanupAll();
        }
        catch(rsgis::RSGISVectorException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
        
    }
    
    void executePolygonsInPolygon(std::string inputVector, std::string inputCoverVector, std::string output_DIR, std::string attributeName, bool force) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).c_str();
            inputCoverVector = boost::filesystem::absolute(inputCoverVector).c_str();
            output_DIR = boost::filesystem::absolute(output_DIR).c_str();
            
            OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string coverSHPFileInLayer = vecUtils.getLayerName(inputCoverVector);
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            
            OGRDataSource *inputCoverSHPDS = NULL;
            OGRLayer *inputCoverSHPLayer = NULL;
            OGRSpatialReference* inputCoverSpatialRef = NULL;
            
            OGRDataSource *inputSHPDS = NULL;
            OGRLayer *inputSHPLayer = NULL;
            OGRSpatialReference* inputSpatialRef = NULL;
            OGRGeometry *coverGeometry = NULL; // OGRGeometry representation of feature in cover layer
            
            // Output shapefile
            OGRSFDriver *shpFiledriver = NULL;
            const char *pszDriverName = "ESRI Shapefile";
            shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            std::string coverFeatureName = "";
            std::string outVector = ""; // Filename of out vector

            /************************
             * OPEN COVER SHAPEFILE *
             ************************/
            
            inputCoverSHPDS = OGRSFDriverRegistrar::Open(inputCoverVector.c_str(), FALSE);
            if(inputCoverSHPDS == NULL)
            {
                std::string message = std::string("Could not open cover vector file ") + inputCoverVector;
                throw rsgis::RSGISFileException(message.c_str());
            }
            inputCoverSHPLayer = inputCoverSHPDS->GetLayerByName(coverSHPFileInLayer.c_str());
            if(inputCoverSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + coverSHPFileInLayer;
                throw rsgis::RSGISFileException(message.c_str());
            }
            inputCoverSpatialRef = inputCoverSHPLayer->GetSpatialRef();
            
            /*********************************
             * OPEN SHAPEFILE                *
             * This shapefile contains the   *
             * polygons to check if          *
             * covered by 'inputCoverVector' *
             *********************************/
            
            inputSHPDS = OGRSFDriverRegistrar::Open(inputVector.c_str(), FALSE);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVector;
                throw rsgis::RSGISFileException(message.c_str());
            }
            inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw rsgis::RSGISFileException(message.c_str());
            }
            inputSpatialRef = inputSHPLayer->GetSpatialRef();
            
            if(inputCoverSpatialRef != inputSpatialRef)
            {
                std::cerr << "Shapefiles are of different projection!" << std::endl;
            }
            
            // Get Geometry Type.
            OGRFeature *feature = inputSHPLayer->GetFeature(0);
            OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
            
            /********************************************
             * Loop through features in input shapefile *
             ********************************************/
            OGRFeature *inCoverFeature = NULL;
            while( (inCoverFeature = inputCoverSHPLayer->GetNextFeature()) != NULL )
            {
                
                // Get name of feature
                int fieldIdx = inCoverFeature->GetFieldIndex(attributeName.c_str());
                coverFeatureName = inCoverFeature->GetFieldAsString(fieldIdx);
                
                // Represent as OGRGeometry
                coverGeometry = inCoverFeature->GetGeometryRef();
                
                outVector = output_DIR + "/" + coverFeatureName + ".shp";
                std::string SHPFileOutLayer = vecUtils.getLayerName(outVector);
                
                OGRDataSource *outputSHPDS = NULL;
                OGRLayer *outputSHPLayer = NULL;
                
                std::string outputDIR = "";
                outputDIR = fileUtils.getFileDirectoryPath(outVector);
                
                if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
                {
                    if(force)
                    {
                        vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
                    }
                    else
                    {
                        throw RSGISException("Shapefile already exists, either delete or select force.");
                    }
                }
                
                /**************************************
                 * CREATE OUTPUT SHAPEFILE            *
                 * This is the shapefile representing *
                 * polygons contained within the	  *
                 * cover feature                      *
                 **************************************/
                
                outputSHPDS = shpFiledriver->CreateDataSource(outVector.c_str(), NULL);
                
                std::cout << "Creating new shapefile" << std::endl;
                if( outputSHPDS == NULL )
                {
                    std::string message = std::string("Could not create vector file ") + outVector;
                    throw rsgis::vec::RSGISVectorOutputException(message.c_str());
                }
                outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
                if( outputSHPLayer == NULL )
                {
                    std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                    throw rsgis::vec::RSGISVectorOutputException(message.c_str());
                }
                
                std::cout << "Created shapefile " << outVector << std::endl;
                
                /************************************
                 * ADD POLYGONS IN POLYGON TO NEW   *
                 * SHAPEFILE						*
                 ************************************/
                rsgis::vec::RSGISCopyPolygonsInPolygon copyPolysinPoly;
                long unsigned numPolygonsInCover = 0;
                try
                {
                    numPolygonsInCover = copyPolysinPoly.copyPolygonsInPoly(inputSHPLayer, outputSHPLayer, coverGeometry);
                    OGRDataSource::DestroyDataSource(outputSHPDS);
                }
                catch (RSGISVectorException &e)
                {
                    OGRDataSource::DestroyDataSource(outputSHPDS);
                    //OGRCleanupAll();
                    throw e;
                }
                // If no polygons in cover polygons, delete shapefile (will be empty)
                if(numPolygonsInCover == 0)
                {
                    std::cout << SHPFileOutLayer << " covers no polygons and will be deleted" << std::endl;
                    vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
                }
                else
                {
                    std::cout << coverFeatureName << " covers " << numPolygonsInCover << " polygons" << std::endl;
                }
                
                
            }
            
            //OGRCleanupAll();
        }
        catch(rsgis::RSGISVectorException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
            
    void executePopulateGeomZField(std::string inputVector, std::string inputImage, unsigned int imgBand, std::string outputVector, bool force) throw(RSGISCmdException)
    {
        // Convert to absolute path
        inputVector = boost::filesystem::absolute(inputVector).c_str();
        
        GDALAllRegister();
        OGRRegisterAll();
        
        rsgis::utils::RSGISFileUtils fileUtils;
        rsgis::vec::RSGISVectorUtils vecUtils;
        
        std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
        std::string SHPFileOutLayer = "";
        
        GDALDataset *inputImageDS = NULL;
        OGRDataSource *inputSHPDS = NULL;
        OGRLayer *inputSHPLayer = NULL;
        OGRSFDriver *shpFiledriver = NULL;
        OGRDataSource *outputSHPDS = NULL;
        OGRLayer *outputSHPLayer = NULL;
        OGRSpatialReference* inputSpatialRef = NULL;
        
        std::string outputDIR = "";
        
        try
        {
            SHPFileOutLayer = vecUtils.getLayerName(outputVector);
            outputDIR = fileUtils.getFileDirectoryPath(outputVector);
            
            if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
            {
                if(force)
                {
                    vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
                }
                else
                {
                    throw RSGISException("Shapefile already exists, either delete or select force.");
                }
            }
            
            /////////////////////////////////////
            //
            // Open Input Image.
            //
            /////////////////////////////////////
            inputImageDS = (GDALDataset *) GDALOpen(inputImage.c_str(), GA_ReadOnly);
            if(inputImageDS == NULL)
            {
                std::string message = std::string("Could not open image ") + inputImage;
                throw RSGISException(message.c_str());
            }
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = OGRSFDriverRegistrar::Open(inputVector.c_str(), FALSE);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVector;
                throw RSGISException(message.c_str());
            }
            inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISException(message.c_str());
            }
            
            inputSpatialRef = inputSHPLayer->GetSpatialRef();
            
            OGRFeature *feature = inputSHPLayer->GetFeature(1);
            OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
            OGRFeature::DestroyFeature(feature);
            
            if(geometryType == wkbPoint)
            {
                geometryType = wkbPoint25D;
            }
            else if(geometryType == wkbLineString)
            {
                geometryType = wkbLineString25D;
            }
            else if(geometryType == wkbPolygon)
            {
                geometryType = wkbPolygon25D;
            }
            else if(geometryType == wkbMultiPoint)
            {
                geometryType = wkbMultiPoint25D;
            }
            else if(geometryType == wkbMultiLineString)
            {
                geometryType = wkbMultiLineString25D;
            }
            else if(geometryType == wkbMultiPolygon)
            {
                geometryType = wkbMultiPolygon25D;
            }
            else if(geometryType == wkbGeometryCollection)
            {
                geometryType = wkbGeometryCollection25D;
            }
            else
            {
                throw RSGISException("Geometry Type not recognised.");
            }
            
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////

            const char *pszDriverName = "ESRI Shapefile";
            shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
            if( shpFiledriver == NULL )
            {
                throw RSGISException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->CreateDataSource(outputVector.c_str(), NULL);
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVector;
                throw RSGISException(message.c_str());
            }
            outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw RSGISException(message.c_str());
            }
            
            rsgis::vec::RSGISProcessOGRFeature *processFeature = new rsgis::vec::RSGISPopulateFeatsElev(inputImageDS, imgBand);
            rsgis::vec::RSGISProcessVector *processVector = new rsgis::vec::RSGISProcessVector(processFeature);
            
            processVector->processVectors(inputSHPLayer, outputSHPLayer, true, false, false);
            
            delete processVector;
            delete processFeature;
            
            GDALClose(inputImageDS);
            OGRDataSource::DestroyDataSource(inputSHPDS);
            OGRDataSource::DestroyDataSource(outputSHPDS);
            
        }
        catch(RSGISException& e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISCmdException(e.what());
        }
    }
            
            
    void executeVectorMaths(std::string inputVector, std::string outputVector, std::string outColumn, std::string expression, bool force, std::vector<RSGISVariableFieldCmds> vars) throw(RSGISCmdException)
    {
        try
        {
            // Convert to absolute path
            inputVector = boost::filesystem::absolute(inputVector).c_str();
            outputVector = boost::filesystem::absolute(outputVector).c_str();
            
            
            OGRRegisterAll();
            
            rsgis::utils::RSGISFileUtils fileUtils;
            rsgis::vec::RSGISVectorUtils vecUtils;
            
            std::string SHPFileInLayer = vecUtils.getLayerName(inputVector);
            std::string SHPFileOutLayer = vecUtils.getLayerName(outputVector);
            
            OGRDataSource *inputSHPDS = NULL;
            OGRLayer *inputSHPLayer = NULL;
            OGRSFDriver *shpFiledriver = NULL;
            OGRDataSource *outputSHPDS = NULL;
            OGRLayer *outputSHPLayer = NULL;
            OGRSpatialReference* inputSpatialRef = NULL;
            OGRFeatureDefn *inFeatureDefn = NULL;
            
            rsgis::vec::RSGISProcessVector *processVector = NULL;
            rsgis::vec::RSGISProcessOGRFeature *processFeature = NULL;
            
            std::string outputDIR = "";
            
            outputDIR = fileUtils.getFileDirectoryPath(outputVector);
            
            if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
            {
                if(force)
                {
                    vecUtils.deleteSHP(outputDIR, SHPFileOutLayer);
                }
                else
                {
                    throw RSGISException("Shapefile already exists, either delete or select force.");
                }
            }
            
            /////////////////////////////////////
            //
            // Open Input Shapfile.
            //
            /////////////////////////////////////
            inputSHPDS = OGRSFDriverRegistrar::Open(inputVector.c_str(), FALSE);
            if(inputSHPDS == NULL)
            {
                std::string message = std::string("Could not open vector file ") + inputVector;
                throw RSGISFileException(message.c_str());
            }
            inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
            if(inputSHPLayer == NULL)
            {
                std::string message = std::string("Could not open vector layer ") + SHPFileInLayer;
                throw RSGISFileException(message.c_str());
            }
            inputSpatialRef = inputSHPLayer->GetSpatialRef();
            inFeatureDefn = inputSHPLayer->GetLayerDefn();
            
            /////////////////////////////////////
            //
            // Create Output Shapfile.
            //
            /////////////////////////////////////
            const char *pszDriverName = "ESRI Shapefile";
            shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
            if( shpFiledriver == NULL )
            {
                throw rsgis::vec::RSGISVectorOutputException("SHP driver not available.");
            }
            outputSHPDS = shpFiledriver->CreateDataSource(outputVector.c_str(), NULL);
            if( outputSHPDS == NULL )
            {
                std::string message = std::string("Could not create vector file ") + outputVector;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, inFeatureDefn->GetGeomType(), NULL );
            if( outputSHPLayer == NULL )
            {
                std::string message = std::string("Could not create vector layer ") + SHPFileOutLayer;
                throw rsgis::vec::RSGISVectorOutputException(message.c_str());
            }
            
            unsigned int numVars = vars.size();
            rsgis::vec::VariableFields **variables = new rsgis::vec::VariableFields*[numVars];
            for(unsigned i = 0; i < numVars; ++i)
            {
                variables[i] = new rsgis::vec::VariableFields();
                variables[i]->name = vars.at(i).name;
                variables[i]->fieldName = vars.at(i).fieldName;
            }
            
            processFeature = new rsgis::vec::RSGISVectorMaths(variables, numVars, expression, outColumn);
            processVector = new rsgis::vec::RSGISProcessVector(processFeature);
            processVector->processVectors(inputSHPLayer, outputSHPLayer, true, true, false);
            
            for(unsigned i = 0; i < numVars; ++i)
            {
                delete variables[i];
            }
            delete[] variables;
            
            OGRDataSource::DestroyDataSource(inputSHPDS);
            OGRDataSource::DestroyDataSource(outputSHPDS);
            
            delete processVector;
            delete processFeature;            
        }
        catch(rsgis::RSGISVectorException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch(rsgis::RSGISException &e)
        {
            throw RSGISCmdException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISCmdException(e.what());
        }
    }
}}
            

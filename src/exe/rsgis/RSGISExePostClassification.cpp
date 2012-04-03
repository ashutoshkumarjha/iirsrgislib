/*
 *  RSGISExePostClassification.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/07/2009.
 *  Copyright 2009 RSGISLib.
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

#include "RSGISExePostClassification.h"



RSGISExePostClassification::RSGISExePostClassification() : RSGISAlgorithmParameters()
{
	this->algorithm = "postclassification";
	
	this-> option = none;
	this->inputVector = "";
	this->inputVectorLarge = "";
	this->inputVectorSmall = "";
	this->outputVector = "";
	this->outputLargeVector = "";
	this->outputSmallVector = "";
	this->classStr = "";
	this->intValueAtt = "";
	this->numStdDevthreshold = 0;
	this->force = false;
	this->resolution = 0;
	this->edgelengththreshold = 0;
	this->maxEdgeLength = 0;
	
	this->alpha = 0;
	this->beta = 0;
	this->gamma = 0;
	this->delta = 0;
	this->classEdgeLengths = 0;
	this->polygonizertype = polyonizernotdefined;
	this->clusterertype = clusterernotdefined;
	
	this->largeThreshold = 0;
	this->largeThreshold1 = 0;
	this->propPolyArea1 = 0;
	this->largeThreshold2 = 0;
	this->propPolyArea2 = 0;
	
	this->overlapThreshold = 0;
	this->intersectRatio = 0;
	
	this->morphologyType = morphologyNotDefined;
	this->buffer = 0;
	
	this->maxlength = 0;
	
	this->dominant = unknown;
}

RSGISAlgorithmParameters* RSGISExePostClassification::getInstance()
{
	return new RSGISExePostClassification();
}

void RSGISExePostClassification::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISMathsUtils mathUtils;	
	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = XMLString::transcode("algor");
	XMLCh *optionXMLStr = XMLString::transcode("option");
	XMLCh *clusterertagXMLStr = XMLString::transcode("rsgis:clusterer");
	XMLCh *edgelengthtagXMLStr = XMLString::transcode("rsgis:edgelength");
	XMLCh *polygonizertagXMLStr = XMLString::transcode("rsgis:polygonizer");
	XMLCh *optionGraphClusterClass = XMLString::transcode("clusterclass");
	XMLCh *optionGraphCluster= XMLString::transcode("cluster");
	XMLCh *optionGraphClusterClassPolygon = XMLString::transcode("clusterclasspolygonize");
	XMLCh *optionGraphClusterPolygon = XMLString::transcode("clusterpolygonize");
	XMLCh *optionSplitLargeSmallPolygons = XMLString::transcode("splitlargesmall");
	XMLCh *optionMergeLargePolygons = XMLString::transcode("mergelargepolygons");
	XMLCh *optionMorphology = XMLString::transcode("morphology");
	XMLCh *optionRemovesmallwithinlarge = XMLString::transcode("removesmallwithinlarge");
	XMLCh *optionMergesmallandlarge = XMLString::transcode("mergesmallandlarge");
	XMLCh *optionClusterremainingsmall = XMLString::transcode("clusterremainingsmall");
	XMLCh *optionMergesmall2nearestlarge = XMLString::transcode("mergesmall2nearestlarge");
	XMLCh *optionMergepolygons = XMLString::transcode("mergepolygons");
	XMLCh *optionLabelPolys = XMLString::transcode("labelpolys");
	
	//Clusterers
	XMLCh *clustererEdgeLength = XMLString::transcode("edgelength");
	XMLCh *clustererEdgeStdDev = XMLString::transcode("edgestddev");
	//Polygonizers
	XMLCh *polygonizerLineProj = XMLString::transcode("lineproj");
	XMLCh *polygonizerDelaunay1 = XMLString::transcode("delaunay1");
	XMLCh *polygonizerDelaunay2 = XMLString::transcode("delaunay2");
	XMLCh *polygonizerConvexHull = XMLString::transcode("convexhull");
	XMLCh *polygonizerMultiPoly = XMLString::transcode("multipoly");
	XMLCh *polygonizerSnakes = XMLString::transcode("snakes");
	XMLCh *polygonizerPolys = XMLString::transcode("polys");
	
	try
	{
		const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
		if(!XMLString::equals(algorName, algorNameEle))
		{
			throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
		}
		
		const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
		
		if(XMLString::equals(optionRemovesmallwithinlarge, optionXML) |
		   XMLString::equals(optionMergesmallandlarge, optionXML) |
		   XMLString::equals(optionClusterremainingsmall, optionXML) |
		   XMLString::equals(optionMergesmall2nearestlarge, optionXML))
		{
			XMLCh *inputlargeXMLStr = XMLString::transcode("inputlarge");
			if(argElement->hasAttribute(inputlargeXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(inputlargeXMLStr));
				this->inputVectorLarge = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'inputlarge\' attribute was provided.");
			}
			XMLString::release(&inputlargeXMLStr);
			
			XMLCh *inputsmallXMLStr = XMLString::transcode("inputsmall");
			if(argElement->hasAttribute(inputsmallXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(inputsmallXMLStr));
				this->inputVectorSmall = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'inputsmall\' attribute was provided.");
			}
			XMLString::release(&inputsmallXMLStr);
		}
		else if(XMLString::equals(optionMergepolygons, optionXML))
		{
			XMLCh *vectorXMLStr = XMLString::transcode("cluster_polys");
			if(argElement->hasAttribute(vectorXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(vectorXMLStr));
				this->inputVector_clusters = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'cluster_polys\' attribute was provided.");
			}
			XMLString::release(&vectorXMLStr);
			
			vectorXMLStr = XMLString::transcode("cluster_inpolys");
			if(argElement->hasAttribute(vectorXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(vectorXMLStr));
				this->inputVector_polys = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'cluster_inpolys\' attribute was provided.");
			}
			XMLString::release(&vectorXMLStr);
			
			vectorXMLStr = XMLString::transcode("cluster_orig");
			if(argElement->hasAttribute(vectorXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(vectorXMLStr));
				this->inputVector_orig = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'cluster_orig\' attribute was provided.");
			}
			XMLString::release(&vectorXMLStr);
		}
		else if(XMLString::equals(optionLabelPolys,optionXML))
		{
			XMLCh *vectorXMLStr = XMLString::transcode("input");
			if(argElement->hasAttribute(vectorXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(vectorXMLStr));
				this->inputVector = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'input\' attribute was provided.");
			}
			XMLString::release(&vectorXMLStr);
			
			XMLCh *labelsXMLStr = XMLString::transcode("labels");
			if(argElement->hasAttribute(labelsXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(labelsXMLStr));
				this->labelsVector = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'labels\' attribute was provided.");
			}
			XMLString::release(&labelsXMLStr);			
			
		}
		else
		{
			XMLCh *vectorXMLStr = XMLString::transcode("input");
			if(argElement->hasAttribute(vectorXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(vectorXMLStr));
				this->inputVector = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'input\' attribute was provided.");
			}
			XMLString::release(&vectorXMLStr);
		}
		
		XMLCh *forceXMLStr = XMLString::transcode("force");
		if(argElement->hasAttribute(forceXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);
			
			if(XMLString::equals(forceValue, yesStr))
			{
				this->force = true;
			}
			else
			{
				this->force = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
		}
		XMLString::release(&forceXMLStr);
		
		
		if(XMLString::equals(optionGraphClusterClassPolygon, optionXML) | XMLString::equals(optionGraphClusterPolygon, optionXML) |
		   XMLString::equals(optionGraphClusterClass, optionXML) | XMLString::equals(optionGraphCluster, optionXML))
		{
			XMLCh *outputXMLStr = XMLString::transcode("output");
			if(argElement->hasAttribute(outputXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
				this->outputVector = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
			}
			XMLString::release(&outputXMLStr);
			
			
			DOMNodeList *clustererNodesList = argElement->getElementsByTagName(clusterertagXMLStr);
			if(clustererNodesList->getLength() == 1)
			{
				DOMElement *clustererElement = static_cast<DOMElement*>(clustererNodesList->item(0));
				if(clustererElement->hasAttribute(algorXMLStr))
				{
					const XMLCh *clustererAlgor = clustererElement->getAttribute(algorXMLStr);
					
					if(XMLString::equals(clustererEdgeLength, clustererAlgor))
					{
						clusterertype = edgelength;
						
						XMLCh *lengthXMLStr = XMLString::transcode("length");
						if(clustererElement->hasAttribute(lengthXMLStr))
						{
							char *charValue = XMLString::transcode(clustererElement->getAttribute(lengthXMLStr));
							this->edgelengththreshold = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'length\' attribute was provided for clusterer.");
						}
						XMLString::release(&lengthXMLStr);
						
						DOMNodeList *edgelengthNodesList = argElement->getElementsByTagName(edgelengthtagXMLStr);
						if(edgelengthNodesList->getLength() > 0)
						{
							classEdgeLengths = new vector<EdgeLengthPair*>();
							DOMElement *edgelengthElement = NULL; 
							EdgeLengthPair *edgeLengthPair = NULL;
							for(unsigned int i = 0; i < edgelengthNodesList->getLength(); ++i)
							{
								edgelengthElement = static_cast<DOMElement*>(edgelengthNodesList->item(i));
								edgeLengthPair = new EdgeLengthPair();
								
								XMLCh *classnameXMLStr = XMLString::transcode("classname");
								if(edgelengthElement->hasAttribute(classnameXMLStr))
								{
									char *charValue = XMLString::transcode(edgelengthElement->getAttribute(classnameXMLStr));
									edgeLengthPair->classname = string(charValue);
									XMLString::release(&charValue);
								}
								else
								{
									throw RSGISXMLArgumentsException("No \'classname\' attribute was provided.");
								}
								XMLString::release(&classnameXMLStr);
								
								XMLCh *lengthXMLStr = XMLString::transcode("length");
								if(edgelengthElement->hasAttribute(lengthXMLStr))
								{
									char *charValue = XMLString::transcode(edgelengthElement->getAttribute(lengthXMLStr));
									edgeLengthPair->lengththreshold = mathUtils.strtofloat(string(charValue));
									XMLString::release(&charValue);
								}
								else
								{
									throw RSGISXMLArgumentsException("No \'length\' attribute was provided for clusterer.");
								}
								XMLString::release(&lengthXMLStr);
								
								classEdgeLengths->push_back(edgeLengthPair);
							}
						}
					}
					else if(XMLString::equals(clustererEdgeStdDev, clustererAlgor))
					{
						clusterertype = edgestddev;
						
						XMLCh *stddevXMLStr = XMLString::transcode("stddev");
						if(clustererElement->hasAttribute(stddevXMLStr))
						{
							char *charValue = XMLString::transcode(clustererElement->getAttribute(stddevXMLStr));
							this->numStdDevthreshold = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'stddev\' attribute was provided for clusterer.");
						}
						XMLString::release(&stddevXMLStr);
						
						XMLCh *maxedgelengthXMLStr = XMLString::transcode("maxedgelength");
						if(clustererElement->hasAttribute(maxedgelengthXMLStr))
						{
							char *charValue = XMLString::transcode(clustererElement->getAttribute(maxedgelengthXMLStr));
							this->maxEdgeLength = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'maxedgelength\' attribute was provided for clusterer.");
						}
						XMLString::release(&maxedgelengthXMLStr);
					}
					else
					{
						throw RSGISXMLArgumentsException("Clusterer algorithm was not recognised");
					}
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'algor\' attribute was provided for clusterer.");
				}
				
			}
			else if(clustererNodesList->getLength() == 0)
			{
				throw RSGISXMLArgumentsException("A rsgis:clusterer tag needs to be provided.");
			}
			else
			{
				throw RSGISXMLArgumentsException("Only one rsgis:clusterer tag should be provided.");
			}
			
			if(XMLString::equals(optionGraphClusterClass, optionXML))
			{		
				this->option = RSGISExePostClassification::clusterclass;
				
				XMLCh *classXMLStr = XMLString::transcode("class");
				if(argElement->hasAttribute(classXMLStr))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(classXMLStr));
					this->classStr = string(charValue);
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'class\' attribute was provided.");
				}
				XMLString::release(&classXMLStr);			
			}
			else if(XMLString::equals(optionGraphCluster, optionXML))
			{		
				this->option = RSGISExePostClassification::cluster;
			}
			else if(XMLString::equals(optionGraphClusterClassPolygon, optionXML) | XMLString::equals(optionGraphClusterPolygon, optionXML))
			{		
				DOMNodeList *polgonizerNodesList = argElement->getElementsByTagName(polygonizertagXMLStr);
				if(polgonizerNodesList->getLength() == 1)
				{
					DOMElement *polygonizerElement = static_cast<DOMElement*>(polgonizerNodesList->item(0));
					if(polygonizerElement->hasAttribute(algorXMLStr))
					{
						const XMLCh *polygonizerAlgor = polygonizerElement->getAttribute(algorXMLStr);
						
						if(XMLString::equals(polygonizerLineProj, polygonizerAlgor))
						{
							polygonizertype = lineproj;
							
							XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
							if(polygonizerElement->hasAttribute(resolutionXMLStr))
							{
								char *charValue = XMLString::transcode(polygonizerElement->getAttribute(resolutionXMLStr));
								this->resolution = mathUtils.strtofloat(string(charValue));
								XMLString::release(&charValue);
							}
							else
							{
								throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided for polygonizer.");
							}
							XMLString::release(&resolutionXMLStr);
							
						}
						else if(XMLString::equals(polygonizerDelaunay1, polygonizerAlgor))
						{
							polygonizertype = delaunay1;
						}
						else if(XMLString::equals(polygonizerDelaunay2, polygonizerAlgor))
						{
							polygonizertype = delaunay2;
						}
						else if(XMLString::equals(polygonizerConvexHull, polygonizerAlgor))
						{
							polygonizertype = convexhull;
						}
						else if(XMLString::equals(polygonizerMultiPoly, polygonizerAlgor))
						{
							polygonizertype = multipoly;
						}
						else if(XMLString::equals(polygonizerPolys, polygonizerAlgor))
						{
							polygonizertype = polys;
						}
						else if(XMLString::equals(polygonizerSnakes, polygonizerAlgor))
						{
							polygonizertype = snakes;
							
							XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
							if(polygonizerElement->hasAttribute(resolutionXMLStr))
							{
								char *charValue = XMLString::transcode(polygonizerElement->getAttribute(resolutionXMLStr));
								this->resolution = mathUtils.strtofloat(string(charValue));
								XMLString::release(&charValue);
							}
							else
							{
								throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided for polygonizer.");
							}
							XMLString::release(&resolutionXMLStr);
							
							XMLCh *alphaXMLStr = XMLString::transcode("alpha");
							if(polygonizerElement->hasAttribute(alphaXMLStr))
							{
								char *charValue = XMLString::transcode(polygonizerElement->getAttribute(alphaXMLStr));
								this->alpha = mathUtils.strtofloat(string(charValue));
								XMLString::release(&charValue);
							}
							else
							{
								throw RSGISXMLArgumentsException("No \'alpha\' attribute was provided for polygonizer.");
							}
							XMLString::release(&alphaXMLStr);
							
							XMLCh *betaXMLStr = XMLString::transcode("beta");
							if(polygonizerElement->hasAttribute(betaXMLStr))
							{
								char *charValue = XMLString::transcode(polygonizerElement->getAttribute(betaXMLStr));
								this->beta = mathUtils.strtofloat(string(charValue));
								XMLString::release(&charValue);
							}
							else
							{
								throw RSGISXMLArgumentsException("No \'beta\' attribute was provided for polygonizer.");
							}
							XMLString::release(&betaXMLStr);
							
							XMLCh *gammaXMLStr = XMLString::transcode("gamma");
							if(polygonizerElement->hasAttribute(gammaXMLStr))
							{
								char *charValue = XMLString::transcode(polygonizerElement->getAttribute(gammaXMLStr));
								this->gamma = mathUtils.strtofloat(string(charValue));
								XMLString::release(&charValue);
							}
							else
							{
								throw RSGISXMLArgumentsException("No \'gamma\' attribute was provided for polygonizer.");
							}
							XMLString::release(&gammaXMLStr);

							
							XMLCh *deltaXMLStr = XMLString::transcode("delta");
							if(polygonizerElement->hasAttribute(deltaXMLStr))
							{
								char *charValue = XMLString::transcode(polygonizerElement->getAttribute(deltaXMLStr));
								this->delta = mathUtils.strtofloat(string(charValue));
								XMLString::release(&charValue);
							}
							else
							{
								throw RSGISXMLArgumentsException("No \'delta\' attribute was provided for polygonizer.");
							}
							XMLString::release(&deltaXMLStr);
							
							XMLCh *maxNumIterationsXMLStr = XMLString::transcode("maxNumIterations");
							if(polygonizerElement->hasAttribute(maxNumIterationsXMLStr))
							{
								char *charValue = XMLString::transcode(polygonizerElement->getAttribute(maxNumIterationsXMLStr));
								this->maxNumIterations = mathUtils.strtoint(string(charValue));
								XMLString::release(&charValue);
							}
							else
							{
								throw RSGISXMLArgumentsException("No \'maxNumIterations\' attribute was provided for polygonizer.");
							}
							XMLString::release(&maxNumIterationsXMLStr);
							
						}
						else
						{
							throw RSGISXMLArgumentsException("Polygonizer algorithm was not recognised");
						}
						
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'algor\' attribute was provided for polygonizer.");
					}
					
				}
				else if(polgonizerNodesList->getLength() == 0)
				{
					throw RSGISXMLArgumentsException("A rsgis:polygonizer tag needs to be provided.");
				}
				else
				{
					throw RSGISXMLArgumentsException("Only one rsgis:polygonizer tag should be provided.");
				}
				
				
				if(XMLString::equals(optionGraphClusterClassPolygon, optionXML))
				{
					this->option = RSGISExePostClassification::clusterclasspolygonize;
					
					XMLCh *classXMLStr = XMLString::transcode("class");
					if(argElement->hasAttribute(classXMLStr))
					{
						char *charValue = XMLString::transcode(argElement->getAttribute(classXMLStr));
						this->classStr = string(charValue);
						XMLString::release(&charValue);
					}
					else
					{
						throw RSGISXMLArgumentsException("No \'class\' attribute was provided.");
					}
					XMLString::release(&classXMLStr);		
				}
				else if(XMLString::equals(optionGraphClusterPolygon, optionXML))
				{
					this->option = RSGISExePostClassification::clusterpolygonize;
				}
			}
		}
		else if(XMLString::equals(optionSplitLargeSmallPolygons, optionXML))
		{
			this->option = RSGISExePostClassification::splitlargesmall;
			
			XMLCh *outputLargeXMLStr = XMLString::transcode("outputlarge");
			if(argElement->hasAttribute(outputLargeXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputLargeXMLStr));
				this->outputLargeVector = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'outputlarge\' attribute was provided.");
			}
			XMLString::release(&outputLargeXMLStr);
			
			XMLCh *outputSmallXMLStr = XMLString::transcode("outputsmall");
			if(argElement->hasAttribute(outputSmallXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputSmallXMLStr));
				this->outputSmallVector = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'outputsmall\' attribute was provided.");
			}
			XMLString::release(&outputSmallXMLStr);	
			
			XMLCh *largethreshold1XMLStr = XMLString::transcode("largethreshold1");
			if(argElement->hasAttribute(largethreshold1XMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(largethreshold1XMLStr));
				this->largeThreshold1 = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'largethreshold1\' attribute was provided.");
			}
			XMLString::release(&largethreshold1XMLStr);
			
			XMLCh *propPolyArea1XMLStr = XMLString::transcode("proppolyarea1");
			if(argElement->hasAttribute(propPolyArea1XMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(propPolyArea1XMLStr));
				this->propPolyArea1 = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'proppolyarea1\' attribute was provided.");
			}
			XMLString::release(&propPolyArea1XMLStr);
			
			XMLCh *largethreshold2XMLStr = XMLString::transcode("largethreshold2");
			if(argElement->hasAttribute(largethreshold2XMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(largethreshold2XMLStr));
				this->largeThreshold2 = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'largethreshold2\' attribute was provided.");
			}
			XMLString::release(&largethreshold2XMLStr);
			
			XMLCh *propPolyArea2XMLStr = XMLString::transcode("proppolyarea2");
			if(argElement->hasAttribute(propPolyArea2XMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(propPolyArea2XMLStr));
				this->propPolyArea2 = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'proppolyarea2\' attribute was provided.");
			}
			XMLString::release(&propPolyArea2XMLStr);
		}
		else if(XMLString::equals(optionMergeLargePolygons, optionXML))
		{
			this->option = RSGISExePostClassification::mergelargepolygons;
			
			XMLCh *outputXMLStr = XMLString::transcode("output");
			if(argElement->hasAttribute(outputXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
				this->outputVector = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
			}
			XMLString::release(&outputXMLStr);
			
			XMLCh *forceXMLStr = XMLString::transcode("force");
			if(argElement->hasAttribute(forceXMLStr))
			{
				XMLCh *yesStr = XMLString::transcode("yes");
				const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);
				
				if(XMLString::equals(forceValue, yesStr))
				{
					this->force = true;
				}
				else
				{
					this->force = false;
				}
				XMLString::release(&yesStr);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
			}
			XMLString::release(&forceXMLStr);
			
			XMLCh *largethresholdXMLStr = XMLString::transcode("largethreshold");
			if(argElement->hasAttribute(largethresholdXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(largethresholdXMLStr));
				this->largeThreshold = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'largethreshold\' attribute was provided.");
			}
			XMLString::release(&largethresholdXMLStr);
			
			XMLCh *intersectRatioXMLStr = XMLString::transcode("intersectratio");
			if(argElement->hasAttribute(intersectRatioXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(intersectRatioXMLStr));
				this->intersectRatio = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'intersectratio\' attribute was provided.");
			}
			XMLString::release(&intersectRatioXMLStr);
		}
		else if(XMLString::equals(optionMorphology, optionXML))
		{
			this->option = RSGISExePostClassification::morphology;
			
			XMLCh *outputXMLStr = XMLString::transcode("output");
			if(argElement->hasAttribute(outputXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
				this->outputVector = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
			}
			XMLString::release(&outputXMLStr);
			
			XMLCh *forceXMLStr = XMLString::transcode("force");
			if(argElement->hasAttribute(forceXMLStr))
			{
				XMLCh *yesStr = XMLString::transcode("yes");
				const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);
				
				if(XMLString::equals(forceValue, yesStr))
				{
					this->force = true;
				}
				else
				{
					this->force = false;
				}
				XMLString::release(&yesStr);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
			}
			XMLString::release(&forceXMLStr);
			
			XMLCh *typeXMLStr = XMLString::transcode("type");
			if(argElement->hasAttribute(typeXMLStr))
			{
				XMLCh *closingStr = XMLString::transcode("closing");
				XMLCh *openingStr = XMLString::transcode("opening");
				XMLCh *dilationStr = XMLString::transcode("dilation");
				XMLCh *erosionStr = XMLString::transcode("erosion");
				const XMLCh *typeValue = argElement->getAttribute(typeXMLStr);
				
				if(XMLString::equals(typeValue, closingStr))
				{
					this->morphologyType = closing;
				}
				else if(XMLString::equals(typeValue, openingStr))
				{
					this->morphologyType = opening;
				}
				else if(XMLString::equals(typeValue, dilationStr))
				{
					this->morphologyType = dilation;
				}
				else if(XMLString::equals(typeValue, erosionStr))
				{
					this->morphologyType = erosion;
				}
				else
				{
					throw RSGISXMLArgumentsException("Unknown morphology type. (closing | opening | dilation | erosion)");
				}
				XMLString::release(&closingStr);
				XMLString::release(&openingStr);
				XMLString::release(&dilationStr);
				XMLString::release(&erosionStr);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'type\' attribute was provided.");
			}
			XMLString::release(&typeXMLStr);
			
			XMLCh *bufferXMLStr = XMLString::transcode("buffer");
			if(argElement->hasAttribute(bufferXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(bufferXMLStr));
				this->buffer = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'buffer\' attribute was provided.");
			}
			XMLString::release(&bufferXMLStr);
		}
		else if(XMLString::equals(optionRemovesmallwithinlarge, optionXML))
		{
			this->option = RSGISExePostClassification::removesmallwithinlarge;
			
			XMLCh *outputLargeXMLStr = XMLString::transcode("outputlarge");
			if(argElement->hasAttribute(outputLargeXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputLargeXMLStr));
				this->outputLargeVector = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'outputlarge\' attribute was provided.");
			}
			XMLString::release(&outputLargeXMLStr);
			
			XMLCh *outputSmallXMLStr = XMLString::transcode("outputsmall");
			if(argElement->hasAttribute(outputSmallXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputSmallXMLStr));
				this->outputSmallVector = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'outputsmall\' attribute was provided.");
			}
			XMLString::release(&outputSmallXMLStr);		
			
			XMLCh *forceXMLStr = XMLString::transcode("force");
			if(argElement->hasAttribute(forceXMLStr))
			{
				XMLCh *yesStr = XMLString::transcode("yes");
				const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);
				
				if(XMLString::equals(forceValue, yesStr))
				{
					this->force = true;
				}
				else
				{
					this->force = false;
				}
				XMLString::release(&yesStr);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
			}
			XMLString::release(&forceXMLStr);
			
			XMLCh *overlapXMLStr = XMLString::transcode("overlapthreshold");
			if(argElement->hasAttribute(overlapXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(overlapXMLStr));
				this->overlapThreshold = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'overlapthreshold\' attribute was provided.");
			}
			XMLString::release(&overlapXMLStr);
		}
		else if(XMLString::equals(optionMergesmallandlarge, optionXML))
		{
			this->option = RSGISExePostClassification::mergesmallandlarge;
			
			XMLCh *outputLargeXMLStr = XMLString::transcode("outputlarge");
			if(argElement->hasAttribute(outputLargeXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputLargeXMLStr));
				this->outputLargeVector = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'outputlarge\' attribute was provided.");
			}
			XMLString::release(&outputLargeXMLStr);
			
			XMLCh *outputSmallXMLStr = XMLString::transcode("outputsmall");
			if(argElement->hasAttribute(outputSmallXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputSmallXMLStr));
				this->outputSmallVector = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'outputsmall\' attribute was provided.");
			}
			XMLString::release(&outputSmallXMLStr);		
			
			XMLCh *forceXMLStr = XMLString::transcode("force");
			if(argElement->hasAttribute(forceXMLStr))
			{
				XMLCh *yesStr = XMLString::transcode("yes");
				const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);
				
				if(XMLString::equals(forceValue, yesStr))
				{
					this->force = true;
				}
				else
				{
					this->force = false;
				}
				XMLString::release(&yesStr);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
			}
			XMLString::release(&forceXMLStr);
			
			XMLCh *overlapXMLStr = XMLString::transcode("overlapthreshold");
			if(argElement->hasAttribute(overlapXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(overlapXMLStr));
				this->overlapThreshold = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'overlapthreshold\' attribute was provided.");
			}
			XMLString::release(&overlapXMLStr);
			
			XMLCh *largethresholdXMLStr = XMLString::transcode("largethreshold");
			if(argElement->hasAttribute(largethresholdXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(largethresholdXMLStr));
				this->largeThreshold = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'largethreshold\' attribute was provided.");
			}
			XMLString::release(&largethresholdXMLStr);
			
			XMLCh *intersectRatioXMLStr = XMLString::transcode("intersectratio");
			if(argElement->hasAttribute(intersectRatioXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(intersectRatioXMLStr));
				this->intersectRatio = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'intersectratio\' attribute was provided.");
			}
			XMLString::release(&intersectRatioXMLStr);
		}
		else if(XMLString::equals(optionClusterremainingsmall, optionXML))
		{
			this->option = RSGISExePostClassification::clusterremainingsmall;
			
			XMLCh *outputXMLStr = XMLString::transcode("output");
			if(argElement->hasAttribute(outputXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
				this->outputVector = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
			}
			XMLString::release(&outputXMLStr);			
			
			XMLCh *forceXMLStr = XMLString::transcode("force");
			if(argElement->hasAttribute(forceXMLStr))
			{
				XMLCh *yesStr = XMLString::transcode("yes");
				const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);
				
				if(XMLString::equals(forceValue, yesStr))
				{
					this->force = true;
				}
				else
				{
					this->force = false;
				}
				XMLString::release(&yesStr);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
			}
			XMLString::release(&forceXMLStr);
			
			XMLCh *maxlengthXMLStr = XMLString::transcode("maxlength");
			if(argElement->hasAttribute(maxlengthXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(maxlengthXMLStr));
				this->maxlength = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'maxlength\' attribute was provided.");
			}
			XMLString::release(&maxlengthXMLStr);			
			
			DOMNodeList *polgonizerNodesList = argElement->getElementsByTagName(polygonizertagXMLStr);
			if(polgonizerNodesList->getLength() == 1)
			{
				DOMElement *polygonizerElement = static_cast<DOMElement*>(polgonizerNodesList->item(0));
				if(polygonizerElement->hasAttribute(algorXMLStr))
				{
					const XMLCh *polygonizerAlgor = polygonizerElement->getAttribute(algorXMLStr);
					
					if(XMLString::equals(polygonizerLineProj, polygonizerAlgor))
					{
						polygonizertype = lineproj;
						
						XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
						if(polygonizerElement->hasAttribute(resolutionXMLStr))
						{
							char *charValue = XMLString::transcode(polygonizerElement->getAttribute(resolutionXMLStr));
							this->resolution = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided for polygonizer.");
						}
						XMLString::release(&resolutionXMLStr);
						
					}
					else if(XMLString::equals(polygonizerDelaunay1, polygonizerAlgor))
					{
						polygonizertype = delaunay1;
					}
					else if(XMLString::equals(polygonizerDelaunay2, polygonizerAlgor))
					{
						polygonizertype = delaunay2;
					}
					else if(XMLString::equals(polygonizerConvexHull, polygonizerAlgor))
					{
						polygonizertype = convexhull;
					}
					else if(XMLString::equals(polygonizerMultiPoly, polygonizerAlgor))
					{
						polygonizertype = multipoly;
					}
					else if(XMLString::equals(polygonizerSnakes, polygonizerAlgor))
					{
						polygonizertype = snakes;
						
						XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
						if(polygonizerElement->hasAttribute(resolutionXMLStr))
						{
							char *charValue = XMLString::transcode(polygonizerElement->getAttribute(resolutionXMLStr));
							this->resolution = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided for polygonizer.");
						}
						XMLString::release(&resolutionXMLStr);
						
						XMLCh *alphaXMLStr = XMLString::transcode("alpha");
						if(polygonizerElement->hasAttribute(alphaXMLStr))
						{
							char *charValue = XMLString::transcode(polygonizerElement->getAttribute(alphaXMLStr));
							this->alpha = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'alpha\' attribute was provided for polygonizer.");
						}
						XMLString::release(&alphaXMLStr);
						
						XMLCh *betaXMLStr = XMLString::transcode("beta");
						if(polygonizerElement->hasAttribute(betaXMLStr))
						{
							char *charValue = XMLString::transcode(polygonizerElement->getAttribute(betaXMLStr));
							this->beta = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'beta\' attribute was provided for polygonizer.");
						}
						XMLString::release(&betaXMLStr);
						
						XMLCh *gammaXMLStr = XMLString::transcode("gamma");
						if(polygonizerElement->hasAttribute(gammaXMLStr))
						{
							char *charValue = XMLString::transcode(polygonizerElement->getAttribute(gammaXMLStr));
							this->gamma = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'gamma\' attribute was provided for polygonizer.");
						}
						XMLString::release(&gammaXMLStr);
						
						
						XMLCh *deltaXMLStr = XMLString::transcode("delta");
						if(polygonizerElement->hasAttribute(deltaXMLStr))
						{
							char *charValue = XMLString::transcode(polygonizerElement->getAttribute(deltaXMLStr));
							this->delta = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'delta\' attribute was provided for polygonizer.");
						}
						XMLString::release(&deltaXMLStr);
						
						XMLCh *maxNumIterationsXMLStr = XMLString::transcode("maxNumIterations");
						if(polygonizerElement->hasAttribute(maxNumIterationsXMLStr))
						{
							char *charValue = XMLString::transcode(polygonizerElement->getAttribute(maxNumIterationsXMLStr));
							this->maxNumIterations = mathUtils.strtoint(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'maxNumIterations\' attribute was provided for polygonizer.");
						}
						XMLString::release(&maxNumIterationsXMLStr);
						
					}
					else
					{
						throw RSGISXMLArgumentsException("Polygonizer algorithm was not recognised");
					}
					
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'algor\' attribute was provided for polygonizer.");
				}
				
			}
			else if(polgonizerNodesList->getLength() == 0)
			{
				throw RSGISXMLArgumentsException("A rsgis:polygonizer tag needs to be provided.");
			}
			else
			{
				throw RSGISXMLArgumentsException("Only one rsgis:polygonizer tag should be provided.");
			}
		}
		else if(XMLString::equals(optionMergesmall2nearestlarge, optionXML))
		{
			this->option = RSGISExePostClassification::mergesmall2nearestlarge;
			
			XMLCh *outputXMLStr = XMLString::transcode("output");
			if(argElement->hasAttribute(outputXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
				this->outputVector = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
			}
			XMLString::release(&outputXMLStr);			
			
			XMLCh *forceXMLStr = XMLString::transcode("force");
			if(argElement->hasAttribute(forceXMLStr))
			{
				XMLCh *yesStr = XMLString::transcode("yes");
				const XMLCh *forceValue = argElement->getAttribute(forceXMLStr);
				
				if(XMLString::equals(forceValue, yesStr))
				{
					this->force = true;
				}
				else
				{
					this->force = false;
				}
				XMLString::release(&yesStr);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'force\' attribute was provided.");
			}
			XMLString::release(&forceXMLStr);
			
			DOMNodeList *polgonizerNodesList = argElement->getElementsByTagName(polygonizertagXMLStr);
			if(polgonizerNodesList->getLength() == 1)
			{
				DOMElement *polygonizerElement = static_cast<DOMElement*>(polgonizerNodesList->item(0));
				if(polygonizerElement->hasAttribute(algorXMLStr))
				{
					const XMLCh *polygonizerAlgor = polygonizerElement->getAttribute(algorXMLStr);
					
					if(XMLString::equals(polygonizerLineProj, polygonizerAlgor))
					{
						polygonizertype = lineproj;
						
						XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
						if(polygonizerElement->hasAttribute(resolutionXMLStr))
						{
							char *charValue = XMLString::transcode(polygonizerElement->getAttribute(resolutionXMLStr));
							this->resolution = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided for polygonizer.");
						}
						XMLString::release(&resolutionXMLStr);
						
					}
					else if(XMLString::equals(polygonizerDelaunay1, polygonizerAlgor))
					{
						polygonizertype = delaunay1;
					}
					else if(XMLString::equals(polygonizerConvexHull, polygonizerAlgor))
					{
						polygonizertype = convexhull;
					}
					else if(XMLString::equals(polygonizerSnakes, polygonizerAlgor))
					{
						polygonizertype = snakes;
						
						XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
						if(polygonizerElement->hasAttribute(resolutionXMLStr))
						{
							char *charValue = XMLString::transcode(polygonizerElement->getAttribute(resolutionXMLStr));
							this->resolution = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided for polygonizer.");
						}
						XMLString::release(&resolutionXMLStr);
						
						XMLCh *alphaXMLStr = XMLString::transcode("alpha");
						if(polygonizerElement->hasAttribute(alphaXMLStr))
						{
							char *charValue = XMLString::transcode(polygonizerElement->getAttribute(alphaXMLStr));
							this->alpha = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'alpha\' attribute was provided for polygonizer.");
						}
						XMLString::release(&alphaXMLStr);
						
						XMLCh *betaXMLStr = XMLString::transcode("beta");
						if(polygonizerElement->hasAttribute(betaXMLStr))
						{
							char *charValue = XMLString::transcode(polygonizerElement->getAttribute(betaXMLStr));
							this->beta = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'beta\' attribute was provided for polygonizer.");
						}
						XMLString::release(&betaXMLStr);
						
						XMLCh *gammaXMLStr = XMLString::transcode("gamma");
						if(polygonizerElement->hasAttribute(gammaXMLStr))
						{
							char *charValue = XMLString::transcode(polygonizerElement->getAttribute(gammaXMLStr));
							this->gamma = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'gamma\' attribute was provided for polygonizer.");
						}
						XMLString::release(&gammaXMLStr);
						
						
						XMLCh *deltaXMLStr = XMLString::transcode("delta");
						if(polygonizerElement->hasAttribute(deltaXMLStr))
						{
							char *charValue = XMLString::transcode(polygonizerElement->getAttribute(deltaXMLStr));
							this->delta = mathUtils.strtofloat(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'delta\' attribute was provided for polygonizer.");
						}
						XMLString::release(&deltaXMLStr);
						
						XMLCh *maxNumIterationsXMLStr = XMLString::transcode("maxNumIterations");
						if(polygonizerElement->hasAttribute(maxNumIterationsXMLStr))
						{
							char *charValue = XMLString::transcode(polygonizerElement->getAttribute(maxNumIterationsXMLStr));
							this->maxNumIterations = mathUtils.strtoint(string(charValue));
							XMLString::release(&charValue);
						}
						else
						{
							throw RSGISXMLArgumentsException("No \'maxNumIterations\' attribute was provided for polygonizer.");
						}
						XMLString::release(&maxNumIterationsXMLStr);
						
					}
					else
					{
						throw RSGISXMLArgumentsException("Polygonizer algorithm was not recognised");
					}
					
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'algor\' attribute was provided for polygonizer.");
				}
				
			}
			else if(polgonizerNodesList->getLength() == 0)
			{
				throw RSGISXMLArgumentsException("A rsgis:polygonizer tag needs to be provided.");
			}
			else
			{
				throw RSGISXMLArgumentsException("Only one rsgis:polygonizer tag should be provided.");
			}
		}
		else if(XMLString::equals(optionMergepolygons, optionXML))
		{
			this->option = RSGISExePostClassification::mergepolygons;
			
			XMLCh *outputXMLStr = XMLString::transcode("output");
			if(argElement->hasAttribute(outputXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
				this->outputVector = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
			}
			XMLString::release(&outputXMLStr);
			
			XMLCh *classXMLStr = XMLString::transcode("class");
			if(argElement->hasAttribute(classXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(classXMLStr));
				this->classStr = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'class\' attribute was provided.");
			}
			XMLString::release(&classXMLStr);
			
			XMLCh *valueattrXMLStr = XMLString::transcode("valueattr");
			if(argElement->hasAttribute(valueattrXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(valueattrXMLStr));
				this->intValueAtt = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'valueattr\' attribute was provided.");
			}
			XMLString::release(&valueattrXMLStr);
			
		}
		else if(XMLString::equals(optionLabelPolys,optionXML))
		{
			this->option = RSGISExePostClassification::labelpolys;
			
			XMLCh *outputXMLStr = XMLString::transcode("output");
			if(argElement->hasAttribute(outputXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
				this->outputVector = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
			}
			XMLString::release(&outputXMLStr);
			
			XMLCh *classXMLStr = XMLString::transcode("classattribute");
			if(argElement->hasAttribute(classXMLStr))
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(classXMLStr));
				this->classAttribute = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'classattribute\' attribute was provided.");
			}
			XMLString::release(&classXMLStr);
			
			XMLCh *dominantXMLStr = XMLString::transcode("dominant");
			if(argElement->hasAttribute(dominantXMLStr))
			{
				XMLCh *countStr = XMLString::transcode("count");
				XMLCh *areaStr = XMLString::transcode("area");
				const XMLCh *dominantValue = argElement->getAttribute(dominantXMLStr);
				
				if(XMLString::equals(dominantValue, countStr))
				{
					this->dominant = speciesCount;
				}
				else if(XMLString::equals(dominantValue, areaStr))
				{
					this->dominant = speciesArea;
				}
				else
				{
					this->dominant = unknown;
				}
				XMLString::release(&countStr);
				XMLString::release(&areaStr);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'dominant\' attribute was provided.");
			}
			XMLString::release(&dominantXMLStr);
		}
		else
		{
			string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExePostClassification.");
			throw RSGISXMLArgumentsException(message.c_str());
		}
	}
	catch(RSGISXMLArgumentsException &e)
	{
		throw e;
	}
	
	XMLString::release(&algorName);
	XMLString::release(&algorXMLStr);
	XMLString::release(&optionXMLStr);
	XMLString::release(&clusterertagXMLStr);
	XMLString::release(&edgelengthtagXMLStr);
	XMLString::release(&polygonizertagXMLStr);
	XMLString::release(&optionGraphCluster);
	XMLString::release(&optionGraphClusterClass);
	XMLString::release(&optionGraphClusterClassPolygon);
	XMLString::release(&optionGraphClusterPolygon);
	XMLString::release(&clustererEdgeLength);
	XMLString::release(&clustererEdgeStdDev);
	XMLString::release(&polygonizerLineProj);
	XMLString::release(&polygonizerDelaunay1);
	XMLString::release(&polygonizerDelaunay2);
	XMLString::release(&polygonizerConvexHull);
	XMLString::release(&polygonizerMultiPoly);
	XMLString::release(&polygonizerPolys);
	XMLString::release(&optionSplitLargeSmallPolygons);
	XMLString::release(&optionMergeLargePolygons);
	XMLString::release(&optionMorphology);
	XMLString::release(&optionRemovesmallwithinlarge);
	XMLString::release(&optionMergesmallandlarge);
	XMLString::release(&optionClusterremainingsmall);
	XMLString::release(&optionMergesmall2nearestlarge);
	XMLString::release(&optionMergepolygons);
	XMLString::release(&optionLabelPolys);
	
	parsed = true;
}

void RSGISExePostClassification::runAlgorithm() throw(RSGISException)
{
	RSGISMathsUtils mathsUtils;
	
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		if((option == RSGISExePostClassification::cluster) | (option == RSGISExePostClassification::clusterpolygonize))
		{
			cout << "Cluster all polygons.\n";
			cout << "Input Shapefile = " << this->inputVector << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;
			
			if(clusterertype == edgestddev)
			{
				cout << "The edge standard deviation cluster will be used\n";
				cout << "Std Dev Threshold = " << this->numStdDevthreshold << endl;
				cout << "Maximum Edge Length Threshold = " << this->maxEdgeLength << endl;
			}
			else if(clusterertype == edgelength)
			{
				cout << "The maximum edge length cluster will be used\n";
				cout << "Edge length threshold = " << edgelengththreshold << endl;
			}
			if(option == RSGISExePostClassification::clusterpolygonize)
			{
				if(polygonizertype == lineproj)
				{
					cout << "The line projection polygonizer will be used\n";
					cout << "Resolution = " << this->resolution << endl;
				}
				else if(polygonizertype == delaunay2)
				{
					cout << "The delaunay1 polygonizer will be used\n";
				}
				else if(polygonizertype == delaunay1)
				{
					cout << "The delaunay2 polygonizer will be used\n";
				}
				else if(polygonizertype == convexhull)
				{
					cout << "The convex hull polygonizer will be used\n";
				}
				else if(polygonizertype == multipoly)
				{
					cout << "The multi-polygon polygonizer will be used\n";
				}
				else if(polygonizertype == polys)
				{
					cout << "The polygons polygonizer will be used\n";
				}
				else if(polygonizertype == snakes)
				{
					cout << "The snakes polygonizer will be used\n";
					cout << "Resolution = " << this->resolution << endl;
					cout << "Alpha = " << this->alpha << endl;
					cout << "Beta = " << this->beta << endl;
					cout << "Gamma = " << this->gamma << endl;
					cout << "Delta = " << this->delta << endl;
				}
			}
			
			OGRRegisterAll();
			
			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISGeometry geomUtils;
			
			string SHPFileInLayer = vecUtils.getLayerName(inputVector);
			
			OGRDataSource *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* spatialRef = NULL;
			
			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;
			
			vector<RSGIS2DPoint*> *data = new vector<RSGIS2DPoint*>();
			RSGISSpatialClustererInterface *clusterer = NULL;
			cout.precision(7);
			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = OGRSFDriverRegistrar::Open(inputVector.c_str(), FALSE);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				spatialRef = inputSHPLayer->GetSpatialRef();	
				
				processFeature = new RSGISPolygonReader(data);
				processVector = new RSGISProcessVector(processFeature);
				
				cout << "Read input Shapefile\n";
				processVector->processVectorsNoOutput(inputSHPLayer, false);
				
				delete processVector;
				delete processFeature;
				
				if(clusterertype == edgestddev)
				{
					clusterer = new RSGISMinSpanTreeClustererStdDevThreshold(this->numStdDevthreshold, this->maxEdgeLength);
				}
				else if(clusterertype == edgelength)
				{
					clusterer = new RSGISMinSpanTreeClustererEdgeLenThreshold(this->edgelengththreshold);
				}
				else
				{
					throw RSGISException("Clusterer is not defined");
				}
				
				int numClusters = 0;
				double edgeRemoveThrershold = 0;
				list<RSGIS2DPoint*> **clusteredData = clusterer->clusterData(data, &numClusters, &edgeRemoveThrershold);
				
				list<Polygon*> **polygons = new list<Polygon*>*[numClusters];
				list<RSGIS2DPoint*>::iterator iter2DPts;
				RSGISPolygon *tmpRSGISPoly;
				for(int i = 0; i < numClusters; ++i)
				{
					polygons[i] = new list<Polygon*>();
					for(iter2DPts = clusteredData[i]->begin(); iter2DPts != clusteredData[i]->end(); ++iter2DPts)
					{
						tmpRSGISPoly = (RSGISPolygon*) *iter2DPts;
						polygons[i]->push_back(tmpRSGISPoly->getPolygon());
					}
				}
				
				if(option == RSGISExePostClassification::cluster)
				{
					RSGISVectorIO vecIO;
					vecIO.exportGEOSPolygonClusters2SHP(this->outputVector, this->force, polygons, numClusters, spatialRef);
				}
				else if(option == RSGISExePostClassification::clusterpolygonize)
				{
					cout << "Calculating Output Polygons\n";
					if(polygonizertype == lineproj)
					{
						RSGISIdentifyNonConvexPolygonsLineProject *identifyNonConvexLineProj = new RSGISIdentifyNonConvexPolygonsLineProject(resolution);
						vector<Polygon*> *nonConvexPolys = identifyNonConvexLineProj->retrievePolygons(polygons, numClusters);
						RSGISVectorIO vecIO;
						vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, nonConvexPolys, spatialRef);
						vector<Polygon*>::iterator iterOutPolys;
						for(iterOutPolys = nonConvexPolys->begin(); iterOutPolys != nonConvexPolys->end(); )
						{
							delete *iterOutPolys;
							nonConvexPolys->erase(iterOutPolys);
						}
						delete nonConvexPolys;
						delete identifyNonConvexLineProj;
					}
					else if(polygonizertype == delaunay1)
					{
						RSGISIdentifyNonConvexPolygonsDelaunay *identifyNonConvexDelaunay = new RSGISIdentifyNonConvexPolygonsDelaunay();
						vector<Polygon*> *nonConvexPolys = new vector<Polygon*>();
						
						for(int i = 0; i < numClusters; ++i)
						{
							if(polygons[i]->size() > 0)
							{
								nonConvexPolys->push_back(identifyNonConvexDelaunay->retrievePolygon(polygons[i]));
							}
						}						
						RSGISVectorIO vecIO;
						vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, nonConvexPolys, spatialRef);
						vector<Polygon*>::iterator iterOutPolys;
						for(iterOutPolys = nonConvexPolys->begin(); iterOutPolys != nonConvexPolys->end(); )
						{
							delete *iterOutPolys;
							nonConvexPolys->erase(iterOutPolys);
						}
						delete nonConvexPolys;
						delete identifyNonConvexDelaunay;
					}
					else if(polygonizertype == delaunay2)
					{
						RSGISIdentifyNonConvexPolygonsDelaunay *identifyNonConvexDelaunay = new RSGISIdentifyNonConvexPolygonsDelaunay();
						vector<Polygon*> *nonConvexPolys = identifyNonConvexDelaunay->retrievePolygons(polygons, numClusters);
						
						RSGISVectorIO vecIO;
						vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, nonConvexPolys, spatialRef);
						vector<Polygon*>::iterator iterOutPolys;
						for(iterOutPolys = nonConvexPolys->begin(); iterOutPolys != nonConvexPolys->end(); )
						{
							delete *iterOutPolys;
							nonConvexPolys->erase(iterOutPolys);
						}
						delete nonConvexPolys;
						delete identifyNonConvexDelaunay;
					}
					else if(polygonizertype == convexhull)
					{
						vector<Polygon*> *convexhulls = new vector<Polygon*>();
						for(int i = 0; i < numClusters; ++i)
						{
							if(polygons[i]->size() > 0)
							{
								convexhulls->push_back(geomUtils.findConvexHull(polygons[i]));
							}
						}
						RSGISVectorIO vecIO;
						vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, convexhulls, spatialRef);
						vector<Polygon*>::iterator iterOutPolys;
						for(iterOutPolys = convexhulls->begin(); iterOutPolys != convexhulls->end(); )
						{
							delete *iterOutPolys;
							convexhulls->erase(iterOutPolys);
						}
						delete convexhulls;
					}
					else if(polygonizertype == multipoly)
					{
						RSGISVectorIO vecIO;
						vecIO.exportGEOSMultiPolygonClusters2SHP(this->outputVector, this->force, polygons, numClusters, spatialRef);
					}
					else if(polygonizertype == polys)
					{
						RSGISVectorIO vecIO;
						vecIO.exportGEOSPolygonClusters2SHP(this->outputVector, this->force, polygons, numClusters, spatialRef);
					}
					else if(polygonizertype == snakes)
					{
						RSGISIdentifyNonConvexPolygonsSnakes *identifyNonConvexSnakes = new RSGISIdentifyNonConvexPolygonsSnakes(resolution, spatialRef, alpha, beta, gamma, delta, maxNumIterations);
						vector<Polygon*> *nonConvexPolys = identifyNonConvexSnakes->retrievePolygons(polygons, numClusters);
						
						RSGISVectorIO vecIO;
						vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, nonConvexPolys, spatialRef);
						vector<Polygon*>::iterator iterOutPolys;
						for(iterOutPolys = nonConvexPolys->begin(); iterOutPolys != nonConvexPolys->end(); )
						{
							delete *iterOutPolys;
							nonConvexPolys->erase(iterOutPolys);
						}
						delete nonConvexPolys;
						delete identifyNonConvexSnakes;
					}
					else
					{
						throw RSGISException("Do not know polygonization option.");
					}
				}				
				
				list<Polygon*>::iterator iterPolys; 
				for(int i = 0; i < numClusters; ++i)
				{
					for(iter2DPts = clusteredData[i]->begin(); iter2DPts != clusteredData[i]->end(); ++iter2DPts)
					{
						delete *iter2DPts;
						clusteredData[i]->erase(iter2DPts);
					}
					delete clusteredData[i];
					
					for(iterPolys = polygons[i]->begin(); iterPolys != polygons[i]->end(); ++iterPolys)
					{
						polygons[i]->erase(iterPolys);
					}
					delete polygons[i];
				}
				delete[] clusteredData;
				delete[] polygons;
				
				delete clusterer;
				
			}
			catch (RSGISException &e) 
			{
				throw e;
			}
		}
		else if((option == RSGISExePostClassification::clusterclass) | (option == RSGISExePostClassification::clusterclasspolygonize))
		{

			cout << "Cluster input polygons by class\n";
			cout << "Input Shapefile = " << this->inputVector << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;
			
			if(clusterertype == edgestddev)
			{
				cout << "The edge standard deviation cluster will be used\n";
				cout << "Std Dev Threshold = " << this->numStdDevthreshold << endl;
				cout << "Maximum Edge Length Threshold = " << this->maxEdgeLength << endl;
			}
			else if(clusterertype == edgelength)
			{
				cout << "The maximum edge length cluster will be used\n";
				cout << "Edge length threshold = " << edgelengththreshold << endl;
				if(classEdgeLengths != NULL)
				{
					cout << "Per class thresholds: \n";
					vector<EdgeLengthPair*>::iterator iterEdgePairs;
					for(iterEdgePairs = classEdgeLengths->begin(); iterEdgePairs != classEdgeLengths->end(); ++iterEdgePairs)
					{
						cout << (*iterEdgePairs)->classname << " = " << (*iterEdgePairs)->lengththreshold << endl;
					}
				}
			}
			if(option == RSGISExePostClassification::clusterclasspolygonize)
			{
				
				if(polygonizertype == lineproj)
				{
					cout << "The line projection polygonizer will be used\n";
					cout << "Resolution = " << this->resolution << endl;
				}
				else if(polygonizertype == delaunay1)
				{
					cout << "The delaunay1 polygonizer will be used\n";
				}
				else if(polygonizertype == delaunay2)
				{
					cout << "The delaunay2 polygonizer will be used\n";
				}
				else if(polygonizertype == convexhull)
				{
					cout << "The convex hull polygonizer will be used\n";
				}
				else if(polygonizertype == multipoly)
				{
					cout << "The multi-polygon polygonizer will be used\n";
				}
				else if(polygonizertype == polys)
				{
					cout << "The polygons polygonizer will be used\n";
				}
				else if(polygonizertype == snakes)
				{
					cout << "The snakes polygonizer will be used\n";
					cout << "Resolution = " << this->resolution << endl;
					cout << "Alpha = " << this->alpha << endl;
					cout << "Beta = " << this->beta << endl;
					cout << "Gamma = " << this->gamma << endl;
					cout << "Delta = " << this->delta << endl;
				}
			}
			
			OGRRegisterAll();
			
			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISGeometry geomUtils;
			
			string SHPFileInLayer = vecUtils.getLayerName(inputVector);
			
			OGRDataSource *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSpatialReference* spatialRef = NULL;
			
			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;
			
			list<RSGIS2DPoint*> *data = new list<RSGIS2DPoint*>();
			RSGISSpatialClustererInterface *clusterer = NULL;
			cout.precision(7);
			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDS = OGRSFDriverRegistrar::Open(inputVector.c_str(), FALSE);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				spatialRef = inputSHPLayer->GetSpatialRef();
				
				processFeature = new RSGISClassificationPolygonReader(this->classStr, data);
				processVector = new RSGISProcessVector(processFeature);
				
				cout << "Read input Shapefile\n";
				processVector->processVectorsNoOutput(inputSHPLayer, false);
				
				delete processVector;
				delete processFeature;
				
				vector<Polygon*> *allPolys = new vector<Polygon*>();
				list<RSGIS2DPoint*>::iterator iterPts;
				for(iterPts = data->begin(); iterPts != data->end(); ++iterPts)
				{
					allPolys->push_back(((RSGISPolygon*)*iterPts)->getPolygon());
				}
				Polygon *convexhullAll = geomUtils.findConvexHull(allPolys);
				allPolys->clear();
				delete allPolys;
				double totalArea = convexhullAll->getArea();
				delete convexhullAll;
				
				// Sort data by class.
				bool classesContains = false;
				vector<string> classes;
				list<RSGIS2DPoint*>::iterator iterData;
				vector<string>::iterator iterClasses;
				RSGISClassificationPolygon *classPoly;
				for(iterData = data->begin(); iterData != data->end(); ++iterData)
				{
					classPoly = (RSGISClassificationPolygon*) (*iterData);
					classesContains = false;
					for(iterClasses = classes.begin(); iterClasses != classes.end(); ++iterClasses)
					{
						if(classPoly->getClassification() == (*iterClasses))
						{
							classesContains = true;
							break;
						}
					}
					if(!classesContains)
					{
						classes.push_back(classPoly->getClassification());
					}
				}
				
				cout << "There are " << classes.size() << " classes within the input.\n";
				
				vector<RSGIS2DPoint*> **dataPerClass = new vector<RSGIS2DPoint*>*[classes.size()];
				string className = "";
				for(unsigned int i = 0; i < classes.size(); ++i)
				{
					dataPerClass[i] = new vector<RSGIS2DPoint*>();
					className = classes.at(i);
					for(iterData = data->begin(); iterData != data->end(); )//++iterData)
					{	
						classPoly = (RSGISClassificationPolygon*) (*iterData);
						if(classPoly->getClassification() == className)
						{
							dataPerClass[i]->push_back(*iterData);
							data->erase(iterData++);
						}
						else 
						{
							++iterData;
						}

					}
				}
				
				for(unsigned int i = 0; i < classes.size(); ++i)
				{					
					cout << i << ": " << classes.at(i) << " has " << dataPerClass[i]->size() << " nodes." << endl;
				}
				
				if(clusterertype == edgestddev)
				{
					clusterer = new RSGISMinSpanTreeClustererStdDevThreshold(this->numStdDevthreshold, this->maxEdgeLength);
				}
				else if(clusterertype == edgelength)
				{
					clusterer = new RSGISMinSpanTreeClustererEdgeLenThreshold(this->edgelengththreshold);
				}
				else
				{
					throw RSGISException("Clusterer is not defined");
				}
				
				list<RSGIS2DPoint*> ***clusters = new list<RSGIS2DPoint*>**[classes.size()];
				int *numClusters = new int[classes.size()];
				double *edgeRemovalThreshold = new double[classes.size()];
				vector<EdgeLengthPair*>::iterator iterEdgePairs;
				bool found = false;
				for(unsigned int i = 0; i < classes.size(); ++i)
				{
					numClusters[i] = 0;
					if(clusterertype == edgelength && classEdgeLengths != NULL)
					{
						found = false;
						for(iterEdgePairs = classEdgeLengths->begin(); iterEdgePairs != classEdgeLengths->end(); ++iterEdgePairs)
						{
							if((*iterEdgePairs)->classname == classes[i])
							{
								found = true;
								((RSGISMinSpanTreeClustererEdgeLenThreshold*)clusterer)->updateLengthThreshold((*iterEdgePairs)->lengththreshold);
							}
						}
						if(!found)
						{	
							((RSGISMinSpanTreeClustererEdgeLenThreshold*)clusterer)->updateLengthThreshold(this->edgelengththreshold);
						}
					}
					clusters[i] = clusterer->clusterData(dataPerClass[i], &numClusters[i], &edgeRemovalThreshold[i]);
				}
				
				int totalNumClusters = 0;
				for(unsigned int i = 0; i < classes.size(); ++i)
				{
					totalNumClusters += numClusters[i];
				}
				
				cout << "Total Number of Clusters = " << totalNumClusters << endl;
				
				vector<string> *textColsNames = new vector<string>();
				textColsNames->push_back("ClustClass");
				vector<string> **textColsData = new vector<string>*[1];
				textColsData[0] = new vector<string>();
				vector<string> *numericColsNames = new vector<string>();
				numericColsNames->push_back("Area");
				numericColsNames->push_back("PolysArea");
				numericColsNames->push_back("ProPolysAr");
				numericColsNames->push_back("ProTotalAr");
				numericColsNames->push_back("EdgeThres");
				vector<float> **numericColsData = new vector<float>*[5];
				numericColsData[0] = new vector<float>();
				numericColsData[1] = new vector<float>();
				numericColsData[2] = new vector<float>();
				numericColsData[3] = new vector<float>();
				numericColsData[4] = new vector<float>();
				
				list<Polygon*> **polygons = new list<Polygon*>*[totalNumClusters];
				int count = 0;
				double crownArea = 0;
				RSGISPolygon *poly = NULL;
				for(unsigned int i = 0; i < classes.size(); ++i)
				{
					for(int j = 0; j <  numClusters[i]; ++j)
					{
						polygons[count] = new list<Polygon*>();
						crownArea = 0;
						for(iterPts = clusters[i][j]->begin(); iterPts != clusters[i][j]->end(); ++iterPts)
						{
							poly = (RSGISPolygon*) (*iterPts);
							crownArea += poly->getPolygon()->getArea();
							polygons[count]->push_back(poly->getPolygon());
						}
						++count;
						
						textColsData[0]->push_back(classes.at(i));
						numericColsData[1]->push_back(crownArea);
						numericColsData[4]->push_back(edgeRemovalThreshold[i]);
					}
				}
				
				if(option == RSGISExePostClassification::clusterclass)
				{
					RSGISVectorIO vecIO;
					vecIO.exportGEOSPolygonClusters2SHP(this->outputVector, this->force, polygons, totalNumClusters, spatialRef);
				}
				else if(option == RSGISExePostClassification::clusterclasspolygonize)
				{					
					cout << "Calculating Output Polygons\n";
					if(polygonizertype == lineproj)
					{
						RSGISIdentifyNonConvexPolygonsLineProject *identifyNonConvexLineProj = new RSGISIdentifyNonConvexPolygonsLineProject(resolution);
						vector<Polygon*> *nonConvexPolys = identifyNonConvexLineProj->retrievePolygons(polygons, totalNumClusters);
						
						float area = 0;
						for(unsigned int i = 0; i < nonConvexPolys->size(); ++i)
						{
							area = nonConvexPolys->at(i)->getArea();
							numericColsData[0]->push_back(area);
							numericColsData[2]->push_back(numericColsData[1]->at(i)/area);
							numericColsData[3]->push_back(area/totalArea);
						}
						
						RSGISVectorIO vecIO;
						vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, nonConvexPolys, spatialRef, numericColsNames, textColsNames, numericColsData, textColsData);
						vector<Polygon*>::iterator iterOutPolys;
						for(iterOutPolys = nonConvexPolys->begin(); iterOutPolys != nonConvexPolys->end(); )
						{
							delete *iterOutPolys;
							nonConvexPolys->erase(iterOutPolys);
						}
						delete nonConvexPolys;
						delete identifyNonConvexLineProj;					
					}
					else if(polygonizertype == delaunay1)
					{
						RSGISIdentifyNonConvexPolygonsDelaunay *identifyNonConvexDelaunay = new RSGISIdentifyNonConvexPolygonsDelaunay();
						vector<Polygon*> *nonConvexPolys = new vector<Polygon*>();
						
						Polygon *poly;
						float area = 0;
						for(int i = 0; i < totalNumClusters; ++i)
						{
							poly = identifyNonConvexDelaunay->retrievePolygon(polygons[i]);
							area = poly->getArea();
							numericColsData[0]->push_back(area);
							numericColsData[2]->push_back(numericColsData[1]->at(i)/area);
							numericColsData[3]->push_back(area/totalArea);
							nonConvexPolys->push_back(poly);
						}						
						RSGISVectorIO vecIO;
						vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, nonConvexPolys, spatialRef, numericColsNames, textColsNames, numericColsData, textColsData);
						vector<Polygon*>::iterator iterOutPolys;
						for(iterOutPolys = nonConvexPolys->begin(); iterOutPolys != nonConvexPolys->end(); )
						{
							delete *iterOutPolys;
							nonConvexPolys->erase(iterOutPolys);
						}
						delete nonConvexPolys;
						delete identifyNonConvexDelaunay;
					}
					else if(polygonizertype == delaunay2)
					{
						RSGISIdentifyNonConvexPolygonsDelaunay *identifyNonConvexDelaunay = new RSGISIdentifyNonConvexPolygonsDelaunay();
						vector<Polygon*> *nonConvexPolys = identifyNonConvexDelaunay->retrievePolygons(polygons, totalNumClusters);
						float area = 0;
						for(unsigned int i = 0; i < nonConvexPolys->size(); ++i)
						{
							area = nonConvexPolys->at(i)->getArea();
							numericColsData[0]->push_back(area);
							numericColsData[2]->push_back(numericColsData[1]->at(i)/area);
							numericColsData[3]->push_back(area/totalArea);
						}
						
						RSGISVectorIO vecIO;
						vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, nonConvexPolys, spatialRef, numericColsNames, textColsNames, numericColsData, textColsData);
						vector<Polygon*>::iterator iterOutPolys;
						for(iterOutPolys = nonConvexPolys->begin(); iterOutPolys != nonConvexPolys->end(); )
						{
							delete *iterOutPolys;
							nonConvexPolys->erase(iterOutPolys);
						}
						delete nonConvexPolys;
						delete identifyNonConvexDelaunay;
					}
					else if(polygonizertype == convexhull)
					{
						vector<Polygon*> *convexhulls = new vector<Polygon*>();
						
						Polygon *poly;
						float area = 0;
						for(int i = 0; i < totalNumClusters; ++i)
						{
							poly = geomUtils.findConvexHull(polygons[i]);
							area = poly->getArea();
							numericColsData[0]->push_back(area);
							numericColsData[2]->push_back(numericColsData[1]->at(i)/area);
							numericColsData[3]->push_back(area/totalArea);
							convexhulls->push_back(poly);
						}						
						RSGISVectorIO vecIO;
						vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, convexhulls, spatialRef, numericColsNames, textColsNames, numericColsData, textColsData);

						vector<Polygon*>::iterator iterOutPolys;
						for(iterOutPolys = convexhulls->begin(); iterOutPolys != convexhulls->end(); )
						{
							delete *iterOutPolys;
							convexhulls->erase(iterOutPolys);
						}
						delete convexhulls;
					}
					else if(polygonizertype == multipoly)
					{
						RSGISVectorIO vecIO;
						vecIO.exportGEOSMultiPolygonClusters2SHP(this->outputVector, this->force, polygons, totalNumClusters, spatialRef);
					}
					else if(polygonizertype == polys)
					{
						RSGISVectorIO vecIO;
						vecIO.exportGEOSPolygonClusters2SHP(this->outputVector, this->force, polygons, totalNumClusters, spatialRef);
					}
					else if(polygonizertype == snakes)
					{
						RSGISIdentifyNonConvexPolygonsSnakes *identifyNonConvexSnakes = new RSGISIdentifyNonConvexPolygonsSnakes(resolution, spatialRef, alpha, beta, gamma, delta, maxNumIterations);
						vector<Polygon*> *nonConvexPolys = identifyNonConvexSnakes->retrievePolygons(polygons, totalNumClusters);
						float area = 0;
						for(unsigned int i = 0; i < nonConvexPolys->size(); ++i)
						{
							area = nonConvexPolys->at(i)->getArea();
							numericColsData[0]->push_back(area);
							numericColsData[2]->push_back(numericColsData[1]->at(i)/area);
							numericColsData[3]->push_back(area/totalArea);
						}
						
						RSGISVectorIO vecIO;
						vecIO.exportGEOSPolygons2SHP(this->outputVector, this->force, nonConvexPolys, spatialRef, numericColsNames, textColsNames, numericColsData, textColsData);
						vector<Polygon*>::iterator iterOutPolys;
						for(iterOutPolys = nonConvexPolys->begin(); iterOutPolys != nonConvexPolys->end(); )
						{
							delete *iterOutPolys;
							nonConvexPolys->erase(iterOutPolys);
						}
						delete nonConvexPolys;
						delete identifyNonConvexSnakes;
					}
					else
					{
						throw RSGISException("Do not know polygonization option.");
					}
				}
				
				textColsNames->clear();
				delete textColsNames;
				textColsData[0]->clear();
				delete textColsData[0];
				delete[] textColsData;
				numericColsNames->clear();
				delete numericColsNames;
				numericColsData[0]->clear();
				delete numericColsData[0];
				numericColsData[1]->clear();
				delete numericColsData[1];
				numericColsData[2]->clear();
				delete numericColsData[2];
				numericColsData[3]->clear();
				delete numericColsData[3];
				numericColsData[4]->clear();
				delete numericColsData[4];
				delete[] numericColsData;
				
				for(int i = 0; i < totalNumClusters; i++)
				{
					polygons[i]->clear();
					delete polygons[i];
				}
				delete[] polygons;
				
				for(unsigned int i = 0; i < classes.size(); ++i)
				{
					for(int j = 0; j < numClusters[i]; ++j)
					{
						clusters[i][j]->clear();
						delete clusters[i][j];
					}
					delete[] clusters[i];
				}
				delete[] clusters;
				
				delete[] numClusters;
				delete[] edgeRemovalThreshold;
				
				for(unsigned int i = 0; i < classes.size(); ++i)
				{
					dataPerClass[i]->clear();
					delete dataPerClass[i];
				}
				delete[] dataPerClass;
				
				delete clusterer;

				for(iterData = data->begin(); iterData != data->end(); ++iterData)
				{
					delete ((RSGISClassificationPolygon*)(*iterData));
					data->erase(iterData);
				}
				delete data;
				
				delete RSGISGEOSFactoryGenerator::getInstance();
				
				OGRDataSource::DestroyDataSource(inputSHPDS);				
				//OGRCleanupAll();
			}
			catch(RSGISException &e)
			{
				throw e;
			}
		}
		else if(option == RSGISExePostClassification::splitlargesmall)
		{
			cout << "Split the input into small and large polygons.\n";
			cout << "Input Shapefile = " << this->inputVector << endl;
			cout << "Output Large Shapefile = " << this->outputLargeVector << endl;
			cout << "Output Small Shapefile = " << this->outputSmallVector << endl;
			cout << "Area Threshold 1 = " << this->largeThreshold1 << endl;
			cout << "Proportional Polygon Area Threshold 1 = " << this->propPolyArea1 << endl;
			cout << "Overlap Threshold = " << this->overlapThreshold << endl;
			cout << "Area Threshold 2 = " << this->largeThreshold2 << endl;
			cout << "Proportional Polygon Area Threshold 2 = " << this->propPolyArea2 << endl;
			OGRRegisterAll();
			
			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;
			
			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
			string SHPFileOutLayerLarge = vecUtils.getLayerName(this->outputLargeVector);
			string SHPFileOutLayerSmall = vecUtils.getLayerName(this->outputSmallVector);
			
			OGRDataSource *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDSLarge = NULL;
			OGRLayer *outputSHPLayerLarge = NULL;
			OGRDataSource *outputSHPDSSmall = NULL;
			OGRLayer *outputSHPLayerSmall = NULL;

			OGRSpatialReference* inputSpatialRef = NULL;
			
			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputLargeVector);
				
				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayerLarge))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayerLarge);
					}
					else
					{
						throw RSGISException("Shapefile already exists, either delete or select force.");
					}
				}
				
				outputDIR = fileUtils.getFileDirectoryPath(this->outputSmallVector);
				
				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayerSmall))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayerSmall);
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
				inputSHPDS = OGRSFDriverRegistrar::Open(this->inputVector.c_str(), FALSE);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();	
				
				// Get Geometry Type.
				OGRFeature *feature = inputSHPLayer->GetFeature(0);
				OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
				OGRFeature::DestroyFeature(feature);
				
				/////////////////////////////////////
				//
				// Create Output Shapfile. -- LARGE
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDSLarge = shpFiledriver->CreateDataSource(this->outputLargeVector.c_str(), NULL);
				if( outputSHPDSLarge == NULL )
				{
					string message = string("Could not create vector file ") + this->outputLargeVector;
					throw RSGISVectorOutputException(message);
				}
				outputSHPLayerLarge = outputSHPDSLarge->CreateLayer(SHPFileOutLayerLarge.c_str(), inputSpatialRef, geometryType, NULL );
				if( outputSHPLayerLarge == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayerLarge;
					throw RSGISVectorOutputException(message.c_str());
				}
				
				/////////////////////////////////////
				//
				// Create Output Shapfile. -- SMALL
				//
				/////////////////////////////////////
				outputSHPDSSmall = shpFiledriver->CreateDataSource(this->outputSmallVector.c_str(), NULL);
				if( outputSHPDSSmall == NULL )
				{
					string message = string("Could not create vector file ") + this->outputSmallVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayerSmall = outputSHPDSSmall->CreateLayer(SHPFileOutLayerSmall.c_str(), inputSpatialRef, geometryType, NULL );
				if( outputSHPLayerSmall == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayerSmall;
					throw RSGISVectorOutputException(message.c_str());
				}
				
				int numFeatures = inputSHPLayer->GetFeatureCount(true);
				RSGISPolygonData **data = new RSGISPolygonData*[numFeatures];
				for(int i = 0; i < numFeatures; ++i)
				{
					data[i] = new RSGISClusteredClassificationPolygon();
				}
				
				vecIO.readPolygons(inputSHPLayer, data, numFeatures);
				
				RSGISGeometry geomUtils;
				vector<Polygon*> *largePolys = new vector<Polygon*>();
				vector<RSGISPolygonData*> *largePolysData = new vector<RSGISPolygonData*>();
				vector<RSGISPolygonData*> *smallPolysData = new vector<RSGISPolygonData*>();
				RSGISClusteredClassificationPolygon *tmpPoly = NULL;
				for(int i = 0; i < numFeatures; ++i)
				{
					tmpPoly = (RSGISClusteredClassificationPolygon*) data[i];
					if((tmpPoly->getArea() >= this->largeThreshold1) & (tmpPoly->getProPolysArea() > propPolyArea1))
					{
						largePolysData->push_back(tmpPoly);
					}
					else if(tmpPoly->getArea() > 0)
					{
						smallPolysData->push_back(tmpPoly);
					}
				}
				
				vector<RSGISPolygonData*>::iterator iterPolys;
				for(iterPolys = largePolysData->begin(); iterPolys != largePolysData->end(); ++iterPolys)
				{
					largePolys->push_back((*iterPolys)->getPolygon());
				}				
				
				//float overlapthreshold = 0.4;
				//float largePolyThreshold = 8000;
				//float propPolyAreaLargeThreshold = 0.3;
				
				
				float overlap = 0;
				iterPolys = smallPolysData->begin();
				while(iterPolys != smallPolysData->end())
				{
					tmpPoly = (RSGISClusteredClassificationPolygon*) *iterPolys;
					if(tmpPoly->getArea() == 0)
					{
						smallPolysData->erase(iterPolys);
					}
					else if((tmpPoly->getArea() >= largeThreshold2) & (tmpPoly->getProPolysArea() > propPolyArea2))
					{
						// get overlap with large polygons
						try
						{
							overlap = geomUtils.amountOfOverlap(tmpPoly->getPolygon(), largePolys);
						}
						catch(RSGISGeometryException &e)
						{
							overlap = 1;
						}
						
						//cout << "Overlap = " << overlap << endl;
						if(overlap < overlapThreshold)
						{
							largePolysData->push_back(tmpPoly);
							smallPolysData->erase(iterPolys);
						}
						else
						{
							++iterPolys;
						}
					}
					else
					{
						++iterPolys;
					}
				}
				
				
				float totalAreaProp = 0;
				vector<RSGISPolygonData*>::iterator iterData;
				for(iterData = largePolysData->begin(); iterData != largePolysData->end(); ++iterData)
				{
					tmpPoly = (RSGISClusteredClassificationPolygon*) *iterData;
					totalAreaProp += tmpPoly->getProTotalAr();
				}
				
				cout << "Large Polygons account for " << totalAreaProp*100 << " % of the total area in the scene\n";
				
				cout << "There are " << largePolysData->size() << " large polygons\n";
				cout << "There are " << smallPolysData->size() << " small polygons\n";
				
				if(largePolysData->size() > 0)
				{
					vecIO.exportPolygons2Shp(outputSHPLayerLarge, largePolysData);
					vecIO.exportPolygons2Shp(outputSHPLayerSmall, smallPolysData);
				}
				
				
				largePolys->clear();
				delete largePolys;
				largePolysData->clear();
				delete largePolysData;
				smallPolysData->clear();
				delete smallPolysData;
				
				for(int i = 0; i < numFeatures; ++i)
				{
					delete data[i];
				}
				delete[] data;
				
				OGRDataSource::DestroyDataSource(inputSHPDS);
				OGRDataSource::DestroyDataSource(outputSHPDSLarge);
				OGRDataSource::DestroyDataSource(outputSHPDSSmall);
				
				//OGRCleanupAll();
			}
			catch(RSGISException &e)
			{
				throw e;
			}
		}
		else if(option == RSGISExePostClassification::mergelargepolygons)
		{
			cout << "Merge the large polygons and remove overlaps\n";
			cout << "Input Shapefile = " << this->inputVector << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;
			cout << "Large Polygon Threshold = " << this->largeThreshold << endl;
			cout << "Intersect Ratio Threshold = " << this->intersectRatio << endl;
			
			OGRRegisterAll();
			
			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;
			
			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);
			
			OGRDataSource *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			
			OGRSpatialReference* inputSpatialRef = NULL;
			
			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);
				
				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
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
				inputSHPDS = OGRSFDriverRegistrar::Open(this->inputVector.c_str(), FALSE);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();	
								
				int numFeatures = inputSHPLayer->GetFeatureCount(true);
								
				cout << "There are " << numFeatures << " large polygons\n";
								
				RSGISPolygonData **data = new RSGISPolygonData*[numFeatures];
				for(int i = 0; i < numFeatures; ++i)
				{
					data[i] = new RSGISClusteredClassificationPolygon();
				}
				
				vecIO.readPolygons(inputSHPLayer, data, numFeatures);
								
				vector<Polygon*> *polys = new vector<Polygon*>();
				for(int i = 0; i < numFeatures; ++i)
				{
					polys->push_back((dynamic_cast<Polygon*>(data[i]->getPolygon()->clone())));
					delete data[i];
				}
				delete[] data;
				
				RSGISGeometry geomUtils;
				
				//float largePolyThreshold = 15000;
				//float intersectRatioThreshold = 0.66;
				
				bool change = true;
				bool change2 = true;
				bool overlapFound = false;
				bool poly1withinpoly2 = false;
				bool poly2withinpoly1 = false;
				bool mergeSmall = false;
				bool first = true;
				
				float intersectArea = 0;
				float geom1IntersectRatio = 0;
				float geom2IntersectRatio = 0;
				float relBorder = 0;
				float maxRelBorder = 0;
				
				int largePolys1Count = 0;
				int largePolys2Count = 0;
				int largePolysIntersectCount = 0;
				int totalLargePolys = 0;
				//int count = 0;
				
				Polygon *poly1 = NULL;
				Polygon *poly2 = NULL;
				Polygon *polyUnion = NULL;
				Polygon *polyUnionBuffer = NULL;
				Polygon *polyTmp1 = NULL;
				Polygon *polyTmp2 = NULL;
				Polygon *maxRelBorderPoly = NULL;
				Geometry *geom1 = NULL;
				Geometry *geom2 = NULL;
				Geometry *intersect = NULL;
				
				vector<Polygon*> *tmpPolysLarge = new vector<Polygon*>();
				vector<Polygon*> *tmpPolysSmall = new vector<Polygon*>();
				vector<Polygon*> *tmpPolys1 = new vector<Polygon*>();
				vector<Polygon*> *tmpPolys2 = new vector<Polygon*>();
				vector<Polygon*> *tmpPolysIntersect = new vector<Polygon*>();
				
				vector<Polygon*>::iterator iterPolysSmall;
				vector<Polygon*>::iterator iterPolys;
				vector<Polygon*>::iterator iterPolys1;
				vector<Polygon*>::iterator iterPolys2;
				while(change)
				{
					change = false;
					for(iterPolys1 = polys->begin(); iterPolys1 != polys->end(); )
					{
						poly1 = *iterPolys1;
						overlapFound = false;
						poly1withinpoly2 = false;
						poly2withinpoly1 = false;
						for(iterPolys2 = polys->begin(); iterPolys2 != polys->end(); ++iterPolys2)
						{
							poly2 = *iterPolys2;
							if((poly1 != poly2) && (poly1->overlaps(poly2)))
							{
								//cout << "Overlap found == TRUE!\n";
							    overlapFound = true;
								break;
							}
							else if((poly1 != poly2) && (poly1->within(poly2)))
							{
								poly1withinpoly2 = true;
								break;
							}
							else if((poly1 != poly2) && (poly2->within(poly1)))
							{
								poly2withinpoly1 = true;
								break;
							}
						}
						
						if(overlapFound)
						{
							//cout << "Overlap found\n";
							intersect = geomUtils.getIntersection(((Geometry*)poly1), ((Geometry*)poly2));
							
							//cout << "Intersect Geom Type = " << intersect->getGeometryType() << endl;
							
							intersectArea = intersect->getArea();
							geom1IntersectRatio = intersectArea/poly1->getArea();
							geom2IntersectRatio = intersectArea/poly2->getArea();
							//cout << "geom1IntersectRatio = " << geom1IntersectRatio << endl;
							//cout << "geom2IntersectRatio = " << geom2IntersectRatio << endl;
							if((geom1IntersectRatio >= this->intersectRatio) | (geom2IntersectRatio >= this->intersectRatio))
							{
								//cout << "Merge poly1 and poly2\n";
								delete intersect;
								
								polyUnion = geomUtils.polygonUnion(poly1, poly2);
								polyUnionBuffer = dynamic_cast<Polygon*>(polyUnion->buffer(0));
								delete polyUnion;
								
								polys->erase(remove(polys->begin(), polys->end(), poly1));
								polys->erase(remove(polys->begin(), polys->end(), poly2));
								delete poly1;
								delete poly2;
								
								polys->push_back(polyUnionBuffer);
								polyUnion = NULL;
								iterPolys1 = polys->begin();
							}
							else
							{
								//cout << "Resolve intersect\n";
								
								mergeSmall = false;
								
								geom1 = geomUtils.getDifference(poly1, poly2);
								geom2 = geomUtils.getDifference(poly2, poly1);
								
								//cout << "geom1 has " << geom1->getNumGeometries() << " geometries.\n";
								//cout << "geom2 has " << geom2->getNumGeometries() << " geometries.\n";
								//cout << "intersect has " << intersect->getNumGeometries() << " geometries.\n";
								
								geomUtils.retrievePolygons(geom1, tmpPolys1);
								geomUtils.retrievePolygons(geom2, tmpPolys2);
								geomUtils.retrievePolygons(intersect, tmpPolysIntersect);
								delete geom1;
								delete geom2;
								delete intersect;
								
								//cout << "geom1 has " << tmpPolys1->size() << " polygons.\n";
								//cout << "geom2 has " << tmpPolys2->size() << " polygons.\n";
								//cout << "intersect has " << tmpPolysIntersect->size() << " polygons.\n";
								
								largePolys1Count = 0;
								for(iterPolys = tmpPolys1->begin(); iterPolys != tmpPolys1->end(); )
								{
									if((*iterPolys)->getArea() >= this->largeThreshold)
									{
										++largePolys1Count;
										++iterPolys;
									}
									else if((*iterPolys)->getArea() < 0.1)
									{
										delete *iterPolys;
										tmpPolys1->erase(iterPolys);
									}
									else
									{
										++iterPolys;
									}
								}
								
								largePolys2Count = 0;
								for(iterPolys = tmpPolys2->begin(); iterPolys != tmpPolys2->end(); )
								{
									if((*iterPolys)->getArea() >= this->largeThreshold)
									{
										++largePolys2Count;
										++iterPolys;
									}
									else if((*iterPolys)->getArea() < 0.1)
									{
										delete *iterPolys;
										tmpPolys2->erase(iterPolys);
									}
									else
									{
										++iterPolys;
									}
									
								}
								
								largePolysIntersectCount = 0;
								for(iterPolys = tmpPolysIntersect->begin(); iterPolys != tmpPolysIntersect->end(); )
								{
									if((*iterPolys)->getArea() >= this->largeThreshold)
									{
										++largePolysIntersectCount;
										++iterPolys;
									}
									else if((*iterPolys)->getArea() < 0.1)
									{
										delete *iterPolys;
										tmpPolysIntersect->erase(iterPolys);
									}
									else
									{
										++iterPolys;
									}
								}
								
								//cout << "geom1 has " << largePolys1Count << " remaining large polygons\n";
								//cout << "geom2 has " << largePolys2Count << " remaining large polygons\n";
								//cout << "Intersect has " << largePolysIntersectCount << " remaining large polygons\n";
								
								totalLargePolys = largePolys1Count + largePolys2Count;
								
								//cout << "total Large Polygons = " << totalLargePolys << endl;
								
								if((largePolys1Count >= 1) & (largePolys2Count >= 1))
								{
									//cout << "((largePolys1Count >= 1) & (largePolys2Count >= 1))" << endl;
									
									// GET ALL LARGE POLYGONS AND MERGE ALL SMALL PARTS (difference and intersect).
									for(iterPolys = tmpPolys1->begin(); iterPolys != tmpPolys1->end(); ++iterPolys)
									{
										if((*iterPolys)->getArea() >= this->largeThreshold)
										{
											tmpPolysLarge->push_back(*iterPolys);
										}
										else
										{
											tmpPolysSmall->push_back(*iterPolys);
										}
									}
									
									for(iterPolys = tmpPolys2->begin(); iterPolys != tmpPolys2->end(); ++iterPolys)
									{
										if((*iterPolys)->getArea() >= this->largeThreshold)
										{
											tmpPolysLarge->push_back(*iterPolys);
										}
										else
										{
											tmpPolysSmall->push_back(*iterPolys);
										}
									}
									
									for(iterPolys = tmpPolysIntersect->begin(); iterPolys != tmpPolysIntersect->end(); ++iterPolys)
									{
										if((*iterPolys)->getArea() >= this->largeThreshold)
										{
											tmpPolysLarge->push_back(*iterPolys);
										}
										else
										{
											tmpPolysSmall->push_back(*iterPolys);
										}
									}
									
									tmpPolys1->clear();
									tmpPolys2->clear();
									tmpPolysIntersect->clear();
									
									polys->erase(remove(polys->begin(), polys->end(), poly1));
									polys->erase(remove(polys->begin(), polys->end(), poly2));
									delete poly1;
									delete poly2;
									
									mergeSmall = true;
								}
								else if(largePolys1Count > 1)
								{
									//cout << "(largePolys1Count > 1)" << endl;
									
									// GET LARGE POLYGONS + ORIGINAL poly2 and merge poly1 difference small bits
									for(iterPolys = tmpPolys1->begin(); iterPolys != tmpPolys1->end(); ++iterPolys)
									{
										if((*iterPolys)->getArea() >= this->largeThreshold)
										{
											tmpPolysLarge->push_back(*iterPolys);
										}
										else
										{
											tmpPolysSmall->push_back(*iterPolys);
										}
									}
									tmpPolysLarge->push_back(poly2);
									
									for(iterPolys = tmpPolys2->begin(); iterPolys != tmpPolys2->end(); )
									{
										delete *iterPolys;
										tmpPolys2->erase(iterPolys);
									}
									
									for(iterPolys = tmpPolysIntersect->begin(); iterPolys != tmpPolysIntersect->end(); )
									{
										delete *iterPolys;
										tmpPolysIntersect->erase(iterPolys);
									}
									
									tmpPolys1->clear();
									tmpPolys2->clear();
									tmpPolysIntersect->clear();
									
									polys->erase(remove(polys->begin(), polys->end(), poly1));
									polys->erase(remove(polys->begin(), polys->end(), poly2));
									delete poly1;
									
									mergeSmall = true;
								}
								else if(largePolys2Count > 1)
								{
									//cout << "(largePolys2Count > 1)" << endl;
									// Get large polygons + orig poly1 and merge poly2 difference small bits
									for(iterPolys = tmpPolys2->begin(); iterPolys != tmpPolys2->end(); ++iterPolys)
									{
										if((*iterPolys)->getArea() >= this->largeThreshold)
										{
											tmpPolysLarge->push_back(*iterPolys);
										}
										else
										{
											tmpPolysSmall->push_back(*iterPolys);
										}
									}
									tmpPolysLarge->push_back(poly1);
									
									
									for(iterPolys = tmpPolys1->begin(); iterPolys != tmpPolys1->end(); )
									{
										delete *iterPolys;
										tmpPolys1->erase(iterPolys);
									}
									
									for(iterPolys = tmpPolysIntersect->begin(); iterPolys != tmpPolysIntersect->end(); )
									{
										delete *iterPolys;
										tmpPolysIntersect->erase(iterPolys);
									}
									
									tmpPolys1->clear();
									tmpPolys2->clear();
									tmpPolysIntersect->clear();
									
									polys->erase(remove(polys->begin(), polys->end(), poly1));
									polys->erase(remove(polys->begin(), polys->end(), poly2));
									delete poly2;
									
									mergeSmall = true;
								}
								else if(totalLargePolys <= 1)
								{
									//cout << "(totalLargePolys <= 1)" << endl;
									// MERGE THE TWO POLYGONS
									polyUnion = geomUtils.polygonUnion(poly1, poly2);
									polyUnionBuffer = dynamic_cast<Polygon*>(polyUnion->buffer(0));
									delete polyUnion;
									
									polys->erase(remove(polys->begin(), polys->end(), poly1));
									polys->erase(remove(polys->begin(), polys->end(), poly2));
									delete poly1;
									delete poly2;
									
									polys->push_back(polyUnionBuffer);
									polyUnion = NULL;
									iterPolys1 = polys->begin();
									
									for(iterPolys = tmpPolys1->begin(); iterPolys != tmpPolys1->end(); )
									{
										delete *iterPolys;
										tmpPolys1->erase(iterPolys);
									}
										
									for(iterPolys = tmpPolys2->begin(); iterPolys != tmpPolys2->end(); )
									{
										delete *iterPolys;
										tmpPolys2->erase(iterPolys);
									}	
									
									for(iterPolys = tmpPolysIntersect->begin(); iterPolys != tmpPolysIntersect->end(); )
									{
										delete *iterPolys;
										tmpPolysIntersect->erase(iterPolys);
									}
									
									tmpPolys1->clear();
									tmpPolys2->clear();
									tmpPolysIntersect->clear();
									
									mergeSmall = false;
								}
								else
								{
									//cout << "ELSE\n";
									// GET ALL LARGE POLYGONS AND MERGE ALL SMALL PARTS (difference and intersect).
									for(iterPolys = tmpPolys1->begin(); iterPolys != tmpPolys1->end(); ++iterPolys)
									{
										if((*iterPolys)->getArea() >= this->largeThreshold)
										{
											tmpPolysLarge->push_back(*iterPolys);
										}
										else
										{
											tmpPolysSmall->push_back(*iterPolys);
										}
									}
									
									for(iterPolys = tmpPolys2->begin(); iterPolys != tmpPolys2->end(); ++iterPolys)
									{
										if((*iterPolys)->getArea() >= this->largeThreshold)
										{
											tmpPolysLarge->push_back(*iterPolys);
										}
										else
										{
											tmpPolysSmall->push_back(*iterPolys);
										}
									}
									
									for(iterPolys = tmpPolysIntersect->begin(); iterPolys != tmpPolysIntersect->end(); ++iterPolys)
									{
										if((*iterPolys)->getArea() >= this->largeThreshold)
										{
											tmpPolysLarge->push_back(*iterPolys);
										}
										else
										{
											tmpPolysSmall->push_back(*iterPolys);
										}
									}
									
										tmpPolys1->clear();
										tmpPolys2->clear();
										tmpPolysIntersect->clear();
										
										polys->erase(remove(polys->begin(), polys->end(), poly1));
										polys->erase(remove(polys->begin(), polys->end(), poly2));
										delete poly1;
										delete poly2;
										
										mergeSmall = true;
								}
								
								if(mergeSmall & (tmpPolysSmall->size() > 0))
								{
									/*
									vecIO.exportGEOSPolygons2SHP("/Users/pete/Temp/Clustering/polygons/large_polys.shp", force, tmpPolysLarge, inputSpatialRef);
									vecIO.exportGEOSPolygons2SHP("/Users/pete/Temp/Clustering/polygons/small_polys.shp", force, tmpPolysSmall, inputSpatialRef);
									
									cout << "mergeSmall\n";
									cout << "Number Large Polygons = " << tmpPolysLarge->size() << endl;
									cout << "Number Small Polygons = " << tmpPolysSmall->size() << endl;
									 */
									change2 = true;
									while(change2)
									{
										change2 = false;
										//cout << "iterate \n";
										for(iterPolysSmall = tmpPolysSmall->begin(); iterPolysSmall != tmpPolysSmall->end(); )
										{
											//cout << "iterate2\n";
											polyTmp1 = *iterPolysSmall;
											
											relBorder = 0;
											maxRelBorder = 0;
											maxRelBorderPoly = NULL;
											first = true;											
											
											for(iterPolys = tmpPolysLarge->begin(); iterPolys != tmpPolysLarge->end(); ++iterPolys)
											{
												polyTmp2 = *iterPolys;
												relBorder = geomUtils.calcRelativeBorder(polyTmp1, polyTmp2);
												//cout << "Rel Border = " << relBorder << endl;
												if(first)
												{
													maxRelBorder = relBorder;
													maxRelBorderPoly = polyTmp2;
													first = false;
												}
												else if(relBorder > maxRelBorder)
												{
													maxRelBorder = relBorder;
													maxRelBorderPoly = polyTmp2;
												}
											}
											
											//cout << "maxRelBorder = " << maxRelBorder << endl;
											
											if(maxRelBorder > 0)
											{
												polyUnion = geomUtils.polygonUnion(polyTmp1, maxRelBorderPoly);
												polyUnionBuffer = dynamic_cast<Polygon*>(polyUnion->buffer(0));
												delete polyUnion;
												tmpPolysLarge->erase(remove(tmpPolysLarge->begin(), tmpPolysLarge->end(), maxRelBorderPoly));
												tmpPolysLarge->push_back(polyUnionBuffer);
												delete maxRelBorderPoly;
												delete *iterPolysSmall;
												tmpPolysSmall->erase(iterPolysSmall);
												change2 = true;
											}
											else
											{
												//cout << "++iterPolysSmall\n";
												++iterPolysSmall;
											}
										}
									}
									
									iterPolys1 = polys->begin();
								}
								
								for(iterPolys = tmpPolysLarge->begin(); iterPolys != tmpPolysLarge->end(); )
								{
									polys->push_back(*iterPolys);
									tmpPolysLarge->erase(iterPolys);
								}
								
								//string filename = string("/Users/pete/Temp/Clustering/polygons/polys_") + mathsUtils.inttostring(count++) + string(".shp");
								//vecIO.exportGEOSPolygons2SHP(filename, force, polys, inputSpatialRef);
								
								geom1 = NULL;
								geom2 = NULL;
								intersect = NULL;
								iterPolys1 = polys->begin();
							}
							
							change = true;
						}
						else if(poly1withinpoly2)
						{
							//cout << "Polygon 1 within polygon 2\n";
							polys->erase(remove(polys->begin(), polys->end(), poly1));
							delete poly1;
							poly2 = NULL;
							iterPolys1 = polys->begin();
							
							change = true;
						}
						else if(poly2withinpoly1)
						{
							//cout << "Polygon 2 within polygon 1\n";
							polys->erase(remove(polys->begin(), polys->end(), poly2));
							poly1 = NULL;
							delete poly2;
							iterPolys1 = polys->begin();
							
							change = true;
						}
						else
						{
							//cout << "No overlap\n";
							++iterPolys1;
						}
					}					
				}
				
				tmpPolysLarge->clear();
				delete tmpPolysLarge;
				
				tmpPolysSmall->clear();
				delete tmpPolysSmall;
				
				tmpPolys1->clear();
				delete tmpPolys1;
				
				tmpPolys2->clear();
				delete tmpPolys2;
				
				tmpPolysIntersect->clear();
				delete tmpPolysIntersect;
				
				vecIO.exportGEOSPolygons2SHP(this->outputVector, force, polys, inputSpatialRef);
				
				for(iterPolys1 = polys->begin(); iterPolys1 != polys->end(); )
				{
					delete *iterPolys1;
					polys->erase(iterPolys1);
				}
				delete polys;
				
				OGRDataSource::DestroyDataSource(inputSHPDS);
				
				//OGRCleanupAll();
			}
			catch(RSGISException &e)
			{
				delete RSGISGEOSFactoryGenerator::getInstance();
				//OGRCleanupAll();
				throw e;
			}
		}
		else if(option == RSGISExePostClassification::morphology)
		{
			cout << "Undertake morphology on the input polygons\n";
			cout << "Input Shapefile = " << this->inputVector << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;
			if(morphologyType == closing)
			{
				cout << "Undertaking a morphological closing\n";
			}
			else if(morphologyType == opening)
			{
				cout << "Undertaking a morphological opening\n";
			}
			else if(morphologyType == dilation)
			{
				cout << "Undertaking a morphological dilation\n";
			}
			else if(morphologyType == erosion)
			{
				cout << "Undertaking a morphological erosion\n";
			}
			else
			{
				cout << "ERROR: Unknown morphological operation\n";
			}
			
			cout << "Buffer = " << buffer << endl;
			
			OGRRegisterAll();
			
			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;
			
			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);
			
			OGRDataSource *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			
			OGRSpatialReference* inputSpatialRef = NULL;
			
			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);
				
				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
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
				inputSHPDS = OGRSFDriverRegistrar::Open(this->inputVector.c_str(), FALSE);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer->GetSpatialRef();	
				
				// Get Geometry Type.
				OGRFeature *feature = inputSHPLayer->GetFeature(0);
				OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
				OGRFeature::DestroyFeature(feature);
				
				/////////////////////////////////////
				//
				// Create Output Shapfile.
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->CreateDataSource(this->outputVector.c_str(), NULL);
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message);
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}
				
				int numFeatures = inputSHPLayer->GetFeatureCount(true);
				RSGISPolygonData **data = new RSGISPolygonData*[numFeatures];
				for(int i = 0; i < numFeatures; ++i)
				{
					data[i] = new RSGISClusteredClassificationPolygon();
				}
				
				vecIO.readPolygons(inputSHPLayer, data, numFeatures);
				
				vector<RSGISPolygonData*> *polys = new vector<RSGISPolygonData*>();
				for(int i = 0; i < numFeatures; ++i)
				{
					polys->push_back(data[i]);
				}
				delete[] data;
				
				RSGISGeometry geomUtils;
				vector<Polygon*>::iterator iterPolys;
				vector<RSGISPolygonData*>::iterator iterClusterPolys;
				RSGISClusteredClassificationPolygon *tmpPoly = NULL;
				vector<Polygon*> *tmpPolys = new vector<Polygon*>();
				vector<RSGISPolygonData*> *polysNew = new vector<RSGISPolygonData*>();
				for(iterClusterPolys = polys->begin(); iterClusterPolys != polys->end(); )
				{
					tmpPoly = (RSGISClusteredClassificationPolygon*) *iterClusterPolys;
					
					tmpPolys->push_back((dynamic_cast<Polygon*>(tmpPoly->getPolygon()->clone())));
					geomUtils.performMorphologicalOperation(tmpPolys, morphologyType, buffer, 30);
					if(tmpPolys->size() == 1)
					{
						tmpPoly->setPolygon(tmpPolys->at(0));
						delete tmpPolys->at(0);
						tmpPolys->clear();
						++iterClusterPolys;
					}
					else if(tmpPolys->size() > 1)
					{
						RSGISClusteredClassificationPolygon *tmpPolyPart;
						
						float totalArea = tmpPoly->getArea() * (1 - tmpPoly->getProTotalAr());
						float areaRatio = 0;
						for(iterPolys = tmpPolys->begin(); iterPolys != tmpPolys->end(); ++iterPolys)
						{
							tmpPolyPart = new RSGISClusteredClassificationPolygon(tmpPoly);
							tmpPolyPart->setPolygon((*iterPolys));
							tmpPolyPart->setArea((*iterPolys)->getArea());
							tmpPolyPart->setProTotalAr((*iterPolys)->getArea()/totalArea);
							areaRatio = tmpPolyPart->getArea()/tmpPoly->getArea();
							tmpPolyPart->setPolysArea(tmpPoly->getPolysArea() * areaRatio);
							tmpPolyPart->setProPolysArea(tmpPolyPart->getPolysArea()/tmpPolyPart->getArea());
							polysNew->push_back(tmpPolyPart);
						}
						delete *iterClusterPolys;
						polys->erase(iterClusterPolys);
						tmpPolys->clear();
					}
					else
					{
						delete *iterClusterPolys;
						polys->erase(iterClusterPolys);
						tmpPolys->clear();
					}
				}
				
				for(iterClusterPolys = polysNew->begin(); iterClusterPolys != polysNew->end(); )
				{
					polys->push_back(*iterClusterPolys);
					polysNew->erase(iterClusterPolys);
				}
				delete polysNew;
				delete tmpPolys;
				
				vecIO.exportPolygons2Shp(outputSHPLayer, polys);
				
				for(iterClusterPolys = polys->begin(); iterClusterPolys != polys->end(); )
				{
					delete *iterClusterPolys;
					polys->erase(iterClusterPolys);
				}
				delete polys;
				
				OGRDataSource::DestroyDataSource(inputSHPDS);
				OGRDataSource::DestroyDataSource(outputSHPDS);
				
				//OGRCleanupAll();
			}
			catch(RSGISException &e)
			{
				delete RSGISGEOSFactoryGenerator::getInstance();
				//OGRCleanupAll();
				throw e;
			}
		}
		else if(option == RSGISExePostClassification::removesmallwithinlarge)
		{
			cout << "Remove any small polygons that are contained within a large polygon.\n";
			cout << "Input Shapefile (Large Polygons) = " << this->inputVectorLarge << endl;
			cout << "Input Shapefile (Small Polygons) = " << this->inputVectorSmall << endl;
			cout << "Output Large Shapefile = " << this->outputLargeVector << endl;
			cout << "Output Small Shapefile = " << this->outputSmallVector << endl;
			cout << "Overlap Threshold = " << this->overlapThreshold << endl;
			
			OGRRegisterAll();
			
			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;
			RSGISGeometry geomUtils;
			
			string SHPFileInLayerLarge = vecUtils.getLayerName(this->inputVectorLarge);
			string SHPFileInLayerSmall = vecUtils.getLayerName(this->inputVectorSmall);
			string SHPFileOutLayerSmall = vecUtils.getLayerName(this->outputSmallVector);
			
			OGRDataSource *inputSHPDSLarge = NULL;
			OGRLayer *inputSHPLayerLarge = NULL;
			OGRDataSource *inputSHPDSSmall = NULL;
			OGRLayer *inputSHPLayerSmall = NULL;
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDSSmall = NULL;
			OGRLayer *outputSHPLayerSmall = NULL;
			
			OGRSpatialReference* inputSpatialRef = NULL;
			
			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;	
			
			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputSmallVector);
				
				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayerSmall))
				{
					if(this->force)
					{
						vecUtils.deleteSHP(outputDIR, SHPFileOutLayerSmall);
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
				inputSHPDSLarge = OGRSFDriverRegistrar::Open(this->inputVectorLarge.c_str(), FALSE);
				if(inputSHPDSLarge == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVectorLarge;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayerLarge = inputSHPDSLarge->GetLayerByName(SHPFileInLayerLarge.c_str());
				if(inputSHPLayerLarge == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayerLarge;
					throw RSGISFileException(message.c_str());
				}
				
				/////////////////////////////////////
				//
				// Open Input Shapfile. -- SMALL
				//
				/////////////////////////////////////
				inputSHPDSSmall = OGRSFDriverRegistrar::Open(this->inputVectorSmall.c_str(), FALSE);
				if(inputSHPDSSmall == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVectorSmall;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayerSmall = inputSHPDSSmall->GetLayerByName(SHPFileInLayerSmall.c_str());
				if(inputSHPLayerSmall == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayerSmall;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayerSmall->GetSpatialRef();	
				
				// Get Geometry Type.
				OGRFeature *feature = inputSHPLayerSmall->GetFeature(1);
				OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
				OGRFeature::DestroyFeature(feature);
				
				/////////////////////////////////////
				//
				// Create Output Shapfile. -- SMALL
				//
				/////////////////////////////////////
				const char *pszDriverName = "ESRI Shapefile";
				shpFiledriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
				if( shpFiledriver == NULL )
				{
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				
				outputSHPDSSmall = shpFiledriver->CreateDataSource(this->outputSmallVector.c_str(), NULL);
				if( outputSHPDSSmall == NULL )
				{
					string message = string("Could not create vector file ") + this->outputSmallVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayerSmall = outputSHPDSSmall->CreateLayer(SHPFileOutLayerSmall.c_str(), inputSpatialRef, geometryType, NULL );
				if( outputSHPLayerSmall == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayerSmall;
					throw RSGISVectorOutputException(message.c_str());
				}	
				
				
				int numFeatures = inputSHPLayerSmall->GetFeatureCount(true);
				RSGISPolygonData **data = new RSGISPolygonData*[numFeatures];
				for(int i = 0; i < numFeatures; ++i)
				{
					data[i] = new RSGISClusteredClassificationPolygon();
				}
				
				cout << "Read inputted small polygons\n";
				vecIO.readPolygons(inputSHPLayerSmall, data, numFeatures);
				
				vector<RSGISPolygonData*> *smallPolys = new vector<RSGISPolygonData*>();
				for(int i = 0; i < numFeatures; ++i)
				{
					//cout << data[i]->getPolygon()->toText() << endl;
					smallPolys->push_back(data[i]);
				}
				
				vector<Polygon*> *largePolys = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(largePolys);
				processVector = new RSGISProcessVector(processFeature);
				
				cout << "Read inputted large polygons\n";
				processVector->processVectorsNoOutput(inputSHPLayerLarge, false);
				
				delete processVector;
				delete processFeature;
				
				
				cout << "There are " << largePolys->size() << " large polygons\n";
				cout << "There are " << smallPolys->size() << " small polygons\n";
				
				//float overlapthreshold = 0.66;
				
				MultiPolygon *mPolyLarge = vecUtils.convertGEOSPolygons2GEOSMultiPolygon(largePolys);
				
				float intersectArea = 0;
				float mPolyLargeIntersectArRatio = 0;
				float maxIntersectArea = 0;
				
				bool first = true;
				bool change = true;
				
				vector<RSGISPolygonData*>::iterator iterSmallPolys;
				vector<Polygon*>::iterator iterLargePolys;
				
				Geometry *intersect = NULL;
				
				Polygon *smallPoly = NULL;
				Polygon *maxIntersect = NULL;
				Polygon *unionPoly = NULL;
				
				int feedback = numFeatures/10;
				int i = 0;
				cout << "Started" << endl;
					
				while(change)
				{
					change = false;
					for(iterSmallPolys = smallPolys->begin(); iterSmallPolys != smallPolys->end(); )
					{
						if(numFeatures > 10)
						{
							if((i % feedback) == 0)
							{
								cout  <<  "." << flush;
							}
						}	
						
						smallPoly = (*iterSmallPolys)->getPolygon();
						if(smallPoly == NULL)
						{
							smallPolys->erase(iterSmallPolys);
						}
						else if(smallPoly->getArea() < 0.1)
						{
							smallPolys->erase(iterSmallPolys);
						}
						else if(smallPoly->within(mPolyLarge))
						{
							smallPolys->erase(iterSmallPolys);
						}
						else if(smallPoly->intersects(mPolyLarge))
						{
							intersect = geomUtils.getIntersection(((Geometry*)smallPoly), ((Geometry*)mPolyLarge));
							intersectArea = intersect->getArea();
							delete intersect;
							mPolyLargeIntersectArRatio = intersectArea/smallPoly->getArea();
							
							if(mPolyLargeIntersectArRatio == 1)
							{
								smallPolys->erase(iterSmallPolys);
							}
							else if(mPolyLargeIntersectArRatio > this->overlapThreshold)
							{
								first = true;
								for(iterLargePolys = largePolys->begin(); iterLargePolys != largePolys->end(); ++iterLargePolys)
								{
									intersect = geomUtils.getIntersection(((Geometry*)smallPoly), ((Geometry*)*iterLargePolys));
									intersectArea = intersect->getArea();
									if(first)
									{
										maxIntersectArea = intersectArea;
										maxIntersect = *iterLargePolys;
										first = false;
									}
									else if(intersectArea > maxIntersectArea)
									{
										maxIntersectArea = intersectArea;
										maxIntersect = *iterLargePolys;
									}
									delete intersect;
								}
								
								largePolys->erase(remove(largePolys->begin(), largePolys->end(), maxIntersect));
								unionPoly = geomUtils.polygonUnion(maxIntersect, smallPoly);
								delete maxIntersect;
								largePolys->push_back(unionPoly);
								
								smallPolys->erase(iterSmallPolys);
								change = true;
							}
							else
							{
								++iterSmallPolys;
							}
						}
						else
						{
							++iterSmallPolys;
						}
						++i;
					}
				}
				
				cout << " Complete.\n";
				
				cout << "Output Polygons:\n";
				vecIO.exportPolygons2Shp(outputSHPLayerSmall, smallPolys);
				vecIO.exportGEOSPolygons2SHP(this->outputLargeVector, force, largePolys, inputSpatialRef);
				
				
				for(int i = 0; i < numFeatures; ++i)
				{
					if(data[i] != NULL)
					{
						delete data[i];
					}
				}
				delete[] data;
				
				for(iterLargePolys = largePolys->begin(); iterLargePolys != largePolys->end(); )
				{
					if(*iterLargePolys != NULL)
					{
						delete *iterLargePolys;
					}
					largePolys->erase(iterLargePolys);
				}
				delete largePolys;
				
				delete mPolyLarge;
				
				smallPolys->clear();
				delete smallPolys;
				
				OGRDataSource::DestroyDataSource(inputSHPDSLarge);
				OGRDataSource::DestroyDataSource(inputSHPDSSmall);
				OGRDataSource::DestroyDataSource(outputSHPDSSmall);
				
				//OGRCleanupAll();
			}
			catch(RSGISException &e)
			{
				delete RSGISGEOSFactoryGenerator::getInstance();
				//OGRCleanupAll();
				throw e;
			}
		}
		else if(option == RSGISExePostClassification::mergesmallandlarge)
		{
			cout << "Merge the small and large polygons together where possible.\n";
			cout << "Input Shapefile (Large Polygons) = " << this->inputVectorLarge << endl;
			cout << "Input Shapefile (Small Polygons) = " << this->inputVectorSmall << endl;
			cout << "Output Large Shapefile = " << this->outputLargeVector << endl;
			cout << "Output Small Shapefile = " << this->outputSmallVector << endl;
			cout << "Large Polygon Threshold = " << this->largeThreshold << endl;
			cout << "Intersect Ratio Threshold = " << this->intersectRatio << endl;
			cout << "Overlap Threshold = " << this->overlapThreshold << endl;
			
			OGRRegisterAll();
			
			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;
			RSGISGeometry geomUtils;
			
			string SHPFileInLayerLarge = vecUtils.getLayerName(this->inputVectorLarge);
			string SHPFileInLayerSmall = vecUtils.getLayerName(this->inputVectorSmall);
			
			OGRDataSource *inputSHPDSLarge = NULL;
			OGRLayer *inputSHPLayerLarge = NULL;
			OGRDataSource *inputSHPDSSmall = NULL;
			OGRLayer *inputSHPLayerSmall = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;
			
			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;	
			
			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDSLarge = OGRSFDriverRegistrar::Open(this->inputVectorLarge.c_str(), FALSE);
				if(inputSHPDSLarge == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVectorLarge;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayerLarge = inputSHPDSLarge->GetLayerByName(SHPFileInLayerLarge.c_str());
				if(inputSHPLayerLarge == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayerLarge;
					throw RSGISFileException(message.c_str());
				}
				
				/////////////////////////////////////
				//
				// Open Input Shapfile. -- SMALL
				//
				/////////////////////////////////////
				inputSHPDSSmall = OGRSFDriverRegistrar::Open(this->inputVectorSmall.c_str(), FALSE);
				if(inputSHPDSSmall == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVectorSmall;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayerSmall = inputSHPDSSmall->GetLayerByName(SHPFileInLayerSmall.c_str());
				if(inputSHPLayerSmall == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayerSmall;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayerSmall->GetSpatialRef();	
				
				vector<Polygon*> *largePolys = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(largePolys);
				processVector = new RSGISProcessVector(processFeature);
				
				cout << "Read inputted large polygons\n";
				processVector->processVectorsNoOutput(inputSHPLayerLarge, false);
				
				delete processVector;
				delete processFeature;
				
				vector<Polygon*> *smallPolys = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(smallPolys);
				processVector = new RSGISProcessVector(processFeature);
				
				cout << "Read inputted small polygons\n";
				processVector->processVectorsNoOutput(inputSHPLayerSmall, false);
				
				delete processVector;
				delete processFeature;
				
				cout << "There are " << largePolys->size() << " large polygons\n";
				cout << "There are " << smallPolys->size() << " small polygons\n";
								
				vector<Polygon*>::iterator iterLargePolys;
				vector<Polygon*>::iterator iterSmallPolys;
				
				//float largePolyThreshold = 10000;
				//float overlapthreshold = 0;
				//float intersectRatioThreshold = 0.66;
				
				
				for(iterSmallPolys = smallPolys->begin(); iterSmallPolys != smallPolys->end(); )
				{
					if((*iterSmallPolys)->getArea() >= this->largeThreshold)
					{
						largePolys->push_back(*iterSmallPolys);
						smallPolys->erase(iterSmallPolys);
					}
					else
					{
						++iterSmallPolys;
					}
				}
				
				bool change = true;
				bool change2 = true;
				bool overlapFound = false;
				bool poly1withinpoly2 = false;
				bool poly2withinpoly1 = false;
				bool mergeSmall = false;
				bool first = true;
				
				float intersectArea = 0;
				float geom1IntersectRatio = 0;
				float geom2IntersectRatio = 0;
				float relBorder = 0;
				float maxRelBorder = 0;
				
				int largePolys1Count = 0;
				int largePolys2Count = 0;
				int largePolysIntersectCount = 0;
				int totalLargePolys = 0;
				//int count = 0;
				
				Polygon *poly1 = NULL;
				Polygon *poly2 = NULL;
				Polygon *polyUnion = NULL;
				Polygon *polyUnionBuffer = NULL;
				Polygon *polyTmp1 = NULL;
				Polygon *polyTmp2 = NULL;
				Polygon *maxRelBorderPoly = NULL;
				Geometry *geom1 = NULL;
				Geometry *geom2 = NULL;
				Geometry *intersect = NULL;
				
				vector<Polygon*> *tmpPolysLarge = new vector<Polygon*>();
				vector<Polygon*> *tmpPolysSmall = new vector<Polygon*>();
				vector<Polygon*> *tmpPolys1 = new vector<Polygon*>();
				vector<Polygon*> *tmpPolys2 = new vector<Polygon*>();
				vector<Polygon*> *tmpPolysIntersect = new vector<Polygon*>();
				
				vector<Polygon*>::iterator iterPolysSmall;
				vector<Polygon*>::iterator iterPolys;
				vector<Polygon*>::iterator iterPolys1;
				vector<Polygon*>::iterator iterPolys2;
								
				while(change)
				{
					change = false;
					for(iterPolys1 = largePolys->begin(); iterPolys1 != largePolys->end(); )
					{
						poly1 = *iterPolys1;
						overlapFound = false;
						poly1withinpoly2 = false;
						poly2withinpoly1 = false;
						for(iterPolys2 = largePolys->begin(); iterPolys2 != largePolys->end(); ++iterPolys2)
						{
							poly2 = *iterPolys2;
							if((poly1 != poly2) && (poly1->overlaps(poly2)))
							{
								//cout << "Overlap found == TRUE!\n";
							    overlapFound = true;
								break;
							}
							else if((poly1 != poly2) && (poly1->within(poly2)))
							{
								poly1withinpoly2 = true;
								break;
							}
							else if((poly1 != poly2) && (poly2->within(poly1)))
							{
								poly2withinpoly1 = true;
								break;
							}
						}
						
						if(overlapFound)
						{
							//cout << "Overlap found\n";
							intersect = geomUtils.getIntersection(((Geometry*)poly1), ((Geometry*)poly2));
							
							//cout << "Intersect Geom Type = " << intersect->getGeometryType() << endl;
							
							intersectArea = intersect->getArea();
							geom1IntersectRatio = intersectArea/poly1->getArea();
							geom2IntersectRatio = intersectArea/poly2->getArea();
							//cout << "geom1IntersectRatio = " << geom1IntersectRatio << endl;
							//cout << "geom2IntersectRatio = " << geom2IntersectRatio << endl;
							if((geom1IntersectRatio >= this->intersectRatio) | (geom2IntersectRatio >= this->intersectRatio))
							{
								//cout << "Merge poly1 and poly2\n";
								delete intersect;
								
								polyUnion = geomUtils.polygonUnion(poly1, poly2);
								polyUnionBuffer = dynamic_cast<Polygon*>(polyUnion->buffer(0));
								delete polyUnion;
								
								largePolys->erase(remove(largePolys->begin(), largePolys->end(), poly1));
								largePolys->erase(remove(largePolys->begin(), largePolys->end(), poly2));
								delete poly1;
								delete poly2;
								
								largePolys->push_back(polyUnionBuffer);
								polyUnion = NULL;
								iterPolys1 = largePolys->begin();
							}
							else
							{
								//cout << "Resolve intersect\n";
								
								mergeSmall = false;
								
								geom1 = geomUtils.getDifference(poly1, poly2);
								geom2 = geomUtils.getDifference(poly2, poly1);
								
								//cout << "geom1 has " << geom1->getNumGeometries() << " geometries.\n";
								//cout << "geom2 has " << geom2->getNumGeometries() << " geometries.\n";
								//cout << "intersect has " << intersect->getNumGeometries() << " geometries.\n";
								
								geomUtils.retrievePolygons(geom1, tmpPolys1);
								geomUtils.retrievePolygons(geom2, tmpPolys2);
								geomUtils.retrievePolygons(intersect, tmpPolysIntersect);
								delete geom1;
								delete geom2;
								delete intersect;
								
								//cout << "geom1 has " << tmpPolys1->size() << " polygons.\n";
								//cout << "geom2 has " << tmpPolys2->size() << " polygons.\n";
								//cout << "intersect has " << tmpPolysIntersect->size() << " polygons.\n";
								
								largePolys1Count = 0;
								for(iterPolys = tmpPolys1->begin(); iterPolys != tmpPolys1->end(); )
								{
									if((*iterPolys)->getArea() >= this->largeThreshold)
									{
										++largePolys1Count;
										++iterPolys;
									}
									else if((*iterPolys)->getArea() < 0.1)
									{
										delete *iterPolys;
										tmpPolys1->erase(iterPolys);
									}
									else
									{
										++iterPolys;
									}
								}
								
								largePolys2Count = 0;
								for(iterPolys = tmpPolys2->begin(); iterPolys != tmpPolys2->end(); )
								{
									if((*iterPolys)->getArea() >= this->largeThreshold)
									{
										++largePolys2Count;
										++iterPolys;
									}
									else if((*iterPolys)->getArea() < 0.1)
									{
										delete *iterPolys;
										tmpPolys2->erase(iterPolys);
									}
									else
									{
										++iterPolys;
									}
									
								}
								
								largePolysIntersectCount = 0;
								for(iterPolys = tmpPolysIntersect->begin(); iterPolys != tmpPolysIntersect->end(); )
								{
									if((*iterPolys)->getArea() >= this->largeThreshold)
									{
										++largePolysIntersectCount;
										++iterPolys;
									}
									else if((*iterPolys)->getArea() < 0.1)
									{
										delete *iterPolys;
										tmpPolysIntersect->erase(iterPolys);
									}
									else
									{
										++iterPolys;
									}
								}
								
								//cout << "geom1 has " << largePolys1Count << " remaining large polygons\n";
								//cout << "geom2 has " << largePolys2Count << " remaining large polygons\n";
								//cout << "Intersect has " << largePolysIntersectCount << " remaining large polygons\n";
								
								totalLargePolys = largePolys1Count + largePolys2Count;
								
								//cout << "total Large Polygons = " << totalLargePolys << endl;
								
								if((largePolys1Count >= 1) & (largePolys2Count >= 1))
								{
									//cout << "((largePolys1Count >= 1) & (largePolys2Count >= 1))" << endl;
									
									// GET ALL LARGE POLYGONS AND MERGE ALL SMALL PARTS (difference and intersect).
									for(iterPolys = tmpPolys1->begin(); iterPolys != tmpPolys1->end(); ++iterPolys)
									{
										if((*iterPolys)->getArea() >= this->largeThreshold)
										{
											tmpPolysLarge->push_back(*iterPolys);
										}
										else
										{
											tmpPolysSmall->push_back(*iterPolys);
										}
									}
									
									for(iterPolys = tmpPolys2->begin(); iterPolys != tmpPolys2->end(); ++iterPolys)
									{
										if((*iterPolys)->getArea() >= this->largeThreshold)
										{
											tmpPolysLarge->push_back(*iterPolys);
										}
										else
										{
											tmpPolysSmall->push_back(*iterPolys);
										}
									}
									
									for(iterPolys = tmpPolysIntersect->begin(); iterPolys != tmpPolysIntersect->end(); ++iterPolys)
									{
										if((*iterPolys)->getArea() >= this->largeThreshold)
										{
											tmpPolysLarge->push_back(*iterPolys);
										}
										else
										{
											tmpPolysSmall->push_back(*iterPolys);
										}
									}
									
									tmpPolys1->clear();
									tmpPolys2->clear();
									tmpPolysIntersect->clear();
									
									largePolys->erase(remove(largePolys->begin(), largePolys->end(), poly1));
									largePolys->erase(remove(largePolys->begin(), largePolys->end(), poly2));
									delete poly1;
									delete poly2;
									
									mergeSmall = true;
								}
								else if(largePolys1Count > 1)
								{
									//cout << "(largePolys1Count > 1)" << endl;
									
									// GET LARGE POLYGONS + ORIGINAL poly2 and merge poly1 difference small bits
									for(iterPolys = tmpPolys1->begin(); iterPolys != tmpPolys1->end(); ++iterPolys)
									{
										if((*iterPolys)->getArea() >= this->largeThreshold)
										{
											tmpPolysLarge->push_back(*iterPolys);
										}
										else
										{
											tmpPolysSmall->push_back(*iterPolys);
										}
									}
									tmpPolysLarge->push_back(poly2);
									
									for(iterPolys = tmpPolys2->begin(); iterPolys != tmpPolys2->end(); )
									{
										delete *iterPolys;
										tmpPolys2->erase(iterPolys);
									}
									
									for(iterPolys = tmpPolysIntersect->begin(); iterPolys != tmpPolysIntersect->end(); )
									{
										delete *iterPolys;
										tmpPolysIntersect->erase(iterPolys);
									}
									
									tmpPolys1->clear();
									tmpPolys2->clear();
									tmpPolysIntersect->clear();
									
									largePolys->erase(remove(largePolys->begin(), largePolys->end(), poly1));
									largePolys->erase(remove(largePolys->begin(), largePolys->end(), poly2));
									delete poly1;
									
									mergeSmall = true;
								}
								else if(largePolys2Count > 1)
								{
									//cout << "(largePolys2Count > 1)" << endl;
									// Get large polygons + orig poly1 and merge poly2 difference small bits
									for(iterPolys = tmpPolys2->begin(); iterPolys != tmpPolys2->end(); ++iterPolys)
									{
										if((*iterPolys)->getArea() >= this->largeThreshold)
										{
											tmpPolysLarge->push_back(*iterPolys);
										}
										else
										{
											tmpPolysSmall->push_back(*iterPolys);
										}
									}
									tmpPolysLarge->push_back(poly1);
									
									
									for(iterPolys = tmpPolys1->begin(); iterPolys != tmpPolys1->end(); )
									{
										delete *iterPolys;
										tmpPolys1->erase(iterPolys);
									}
									
									for(iterPolys = tmpPolysIntersect->begin(); iterPolys != tmpPolysIntersect->end(); )
									{
										delete *iterPolys;
										tmpPolysIntersect->erase(iterPolys);
									}
									
									tmpPolys1->clear();
									tmpPolys2->clear();
									tmpPolysIntersect->clear();
									
									largePolys->erase(remove(largePolys->begin(), largePolys->end(), poly1));
									largePolys->erase(remove(largePolys->begin(), largePolys->end(), poly2));
									delete poly2;
									
									mergeSmall = true;
								}
								else if(totalLargePolys <= 1)
								{
									//cout << "(totalLargePolys <= 1)" << endl;
									// MERGE THE TWO POLYGONS
									polyUnion = geomUtils.polygonUnion(poly1, poly2);
									polyUnionBuffer = dynamic_cast<Polygon*>(polyUnion->buffer(0));
									delete polyUnion;
									
									largePolys->erase(remove(largePolys->begin(), largePolys->end(), poly1));
									largePolys->erase(remove(largePolys->begin(), largePolys->end(), poly2));
									delete poly1;
									delete poly2;
									
									largePolys->push_back(polyUnionBuffer);
									polyUnion = NULL;
									iterPolys1 = largePolys->begin();
									
									for(iterPolys = tmpPolys1->begin(); iterPolys != tmpPolys1->end(); )
									{
										delete *iterPolys;
										tmpPolys1->erase(iterPolys);
									}
										
									for(iterPolys = tmpPolys2->begin(); iterPolys != tmpPolys2->end(); )
									{
										delete *iterPolys;
										tmpPolys2->erase(iterPolys);
									}	
									
									for(iterPolys = tmpPolysIntersect->begin(); iterPolys != tmpPolysIntersect->end(); )
									{
										delete *iterPolys;
										tmpPolysIntersect->erase(iterPolys);
									}
									
									tmpPolys1->clear();
									tmpPolys2->clear();
									tmpPolysIntersect->clear();
									
									mergeSmall = false;
								}
								else
								{
									//cout << "ELSE\n";
									// GET ALL LARGE POLYGONS AND MERGE ALL SMALL PARTS (difference and intersect).
									for(iterPolys = tmpPolys1->begin(); iterPolys != tmpPolys1->end(); ++iterPolys)
									{
										if((*iterPolys)->getArea() >= this->largeThreshold)
										{
											tmpPolysLarge->push_back(*iterPolys);
										}
										else
										{
											tmpPolysSmall->push_back(*iterPolys);
										}
									}
									
									for(iterPolys = tmpPolys2->begin(); iterPolys != tmpPolys2->end(); ++iterPolys)
									{
										if((*iterPolys)->getArea() >= this->largeThreshold)
										{
											tmpPolysLarge->push_back(*iterPolys);
										}
										else
										{
											tmpPolysSmall->push_back(*iterPolys);
										}
									}
									
									for(iterPolys = tmpPolysIntersect->begin(); iterPolys != tmpPolysIntersect->end(); ++iterPolys)
									{
										if((*iterPolys)->getArea() >= this->largeThreshold)
										{
											tmpPolysLarge->push_back(*iterPolys);
										}
										else
										{
											tmpPolysSmall->push_back(*iterPolys);
										}
									}
									
										tmpPolys1->clear();
										tmpPolys2->clear();
										tmpPolysIntersect->clear();
										
										largePolys->erase(remove(largePolys->begin(), largePolys->end(), poly1));
										largePolys->erase(remove(largePolys->begin(), largePolys->end(), poly2));
										delete poly1;
										delete poly2;
										
										mergeSmall = true;
								}
								
								if(mergeSmall & (tmpPolysSmall->size() > 0))
								{
									/*
									vecIO.exportGEOSPolygons2SHP("/Users/pete/Temp/Clustering/polygons/large_polys.shp", force, tmpPolysLarge, inputSpatialRef);
									vecIO.exportGEOSPolygons2SHP("/Users/pete/Temp/Clustering/polygons/small_polys.shp", force, tmpPolysSmall, inputSpatialRef);
									
									cout << "mergeSmall\n";
									cout << "Number Large Polygons = " << tmpPolysLarge->size() << endl;
									cout << "Number Small Polygons = " << tmpPolysSmall->size() << endl;
									 */
									change2 = true;
									while(change2)
									{
										change2 = false;
										//cout << "iterate \n";
										for(iterPolysSmall = tmpPolysSmall->begin(); iterPolysSmall != tmpPolysSmall->end(); )
										{
											//cout << "iterate2\n";
											polyTmp1 = *iterPolysSmall;
											
											relBorder = 0;
											maxRelBorder = 0;
											maxRelBorderPoly = NULL;
											first = true;											
											
											for(iterPolys = tmpPolysLarge->begin(); iterPolys != tmpPolysLarge->end(); ++iterPolys)
											{
												polyTmp2 = *iterPolys;
												relBorder = geomUtils.calcRelativeBorder(polyTmp1, polyTmp2);
												//cout << "Rel Border = " << relBorder << endl;
												if(first)
												{
													maxRelBorder = relBorder;
													maxRelBorderPoly = polyTmp2;
													first = false;
												}
												else if(relBorder > maxRelBorder)
												{
													maxRelBorder = relBorder;
													maxRelBorderPoly = polyTmp2;
												}
											}
											
											//cout << "maxRelBorder = " << maxRelBorder << endl;
											
											if(maxRelBorder > 0)
											{
												polyUnion = geomUtils.polygonUnion(polyTmp1, maxRelBorderPoly);
												polyUnionBuffer = dynamic_cast<Polygon*>(polyUnion->buffer(0));
												delete polyUnion;
												tmpPolysLarge->erase(remove(tmpPolysLarge->begin(), tmpPolysLarge->end(), maxRelBorderPoly));
												tmpPolysLarge->push_back(polyUnionBuffer);
												delete maxRelBorderPoly;
												delete *iterPolysSmall;
												tmpPolysSmall->erase(iterPolysSmall);
												change2 = true;
											}
											else
											{
												//cout << "++iterPolysSmall\n";
												++iterPolysSmall;
											}
										}
									}
									
									iterPolys1 = largePolys->begin();
								}
								
								for(iterPolys = tmpPolysLarge->begin(); iterPolys != tmpPolysLarge->end(); )
								{
									largePolys->push_back(*iterPolys);
									tmpPolysLarge->erase(iterPolys);
								}
								
								//string filename = string("/Users/pete/Temp/Clustering/polygons/polys_") + mathsUtils.inttostring(count++) + string(".shp");
								//vecIO.exportGEOSPolygons2SHP(filename, force, polys, inputSpatialRef);
								
								geom1 = NULL;
								geom2 = NULL;
								intersect = NULL;
								iterPolys1 = largePolys->begin();
							}
							
							change = true;
						}
						else if(poly1withinpoly2)
						{
							//cout << "Polygon 2 within polygon 1\n";
							largePolys->erase(remove(largePolys->begin(), largePolys->end(), poly1));
							delete poly1;
							poly2 = NULL;
							iterPolys1 = largePolys->begin();
							
							change = true;
						}
						else if(poly2withinpoly1)
						{
							//cout << "Polygon 2 within polygon 1\n";
							largePolys->erase(remove(largePolys->begin(), largePolys->end(), poly2));
							poly1 = NULL;
							delete poly2;
							iterPolys1 = largePolys->begin();
							
							change = true;
						}
						else
						{
							//cout << "No overlap\n";
							++iterPolys1;
						}
					}					
				}
								
				tmpPolysLarge->clear();
				delete tmpPolysLarge;
				
				tmpPolysSmall->clear();
				delete tmpPolysSmall;
				
				tmpPolys1->clear();
				delete tmpPolys1;
				
				tmpPolys2->clear();
				delete tmpPolys2;
				
				tmpPolysIntersect->clear();
				delete tmpPolysIntersect;
				
				MultiPolygon *mPolyLarge = vecUtils.convertGEOSPolygons2GEOSMultiPolygon(largePolys);
				
				intersectArea = 0;
				float mPolyLargeIntersectArRatio = 0;
				float maxIntersectArea = 0;
				
				//vector<Polygon*>::iterator iterSmallPolys;
				//vector<Polygon*>::iterator iterLargePolys;
				
				intersect = NULL;
				
				Polygon *smallPoly = NULL;
				Polygon *maxIntersect = NULL;
				Polygon *unionPoly = NULL;
				
				int numFeatures = smallPolys->size();
				int feedback = numFeatures/10;
				int i = 0;
				cout << "Started" << endl;
					
				while(change)
				{
					change = false;
					for(iterSmallPolys = smallPolys->begin(); iterSmallPolys != smallPolys->end(); )
					{
						if(numFeatures > 10)
						{
							if((i % feedback) == 0)
							{
								cout  <<  "." << flush;
							}
						}	
						
						smallPoly = *iterSmallPolys;
						if(smallPoly == NULL)
						{
							smallPolys->erase(iterSmallPolys);
						}
						else if(smallPoly->within(mPolyLarge))
						{
							delete *iterSmallPolys;
							smallPolys->erase(iterSmallPolys);
						}
						else if(smallPoly->intersects(mPolyLarge))
						{
							intersect = geomUtils.getIntersection(((Geometry*)smallPoly), ((Geometry*)mPolyLarge));
							intersectArea = intersect->getArea();
							delete intersect;
							mPolyLargeIntersectArRatio = intersectArea/smallPoly->getArea();
							
							if(mPolyLargeIntersectArRatio == 1)
							{
								delete *iterSmallPolys;
								smallPolys->erase(iterSmallPolys);
							}
							else if(mPolyLargeIntersectArRatio > this->overlapThreshold)
							{
								first = true;
								for(iterLargePolys = largePolys->begin(); iterLargePolys != largePolys->end(); ++iterLargePolys)
								{
									intersect = geomUtils.getIntersection(((Geometry*)smallPoly), ((Geometry*)*iterLargePolys));
									intersectArea = intersect->getArea();
									if(first)
									{
										maxIntersectArea = intersectArea;
										maxIntersect = *iterLargePolys;
										first = false;
									}
									else if(intersectArea > maxIntersectArea)
									{
										maxIntersectArea = intersectArea;
										maxIntersect = *iterLargePolys;
									}
									delete intersect;
								}
								
								largePolys->erase(remove(largePolys->begin(), largePolys->end(), maxIntersect));
								unionPoly = geomUtils.polygonUnion(maxIntersect, smallPoly);
								delete maxIntersect;
								largePolys->push_back(unionPoly);
								
								delete *iterSmallPolys;
								smallPolys->erase(iterSmallPolys);
								change = true;
							}
							else
							{
								++iterSmallPolys;
							}
						}
						else
						{
							++iterSmallPolys;
						}
						++i;
					}
				}
				
				cout << " Complete.\n";
								
				cout << "Output Polygons:\n";
				vecIO.exportGEOSPolygons2SHP(this->outputSmallVector, force, smallPolys, inputSpatialRef);
				vecIO.exportGEOSPolygons2SHP(this->outputLargeVector, force, largePolys, inputSpatialRef);
				
				delete mPolyLarge;
				
				for(iterLargePolys = largePolys->begin(); iterLargePolys != largePolys->end(); )
				{
					delete *iterLargePolys;
					largePolys->erase(iterLargePolys);
				}
				delete largePolys;
				
				for(iterSmallPolys = smallPolys->begin(); iterSmallPolys != smallPolys->end(); )
				{
					delete *iterSmallPolys;
					smallPolys->erase(iterSmallPolys);
				}
				delete smallPolys;
				
				OGRDataSource::DestroyDataSource(inputSHPDSLarge);
				OGRDataSource::DestroyDataSource(inputSHPDSSmall);
				
				//OGRCleanupAll();
			}
			catch(RSGISException &e)
			{
				delete RSGISGEOSFactoryGenerator::getInstance();
				//OGRCleanupAll();
				throw e;
			}
		}
		else if(option == RSGISExePostClassification::clusterremainingsmall)
		{
			cout << "Cluster remaining smaller polygons without crossing any existing large polygons\n";
			cout << "Input Shapefile (Large Polygons) = " << this->inputVectorLarge << endl;
			cout << "Input Shapefile (Small Polygons) = " << this->inputVectorSmall << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;
			cout << "Max Length = " << this->maxlength << endl;
			
			if(polygonizertype == lineproj)
			{
				cout << "The line projection polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
			}
			else if(polygonizertype == delaunay1)
			{
				cout << "The delaunay1 polygonizer will be used\n";
			}
			else if(polygonizertype == convexhull)
			{
				cout << "The convex hull polygonizer will be used\n";
			}
			else if(polygonizertype == snakes)
			{
				cout << "The snakes polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
				cout << "Alpha = " << this->alpha << endl;
				cout << "Beta = " << this->beta << endl;
				cout << "Gamma = " << this->gamma << endl;
				cout << "Delta = " << this->delta << endl;
			}
			
			OGRRegisterAll();
			
			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;
			RSGISGeometry geomUtils;
			RSGISMathsUtils mathsUtils;
			
			string SHPFileInLayerLarge = vecUtils.getLayerName(this->inputVectorLarge);
			string SHPFileInLayerSmall = vecUtils.getLayerName(this->inputVectorSmall);
			
			OGRDataSource *inputSHPDSLarge = NULL;
			OGRLayer *inputSHPLayerLarge = NULL;
			OGRDataSource *inputSHPDSSmall = NULL;
			OGRLayer *inputSHPLayerSmall = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;
			
			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;	
			
			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDSLarge = OGRSFDriverRegistrar::Open(this->inputVectorLarge.c_str(), FALSE);
				if(inputSHPDSLarge == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVectorLarge;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayerLarge = inputSHPDSLarge->GetLayerByName(SHPFileInLayerLarge.c_str());
				if(inputSHPLayerLarge == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayerLarge;
					throw RSGISFileException(message.c_str());
				}
				
				/////////////////////////////////////
				//
				// Open Input Shapfile. -- SMALL
				//
				/////////////////////////////////////
				inputSHPDSSmall = OGRSFDriverRegistrar::Open(this->inputVectorSmall.c_str(), FALSE);
				if(inputSHPDSSmall == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVectorSmall;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayerSmall = inputSHPDSSmall->GetLayerByName(SHPFileInLayerSmall.c_str());
				if(inputSHPLayerSmall == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayerSmall;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayerSmall->GetSpatialRef();	
				
				vector<Polygon*> *largePolys = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(largePolys);
				processVector = new RSGISProcessVector(processFeature);
				
				cout << "Read inputted large polygons\n";
				processVector->processVectorsNoOutput(inputSHPLayerLarge, false);
				
				delete processVector;
				delete processFeature;
				
				vector<Polygon*> *smallPolys = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(smallPolys);
				processVector = new RSGISProcessVector(processFeature);
				
				cout << "Read inputted small polygons\n";
				processVector->processVectorsNoOutput(inputSHPLayerSmall, false);
				
				delete processVector;
				delete processFeature;
				
				cout << "There are " << largePolys->size() << " large polygons\n";
				cout << "There are " << smallPolys->size() << " small polygons\n";
				
				
				//polygonizertype = lineproj;
				//resolution = 0.5;
				//float maxLength = 150;
				
				
				vector<Polygon*>::iterator iterLargePolys;
				vector<Polygon*>::iterator iterSmallPolys;
				vector<Polygon*>::iterator iterSmallPolys2;
								
				// Implement clusterer... 
				MultiPolygon *mPolyLarge = vecUtils.convertGEOSPolygons2GEOSMultiPolygon(largePolys);
				
				Polygon *smallPoly = NULL;
				Polygon *newPoly = NULL;
				vector<Polygon*> *toMerge = new vector<Polygon*>();
				
				bool change = true;
				
				int i = 0;
				
				cout << "Started (Merge fast method) ." << flush;
				
				while(change)
				{
					change = false;
										
					for(iterSmallPolys = smallPolys->begin(); iterSmallPolys != smallPolys->end(); )
					{
						smallPoly = *iterSmallPolys;
						
						if( i % 10 == 0)
						{
							cout << "." << flush;
						}
												
						for(iterSmallPolys2 = smallPolys->begin(); iterSmallPolys2 != smallPolys->end(); ++iterSmallPolys2)
						{
							if((smallPoly != (*iterSmallPolys2)) && (smallPoly->distance(*iterSmallPolys2) < this->maxlength) && (!geomUtils.geometryBetweenFast(smallPoly, *iterSmallPolys2, mPolyLarge)))
							{
								toMerge->push_back(*iterSmallPolys2);
							}
						}
						
						if(toMerge->size() > 0)
						{
							toMerge->push_back(smallPoly);
							if(polygonizertype == lineproj)
							{
								RSGISIdentifyNonConvexPolygonsLineProject *identifyNonConvexLineProj = new RSGISIdentifyNonConvexPolygonsLineProject(resolution);
								newPoly = identifyNonConvexLineProj->retrievePolygon(toMerge);
								delete identifyNonConvexLineProj;
							}
							else if(polygonizertype == delaunay1)
							{
								RSGISIdentifyNonConvexPolygonsDelaunay *identifyNonConvexDelaunay = new RSGISIdentifyNonConvexPolygonsDelaunay();
								newPoly = identifyNonConvexDelaunay->retrievePolygon(toMerge);
								delete identifyNonConvexDelaunay;
							}
							else if(polygonizertype == convexhull)
							{
								newPoly = geomUtils.findConvexHull(toMerge);
							}
							else if(polygonizertype == snakes)
							{
								RSGISIdentifyNonConvexPolygonsSnakes *identifyNonConvexSnakes = new RSGISIdentifyNonConvexPolygonsSnakes(resolution, inputSpatialRef, alpha, beta, gamma, delta, maxNumIterations);
								newPoly = identifyNonConvexSnakes->retrievePolygon(toMerge);
								delete identifyNonConvexSnakes;
							}
							else
							{
								throw RSGISException("Do not know polygonization option.");
							}
							
							for(iterSmallPolys2 = toMerge->begin(); iterSmallPolys2 != toMerge->end(); )
							{
								smallPolys->erase(remove(smallPolys->begin(), smallPolys->end(), *iterSmallPolys2));
								delete *iterSmallPolys2;
								toMerge->erase(iterSmallPolys2);
							}
							toMerge->clear();
							smallPolys->push_back(newPoly);
							change = true;
							iterSmallPolys = smallPolys->begin();
						}
						else
						{
							++iterSmallPolys;
						}
						++i;
					}
					
				}
				cout << ". Complete\n" << flush;
				
				geomUtils.performMorphologicalOperation(smallPolys, closing, 5, 30);
				
				geomUtils.mergeTouchingPolygons(smallPolys);
				
				cout << "Output polygons to shapefile:\n";
				vecIO.exportGEOSPolygons2SHP(this->outputVector, force, smallPolys, inputSpatialRef);
				
				for(iterLargePolys = largePolys->begin(); iterLargePolys != largePolys->end(); )
				{
					delete *iterLargePolys;
					largePolys->erase(iterLargePolys);
				}
				delete largePolys;
				
				delete mPolyLarge;
				
				for(iterSmallPolys = smallPolys->begin(); iterSmallPolys != smallPolys->end(); )
				{
					delete *iterSmallPolys;
					smallPolys->erase(iterSmallPolys);
				}
				delete smallPolys;
				
				
				OGRDataSource::DestroyDataSource(inputSHPDSLarge);
				OGRDataSource::DestroyDataSource(inputSHPDSSmall);
				
				//OGRCleanupAll();
			}
			catch(RSGISException &e)
			{
				delete RSGISGEOSFactoryGenerator::getInstance();
				//OGRCleanupAll();
				throw e;
			}
		}
		else if(option == RSGISExePostClassification::mergesmall2nearestlarge)
		{
			cout << "Merge all the small polygons to their nearest large polygon.\n";
			cout << "Input Shapefile (Large Polygons) = " << this->inputVectorLarge << endl;
			cout << "Input Shapefile (Small Polygons) = " << this->inputVectorSmall << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;

			if(polygonizertype == lineproj)
			{
				cout << "The line projection polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
			}
			else if(polygonizertype == delaunay1)
			{
				cout << "The delaunay1 polygonizer will be used\n";
			}
			else if(polygonizertype == convexhull)
			{
				cout << "The convex hull polygonizer will be used\n";
			}
			else if(polygonizertype == snakes)
			{
				cout << "The snakes polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
				cout << "Alpha = " << this->alpha << endl;
				cout << "Beta = " << this->beta << endl;
				cout << "Gamma = " << this->gamma << endl;
				cout << "Delta = " << this->delta << endl;
			}
			
			OGRRegisterAll();
			
			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;
			RSGISGeometry geomUtils;
			RSGISMathsUtils mathsUtils;
			
			string SHPFileInLayerLarge = vecUtils.getLayerName(this->inputVectorLarge);
			string SHPFileInLayerSmall = vecUtils.getLayerName(this->inputVectorSmall);
			
			OGRDataSource *inputSHPDSLarge = NULL;
			OGRLayer *inputSHPLayerLarge = NULL;
			OGRDataSource *inputSHPDSSmall = NULL;
			OGRLayer *inputSHPLayerSmall = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;
			
			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;	
			
			try
			{
				/////////////////////////////////////
				//
				// Open Input Shapfile.
				//
				/////////////////////////////////////
				inputSHPDSLarge = OGRSFDriverRegistrar::Open(this->inputVectorLarge.c_str(), FALSE);
				if(inputSHPDSLarge == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVectorLarge;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayerLarge = inputSHPDSLarge->GetLayerByName(SHPFileInLayerLarge.c_str());
				if(inputSHPLayerLarge == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayerLarge;
					throw RSGISFileException(message.c_str());
				}
				
				/////////////////////////////////////
				//
				// Open Input Shapfile. -- SMALL
				//
				/////////////////////////////////////
				inputSHPDSSmall = OGRSFDriverRegistrar::Open(this->inputVectorSmall.c_str(), FALSE);
				if(inputSHPDSSmall == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVectorSmall;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayerSmall = inputSHPDSSmall->GetLayerByName(SHPFileInLayerSmall.c_str());
				if(inputSHPLayerSmall == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayerSmall;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayerSmall->GetSpatialRef();	
				
				vector<Polygon*> *largePolys = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(largePolys);
				processVector = new RSGISProcessVector(processFeature);
				
				cout << "Read inputted large polygons\n";
				processVector->processVectorsNoOutput(inputSHPLayerLarge, false);
				
				delete processVector;
				delete processFeature;
				
				vector<Polygon*> *smallPolys = new vector<Polygon*>();
				processFeature = new RSGISGEOSPolygonReader(smallPolys);
				processVector = new RSGISProcessVector(processFeature);
				
				cout << "Read inputted small polygons\n";
				processVector->processVectorsNoOutput(inputSHPLayerSmall, false);
				
				delete processVector;
				delete processFeature;
				
				cout << "There are " << largePolys->size() << " large polygons\n";
				cout << "There are " << smallPolys->size() << " small polygons\n";
				
				
				//polygonizertype = lineproj;
				//resolution = 0.5;				
				
				vector<Polygon*>::iterator iterLargePolys;
				vector<Polygon*>::iterator iterSmallPolys;
				
				Polygon *smallPoly = NULL;
				
				vector<Polygon*> *newLargePolys = new vector<Polygon*>();
				
				//int index = 0;
				int minIndex = 0;
				
				float minDistance = 0;
				float distance = 0;
				Polygon *tmpLargePoly = NULL;
				
				bool first = true;
				bool intersected = false;
				
				if(smallPolys->size() > 0)
				{
					vector<Polygon*> **toMerge = new vector<Polygon*>*[largePolys->size()];
					for(unsigned int i = 0; i < largePolys->size(); ++i)
					{
						toMerge[i] =  new vector<Polygon*>();
						toMerge[i]->push_back(largePolys->at(i));
					}
					
					for(iterSmallPolys = smallPolys->begin(); iterSmallPolys != smallPolys->end(); ++iterSmallPolys)
					{
						smallPoly = *iterSmallPolys;
						
						intersected = false;
						first = true;
						for(unsigned int i = 0; i < largePolys->size(); ++i)
						{
							tmpLargePoly = toMerge[i]->at(0);
							distance = smallPoly->distance(tmpLargePoly);
							
							if(smallPoly->intersects(tmpLargePoly))
							{
								toMerge[i]->push_back(smallPoly);
								intersected = true;
								break;
							}
							else
							{
								if(first)
								{
									minDistance = distance;
									minIndex = i;
									first = false;
								}
								else if(distance < minDistance)
								{
									minDistance = distance;
									minIndex = i;
								}
							}							
						}
						
						if(!intersected)
						{
							toMerge[minIndex]->push_back(smallPoly);
						}
					}
					
					
					for(unsigned int i = 0; i < largePolys->size(); ++i)
					{
						cout << "Processing Large Polygon " << i;
						if(toMerge[i]->size() > 1)
						{
							cout << " is to be merged with " << toMerge[i]->size()-1 << " polygons\n";
							if(polygonizertype == lineproj)
							{
								RSGISIdentifyNonConvexPolygonsLineProject *identifyNonConvexLineProj = new RSGISIdentifyNonConvexPolygonsLineProject(resolution);
								newLargePolys->push_back(identifyNonConvexLineProj->retrievePolygon(toMerge[i]));
								delete identifyNonConvexLineProj;
							}
							else if(polygonizertype == delaunay1)
							{
								RSGISIdentifyNonConvexPolygonsDelaunay *identifyNonConvexDelaunay = new RSGISIdentifyNonConvexPolygonsDelaunay();
								newLargePolys->push_back(identifyNonConvexDelaunay->retrievePolygon(toMerge[i]));
								delete identifyNonConvexDelaunay;
							}
							else if(polygonizertype == convexhull)
							{
								newLargePolys->push_back(geomUtils.findConvexHull(toMerge[i]));
							}
							else if(polygonizertype == snakes)
							{
								RSGISIdentifyNonConvexPolygonsSnakes *identifyNonConvexSnakes = new RSGISIdentifyNonConvexPolygonsSnakes(resolution, inputSpatialRef, alpha, beta, gamma, delta, maxNumIterations);
								newLargePolys->push_back(identifyNonConvexSnakes->retrievePolygon(toMerge[i]));
								delete identifyNonConvexSnakes;
							}
							else
							{
								throw RSGISException("Do not know polygonization option.");
							}
							
						}
						else
						{
							cout << " is not to be merged\n";
							newLargePolys->push_back(toMerge[i]->at(0));
						}
					}
					
					for(unsigned int i = 0; i < largePolys->size(); ++i)
					{
						toMerge[i]->clear();
						delete toMerge[i];
					}
					delete[] toMerge;
					
					//geomUtils.performMorphologicalOperation(newLargePolys, closing, 2, 6);
					
					cout << "Output polygons to shapefile:\n";
					vecIO.exportGEOSPolygons2SHP(this->outputVector, force, newLargePolys, inputSpatialRef);
					
					for(iterLargePolys = newLargePolys->begin(); iterLargePolys != newLargePolys->end(); )
					{
						delete *iterLargePolys;
						newLargePolys->erase(iterLargePolys);
					}
					delete newLargePolys;
				}
				else
				{
					cout << "Output polygons to shapefile:\n";
					vecIO.exportGEOSPolygons2SHP(this->outputVector, force, largePolys, inputSpatialRef);
				}
												
				for(iterLargePolys = largePolys->begin(); iterLargePolys != largePolys->end(); )
				{
					//delete *iterLargePolys;
					largePolys->erase(iterLargePolys);
				}
				delete largePolys;

								
				
				for(iterSmallPolys = smallPolys->begin(); iterSmallPolys != smallPolys->end(); )
				{
					delete *iterSmallPolys;
					smallPolys->erase(iterSmallPolys);
				}
				delete smallPolys;
				
				OGRDataSource::DestroyDataSource(inputSHPDSLarge);
				OGRDataSource::DestroyDataSource(inputSHPDSSmall);
				
				//OGRCleanupAll();
			}
			catch(RSGISException &e)
			{
				delete RSGISGEOSFactoryGenerator::getInstance();
				//OGRCleanupAll();
				throw e;
			}
		}		
		else if(option == RSGISExePostClassification::mergepolygons)
		{
			cout << "Merge the polygons outputted from the cluster process to produce a single output.\n";
			cout << "Input Cluster Polygons Shapefile = " << this->inputVector_clusters << endl;
			cout << "Input Cluster Individual Polygons Shapefile = " << this->inputVector_polys << endl;
			cout << "Input Original Polygons Shapefile = " << this->inputVector_orig << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;
			
			OGRRegisterAll();
			
			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			RSGISVectorIO vecIO;
			
			string SHPFileInLayer_Clusters = vecUtils.getLayerName(this->inputVector_clusters);
			string SHPFileInLayer_Polys = vecUtils.getLayerName(this->inputVector_polys);
			string SHPFileInLayer_Orig = vecUtils.getLayerName(this->inputVector_orig);
			
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);
			
			OGRDataSource *inputSHPDS_Clusters = NULL;
			OGRLayer *inputSHPLayer_Clusters = NULL;
			OGRDataSource *inputSHPDS_Poly = NULL;
			OGRLayer *inputSHPLayer_Poly = NULL;
			OGRDataSource *inputSHPDS_Orig = NULL;
			OGRLayer *inputSHPLayer_Orig = NULL;
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			
			OGRSpatialReference* inputSpatialRef = NULL;
			
			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);
				
				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
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
				// Open Input Shapfile. - Clusters
				//
				/////////////////////////////////////
				inputSHPDS_Clusters = OGRSFDriverRegistrar::Open(this->inputVector_clusters.c_str(), FALSE);
				if(inputSHPDS_Clusters == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector_clusters;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer_Clusters = inputSHPDS_Clusters->GetLayerByName(SHPFileInLayer_Clusters.c_str());
				if(inputSHPLayer_Clusters == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer_Clusters;
					throw RSGISFileException(message.c_str());
				}
				inputSpatialRef = inputSHPLayer_Clusters->GetSpatialRef();	
				
				// Get Geometry Type.
				OGRFeature *feature = inputSHPLayer_Clusters->GetFeature(1);
				OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
				OGRFeature::DestroyFeature(feature);
				
				/////////////////////////////////////////
				//
				// Open Input Shapfile. - Multi-Polygons
				//
				/////////////////////////////////////////
				inputSHPDS_Poly = OGRSFDriverRegistrar::Open(this->inputVector_polys.c_str(), FALSE);
				if(inputSHPDS_Poly == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector_polys;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer_Poly = inputSHPDS_Poly->GetLayerByName(SHPFileInLayer_Polys.c_str());
				if(inputSHPLayer_Poly == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer_Polys;
					throw RSGISFileException(message.c_str());
				}
				
				///////////////////////////////////////////////////
				//
				// Open Input Shapfile. - Original Class Polygons
				//
				///////////////////////////////////////////////////
				inputSHPDS_Orig = OGRSFDriverRegistrar::Open(this->inputVector_orig.c_str(), FALSE);
				if(inputSHPDS_Orig == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector_orig;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer_Orig = inputSHPDS_Orig->GetLayerByName(SHPFileInLayer_Orig.c_str());
				if(inputSHPLayer_Orig == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer_Orig;
					throw RSGISFileException(message.c_str());
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
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->CreateDataSource(this->outputVector.c_str(), NULL);
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message);
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, geometryType, NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}
				
				// Reading input cluster polygons.
				
				int numFeatures = inputSHPLayer_Clusters->GetFeatureCount(true);
				RSGISPolygonData **dataClusters = new RSGISPolygonData*[numFeatures];
				for(int i = 0; i < numFeatures; ++i)
				{
					dataClusters[i] = new RSGISClusteredClassificationPolygon();
				}
				
				cout << "Read input cluster polygons\n";
				vecIO.readPolygons(inputSHPLayer_Clusters, dataClusters, numFeatures);
				
				// Reading input multi-polygon clusters
				
				list<RSGIS2DPoint*> *clusterPolygons = new list<RSGIS2DPoint*>();
				RSGISIntValuePolygonReader *processFeatureIntValueReader = new RSGISIntValuePolygonReader(intValueAtt, clusterPolygons);
				RSGISProcessVector *processVector = new RSGISProcessVector(processFeatureIntValueReader);
				
				cout << "Read input individual cluster polygons\n";
				processVector->processVectorsNoOutput(inputSHPLayer_Poly, false);
				
				delete processVector;
				delete processFeatureIntValueReader;
				
				// Reading original cluster polygons.
				
				list<RSGIS2DPoint*> *dataOrigPolys = new list<RSGIS2DPoint*>();
				RSGISClassificationPolygonReader *processFeatureClassPolyReader = new RSGISClassificationPolygonReader(this->classStr, dataOrigPolys);
				processVector = new RSGISProcessVector(processFeatureClassPolyReader);
				
				cout << "Read input original polygons\n";
				processVector->processVectorsNoOutput(inputSHPLayer_Orig, false);
				
				delete processVector;
				delete processFeatureClassPolyReader;
				
				
				vector<RSGISClusterData*> *clusters = new vector<RSGISClusterData*>();
				this->buildClusterDataStructure(clusters, dataClusters, numFeatures, clusterPolygons, dataOrigPolys);
				
				vector<RSGISClusterData*>::iterator iterClusterData;
				RSGISClusterData *cluster = NULL;
				int count = 0;
				for(iterClusterData = clusters->begin(); iterClusterData != clusters->end(); ++iterClusterData)
				{
					cluster = *iterClusterData;
					cout << count << ") Cluster - " << cluster->getArea() << " sqm and has " << cluster->getAllIntersect()->size() << " intersecting crowns with " << cluster->getClusterPolys()->size() << " crowns creating the cluster\n";
					++count;
				}
				
				
				
				/*string polygonName = "_Cluster.shp";
				string clusterPolysName = "_ClusterPolys.shp";
				string intersectingPolysName = "_IntersectPolys.shp";
				string outputDIR = "/Users/pete/Temp/Clustering/test/output_dir/";
				
				string polyOutputPath = "";
				string clusterPolysOutPath = "";
				string intersectPolysOutPath = "";
				
				vector<RSGISClassificationPolygon*>::iterator iterClassPolys;
				vector<RSGISClassificationPolygon*> *intersectPolys = NULL;
				vector<RSGISClassificationPolygon*> *clusterPolys = NULL;
				vector<Polygon*> *outputPolys = new vector<Polygon*>();
				RSGISVectorIO vecIO;
				
				for(unsigned int i = 0; i < clusters->size(); ++i)
				{
					cluster = clusters->at(i);
					
					polyOutputPath = outputDIR + mathsUtils.inttostring(i) + polygonName;
					clusterPolysOutPath = outputDIR + mathsUtils.inttostring(i) + clusterPolysName;
					intersectPolysOutPath = outputDIR + mathsUtils.inttostring(i) + intersectingPolysName;
					
					outputPolys->push_back(cluster->getPolygon());
					vecIO.exportGEOSPolygons2SHP(polyOutputPath, true, outputPolys, inputSpatialRef);
					outputPolys->clear();
					
					clusterPolys = cluster->getClusterPolys();
					for(iterClassPolys = clusterPolys->begin(); iterClassPolys != clusterPolys->end(); ++iterClassPolys)
					{
						outputPolys->push_back((*iterClassPolys)->getPolygon());
					}
					vecIO.exportGEOSPolygons2SHP(clusterPolysOutPath, true, outputPolys, inputSpatialRef);
					outputPolys->clear();
					
					intersectPolys = cluster->getAllIntersect();
					for(iterClassPolys = intersectPolys->begin(); iterClassPolys != intersectPolys->end(); ++iterClassPolys)
					{
						outputPolys->push_back((*iterClassPolys)->getPolygon());
					}
					vecIO.exportGEOSPolygons2SHP(intersectPolysOutPath, true, outputPolys, inputSpatialRef);
					outputPolys->clear();
					
				}
				 */
				
				for(int i = 0; i < numFeatures; ++i)
				{
					delete dataClusters[i];
				}
				delete[] dataClusters;
				
				list<RSGIS2DPoint*>::iterator iterInClusterPolys;
				for(iterInClusterPolys = clusterPolygons->begin(); iterInClusterPolys != clusterPolygons->end(); )//++iterOrigData)
				{
					delete ((RSGISIntValuePolygon*)(*iterInClusterPolys));
					clusterPolygons->erase(iterInClusterPolys++);
				}
				delete clusterPolygons;
				
				
				list<RSGIS2DPoint*>::iterator iterOrigData;
				for(iterOrigData = dataOrigPolys->begin(); iterOrigData != dataOrigPolys->end(); )//++iterOrigData)
				{
					delete ((RSGISClassificationPolygon*)(*iterOrigData));
					dataOrigPolys->erase(iterOrigData++);
				}
				delete dataOrigPolys;
				
				//vector<RSGISClusterData*>::iterator iterClusterData;
				for(iterClusterData = clusters->begin(); iterClusterData != clusters->end(); )
				{
					delete *iterClusterData;
					clusters->erase(iterClusterData);
				}
				delete clusters;
				
				OGRDataSource::DestroyDataSource(inputSHPDS_Clusters);
				OGRDataSource::DestroyDataSource(inputSHPDS_Poly);
				OGRDataSource::DestroyDataSource(inputSHPDS_Orig);
				OGRDataSource::DestroyDataSource(outputSHPDS);
				
				//OGRCleanupAll();
			}
			catch(RSGISException &e)
			{
				throw e;
			}
			
		}
		else if(option == RSGISExePostClassification::labelpolys)
		{
			cout << "Label polygons with summaries of small polygons.\n";
			cout << "Input Vector = " << this->inputVector << endl;
			cout << "Input Label Vector = " << this->labelsVector << endl;
			cout << "Labels Attribute = " << this->classAttribute << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;
			if(dominant == speciesCount)
			{
				cout << "Dominant class defined by object count\n";
			}
			else if(dominant == speciesArea)
			{
				cout << "Dominant class defind by object area\n";
			}
			else
			{
				cout << "Dominant class calculation is unknown - this may result in an error!!\n";
			}
			
			OGRRegisterAll();
			
			RSGISFileUtils fileUtils;
			RSGISVectorUtils vecUtils;
			
			string SHPFileInLabelLayer = vecUtils.getLayerName(this->labelsVector);
			string SHPFileInLayer = vecUtils.getLayerName(this->inputVector);
			string SHPFileOutLayer = vecUtils.getLayerName(this->outputVector);
			
			OGRDataSource *inputLabelsSHPDS = NULL;
			OGRLayer *inputLabelsSHPLayer = NULL;
			OGRDataSource *inputSHPDS = NULL;
			OGRLayer *inputSHPLayer = NULL;
			OGRSFDriver *shpFiledriver = NULL;
			OGRDataSource *outputSHPDS = NULL;
			OGRLayer *outputSHPLayer = NULL;
			OGRSpatialReference* inputSpatialRef = NULL;
			OGRFeatureDefn *inFeatureDefn = NULL;
			
			RSGISProcessVector *processVector = NULL;
			RSGISProcessOGRFeature *processFeature = NULL;
			
			string outputDIR = "";
			try
			{
				outputDIR = fileUtils.getFileDirectoryPath(this->outputVector);
				
				if(vecUtils.checkDIR4SHP(outputDIR, SHPFileOutLayer))
				{
					if(this->force)
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
				// Open Input Shapfile. (Input)
				//
				/////////////////////////////////////
				inputLabelsSHPDS = OGRSFDriverRegistrar::Open(this->labelsVector.c_str(), FALSE);
				if(inputLabelsSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->labelsVector;
					throw RSGISFileException(message.c_str());
				}
				inputLabelsSHPLayer = inputLabelsSHPDS->GetLayerByName(SHPFileInLabelLayer.c_str());
				if(inputLabelsSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLabelLayer;
					throw RSGISFileException(message.c_str());
				}

				/////////////////////////////////////
				//
				// Open Input Shapfile. (Input)
				//
				/////////////////////////////////////
				inputSHPDS = OGRSFDriverRegistrar::Open(this->inputVector.c_str(), FALSE);
				if(inputSHPDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputSHPLayer = inputSHPDS->GetLayerByName(SHPFileInLayer.c_str());
				if(inputSHPLayer == NULL)
				{
					string message = string("Could not open vector layer ") + SHPFileInLayer;
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
					throw RSGISVectorOutputException("SHP driver not available.");
				}
				outputSHPDS = shpFiledriver->CreateDataSource(this->outputVector.c_str(), NULL);
				if( outputSHPDS == NULL )
				{
					string message = string("Could not create vector file ") + this->outputVector;
					throw RSGISVectorOutputException(message.c_str());
				}
				outputSHPLayer = outputSHPDS->CreateLayer(SHPFileOutLayer.c_str(), inputSpatialRef, inFeatureDefn->GetGeomType(), NULL );
				if( outputSHPLayer == NULL )
				{
					string message = string("Could not create vector layer ") + SHPFileOutLayer;
					throw RSGISVectorOutputException(message.c_str());
				}
				
				/////////////////////////////////////
				//
				// Read data from the labels layer
				//
				/////////////////////////////////////
				
				unsigned long numFeatures = inputLabelsSHPLayer->GetFeatureCount(true);
				RSGISPolygonData **labelsData = new RSGISPolygonData*[numFeatures];
				for(unsigned long i = 0; i < numFeatures; ++i)
				{
					labelsData[i] = new RSGISClassPolygon(this->classAttribute);
				}
				RSGISVectorIO vecIO;
				vecIO.readPolygons(inputLabelsSHPLayer, labelsData, numFeatures);
				RSGISClassPolygon **labelsClassData = new RSGISClassPolygon*[numFeatures];
				cout << "Number of tree crowns = " << numFeatures << endl;
				for(unsigned long i = 0; i < numFeatures; ++i)
				{
					labelsClassData[i] = (RSGISClassPolygon*)(labelsData[i]);
				}
				delete[] labelsData;
				
				processFeature = new RSGISLabelPolygonsFromClassification(labelsClassData, numFeatures, dominant);
				processVector = new RSGISProcessVector(processFeature);
				
				processVector->processVectors(inputSHPLayer, outputSHPLayer, true, false, false);
				
				for(unsigned long i = 0; i < numFeatures; ++i)
				{
					delete labelsClassData[i];
				}
				
				delete[] labelsClassData;
				
				OGRDataSource::DestroyDataSource(inputLabelsSHPDS);
				OGRDataSource::DestroyDataSource(inputSHPDS);
				OGRDataSource::DestroyDataSource(outputSHPDS);
				
				delete processVector;
				delete processFeature;
				
				//OGRCleanupAll();
			}
			catch (RSGISException& e) 
			{
				throw e;
			}
		}
		else
		{
			cout << "Options not recognised\n";
		}
		delete RSGISGEOSFactoryGenerator::getInstance();
	}
}


void RSGISExePostClassification::printParameters()
{
	if(parsed)
	{
		if(option == RSGISExePostClassification::cluster)
		{
			cout << "Cluster all polygons but does not polygonize the output\n";
			cout << "Input Shapefile = " << this->inputVector << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;
			
			if(clusterertype == edgestddev)
			{
				cout << "The edge standard deviation cluster will be used\n";
				cout << "Std Dev Threshold = " << this->numStdDevthreshold << endl;
				cout << "Maximum Edge Length Threshold = " << this->maxEdgeLength << endl;
			}
			else if(clusterertype == edgelength)
			{
				cout << "The maximum edge length cluster will be used\n";
				cout << "Edge length threshold = " << edgelengththreshold << endl;
			}
		}
		else if(option == RSGISExePostClassification::clusterclass)
		{
			cout << "Cluster input polygons by class but does not polygonize the output\n";
			cout << "Input Shapefile = " << this->inputVector << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;
			
			if(clusterertype == edgestddev)
			{
				cout << "The edge standard deviation cluster will be used\n";
				cout << "Std Dev Threshold = " << this->numStdDevthreshold << endl;
				cout << "Maximum Edge Length Threshold = " << this->maxEdgeLength << endl;
			}
			else if(clusterertype == edgelength)
			{
				cout << "The maximum edge length cluster will be used\n";
				cout << "Edge length threshold = " << edgelengththreshold << endl;
				if(classEdgeLengths != NULL)
				{
					cout << "Per class thresholds: \n";
					vector<EdgeLengthPair*>::iterator iterEdgePairs;
					for(iterEdgePairs = classEdgeLengths->begin(); iterEdgePairs != classEdgeLengths->end(); ++iterEdgePairs)
					{
						cout << (*iterEdgePairs)->classname << " = " << (*iterEdgePairs)->lengththreshold << endl;
					}
				}
			}
		}
		else if(option == RSGISExePostClassification::clusterpolygonize)
		{
			cout << "Cluster all polygons and polygonize the output\n";
			cout << "Input Shapefile = " << this->inputVector << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;
			
			if(clusterertype == edgestddev)
			{
				cout << "The edge standard deviation cluster will be used\n";
				cout << "Std Dev Threshold = " << this->numStdDevthreshold << endl;
				cout << "Maximum Edge Length Threshold = " << this->maxEdgeLength << endl;
			}
			else if(clusterertype == edgelength)
			{
				cout << "The maximum edge length cluster will be used\n";
				cout << "Edge length threshold = " << edgelengththreshold << endl;
			}
			
			if(polygonizertype == lineproj)
			{
				cout << "The line projection polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
			}
			else if(polygonizertype == delaunay1)
			{
				cout << "The delaunay1 polygonizer will be used\n";
			}
			else if(polygonizertype == delaunay2)
			{
				cout << "The delaunay2 polygonizer will be used\n";
			}
			else if(polygonizertype == convexhull)
			{
				cout << "The convex hull polygonizer will be used\n";
			}
			else if(polygonizertype == multipoly)
			{
				cout << "The multi-polygon polygonizer will be used\n";
			}
			else if(polygonizertype == polys)
			{
				cout << "The polygons polygonizer will be used\n";
			}
			else if(polygonizertype == snakes)
			{
				cout << "The snakes polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
				cout << "Alpha = " << this->alpha << endl;
				cout << "Beta = " << this->beta << endl;
				cout << "Gamma = " << this->gamma << endl;
				cout << "Delta = " << this->delta << endl;
			}
		}
		else if(option == RSGISExePostClassification::clusterclasspolygonize)
		{
			cout << "Cluster polygons by class and polygonize the output\n";
			cout << "Input Shapefile = " << this->inputVector << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;
			
			if(clusterertype == edgestddev)
			{
				cout << "The edge standard deviation cluster will be used\n";
				cout << "Std Dev Threshold = " << this->numStdDevthreshold << endl;
				cout << "Maximum Edge Length Threshold = " << this->maxEdgeLength << endl;
			}
			else if(clusterertype == edgelength)
			{
				cout << "The maximum edge length cluster will be used\n";
				cout << "Edge length threshold = " << edgelengththreshold << endl;
				if(classEdgeLengths != NULL)
				{
					cout << "Per class thresholds: \n";
					vector<EdgeLengthPair*>::iterator iterEdgePairs;
					for(iterEdgePairs = classEdgeLengths->begin(); iterEdgePairs != classEdgeLengths->end(); ++iterEdgePairs)
					{
						cout << (*iterEdgePairs)->classname << " = " << (*iterEdgePairs)->lengththreshold << endl;
					}
				}
			}
			
			if(polygonizertype == lineproj)
			{
				cout << "The line projection polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
			}
			else if(polygonizertype == delaunay1)
			{
				cout << "The delaunay1 polygonizer will be used\n";
			}
			else if(polygonizertype == delaunay2)
			{
				cout << "The delaunay2 polygonizer will be used\n";
			}
			else if(polygonizertype == convexhull)
			{
				cout << "The convex hull polygonizer will be used\n";
			}
			else if(polygonizertype == multipoly)
			{
				cout << "The multi-polygon polygonizer will be used\n";
			}
			else if(polygonizertype == polys)
			{
				cout << "The polygons polygonizer will be used\n";
			}
			else if(polygonizertype == snakes)
			{
				cout << "The snakes polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
				cout << "Alpha = " << this->alpha << endl;
				cout << "Beta = " << this->beta << endl;
				cout << "Gamma = " << this->gamma << endl;
				cout << "Delta = " << this->delta << endl;
			}
		}
		else if(option == RSGISExePostClassification::splitlargesmall)
		{
			cout << "Split the large and small polygons\n";
			cout << "Input Shapefile = " << this->inputVector << endl;
			cout << "Output Large Shapefile = " << this->outputLargeVector << endl;
			cout << "Output Small Shapefile = " << this->outputSmallVector << endl;
			cout << "Area Threshold 1 = " << this->largeThreshold1 << endl;
			cout << "Proportional Polygon Area Threshold 1 = " << this->propPolyArea1 << endl;
			cout << "Overlap Threshold = " << this->overlapThreshold << endl;
			cout << "Area Threshold 2 = " << this->largeThreshold2 << endl;
			cout << "Proportional Polygon Area Threshold 2 = " << this->propPolyArea2 << endl;
		}
		else if(option == RSGISExePostClassification::mergelargepolygons)
		{
			cout << "Merge the large polygons and remove overlaps\n";
			cout << "Input Shapefile = " << this->inputVector << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;
			cout << "Large Polygon Threshold = " << this->largeThreshold << endl;
			cout << "Intersect Ratio Threshold = " << this->intersectRatio << endl;
		}
		else if(option == RSGISExePostClassification::morphology)
		{
			cout << "Undertake morphology on the input polygons\n";
			cout << "Input Shapefile = " << this->inputVector << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;
			cout << "Buffer = " << this->buffer << endl;
			if(morphologyType == closing)
			{
				cout << "Undertaking a morphological closing\n";
			}
			else if(morphologyType == opening)
			{
				cout << "Undertaking a morphological opening\n";
			}
			else if(morphologyType == dilation)
			{
				cout << "Undertaking a morphological dilation\n";
			}
			else if(morphologyType == erosion)
			{
				cout << "Undertaking a morphological erosion\n";
			}
			else
			{
				cout << "ERROR: Unknown morphological operation\n";
			}
		}
		else if(option == RSGISExePostClassification::removesmallwithinlarge)
		{
			cout << "Remove any small polygons that are contained within a large polygon.\n";
			cout << "Input Shapefile (Large Polygons) = " << this->inputVectorLarge << endl;
			cout << "Input Shapefile (Small Polygons) = " << this->inputVectorSmall << endl;
			cout << "Output Large Shapefile = " << this->outputLargeVector << endl;
			cout << "Output Small Shapefile = " << this->outputSmallVector << endl;
			cout << "Overlap Threshold = " << this->overlapThreshold << endl;
		}
		else if(option == RSGISExePostClassification::mergesmallandlarge)
		{
			cout << "Merge the small and large polygons together where possible.\n";
			cout << "Input Shapefile (Large Polygons) = " << this->inputVectorLarge << endl;
			cout << "Input Shapefile (Small Polygons) = " << this->inputVectorSmall << endl;
			cout << "Output Large Shapefile = " << this->outputLargeVector << endl;
			cout << "Output Small Shapefile = " << this->outputSmallVector << endl;
			cout << "Large Polygon Threshold = " << this->largeThreshold << endl;
			cout << "Intersect Ratio Threshold = " << this->intersectRatio << endl;
			cout << "Overlap Threshold = " << this->overlapThreshold << endl;
		}
		else if(option == RSGISExePostClassification::clusterremainingsmall)
		{
			cout << "Cluster remaining smaller polygons without crossing any existing large polygons\n";
			cout << "Input Shapefile (Large Polygons) = " << this->inputVectorLarge << endl;
			cout << "Input Shapefile (Small Polygons) = " << this->inputVectorSmall << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;
			cout << "Max Length = " << this->maxlength << endl;
			
			if(polygonizertype == lineproj)
			{
				cout << "The line projection polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
			}
			else if(polygonizertype == delaunay1)
			{
				cout << "The delaunay1 polygonizer will be used\n";
			}
			else if(polygonizertype == convexhull)
			{
				cout << "The convex hull polygonizer will be used\n";
			}
			else if(polygonizertype == snakes)
			{
				cout << "The snakes polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
				cout << "Alpha = " << this->alpha << endl;
				cout << "Beta = " << this->beta << endl;
				cout << "Gamma = " << this->gamma << endl;
				cout << "Delta = " << this->delta << endl;
			}
		}
		else if(option == RSGISExePostClassification::mergesmall2nearestlarge)
		{
			cout << "Merge all the small polygons to their nearest large polygon.\n";
			cout << "Input Shapefile (Large Polygons) = " << this->inputVectorLarge << endl;
			cout << "Input Shapefile (Small Polygons) = " << this->inputVectorSmall << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;
			
			if(polygonizertype == lineproj)
			{
				cout << "The line projection polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
			}
			else if(polygonizertype == delaunay1)
			{
				cout << "The delaunay1 polygonizer will be used\n";
			}
			else if(polygonizertype == convexhull)
			{
				cout << "The convex hull polygonizer will be used\n";
			}
			else if(polygonizertype == snakes)
			{
				cout << "The snakes polygonizer will be used\n";
				cout << "Resolution = " << this->resolution << endl;
				cout << "Alpha = " << this->alpha << endl;
				cout << "Beta = " << this->beta << endl;
				cout << "Gamma = " << this->gamma << endl;
				cout << "Delta = " << this->delta << endl;
			}
		}
		else if(option == RSGISExePostClassification::mergepolygons)
		{
			cout << "Merge the polygons outputted from the cluster process to produce a single output.\n";
			cout << "Input Cluster Polygons Shapefile = " << this->inputVector_clusters << endl;
			cout << "Input Cluster Individual Polygons Shapefile = " << this->inputVector_polys << endl;
			cout << "Input Original Polygons Shapefile = " << this->inputVector_orig << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;
		}
		else if(option == RSGISExePostClassification::labelpolys)
		{
			cout << "Label polygons with summaries of small polygons.\n";
			cout << "Input Vector = " << this->inputVector << endl;
			cout << "Input Label Vector = " << this->labelsVector << endl;
			cout << "Labels Attribute = " << this->classAttribute << endl;
			cout << "Output Shapefile = " << this->outputVector << endl;
			if(dominant == speciesCount)
			{
				cout << "Dominant class defined by object count\n";
			}
			else if(dominant == speciesArea)
			{
				cout << "Dominant class defind by object area\n";
			}
			else
			{
				cout << "Dominant class calculation is unknown - this may result in an error!!\n";
			}
		}
		else
		{
			cout << "Options not recognised\n";
		}
	}
	else
	{
		cout << "The parameters have yet to be parsed\n";
	}
}

void RSGISExePostClassification::help()
{
	cout << "<rsgis:commands>\n";
	cout << "\t<rsgis:command algor=\"postclassification\" option=\"cluster\" input=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\">\n";
	cout << "\t\t<rsgis:clusterer algor=\"edgestddev | edgelength\" length=\"float\" stddev=\"float\" maxedgelength=\"float\" >\n";
	cout << "\t\t\t<rsgis:edgelength classname=\"string\" length=\"float\" />\n";
	cout << "\t\t</rsgis:clusterer>\n";
	cout << "\t</rsgis:command>\n";
	cout << "\t<rsgis:command algor=\"postclassification\" option=\"clusterclass\" input=\"vector.shp\" class=\"string\" output=\"vector_out.shp\" force=\"yes | no\">\n";
	cout << "\t\t<rsgis:clusterer algor=\"edgestddev | edgelength\" length=\"float\" stddev=\"float\" maxedgelength=\"float\" >\n";
	cout << "\t\t\t<rsgis:edgelength classname=\"string\" length=\"float\" />\n";
	cout << "\t\t</rsgis:clusterer>\n";
	cout << "\t</rsgis:command>\n";
	cout << "\t<rsgis:command algor=\"postclassification\" option=\"clusterpolygonize\" input=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\">\n";
	cout << "\t\t<rsgis:clusterer algor=\"edgestddev | edgelength\" length=\"float\" stddev=\"float\" maxedgelength=\"float\" >\n";
	cout << "\t\t\t<rsgis:edgelength classname=\"string\" length=\"float\" />\n";
	cout << "\t\t</rsgis:clusterer>\n";
	cout << "\t\t<rsgis:polygonizer algor=\"lineproj | delaunay1 | delaunay2 | convexhull | multipoly | snakes | polys\" resolution=\"float\" alpha=\"float\" beta=\"float\" gamma=\"float\" delta=\"float\"/>\n";
	cout << "\t</rsgis:command>\n";
	cout << "\t<rsgis:command algor=\"postclassification\" option=\"clusterclasspolygonize\" input=\"vector.shp\" class=\"string\" output=\"vector_out.shp\" force=\"yes | no\">\n";
	cout << "\t\t<rsgis:clusterer algor=\"edgestddev | edgelength\" length=\"float\" stddev=\"float\" maxedgelength=\"float\" >\n";
	cout << "\t\t\t<rsgis:edgelength classname=\"string\" length=\"float\" />\n";
	cout << "\t\t</rsgis:clusterer>\n";
	cout << "\t\t<rsgis:polygonizer algor=\"lineproj | delaunay1 | delaunay2 | convexhull | multipoly | snakes | polys\" resolution=\"float\" alpha=\"float\" beta=\"float\" gamma=\"float\" delta=\"float\"/>\n";
	cout << "\t</rsgis:command>\n";
	cout << "\t<rsgis:command algor=\"postclassification\" option=\"splitlargesmall\" input=\"vector.shp\" outputlarge=\"vector_out.shp\" outputsmall=\"vector_out.shp\" force=\"yes | no\" largethreshold1=\"float\" proppolyarea1=\"float\" overlapthreshold=\"float\" largethreshold2=\"float\" proppolyarea2=\"float\"  />\n";
	cout << "\t<rsgis:command algor=\"postclassification\" option=\"morphology\" type=\"closing | opening | dilation | erosion\" input=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" buffer=\"float\" />\n";
	cout << "\t<rsgis:command algor=\"postclassification\" option=\"mergelargepolygons\" input=\"vector.shp\" output=\"vector_out.shp\" force=\"yes | no\" largethreshold=\"float\" intersectratio=\"float\" />\n";
	cout << "\t<rsgis:command algor=\"postclassification\" option=\"removesmallwithinlarge\" inputlarge=\"vector_large.shp\" inputsmall=\"vector_small.shp\" outputlarge=\"vector_out.shp\" outputsmall=\"vector_out.shp\" force=\"yes | no\" overlapthreshold=\"float\"/>\n";
	cout << "\t<rsgis:command algor=\"postclassification\" option=\"mergesmallandlarge\" inputlarge=\"vector_large.shp\" inputsmall=\"vector_small.shp\" outputlarge=\"vector_out.shp\" outputsmall=\"vector_out.shp\" force=\"yes | no\" largethreshold=\"float\" intersectratio=\"float\" overlapthreshold=\"float\" />\n";
	cout << "\t<rsgis:command algor=\"postclassification\" option=\"clusterremainingsmall\" inputlarge=\"vector_large.shp\" inputsmall=\"vector_small.shp\" output=\"vector_out.shp\" force=\"yes | no\" maxlength=\"float\" >\n";
	cout << "\t\t<rsgis:polygonizer algor=\"lineproj | delaunay1 | convexhull | snakes\" resolution=\"float\" alpha=\"float\" beta=\"float\" gamma=\"float\" delta=\"float\"/>\n";
	cout << "\t</rsgis:command>\n";
	cout << "\t<rsgis:command algor=\"postclassification\" option=\"mergesmall2nearestlarge\" inputlarge=\"vector_large.shp\" inputsmall=\"vector_small.shp\" output=\"vector_out.shp\" force=\"yes | no\" >\n";
	cout << "\t\t<rsgis:polygonizer algor=\"lineproj | delaunay1 | convexhull | snakes\" resolution=\"float\" alpha=\"float\" beta=\"float\" gamma=\"float\" delta=\"float\"/>\n";
	cout << "\t</rsgis:command>\n";
	cout << "</rsgis:commands>\n";
}

string RSGISExePostClassification::getDescription()
{
	return "Utilities for manipulating a classification.";
}

string RSGISExePostClassification::getXMLSchema()
{
	return "NOT DONE!";
}

void RSGISExePostClassification::buildClusterDataStructure(vector<RSGISClusterData*> *clusters, RSGISPolygonData **dataClusters, int numClusters, list<RSGIS2DPoint*> *inClusterPolygons, list<RSGIS2DPoint*> *dataOrigPolys) throw(RSGISException)
{
	try 
	{
		RSGISGeometry geomUtils;
		
		vector<RSGISClassificationPolygon*>* clusterPolygons = NULL;
		vector<RSGISClassificationPolygon*>* allIntersect = NULL;
		Polygon *poly = NULL;
		RSGISClusterData *cluster = NULL;
		
		vector<RSGISClassificationPolygon*>::iterator iterIndividuals;
		vector<RSGISClassificationPolygon*> *individuals = new vector<RSGISClassificationPolygon*>();
		list<RSGIS2DPoint*>::iterator iterClassPolys;
		for(iterClassPolys = dataOrigPolys->begin(); iterClassPolys != dataOrigPolys->end(); ++iterClassPolys)
		{
			individuals->push_back(((RSGISClassificationPolygon*)(*iterClassPolys)));
		}
		
		vector<RSGISIntValuePolygon*>::iterator iterInClusterPolys;
		vector<RSGISIntValuePolygon*> *inClusterPolys = new vector<RSGISIntValuePolygon*>();
		list<RSGIS2DPoint*>::iterator iterInClusterPolys1;
		for(iterInClusterPolys1 = inClusterPolygons->begin(); iterInClusterPolys1 != inClusterPolygons->end(); ++iterInClusterPolys1)
		{
			inClusterPolys->push_back(((RSGISIntValuePolygon*)(*iterInClusterPolys1)));
		}
		
		//int countIndividuals = 0;
		
		cout << "There are " << numClusters << " clusters\n";
		
		for(int i = 0; i < numClusters; ++i)
		{
			cout << "Cluster " << i << endl;
			poly = dataClusters[i]->getPolygon();
			
			/*
			cout << "Morphological 0 on retrieved polygons\n";
			geomUtils.performMorphologicalOperation(polys, closing, 0, 30);
			cout << "Completed Morphological 0 on polygons\n";
			 */
			
			/*
			vector<Polygon*> *outPolys = new vector<Polygon*>();
			outPolys->push_back(poly);
			RSGISVectorIO vecIO;
			vecIO.exportGEOSPolygons2SHP("/Users/pete/Temp/Clustering/polygons/polys1.shp", true, outPolys);
			vecIO.exportGEOSPolygons2SHP("/Users/pete/Temp/Clustering/polygons/polys2.shp", true, polys);
			//return 0;
			*/
			
			clusterPolygons = new vector<RSGISClassificationPolygon*>();
			allIntersect = new vector<RSGISClassificationPolygon*>();
			//countIndividuals = 0;
			for(iterIndividuals = individuals->begin(); iterIndividuals != individuals->end(); ++iterIndividuals)
			{
				//cout << "Individual " << countIndividuals << endl;
				
				try 
				{
					/*if((poly->overlaps((*iterIndividuals)->getPolygon())) |
					   (poly->within((*iterIndividuals)->getPolygon())) |
					   (poly->contains((*iterIndividuals)->getPolygon())))*/
					if(poly->contains((*iterIndividuals)->getPolygon()))
					{
						//cout << countIndividuals <<  ") poly overlap\n";
						for(iterInClusterPolys = inClusterPolys->begin(); iterInClusterPolys != inClusterPolys->end(); ++iterInClusterPolys)
						{
							if((*iterInClusterPolys)->getValue() == i)
							{
								try 
								{
									if(((*iterInClusterPolys)->getPolygon())->equalsExact((*iterIndividuals)->getPolygon(), 1))
									{
										//cout << "polygons equal\n";
										clusterPolygons->push_back(*iterIndividuals);
									}
								}
								catch (TopologyException &e) 
								{
									
									vector<Polygon*> *outPolys = new vector<Polygon*>();
									outPolys->push_back(poly);
									outPolys->push_back((*iterIndividuals)->getPolygon());
									RSGISVectorIO vecIO;
									vecIO.exportGEOSPolygons2SHP("/Users/pete/Temp/Clustering/polygons/error_polys2.shp", true, outPolys);
									
									throw RSGISException(e.what());
								}
							}
						}
						
						allIntersect->push_back(*iterIndividuals);
					}
					//++countIndividuals;
				}
				catch (TopologyException &e) 
				{
					/*
					vector<Polygon*> *outPolys = new vector<Polygon*>();
					outPolys->push_back(poly);
					outPolys->push_back((*iterIndividuals)->getPolygon());
					RSGISVectorIO vecIO;
					vecIO.exportGEOSPolygons2SHP("/Users/pete/Temp/Clustering/polygons/error_polys1.shp", true, outPolys);
					 */
					throw RSGISException(e.what());
				}
				
			}
			
			if(allIntersect->size() == 0)
			{
				throw RSGISException("Could not identify any polygons within cluster.");
			}
			
			if(clusterPolygons->size() == 0)
			{
				RSGISVectorIO vecIO;
				
				vector<Polygon*> *outPolys = new vector<Polygon*>();
				for(iterIndividuals = allIntersect->begin(); iterIndividuals != allIntersect->end(); ++iterIndividuals)
				{
					outPolys->push_back((*iterIndividuals)->getPolygon());
				}
				vecIO.exportGEOSPolygons2SHP("/Users/pete/Temp/Clustering/polygons/polys_allIntersect.shp", true, outPolys);
				
				throw RSGISException("Could not identify polygons associated with cluster.");
			}
			
			cluster = new RSGISClusterData(poly, clusterPolygons, allIntersect);
			clusters->push_back(cluster);
		}
		
		individuals->clear();
		delete individuals;
		inClusterPolys->clear();
		delete inClusterPolys;
	}
	catch (TopologyException &e) 
	{
		throw RSGISException(e.what());
	}
	catch(RSGISException &e)
	{
		throw e;
	}
}

RSGISExePostClassification::~RSGISExePostClassification()
{
	if(classEdgeLengths != NULL)
	{
		vector<EdgeLengthPair*>::iterator iterEdgePairs;
		for(iterEdgePairs = classEdgeLengths->begin(); iterEdgePairs != classEdgeLengths->end(); )
		{
			delete *iterEdgePairs;
			classEdgeLengths->erase(iterEdgePairs);
		}
		delete classEdgeLengths;
	}
}




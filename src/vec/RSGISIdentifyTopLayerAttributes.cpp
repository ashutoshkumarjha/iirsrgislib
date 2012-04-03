/*
 *  RSGISIdentifyTopLayerAttributes.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 24/02/2009.
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

#include "RSGISIdentifyTopLayerAttributes.h"


namespace rsgis{namespace vec{
	/*
	RSGISIdentifyTopLayerAttributes::RSGISIdentifyTopLayerAttributes()
	{
		
	}
	
	void RSGISIdentifyTopLayerAttributes::computeTopLayerAttributes(OGRLayer *inputLayer, OGRLayer *outputLayer, string *attributes, int numAttributes, int numTop, summarytype summary) throw(RSGISVectorException)
	{
		RSGISMathsUtils mathUtils;

		OGRFeatureDefn *layerDef = inputLayer->GetLayerDefn();
		int fieldIdx = 0;
		
		for(int i = 0; i < numAttributes; i++)
		{
			fieldIdx = layerDef->GetFieldIndex(attributes[i].c_str());
			if(fieldIdx < 0)
			{
				string message = "This layer does not contain a field with the name \'" + attributes[i] + "\'";
				throw RSGISVectorException(message.c_str());
			}
		}
		
		OGRFeatureDefn *outputDefn = NULL;
		
		OGRGeometry *geometry = NULL;
		OGRPolygon *polygon = NULL;
		OGRMultiPolygon *multiPolygon = NULL;
		
		OGRFeature *inFeature = NULL;
		OGRFeature *outFeature = NULL;
		
		SortedGenericList<RSGISFloatAttribute> *sortedAttributes = NULL;
		
		int fid = 0;
		int inFieldCount = 0;
		
		double value = 0;
		double sum = 0;
		
		string name = "";
		
		try
		{
			sortedAttributes = new SortedGenericList<RSGISFloatAttribute>(numAttributes, numAttributes/2);
			
			this->createOutputSHPDefinition(outputLayer, numTop, summary, layerDef);
			
			outputDefn = outputLayer->GetLayerDefn();
			
			int numFeatures = inputLayer->GetFeatureCount(TRUE);
			
			int feedback = numFeatures/10;
			int feedbackCounter = 0;
			int i = 0;
			cout << "Started " << flush;
			
			inputLayer->ResetReading();
			while( (inFeature = inputLayer->GetNextFeature()) != NULL )
			{
				if((i % feedback) == 0)
				{
					cout << ".." << feedbackCounter << ".." << flush;
					feedbackCounter = feedbackCounter + 10;
				}
				
				fid = inFeature->GetFID();
				//cout << " .. " << fid << " .. " ;
				
				outFeature = OGRFeature::CreateFeature(outputDefn);
				// Get Geometry.
				geometry = inFeature->GetGeometryRef();
				if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPolygon )
				{
					//cout << "Polygon";
					polygon = (OGRPolygon *) geometry;
					outFeature->SetGeometry(polygon);
				} 
				else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbMultiPolygon )
				{
					//cout << "MultiPolygon";
					multiPolygon = (OGRMultiPolygon *) geometry;
					outFeature->SetGeometry(multiPolygon);
				}
				else
				{
					throw RSGISVectorException("Unsupport data type.");
				}
				
				outFeature->SetFID(fid);
								
				// Copy Data to feature
				inFieldCount = layerDef->GetFieldCount();
				for(int j = 0; j < inFieldCount; j++)
				{
					outFeature->SetField(outputDefn->GetFieldIndex(layerDef->GetFieldDefn(j)->GetNameRef()), inFeature->GetRawFieldRef(layerDef->GetFieldIndex(layerDef->GetFieldDefn(j)->GetNameRef())));
				}
				
				// Identify top columns
				sum = 0;
				for(int j = 0; j < numAttributes; j++)
				{
					value = inFeature->GetFieldAsDouble(layerDef->GetFieldIndex(attributes[j].c_str()));
					sortedAttributes->add(new RSGISFloatAttribute(attributes[j], value));
					sum += value;
				}
				
				
				//cout << "\nTOP: " << sortedAttributes->getAt(0)->getName().c_str() << endl;
				//sortedAttributes->printAsc();
				//cout << endl;
				 
				
				// Write top columns.
				for(int j = 0; j < numTop; j++)
				{
					name = "Top_" + mathUtils.inttostring(j+1);
					if(sortedAttributes->getAt(j)->getValue() > 0)
					{
						outFeature->SetField(outputDefn->GetFieldIndex(name.c_str()), sortedAttributes->getAt(j)->getName().c_str());
					}
					else
					{
						outFeature->SetField(outputDefn->GetFieldIndex(name.c_str()), "NULL");
					}
				}
				
				if(summary == rsgis::math::aggregate)
				{
					for(int j = 0; j < numTop; j++)
					{
						name = "Top_" + mathUtils.inttostring(j+1) + "_Ag";
						if(sortedAttributes->getAt(j)->getValue() > 0)
						{
							outFeature->SetField(outputDefn->GetFieldIndex(name.c_str()), (sortedAttributes->getAt(j)->getValue()/sum));
						}
						else
						{
							outFeature->SetField(outputDefn->GetFieldIndex(name.c_str()), 0);
						}
					}
				}
								
				if( outputLayer->CreateFeature(outFeature) != OGRERR_NONE )
				{
					throw RSGISVectorOutputException("Failed to write feature to the output shapefile.");
				}
				sortedAttributes->clearListDelete();
				OGRFeature::DestroyFeature(inFeature);
				OGRFeature::DestroyFeature(outFeature);
				i++;
			}
			cout << " Complete.\n";
		}
		catch(RSGISVectorOutputException e)
		{
			throw RSGISVectorException(e.what());
		}
		
		
	}
	
	
	void RSGISIdentifyTopLayerAttributes::createOutputSHPDefinition(OGRLayer *outputSHPLayer, int numTop, summarytype summary, OGRFeatureDefn *inLayerDef) throw(RSGISVectorOutputException)
	{
		//Add new columns
		
		RSGISMathsUtils mathUtils;
		string name = "";
		for(int i = 0; i < numTop; i++)
		{
			name = "Top_" + mathUtils.inttostring(i+1);
			OGRFieldDefn shpField(name.c_str(), OFTString);
			shpField.SetWidth(10);
			if( outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				string message = string("Creating shapefile field") + name + string(" has failed");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
		
		if(summary == rsgis::math::aggregate)
		{
			for(int i = 0; i < numTop; i++)
			{
				name = "Top_" + mathUtils.inttostring(i+1) + "_Ag";
				OGRFieldDefn shpField(name.c_str(), OFTReal);
				shpField.SetPrecision(10);
				if( outputSHPLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					string message = string("Creating shapefile field") + name + string(" has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
		}
		
		// Copy Existing Columns
		
		int fieldCount = inLayerDef->GetFieldCount();
		for(int i = 0; i < fieldCount; i++)
		{
			if( outputSHPLayer->CreateField( inLayerDef->GetFieldDefn(i) ) != OGRERR_NONE )
			{
				string message = string("Creating ") + string(inLayerDef->GetFieldDefn(i)->GetNameRef()) + string(" field failed.");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
	}
	
	RSGISIdentifyTopLayerAttributes::~RSGISIdentifyTopLayerAttributes()
	{
		
	}
	*/
	
	
	RSGISIdentifyTopLayerAttributes::RSGISIdentifyTopLayerAttributes(string *attributes, int numAttributes, int numTop, rsgissummarytype summary)
	{
		this->attributes = attributes;
		this->numAttributes = numAttributes;
		this->numTop = numTop;
		this->summary = summary;
		this->sortedAttributes = new SortedGenericList<RSGISFloatAttribute>(numAttributes, numAttributes/2);
	}
	
	void RSGISIdentifyTopLayerAttributes::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException)
	{
		RSGISMathsUtils mathUtils;
		this->sortedAttributes->clearListDelete();
		
		OGRFeatureDefn *inFeatureDefn = inFeature->GetDefnRef();
		OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
		
		double value = 0;
		double sum = 0;
		for(int j = 0; j < numAttributes; j++)
		{
			value = inFeature->GetFieldAsDouble(inFeatureDefn->GetFieldIndex(attributes[j].c_str()));
			sortedAttributes->add(new RSGISFloatAttribute(attributes[j], value));
			sum += value;
		}
		
		
		//cout << "\nTOP: " << sortedAttributes->getAt(0)->getName().c_str() << endl;
		//sortedAttributes->printAsc();
		//cout << endl;
		
		string name = "";
		
		// Write top columns.
		for(int j = 0; j < numTop; j++)
		{
			name = "Top_" + mathUtils.inttostring(j+1);
			if(sortedAttributes->getAt(j)->getValue() > 0)
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex(name.c_str()), sortedAttributes->getAt(j)->getName().c_str());
			}
			else
			{
				outFeature->SetField(outFeatureDefn->GetFieldIndex(name.c_str()), "NULL");
			}
		}
		
		if(summary == rsgis::math::sumtype_aggregate)
		{
			for(int j = 0; j < numTop; j++)
			{
				name = "Top_" + mathUtils.inttostring(j+1) + "_Ag";
				if(sortedAttributes->getAt(j)->getValue() > 0)
				{
					outFeature->SetField(outFeatureDefn->GetFieldIndex(name.c_str()), (sortedAttributes->getAt(j)->getValue()/sum));
				}
				else
				{
					outFeature->SetField(outFeatureDefn->GetFieldIndex(name.c_str()), 0);
				}
			}
		}
	}
	
	void RSGISIdentifyTopLayerAttributes::processFeature(OGRFeature *feature, Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not Implemented");
	}
	
	void RSGISIdentifyTopLayerAttributes::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		RSGISMathsUtils mathUtils;
		string name = "";
		for(int i = 0; i < numTop; i++)
		{
			name = "Top_" + mathUtils.inttostring(i+1);
			OGRFieldDefn shpField(name.c_str(), OFTString);
			shpField.SetWidth(10);
			if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				string message = string("Creating shapefile field") + name + string(" has failed");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
		
		if(this->summary == rsgis::math::sumtype_aggregate)
		{
			for(int i = 0; i < numTop; i++)
			{
				name = "Top_" + mathUtils.inttostring(i+1) + "_Ag";
				OGRFieldDefn shpField(name.c_str(), OFTReal);
				shpField.SetPrecision(10);
				if( outputLayer->CreateField( &shpField ) != OGRERR_NONE )
				{
					string message = string("Creating shapefile field") + name + string(" has failed");
					throw RSGISVectorOutputException(message.c_str());
				}
			}
		}
	}
	
	RSGISIdentifyTopLayerAttributes::~RSGISIdentifyTopLayerAttributes()
	{
		
	}
	
	
	RSGISFloatAttribute::RSGISFloatAttribute(string name, double value)
	{
		this->name = name;
		this->value = value;
	}
	
	string RSGISFloatAttribute::getName() const
	{
		return this->name;
	}
	
	double RSGISFloatAttribute::getValue() const
	{
		return this->value;
	}
	
	ostream& operator<<(ostream& ostr, const RSGISFloatAttribute& attribute)
	{
		ostr << attribute.getName() << " = " << attribute.getValue();
		return ostr;
	}
	
	ostream& RSGISFloatAttribute::operator<<(ostream& ostr)
	{
		ostr << this->name << " = " << this->value;
		return ostr;
	}
	
	bool RSGISFloatAttribute::operator==(RSGISFloatAttribute attribute) const
	{
		bool returnVal = false;
		if(attribute.value == this->value)
		{
			returnVal = true;
		}
		return returnVal;
	}
	
	bool RSGISFloatAttribute::operator!=(RSGISFloatAttribute attribute) const
	{
		bool returnVal = false;
		if(attribute.value != this->value)
		{
			returnVal = true;
		}
		return returnVal;
	}
	
	bool RSGISFloatAttribute::operator>(RSGISFloatAttribute attribute) const
	{
		bool returnVal = false;
		if(attribute.value < this->value)
		{
			returnVal = true;
		}
		return returnVal;
	}
	
	bool RSGISFloatAttribute::operator<(RSGISFloatAttribute attribute) const
	{
		bool returnVal = false;
		if(attribute.value > this->value)
		{
			returnVal = true;
		}
		return returnVal;
	}
	
	bool RSGISFloatAttribute::operator>=(RSGISFloatAttribute attribute) const
	{
		bool returnVal = false;
		if(attribute.value <= this->value)
		{
			returnVal = true;
		}
		return returnVal;
	}
	
	bool RSGISFloatAttribute::operator<=(RSGISFloatAttribute attribute) const
	{
		bool returnVal = false;
		if(attribute.value >= this->value)
		{
			returnVal = true;
		}
		return returnVal;
	}
	
	RSGISFloatAttribute::~RSGISFloatAttribute()
	{
		
	}
	
}}




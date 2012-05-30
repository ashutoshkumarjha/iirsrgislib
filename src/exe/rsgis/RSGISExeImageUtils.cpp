/*
 *  RSGISExeImageUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 11/12/2008.
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

#include "RSGISExeImageUtils.h"


RSGISExeImageUtils::RSGISExeImageUtils() : RSGISAlgorithmParameters()
{
	this->algorithm = "imageutils";
	
	this->option = RSGISExeImageUtils::none;
	this->interpolator = RSGISExeImageUtils::cubic;
	
	this->inputImage = "";
	this->outputImage = "";
	this->inputImages = NULL;
	this->inputVector = "";
	this->filenameAttribute = "";
	this->imageMask = "";
	this->inputDIR = "";
	this->outputDIR = "";
	this->proj = "";
	this->classColour = NULL;
	this->nodataValue = 0;
	this->maskValue = 0;
	this->resampleScale = 1;
	this->numImages = 0;
	this->numClasses = 0;
	this->projFromImage = true;
    this->imageFormat = "ENVI";
    this->outDataType = GDT_Float32;
}

RSGISAlgorithmParameters* RSGISExeImageUtils::getInstance()
{
	return new RSGISExeImageUtils();
}

void RSGISExeImageUtils::retrieveParameters(DOMElement *argElement) throw(RSGISXMLArgumentsException)
{
	RSGISMathsUtils mathUtils;
	RSGISFileUtils fileUtils;
    RSGISTextUtils textUtils;
	
	XMLCh *algorName = XMLString::transcode(this->algorithm.c_str());
	XMLCh *algorXMLStr = XMLString::transcode("algor");
	XMLCh *optionXMLStr = XMLString::transcode("option");
	XMLCh *optionColour = XMLString::transcode("colourimage");
	XMLCh *optionMosaic = XMLString::transcode("mosaic");
	XMLCh *optionInclude = XMLString::transcode("include");
	XMLCh *optionCut2Poly = XMLString::transcode("cut2poly");
    XMLCh *optionCut2Polys = XMLString::transcode("cut2polys");
	XMLCh *optionMask = XMLString::transcode("mask");
	XMLCh *optionResample = XMLString::transcode("resample");
	XMLCh *optionRasteriseDef = XMLString::transcode("rasterisedefiniens");
	XMLCh *projImage = XMLString::transcode("IMAGE");
	XMLCh *projOSGB = XMLString::transcode("OSGB");
    XMLCh *projNZ2000 = XMLString::transcode("NZ2000");
    XMLCh *projNZ1949 = XMLString::transcode("NZ1949");
	XMLCh *rsgisimageXMLStr = XMLString::transcode("rsgis:image");
	XMLCh *optionPrintProj4 = XMLString::transcode("printProj4");
	XMLCh *optionPrintWKT = XMLString::transcode("printWKT");
	XMLCh *optionExtract2DScatterPtxt = XMLString::transcode("extract2dscatterptxt");
	XMLCh *optionSGSmoothing = XMLString::transcode("sgsmoothing");
	XMLCh *optionCumulativeArea = XMLString::transcode("cumulativearea");
	XMLCh *optionCreateImage = XMLString::transcode("createimage");
	XMLCh *optionStretchImage = XMLString::transcode("stretch");
	XMLCh *optionHueColour = XMLString::transcode("huecolour");
	XMLCh *optionRemoveSpatialRef = XMLString::transcode("removespatialref");
	XMLCh *optionAddnoise = XMLString::transcode("addnoise");
    XMLCh *optionSubset = XMLString::transcode("subset");
    XMLCh *optionSubset2Polys = XMLString::transcode("subset2polys");
	XMLCh *optionDefineSpatialRef = XMLString::transcode("definespatialref");
	XMLCh *optionPanSharpen = XMLString::transcode("pansharpen");
    XMLCh *optionColourImageBands = XMLString::transcode("colourimagebands");
    XMLCh *optionCreateSlices = XMLString::transcode("createslices");
	XMLCh *optionClump = XMLString::transcode("clump");
    XMLCh *optionComposite = XMLString::transcode("composite");
    XMLCh *optionRelabel = XMLString::transcode("relabel");
    XMLCh *optionAssignProj = XMLString::transcode("assignproj");
    XMLCh *optionPopImgStats = XMLString::transcode("popimgstats");
    XMLCh *optionCreateCopy = XMLString::transcode("createcopy");
    
	const XMLCh *algorNameEle = argElement->getAttribute(algorXMLStr);
	if(!XMLString::equals(algorName, algorNameEle))
	{
		throw RSGISXMLArgumentsException("The algorithm name is incorrect.");
	}
    
    // Set output image fomat (defaults to ENVI)
	this->imageFormat = "ENVI";
	XMLCh *formatXMLStr = XMLString::transcode("format");
	if(argElement->hasAttribute(formatXMLStr))
	{
		char *charValue = XMLString::transcode(argElement->getAttribute(formatXMLStr));
		this->imageFormat = string(charValue);
		XMLString::release(&charValue);
	}
	XMLString::release(&formatXMLStr);
    
    this->outDataType = GDT_Float32;
	XMLCh *datatypeXMLStr = XMLString::transcode("datatype");
	if(argElement->hasAttribute(datatypeXMLStr))
	{
        XMLCh *dtByte = XMLString::transcode("Byte");
        XMLCh *dtUInt16 = XMLString::transcode("UInt16");
        XMLCh *dtInt16 = XMLString::transcode("Int16");
        XMLCh *dtUInt32 = XMLString::transcode("UInt32");
        XMLCh *dtInt32 = XMLString::transcode("Int32");
        XMLCh *dtFloat32 = XMLString::transcode("Float32");
        XMLCh *dtFloat64 = XMLString::transcode("Float64");
        
        const XMLCh *dtXMLValue = argElement->getAttribute(datatypeXMLStr);
        if(XMLString::equals(dtByte, dtXMLValue))
        {
            this->outDataType = GDT_Byte;
        }
        else if(XMLString::equals(dtUInt16, dtXMLValue))
        {
            this->outDataType = GDT_UInt16;
        }
        else if(XMLString::equals(dtInt16, dtXMLValue))
        {
            this->outDataType = GDT_Int16;
        }
        else if(XMLString::equals(dtUInt32, dtXMLValue))
        {
            this->outDataType = GDT_UInt32;
        }
        else if(XMLString::equals(dtInt32, dtXMLValue))
        {
            this->outDataType = GDT_Int32;
        }
        else if(XMLString::equals(dtFloat32, dtXMLValue))
        {
            this->outDataType = GDT_Float32;
        }
        else if(XMLString::equals(dtFloat64, dtXMLValue))
        {
            this->outDataType = GDT_Float64;
        }
        else
        {
            cerr << "Data type not recognised, defaulting to 32 bit float.";
            this->outDataType = GDT_Float32;
        }
        
        XMLString::release(&dtByte);
        XMLString::release(&dtUInt16);
        XMLString::release(&dtInt16);
        XMLString::release(&dtUInt32);
        XMLString::release(&dtInt32);
        XMLString::release(&dtFloat32);
        XMLString::release(&dtFloat64);
	}
	XMLString::release(&datatypeXMLStr);
	
	const XMLCh *optionXML = argElement->getAttribute(optionXMLStr);
	if(XMLString::equals(optionColour, optionXML))
	{		
		this->option = RSGISExeImageUtils::colour;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		DOMNodeList *classNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:colour"));
		this->numClasses = classNodesList->getLength();		
		
		cout << "Found " << this->numClasses << " Classes \n";
		
		DOMElement *classElement = NULL;
		classColour = new ClassColour*[numClasses];
		for(int i = 0; i < numClasses; i++)
		{
			classColour[i] = new ClassColour();
			classElement = static_cast<DOMElement*>(classNodesList->item(i));
			
			XMLCh *nameXMLStr = XMLString::transcode("name");
			if(classElement->hasAttribute(nameXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(nameXMLStr));
				classColour[i]->className = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
			}
			XMLString::release(&nameXMLStr);
			
			
			XMLCh *idXMLStr = XMLString::transcode("id");
			if(classElement->hasAttribute(idXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(idXMLStr));
				classColour[i]->classID = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'id\' attribute was provided.");
			}
			XMLString::release(&idXMLStr);
			
			XMLCh *bandXMLStr = XMLString::transcode("band");
			if(classElement->hasAttribute(bandXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(bandXMLStr));
				classColour[i]->imgBand = mathUtils.strtoint(string(charValue))-1; // Band refers to the array index not image band
 				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
			}
			XMLString::release(&bandXMLStr);
			
			XMLCh *lowerXMLStr = XMLString::transcode("lower");
			if(classElement->hasAttribute(lowerXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(lowerXMLStr));
				classColour[i]->lower = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'lower\' attribute was provided.");
			}
			XMLString::release(&lowerXMLStr);
			
			XMLCh *upperXMLStr = XMLString::transcode("upper");
			if(classElement->hasAttribute(upperXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(upperXMLStr));
				classColour[i]->upper = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'upper\' attribute was provided.");
			}
			XMLString::release(&upperXMLStr);
			
			XMLCh *redXMLStr = XMLString::transcode("red");
			if(classElement->hasAttribute(redXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(redXMLStr));
				classColour[i]->red = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'red\' attribute was provided.");
			}
			XMLString::release(&redXMLStr);
			
			XMLCh *greenXMLStr = XMLString::transcode("green");
			if(classElement->hasAttribute(greenXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(greenXMLStr));
				classColour[i]->green = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'green\' attribute was provided.");
			}
			XMLString::release(&greenXMLStr);
			
			XMLCh *blueXMLStr = XMLString::transcode("blue");
			if(classElement->hasAttribute(blueXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(blueXMLStr));
				classColour[i]->blue = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'blue\' attribute was provided.");
			}
			XMLString::release(&blueXMLStr);
		}
	}
    else if(XMLString::equals(optionColourImageBands, optionXML))
	{		
		this->option = RSGISExeImageUtils::colourimagebands;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		DOMNodeList *classNodesList = argElement->getElementsByTagName(XMLString::transcode("rsgis:colour"));
		this->numClasses = classNodesList->getLength();		
		
		cout << "Found " << this->numClasses << " Classes \n";
		
		DOMElement *classElement = NULL;
		classColour = new ClassColour*[numClasses];
		for(int i = 0; i < numClasses; i++)
		{
			classColour[i] = new ClassColour();
			classElement = static_cast<DOMElement*>(classNodesList->item(i));
			
			XMLCh *nameXMLStr = XMLString::transcode("name");
			if(classElement->hasAttribute(nameXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(nameXMLStr));
				classColour[i]->className = string(charValue);
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'name\' attribute was provided.");
			}
			XMLString::release(&nameXMLStr);
			
			
			XMLCh *idXMLStr = XMLString::transcode("id");
			if(classElement->hasAttribute(idXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(idXMLStr));
				classColour[i]->classID = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'id\' attribute was provided.");
			}
			XMLString::release(&idXMLStr);
			
			XMLCh *lowerXMLStr = XMLString::transcode("lower");
			if(classElement->hasAttribute(lowerXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(lowerXMLStr));
				classColour[i]->lower = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'lower\' attribute was provided.");
			}
			XMLString::release(&lowerXMLStr);
			
			XMLCh *upperXMLStr = XMLString::transcode("upper");
			if(classElement->hasAttribute(upperXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(upperXMLStr));
				classColour[i]->upper = mathUtils.strtofloat(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'upper\' attribute was provided.");
			}
			XMLString::release(&upperXMLStr);
			
			XMLCh *redXMLStr = XMLString::transcode("red");
			if(classElement->hasAttribute(redXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(redXMLStr));
				classColour[i]->red = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'red\' attribute was provided.");
			}
			XMLString::release(&redXMLStr);
			
			XMLCh *greenXMLStr = XMLString::transcode("green");
			if(classElement->hasAttribute(greenXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(greenXMLStr));
				classColour[i]->green = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'green\' attribute was provided.");
			}
			XMLString::release(&greenXMLStr);
			
			XMLCh *blueXMLStr = XMLString::transcode("blue");
			if(classElement->hasAttribute(blueXMLStr))
			{
				char *charValue = XMLString::transcode(classElement->getAttribute(blueXMLStr));
				classColour[i]->blue = mathUtils.strtoint(string(charValue));
				XMLString::release(&charValue);
			}
			else
			{
				throw RSGISXMLArgumentsException("No \'blue\' attribute was provided.");
			}
			XMLString::release(&blueXMLStr);
		}
	}
	else if (XMLString::equals(optionMosaic, optionXML))
	{		
		this->option = RSGISExeImageUtils::mosaic;
		this->mosaicSkipVals = false;
		this->mosaicSkipThreash = false;
		this->skipLowerThreash = -numeric_limits<double>::infinity();
		this->skipUpperThreash = +numeric_limits<double>::infinity();
		this->skipBand = 0;
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);

		XMLCh *nodataXMLStr = XMLString::transcode("nodata");
		if(argElement->hasAttribute(nodataXMLStr))
		{
            XMLCh *NaNStr = XMLString::transcode("NaN");
			const XMLCh *noDataValue = argElement->getAttribute(nodataXMLStr);
			if(XMLString::equals(noDataValue, NaNStr))
			{
                const char *val = "NaN";
				this->nodataValue = nan(val);
			}
			else
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(nodataXMLStr));
                this->nodataValue = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
			}
			XMLString::release(&NaNStr);
		}
		else
		{
			//throw RSGISXMLArgumentsException("No \'nodata\' attribute was provided.");
			cout << "\tUsing default of 0 for background values" << endl;
			this->nodataValue = 0;
		}
		XMLString::release(&nodataXMLStr);
		
		// Set value in first band to skip, if not set no error will be printed and standard mosaic method will be used.
		XMLCh *skipValueXMLStr = XMLString::transcode("skipValue");
		if(argElement->hasAttribute(skipValueXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(skipValueXMLStr));
			this->skipValue = mathUtils.strtofloat(string(charValue));
			this->mosaicSkipVals = true;
			XMLString::release(&charValue);
		}
		XMLString::release(&skipValueXMLStr);
		
		// Set upper threashold to skip in all bands, if not set no error will be printed and standard mosaic method will be used.
		XMLCh *skipUpperThreashXMLStr = XMLString::transcode("skipUpperThreash");
		if(argElement->hasAttribute(skipUpperThreashXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(skipUpperThreashXMLStr));
			this->skipUpperThreash = mathUtils.strtofloat(string(charValue));
			this->mosaicSkipThreash = true;
			XMLString::release(&charValue);
			if (mosaicSkipVals) 
			{
				cout << "\tCan't use \'skipValue\' with \'skipUpperThreash\', using threashold instead" << endl;
			}
		}
		XMLString::release(&skipUpperThreashXMLStr);
		
		// Set lower threashold to skip in all bands, if not set no error will be printed and standard mosaic method will be used.
		XMLCh *skipLowerThreashXMLStr = XMLString::transcode("skipLowerThreash");
		if(argElement->hasAttribute(skipLowerThreashXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(skipLowerThreashXMLStr));
			this->skipLowerThreash = mathUtils.strtofloat(string(charValue));
			this->mosaicSkipThreash = true;
			XMLString::release(&charValue);
			if (mosaicSkipVals) 
			{
				cout << "\tCan't use \'skipValue\' with \'skipLowerThreash\', using threashold instead" << endl;
			}
		}
		XMLString::release(&skipLowerThreashXMLStr);
		
		XMLCh *skipBandXMLStr = XMLString::transcode("setSkipBand");
		if(argElement->hasAttribute(skipBandXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(skipBandXMLStr));
			this->skipBand = mathUtils.strtofloat(string(charValue)) - 1;
			XMLString::release(&charValue);
			if (this->mosaicSkipVals) 
			{
				cout << "\tSkiping pixels with a value of \'" << this->skipValue << "\' in band \'" << this->skipBand + 1 << "\'" << endl;
			}
			else if (this->mosaicSkipThreash) 
			{
				cout << "\tSkiping pixels with a value between \'" << this->skipLowerThreash << "\' and \'" << this->skipUpperThreash << "\' in band \'" << this->skipBand + 1<< "\'" << endl;
			}
			else 
			{
				cout << "\tBand set to define values to skip using \'setSkipBand\' but no value or threasholds set - IGNORING" << endl;
			}

		}
		else 
		{
			if (this->mosaicSkipVals) 
			{
				cout << "\tSkiping pixels with a value of \'" << this->skipValue << "\' using the first band (default)" << endl;
			}
			else if (this->mosaicSkipThreash) 
			{
				cout << "\tSkiping pixels with a value between \'" << this->skipLowerThreash << "\' and \'" << this->skipUpperThreash << "\' using the first band (default)" << endl;			}
		}

		XMLString::release(&skipBandXMLStr);
		
		XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
		
		
		XMLCh *dirXMLStr = XMLString::transcode("dir");
		XMLCh *extXMLStr = XMLString::transcode("ext");
		if(argElement->hasAttribute(dirXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(dirXMLStr));
			string dirStr = string(charValue);
			XMLString::release(&charValue);
			
			charValue = XMLString::transcode(argElement->getAttribute(extXMLStr));
			string extStr = string(charValue);
			XMLString::release(&charValue);
			
			try
			{
				this->inputImages = fileUtils.getDIRList(dirStr, extStr, &this->numImages, false);
			}
			catch(RSGISException e)
			{
				throw RSGISXMLArgumentsException(e.what());
			}
		}
		else
		{
			DOMElement *imageElement = NULL;
			
			DOMNodeList *imagesList = argElement->getElementsByTagName(rsgisimageXMLStr);
			this->numImages = imagesList->getLength();
			this->inputImages = new string[numImages];
			
			XMLCh *fileXMLStr = XMLString::transcode("file");
			for(int i = 0; i < numImages; i++)
			{
				imageElement = static_cast<DOMElement*>(imagesList->item(i));
				
				if(imageElement->hasAttribute(fileXMLStr))
				{
					char *charValue = XMLString::transcode(imageElement->getAttribute(fileXMLStr));
					this->inputImages[i] = string(charValue);
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'file\' attribute was provided.");
				}
			}
			XMLString::release(&fileXMLStr);
		}
		XMLString::release(&dirXMLStr);
		XMLString::release(&extXMLStr);
	}
	else if (XMLString::equals(optionInclude, optionXML))
	{		
		this->option = RSGISExeImageUtils::include;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
        
        XMLCh *bandsXMLStr = XMLString::transcode("bands");
		if(argElement->hasAttribute(bandsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(bandsXMLStr));
			string bandsList = string(charValue);
			XMLString::release(&charValue);
            
            vector<string> *tokens = new vector<string>();
            textUtils.tokenizeString(bandsList, ',', tokens, true, true);
            for(vector<string>::iterator iterTokens = tokens->begin(); iterTokens != tokens->end(); ++iterTokens)
            {
                try 
                {
                    bands.push_back(mathUtils.strtoint(*iterTokens));
                } 
                catch (RSGISMathException &e) 
                {
                    cout << "Warning: " << *iterTokens << " is not an integer!\n";
                }
            }
            bandsDefined = true;
		}
		else
		{
			bandsDefined = false;
		}
		XMLString::release(&bandsXMLStr);
        
		
		XMLCh *dirXMLStr = XMLString::transcode("dir");
		XMLCh *extXMLStr = XMLString::transcode("ext");
		if(argElement->hasAttribute(dirXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(dirXMLStr));
			string dirStr = string(charValue);
			XMLString::release(&charValue);
			
			charValue = XMLString::transcode(argElement->getAttribute(extXMLStr));
			string extStr = string(charValue);
			XMLString::release(&charValue);
			
			try
			{
				this->inputImages = fileUtils.getDIRList(dirStr, extStr, &this->numImages, false);
			}
			catch(RSGISException e)
			{
				throw RSGISXMLArgumentsException(e.what());
			}
		}
		else
		{
			DOMElement *imageElement = NULL;
			
			DOMNodeList *imagesList = argElement->getElementsByTagName(rsgisimageXMLStr);
			this->numImages = imagesList->getLength();
			this->inputImages = new string[numImages];
			
			XMLCh *fileXMLStr = XMLString::transcode("file");
			for(int i = 0; i < numImages; i++)
			{
				imageElement = static_cast<DOMElement*>(imagesList->item(i));
				
				if(imageElement->hasAttribute(fileXMLStr))
				{
					char *charValue = XMLString::transcode(imageElement->getAttribute(fileXMLStr));
					this->inputImages[i] = string(charValue);
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'file\' attribute was provided.");
				}
			}
			XMLString::release(&fileXMLStr);
		}
		XMLString::release(&dirXMLStr);
		XMLString::release(&extXMLStr);	
	}
	else if ((XMLString::equals(optionCut2Poly, optionXML)) | (XMLString::equals(optionCut2Polys, optionXML))) 
	{		
		/* Changed to cut2polys for concistency with subset to polys. 
           Also works with cut2poly to enable compatibility with old scripts - Dan */
        this->option = RSGISExeImageUtils::cut2poly;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		
		XMLCh *vectorXMLStr = XMLString::transcode("vector");
		if(argElement->hasAttribute(vectorXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(vectorXMLStr));
			this->inputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'vector\' attribute was provided.");
		}
		XMLString::release(&vectorXMLStr);
		
		
		XMLCh *outfilenameXMLStr = XMLString::transcode("outfilename");
		if(argElement->hasAttribute(outfilenameXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outfilenameXMLStr));
			this->filenameAttribute = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'outfilename\' attribute was provided.");
		}
		XMLString::release(&outfilenameXMLStr);
		
		XMLCh *nodataXMLStr = XMLString::transcode("nodata");
		if(argElement->hasAttribute(nodataXMLStr))
		{
            XMLCh *NaNStr = XMLString::transcode("NaN");
			const XMLCh *noDataValue = argElement->getAttribute(nodataXMLStr);
			if(XMLString::equals(noDataValue, NaNStr))
			{
                const char *val = "NaN";
				this->nodataValue = nan(val);
			}
			else
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(nodataXMLStr));
                this->nodataValue = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
			}
			XMLString::release(&NaNStr);

		}
		else
		{
			throw RSGISXMLArgumentsException("No \'nodata\' attribute was provided.");
		}
		XMLString::release(&nodataXMLStr);
	}
	else if (XMLString::equals(optionMask, optionXML))
	{		
		this->option = RSGISExeImageUtils::mask;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);

		
		XMLCh *maskXMLStr = XMLString::transcode("mask");
		if(argElement->hasAttribute(maskXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maskXMLStr));
			this->imageMask = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'mask\' attribute was provided.");
		}
		XMLString::release(&maskXMLStr);

		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
				
		XMLCh *maskvalueXMLStr = XMLString::transcode("maskvalue");
		if(argElement->hasAttribute(maskvalueXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(maskvalueXMLStr));
			this->maskValue = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'maskvalue\' attribute was provided.");
		}
		XMLString::release(&maskvalueXMLStr);
		
	}
	else if (XMLString::equals(optionResample, optionXML))
	{		
		this->option = RSGISExeImageUtils::resample;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
				
		XMLCh *scaleXMLStr = XMLString::transcode("scale");
		if(argElement->hasAttribute(scaleXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(scaleXMLStr));
			this->resampleScale = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'scale\' attribute was provided.");
		}
		XMLString::release(&scaleXMLStr);
				
		XMLCh *interpolatorCubic = XMLString::transcode("Cubic");
		XMLCh *interpolatorBilinearArea = XMLString::transcode("BilinearArea");
		XMLCh *interpolatorBilinearPoint = XMLString::transcode("BilinearPoint");
		XMLCh *interpolatorNN = XMLString::transcode("NN");
		XMLCh *interpolatorTriangular = XMLString::transcode("Triangular");
		
		XMLCh *interpolatorXMLStr = XMLString::transcode("interpolation");
		if(argElement->hasAttribute(interpolatorXMLStr))
		{
			const XMLCh *interpolatorXMLValue = argElement->getAttribute(interpolatorXMLStr);
			if(XMLString::equals(interpolatorCubic, interpolatorXMLValue))
			{
				this->interpolator = RSGISExeImageUtils::cubic;
			}
			else if (XMLString::equals(interpolatorBilinearArea, interpolatorXMLValue))
			{
				this->interpolator = RSGISExeImageUtils::billinearArea;
			}
			else if (XMLString::equals(interpolatorBilinearPoint, interpolatorXMLValue))
			{
				this->interpolator = RSGISExeImageUtils::billinearPt;
			}
			else if (XMLString::equals(interpolatorNN, interpolatorXMLValue))
			{
				this->interpolator = RSGISExeImageUtils::nn;
			}
			else if (XMLString::equals(interpolatorTriangular, interpolatorXMLValue))
			{
				this->interpolator = RSGISExeImageUtils::trangular;
			}
			else
			{
				throw RSGISXMLArgumentsException("Interpolator was not recognized.");
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'interpolation\' attribute was provided.");
		}
		XMLString::release(&interpolatorXMLStr);
		
		XMLString::release(&interpolatorCubic);
		XMLString::release(&interpolatorBilinearArea);
		XMLString::release(&interpolatorBilinearPoint);
		XMLString::release(&interpolatorNN);
		XMLString::release(&interpolatorTriangular);
	}
	else if (XMLString::equals(optionRasteriseDef, optionXML))
	{		
		this->option = RSGISExeImageUtils::rasterisedefiniens;
        
        this->definiensTiles = false;
        
		XMLCh *inDIRXMLStr = XMLString::transcode("inDIR");
        XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(inDIRXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(inDIRXMLStr));
			this->inputDIR = string(charValue);
			XMLString::release(&charValue);
            
            
            XMLCh *outDIRXMLStr = XMLString::transcode("outDIR");
            if(argElement->hasAttribute(outDIRXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(outDIRXMLStr));
                this->outputDIR = string(charValue);
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'outDIR\' attribute was provided.");
            }
            XMLString::release(&outDIRXMLStr);
            
            this->definiensTiles = true;
		}
		else if(argElement->hasAttribute(imageXMLStr))
		{
            
            char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
            
            XMLCh *inCSVXMLStr = XMLString::transcode("csv");
            if(argElement->hasAttribute(inCSVXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(inCSVXMLStr));
                this->inputCSV = string(charValue);
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'csv\' attribute was provided.");
            }
            XMLString::release(&inCSVXMLStr);
            
            
            XMLCh *outputXMLStr = XMLString::transcode("output");
            if(argElement->hasAttribute(outputXMLStr))
            {
                char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
                this->outputImage = string(charValue);
                XMLString::release(&charValue);
            }
            else
            {
                throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
            }
            XMLString::release(&outputXMLStr);
            
            this->definiensTiles = false;
		}
        else
		{
			throw RSGISXMLArgumentsException("No \'image\' or \'inDIR\' attribute was provided one or other is required.");
		}
		XMLString::release(&inDIRXMLStr);
		XMLString::release(&imageXMLStr);
		
		XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cout << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
	}
	else if (XMLString::equals(optionPrintProj4, optionXML))
	{		
		this->option = RSGISExeImageUtils::printProj4;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
	}
	else if (XMLString::equals(optionPrintWKT, optionXML))
	{		
		this->option = RSGISExeImageUtils::printWKT;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
	}
	else if (XMLString::equals(optionExtract2DScatterPtxt, optionXML))
	{		
		this->option = RSGISExeImageUtils::extract2dscatterptxt;
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		XMLCh *band1XMLStr = XMLString::transcode("band1");
		if(argElement->hasAttribute(band1XMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(band1XMLStr));
			this->imgBand1 = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'band1\' attribute was provided.");
		}
		XMLString::release(&band1XMLStr);

		XMLCh *band2XMLStr = XMLString::transcode("band2");
		if(argElement->hasAttribute(band2XMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(band2XMLStr));
			this->imgBand2 = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'band2\' attribute was provided.");
		}
		XMLString::release(&band2XMLStr);
				
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->numImages = 1;
			this->inputImages = new string[numImages];
			this->inputImages[0] = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			DOMElement *imageElement = NULL;
			
			DOMNodeList *imagesList = argElement->getElementsByTagName(rsgisimageXMLStr);
			this->numImages = imagesList->getLength();
			this->inputImages = new string[numImages];
			
			XMLCh *fileXMLStr = XMLString::transcode("file");
			for(int i = 0; i < numImages; i++)
			{
				imageElement = static_cast<DOMElement*>(imagesList->item(i));
				
				if(imageElement->hasAttribute(fileXMLStr))
				{
					char *charValue = XMLString::transcode(imageElement->getAttribute(fileXMLStr));
					this->inputImages[i] = string(charValue);
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'file\' attribute was provided.");
				}
			}
			XMLString::release(&fileXMLStr);
		}
		XMLString::release(&imageXMLStr);
	}
	else if (XMLString::equals(optionSGSmoothing, optionXML))
	{		
		this->option = RSGISExeImageUtils::sgsmoothing;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		XMLCh *orderXMLStr = XMLString::transcode("order");
		if(argElement->hasAttribute(orderXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(orderXMLStr));
			this->order = mathUtils.strtoint(string(charValue))+1; // Order starts at zero therefore +1
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'order\' attribute was provided.");
		}
		XMLString::release(&orderXMLStr);
		
		XMLCh *windowXMLStr = XMLString::transcode("window");
		if(argElement->hasAttribute(windowXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(windowXMLStr));
			this->window = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'window\' attribute was provided.");
		}
		XMLString::release(&windowXMLStr);

		XMLCh *imagebandsXMLStr = XMLString::transcode("imagebands");
		if(argElement->hasAttribute(imagebandsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imagebandsXMLStr));
			this->bandFloatValuesVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'imagebands\' attribute was provided.");
		}
		XMLString::release(&imagebandsXMLStr);
	}
	else if (XMLString::equals(optionCumulativeArea, optionXML))
	{		
		this->option = RSGISExeImageUtils::cumulativearea;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		XMLCh *imagebandsXMLStr = XMLString::transcode("imagebands");
		if(argElement->hasAttribute(imagebandsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imagebandsXMLStr));
			this->inMatrixfile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'imagebands\' attribute was provided.");
		}
		XMLString::release(&imagebandsXMLStr);
	}
	else if (XMLString::equals(optionCreateImage, optionXML))
	{
		this->option = RSGISExeImageUtils::createimage;
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		XMLCh *numBandsXMLStr = XMLString::transcode("numbands");
		if(argElement->hasAttribute(numBandsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(numBandsXMLStr));
			this->numBands = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'numbands\' attribute was provided.");
		}
		XMLString::release(&numBandsXMLStr);
		
		XMLCh *widthXMLStr = XMLString::transcode("width");
		if(argElement->hasAttribute(widthXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(widthXMLStr));
			this->width = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'width\' attribute was provided.");
		}
		XMLString::release(&widthXMLStr);
		
		XMLCh *heightXMLStr = XMLString::transcode("height");
		if(argElement->hasAttribute(heightXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(heightXMLStr));
			this->height = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'height\' attribute was provided.");
		}
		XMLString::release(&heightXMLStr);
		
		
		XMLCh *eastingsXMLStr = XMLString::transcode("eastings");
		if(argElement->hasAttribute(eastingsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(eastingsXMLStr));
			this->eastings = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'eastings\' attribute was provided.");
		}
		XMLString::release(&eastingsXMLStr);

		
		XMLCh *northingsXMLStr = XMLString::transcode("northings");
		if(argElement->hasAttribute(northingsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(northingsXMLStr));
			this->northings = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'northings\' attribute was provided.");
		}
		XMLString::release(&northingsXMLStr);
		
		XMLCh *valueXMLStr = XMLString::transcode("value");
		if(argElement->hasAttribute(valueXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(valueXMLStr));
			this->outValue = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'value\' attribute was provided.");
		}
		XMLString::release(&valueXMLStr);

		
		XMLCh *resolutionXMLStr = XMLString::transcode("resolution");
		if(argElement->hasAttribute(resolutionXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(resolutionXMLStr));
			this->resolution = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'resolution\' attribute was provided.");
		}
		XMLString::release(&resolutionXMLStr);
		
		
		XMLCh *proj4XMLStr = XMLString::transcode("proj4");
		if(argElement->hasAttribute(proj4XMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(proj4XMLStr));
			this->proj = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj4\' attribute was provided.");
		}
		XMLString::release(&proj4XMLStr);
		
		
	}
	else if (XMLString::equals(optionStretchImage, optionXML))
	{		
		this->option = RSGISExeImageUtils::stretch;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
				
		XMLCh *stretchLinearMinMax = XMLString::transcode("LinearMinMax");
		XMLCh *stretchLinearPercent = XMLString::transcode("LinearPercent");
		XMLCh *stretchLinearStdDev = XMLString::transcode("LinearStdDev");
		XMLCh *stretchHistogram = XMLString::transcode("Histogram");
		XMLCh *stretchExponential = XMLString::transcode("Exponential");
		XMLCh *stretchLogarithmic = XMLString::transcode("Logarithmic");
		XMLCh *stretchPowerLaw = XMLString::transcode("PowerLaw");
		
		XMLCh *stretchXMLStr = XMLString::transcode("stretch");
		if(argElement->hasAttribute(stretchXMLStr))
		{
			const XMLCh *stretchXMLValue = argElement->getAttribute(stretchXMLStr);
			if(XMLString::equals(stretchLinearMinMax, stretchXMLValue))
			{
				this->stretchType = RSGISExeImageUtils::linearMinMax;
			}
			else if (XMLString::equals(stretchLinearPercent, stretchXMLValue))
			{
				this->stretchType = RSGISExeImageUtils::linearPercent;
				
				XMLCh *percentXMLStr = XMLString::transcode("percent");
				if(argElement->hasAttribute(percentXMLStr))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(percentXMLStr));
					this->percent = mathUtils.strtofloat(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'percent\' attribute was provided.");
				}
				XMLString::release(&percentXMLStr);				
			}
			else if (XMLString::equals(stretchLinearStdDev, stretchXMLValue))
			{
				this->stretchType = RSGISExeImageUtils::linearStdDev;
				
				XMLCh *stdDevXMLStr = XMLString::transcode("stddev");
				if(argElement->hasAttribute(stdDevXMLStr))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(stdDevXMLStr));
					this->stddev = mathUtils.strtofloat(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'stddev\' attribute was provided.");
				}
				XMLString::release(&stdDevXMLStr);				
			}
			else if (XMLString::equals(stretchHistogram, stretchXMLValue))
			{
				this->stretchType = RSGISExeImageUtils::histogram;
			}
			else if (XMLString::equals(stretchExponential, stretchXMLValue))
			{
				this->stretchType = RSGISExeImageUtils::exponential;
			}
			else if (XMLString::equals(stretchLogarithmic, stretchXMLValue))
			{
				this->stretchType = RSGISExeImageUtils::logarithmic;
			}
			else if (XMLString::equals(stretchPowerLaw, stretchXMLValue))
			{
				this->stretchType = RSGISExeImageUtils::powerLaw;
				XMLCh *powerXMLStr = XMLString::transcode("power");
				if(argElement->hasAttribute(powerXMLStr))
				{
					char *charValue = XMLString::transcode(argElement->getAttribute(powerXMLStr));
					this->power = mathUtils.strtofloat(string(charValue));
					XMLString::release(&charValue);
				}
				else
				{
					throw RSGISXMLArgumentsException("No \'power\' attribute was provided.");
				}
				XMLString::release(&powerXMLStr);	
			}			
			else
			{
				throw RSGISXMLArgumentsException("Stretch was not recognized.");
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'stretch\' attribute was provided.");
		}
		XMLString::release(&stretchXMLStr);
		
		XMLString::release(&stretchLinearMinMax);
		XMLString::release(&stretchLinearPercent);
		XMLString::release(&stretchLinearStdDev);
		XMLString::release(&stretchHistogram);
		XMLString::release(&stretchExponential);
		XMLString::release(&stretchLogarithmic);
		XMLString::release(&stretchPowerLaw);
        
        XMLCh *ignoreZerosXMLStr = XMLString::transcode("ignorezeros");
		if(argElement->hasAttribute(ignoreZerosXMLStr))
		{
            XMLCh *noStr = XMLString::transcode("no");
			const XMLCh *ignoreValue = argElement->getAttribute(ignoreZerosXMLStr);
			if(XMLString::equals(ignoreValue, noStr))
			{
                this->ignoreZeros = false;
			}
			else
			{
				this->ignoreZeros = true;
			}
			XMLString::release(&noStr);
		}
		else
		{
			cerr << "No \'ignorezeros\' attribute was provided so defaulting to ignore zeros.\n";
            this->ignoreZeros = true;
		}
		XMLString::release(&ignoreZerosXMLStr);

	}
	else if (XMLString::equals(optionHueColour, optionXML))
	{		
		this->option = RSGISExeImageUtils::huecolour;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		XMLCh *bandXMLStr = XMLString::transcode("band");
		if(argElement->hasAttribute(bandXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(bandXMLStr));
			this->imgBand1 = mathUtils.strtoint(string(charValue))-1; // -1 so interface starts at 1 not 0.
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'band\' attribute was provided.");
		}
		XMLString::release(&bandXMLStr);
		
		
		XMLCh *lowValueXMLStr = XMLString::transcode("lowvalue");
		if(argElement->hasAttribute(lowValueXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(lowValueXMLStr));
			this->lowerRangeValue = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'lowvalue\' attribute was provided.");
		}
		XMLString::release(&lowValueXMLStr);
		
		XMLCh *highValueXMLStr = XMLString::transcode("highvalue");
		if(argElement->hasAttribute(highValueXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(highValueXMLStr));
			this->upperRangeValue = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'highvalue\' attribute was provided.");
		}
		XMLString::release(&highValueXMLStr);

		XMLCh *backgroundXMLStr = XMLString::transcode("background");
		if(argElement->hasAttribute(backgroundXMLStr))
		{
            XMLCh *NaNStr = XMLString::transcode("NaN");
			const XMLCh *noDataValue = argElement->getAttribute(backgroundXMLStr);
			if(XMLString::equals(noDataValue, NaNStr))
			{
                const char *val = "NaN";
				this->nodataValue = nan(val);
			}
			else
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(backgroundXMLStr));
                this->nodataValue = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
			}
			XMLString::release(&NaNStr);

		}
		else
		{
			throw RSGISXMLArgumentsException("No \'background\' attribute was provided.");
		}
		XMLString::release(&backgroundXMLStr);
		
		
	}
	else if (XMLString::equals(optionRemoveSpatialRef, optionXML))
	{		
		this->option = RSGISExeImageUtils::removespatialref;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
	}
	else if (XMLString::equals(optionAddnoise, optionXML))
	{		
		this->option = RSGISExeImageUtils::addnoise;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		XMLCh *scaleXMLStr = XMLString::transcode("scale");
		if(argElement->hasAttribute(scaleXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(scaleXMLStr));
			this->scale = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'scale\' attribute was provided.");
		}
		XMLString::release(&scaleXMLStr);
		
		XMLCh *typeXMLStr = XMLString::transcode("type");
		if(argElement->hasAttribute(typeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(typeXMLStr));
			string typeStr = string(charValue);
			if (typeStr == "percentGaussianNoise") 
			{
				this->noise = percentGaussianNoise;
			}
			else 
			{
				this->noise = randomNoise;
			}

			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'scale\' attribute was provided.");
		}
		XMLString::release(&typeXMLStr);
		
		
	}
	else if (XMLString::equals(optionDefineSpatialRef, optionXML))
	{		
		this->option = RSGISExeImageUtils::definespatialref;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		
		XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(projXMLStr));
			this->proj = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);
		
		XMLCh *tlxXMLStr = XMLString::transcode("tlx");
		if(argElement->hasAttribute(tlxXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(tlxXMLStr));
			this->eastings = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'tlx\' attribute was provided.");
		}
		XMLString::release(&tlxXMLStr);

		XMLCh *tlyXMLStr = XMLString::transcode("tly");
		if(argElement->hasAttribute(tlyXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(tlyXMLStr));
			this->northings = mathUtils.strtodouble(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'tly\' attribute was provided.");
		}
		XMLString::release(&tlyXMLStr);
		
		XMLCh *resxXMLStr = XMLString::transcode("resx");
		if(argElement->hasAttribute(resxXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(resxXMLStr));
			this->xRes = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'resx\' attribute was provided.");
		}
		XMLString::release(&resxXMLStr);
		
		XMLCh *resyXMLStr = XMLString::transcode("resy");
		if(argElement->hasAttribute(resyXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(resyXMLStr));
			this->yRes = mathUtils.strtofloat(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'resy\' attribute was provided.");
		}
		XMLString::release(&resyXMLStr);
		
	}
    else if (XMLString::equals(optionSubset, optionXML))
	{		
		this->option = RSGISExeImageUtils::subset;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		
		XMLCh *vectorXMLStr = XMLString::transcode("vector");
		if(argElement->hasAttribute(vectorXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(vectorXMLStr));
			this->inputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'vector\' attribute was provided.");
		}
		XMLString::release(&vectorXMLStr);
	}
    else if (XMLString::equals(optionSubset2Polys, optionXML))
	{		
		this->option = RSGISExeImageUtils::subset2polys;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		
		XMLCh *vectorXMLStr = XMLString::transcode("vector");
		if(argElement->hasAttribute(vectorXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(vectorXMLStr));
			this->inputVector = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'vector\' attribute was provided.");
		}
		XMLString::release(&vectorXMLStr);
		
		
		XMLCh *outfilenameXMLStr = XMLString::transcode("outfilename");
		if(argElement->hasAttribute(outfilenameXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outfilenameXMLStr));
			this->filenameAttribute = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'outfilename\' attribute was provided.");
		}
		XMLString::release(&outfilenameXMLStr);
    }
	else if (XMLString::equals(optionPanSharpen, optionXML))
	{		
		this->option = RSGISExeImageUtils::pansharpen;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		XMLCh *winSizeXMLStr = XMLString::transcode("winSize");
		if(argElement->hasAttribute(winSizeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(winSizeXMLStr));
			this->panWinSize = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			this->panWinSize = 7;
		}
		XMLString::release(&winSizeXMLStr);
	
		
	}
    else if (XMLString::equals(optionCreateSlices, optionXML))
	{		
		this->option = RSGISExeImageUtils::createslices;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);		
	}
    else if(XMLString::equals(optionClump, optionXML))
    {
        this->option = RSGISExeImageUtils::clump;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
        
        XMLCh *inMemoryXMLStr = XMLString::transcode("inmemory");
		if(argElement->hasAttribute(inMemoryXMLStr))
		{
			XMLCh *yesStr = XMLString::transcode("yes");
			const XMLCh *inMemValue = argElement->getAttribute(inMemoryXMLStr);
			
			if(XMLString::equals(inMemValue, yesStr))
			{
				this->processInMemory = true;
			}
			else
			{
				this->processInMemory = false;
			}
			XMLString::release(&yesStr);
		}
		else
		{
			cerr << "WARNING: No \'inmemory\' attribute was provided so processing from disk - this could be slow!\n";
            this->processInMemory = false;
		}
		XMLString::release(&inMemoryXMLStr);
        
        XMLCh *projXMLStr = XMLString::transcode("proj");
		if(argElement->hasAttribute(projXMLStr))
		{
			const XMLCh *projXMLValue = argElement->getAttribute(projXMLStr);
			if(XMLString::equals(projXMLValue, projImage))
			{
				this->projFromImage = true;
				this->proj = "";
			}
			else if(XMLString::equals(projXMLValue, projOSGB))
			{
				this->projFromImage = false;
				this->proj = OSGB_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ2000))
			{
				this->projFromImage = false;
				this->proj = NZ2000_Proj;
			}
            else if(XMLString::equals(projXMLValue, projNZ1949))
			{
				this->projFromImage = false;
				this->proj = NZ1949_Proj;
			}
			else
			{
				cerr << "Proj not reconized therefore defaulting to image.";
				this->projFromImage = true;
				this->proj = "";
			}
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'proj\' attribute was provided.");
		}
		XMLString::release(&projXMLStr);

    }
    else if (XMLString::equals(optionComposite, optionXML))
	{		
		this->option = RSGISExeImageUtils::imageComposite;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
		
		XMLCh *cBandsXMLStr = XMLString::transcode("compositeBands");
		if(argElement->hasAttribute(cBandsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(cBandsXMLStr));
			this->compositeBands = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'compositeBands\' attribute was provided.");
		}
		XMLString::release(&cBandsXMLStr);
		
		XMLCh *typeXMLStr = XMLString::transcode("stats");
        this->outCompStat = compositeMean; // Set to default (mean)
		if(argElement->hasAttribute(typeXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(typeXMLStr));
			string typeStr = string(charValue);
			if (typeStr == "mean") 
			{
				this->outCompStat = compositeMean;
			}
			else if (typeStr == "min") 
			{
				this->outCompStat = compositeMin;
			}
            else if (typeStr == "max") 
			{
				this->outCompStat = compositeMax;
			}
            else if (typeStr == "range") 
			{
				this->outCompStat = compositeRange;
			}
            else 
            {
                throw RSGISXMLArgumentsException("Statistics not recognised / available. Options are mean, min, max and range.");
            }

			XMLString::release(&charValue);
		}
		else
		{
			cout << "No \'stats\' attribute was provided, assuming default of mean" << endl;
		}
		XMLString::release(&typeXMLStr);
		
		
	}
    else if (XMLString::equals(optionRelabel, optionXML))
	{		
		this->option = RSGISExeImageUtils::relabel;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
        
        XMLCh *lutXMLStr = XMLString::transcode("lut");
		if(argElement->hasAttribute(lutXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(lutXMLStr));
			this->lutMatrixFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'lut\' attribute was provided.");
		}
		XMLString::release(&lutXMLStr);
	}
    else if (XMLString::equals(optionAssignProj, optionXML))
    {
        this->option = RSGISExeImageUtils::assignproj;
        
        XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
        
        
        XMLCh *projWKTXMLStr = XMLString::transcode("projwkt");
		if(argElement->hasAttribute(projWKTXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(projWKTXMLStr));
			this->projFile = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'projwkt\' attribute was provided.");
		}
		XMLString::release(&projWKTXMLStr);
    }
    else if (XMLString::equals(optionPopImgStats, optionXML))
    {
        this->option = RSGISExeImageUtils::popimgstats;
        
        XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
        
        
        XMLCh *nodataXMLStr = XMLString::transcode("ignore");
		if(argElement->hasAttribute(nodataXMLStr))
		{
            XMLCh *NaNStr = XMLString::transcode("NaN");
			const XMLCh *noDataValue = argElement->getAttribute(nodataXMLStr);
			if(XMLString::equals(noDataValue, NaNStr))
			{
                const char *val = "NaN";
				this->nodataValue = nan(val);
			}
			else
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(nodataXMLStr));
                this->nodataValue = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
			}
			XMLString::release(&NaNStr);
            
            this->useIgnoreVal = true;
		}
		else
		{
			this->useIgnoreVal = false;
		}
		XMLString::release(&nodataXMLStr);
        
        XMLCh *pyramidsXMLStr = XMLString::transcode("pyramids");
		if(argElement->hasAttribute(pyramidsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(pyramidsXMLStr));
			string typeStr = string(charValue);
			if(typeStr == "yes") 
			{
				this->calcImgPyramids = true;
			}
            else 
            {
                this->calcImgPyramids = false;
            }
            
			XMLString::release(&charValue);
		}
		else
		{
			this->calcImgPyramids = true;
		}
		XMLString::release(&pyramidsXMLStr);
        
    }
    else if (XMLString::equals(optionCreateCopy, optionXML))
	{		
		this->option = RSGISExeImageUtils::createcopy;
		
		XMLCh *imageXMLStr = XMLString::transcode("image");
		if(argElement->hasAttribute(imageXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(imageXMLStr));
			this->inputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'image\' attribute was provided.");
		}
		XMLString::release(&imageXMLStr);
		
		
		XMLCh *outputXMLStr = XMLString::transcode("output");
		if(argElement->hasAttribute(outputXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(outputXMLStr));
			this->outputImage = string(charValue);
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'output\' attribute was provided.");
		}
		XMLString::release(&outputXMLStr);
        
        
        XMLCh *pixelValXMLStr = XMLString::transcode("pixelval");
		if(argElement->hasAttribute(pixelValXMLStr))
		{
            XMLCh *NaNStr = XMLString::transcode("NaN");
			const XMLCh *dataValue = argElement->getAttribute(pixelValXMLStr);
			if(XMLString::equals(dataValue, NaNStr))
			{
                const char *val = "NaN";
				this->dataValue = nan(val);
			}
			else
			{
				char *charValue = XMLString::transcode(argElement->getAttribute(pixelValXMLStr));
                this->dataValue = mathUtils.strtofloat(string(charValue));
                XMLString::release(&charValue);
			}
			XMLString::release(&NaNStr);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'pixelval\' attribute was provided.");
		}
		XMLString::release(&pixelValXMLStr);
        
        XMLCh *numBandsXMLStr = XMLString::transcode("numbands");
		if(argElement->hasAttribute(numBandsXMLStr))
		{
			char *charValue = XMLString::transcode(argElement->getAttribute(numBandsXMLStr));
			this->numBands = mathUtils.strtoint(string(charValue));
			XMLString::release(&charValue);
		}
		else
		{
			throw RSGISXMLArgumentsException("No \'numbands\' attribute was provided.");
		}
		XMLString::release(&numBandsXMLStr);
        
	}
	else
	{
		string message = string("The option (") + string(XMLString::transcode(optionXML)) + string(") is not known: RSGISExeImageUtils.");
		throw RSGISXMLArgumentsException(message.c_str());
	}
	
	XMLString::release(&algorName);
	XMLString::release(&algorXMLStr);
	XMLString::release(&optionXMLStr);
	XMLString::release(&optionColour);
	XMLString::release(&optionMosaic);
	XMLString::release(&optionInclude);
	XMLString::release(&optionCut2Poly);
    XMLString::release(&optionCut2Polys);
	XMLString::release(&optionMask);
	XMLString::release(&optionResample);
	XMLString::release(&optionRasteriseDef);
	XMLString::release(&projImage);
	XMLString::release(&projOSGB);
    XMLString::release(&projNZ2000);
    XMLString::release(&projNZ1949);
	XMLString::release(&rsgisimageXMLStr);
	XMLString::release(&optionPrintProj4);
	XMLString::release(&optionPrintWKT);
	XMLString::release(&optionExtract2DScatterPtxt);
	XMLString::release(&optionSGSmoothing);
	XMLString::release(&optionCumulativeArea);
	XMLString::release(&optionCreateImage);
	XMLString::release(&optionStretchImage);
	XMLString::release(&optionHueColour);
	XMLString::release(&optionRemoveSpatialRef);
	XMLString::release(&optionAddnoise);
	XMLString::release(&optionDefineSpatialRef);
    XMLString::release(&optionSubset);
    XMLString::release(&optionSubset2Polys);
    XMLString::release(&optionPanSharpen);
    XMLString::release(&optionColourImageBands);
    XMLString::release(&optionCreateSlices);
    XMLString::release(&optionClump);
    XMLString::release(&optionComposite);
    XMLString::release(&optionRelabel);
    XMLString::release(&optionAssignProj);
    XMLString::release(&optionPopImgStats);
	XMLString::release(&optionCreateCopy);
    
	parsed = true;
}

void RSGISExeImageUtils::runAlgorithm() throw(RSGISException)
{
	if(!parsed)
	{
		throw RSGISException("Before running the parameters much be retrieved");
	}
	else
	{
		if(option == RSGISExeImageUtils::colour)
		{
			cout << "Colouring image\n";
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISColourUpImage *colourImage = NULL;
			RSGISCalcImage *calcImage = NULL;

			int numBands = 0;
			try
			{
				datasets = new GDALDataset*[1];
				cout << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				numBands = datasets[0]->GetRasterCount();
				
				for(int i = 0; i < this->numClasses; i++)
				{
					classColour[i]->numInputBands = numBands;
					
					cout << i <<  ") Class " << classColour[i]->className << " with ID ";
					cout << classColour[i]->classID << " uses image band " << classColour[i]->imgBand;
					cout << " where a total of " << classColour[i]->numInputBands << " is available:\n";
					cout << "Lower = " << classColour[i]->lower << endl;
					cout << "Upper = " << classColour[i]->upper << endl;
					cout << "Red = " << classColour[i]->red << endl;
					cout << "Green = " << classColour[i]->green << endl;
					cout << "Blue = " << classColour[i]->blue << endl;
				}
				
				colourImage = new RSGISColourUpImage(3, this->classColour, this->numClasses);
				calcImage = new RSGISCalcImage(colourImage, "", true);
				
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				if(datasets != NULL)
				{
					GDALClose(datasets[0]);
					delete[] datasets;
				}
				GDALDestroyDriverManager();
				delete calcImage;
				delete colourImage;
			}
			catch (RSGISException e) 
			{
				cout << "Exception occured: " << e.what() << endl;
			}
			
		}
        else if(option == RSGISExeImageUtils::colourimagebands)
		{
			cout << "Colouring image bands\n";
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISColourUpImageBand *colourImage = NULL;
			RSGISCalcImage *calcImage = NULL;
            
			int numBands = 0;
			try
			{
				datasets = new GDALDataset*[1];
				cout << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				numBands = datasets[0]->GetRasterCount();
				
				for(int i = 0; i < this->numClasses; i++)
				{
					classColour[i]->numInputBands = numBands;
					
					cout << i <<  ") Class " << classColour[i]->className << " with ID ";
					cout << classColour[i]->classID << endl;
					cout << "Lower = " << classColour[i]->lower << endl;
					cout << "Upper = " << classColour[i]->upper << endl;
					cout << "Red = " << classColour[i]->red << endl;
					cout << "Green = " << classColour[i]->green << endl;
					cout << "Blue = " << classColour[i]->blue << endl;
				}
				
				colourImage = new RSGISColourUpImageBand(3, this->classColour, this->numClasses);
				calcImage = new RSGISCalcImage(colourImage, "", true);
				
				calcImage->calcImageBand(datasets, 1, this->outputImage);
				
				if(datasets != NULL)
				{
					GDALClose(datasets[0]);
					delete[] datasets;
				}
				GDALDestroyDriverManager();
				delete calcImage;
				delete colourImage;
			}
			catch (RSGISException e) 
			{
				cout << "Exception occured: " << e.what() << endl;
			}
			
		}
		else if(option == RSGISExeImageUtils::mosaic)
		{
			cout << "Mosaicing Images\n";
			RSGISImageMosaic mosaic;
			try
			{
				if (this->mosaicSkipVals) 
				{
					mosaic.mosaicSkipVals(inputImages, this->numImages, this->outputImage, this->nodataValue, this->skipValue, this->projFromImage, this->proj, this->skipBand);
				}
				else if (this->mosaicSkipThreash)
				{
					mosaic.mosaicSkipThreash(inputImages, this->numImages, this->outputImage, this->nodataValue, this->skipLowerThreash, this->skipUpperThreash, this->projFromImage, this->proj, this->skipBand);
				}
				else 
				{
					mosaic.mosaic(inputImages, this->numImages, this->outputImage, this->nodataValue, this->projFromImage, this->proj);
				}
								
				delete[] inputImages;
			}
			catch(RSGISException e) 
			{
				throw e;
			}
			
		}
		else if(option == RSGISExeImageUtils::include)
		{
            cout << "Include images into a larger image\n";
            if(bandsDefined)
            {
                for(vector<int>::iterator iterBands = bands.begin(); iterBands != bands.end(); ++iterBands)
                {
                    cout << "Band " << *iterBands << endl;
                }
            }
			GDALAllRegister();
			GDALDataset *baseDS = NULL;
			RSGISImageMosaic mosaic;
			try
			{
				cout << this->inputImage << endl;
				baseDS = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_Update);
				if(baseDS == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				mosaic.includeDatasets(baseDS, this->inputImages, this->numImages, this->bands, this->bandsDefined);
				
				GDALClose(baseDS);
				GDALDestroyDriverManager();
				delete[] inputImages;
			}
			catch (RSGISException e) 
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageUtils::cut2poly)
		{
			cout << "Cutting image to polygons\n";
			GDALAllRegister();
			OGRRegisterAll();
			
			GDALDataset **dataset = NULL;
			OGRDataSource *inputVecDS = NULL;
			OGRLayer *inputVecLayer = NULL;
			
			RSGISCopyImage *copyImage = NULL;
			RSGISCalcImage *calcImage = NULL;
			
			RSGISVectorIO vecIO;
			RSGISPolygonData **polyData = NULL;
			RSGISImageTileVector **data = NULL;
			RSGISVectorUtils vecUtils;
			
			string vectorLayerName = vecUtils.getLayerName(this->inputVector);
			int numImageBands = 0;
			int numFeatures = 0;
			string outputFilePath;
			
			try
			{
				// Open Image
				dataset = new GDALDataset*[1];
				cout << this->inputImage << endl;
				dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(dataset[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				numImageBands = dataset[0]->GetRasterCount();
				cout << "Raster Band Count = " << numImageBands << endl;
				
				// Open vector
				inputVecDS = OGRSFDriverRegistrar::Open(this->inputVector.c_str(), FALSE);
				if(inputVecDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputVecLayer = inputVecDS->GetLayerByName(vectorLayerName.c_str());
				if(inputVecLayer == NULL)
				{
					string message = string("Could not open vector layer ") + vectorLayerName;
					throw RSGISFileException(message.c_str());
				}
				
				// READ IN SHAPEFILE
				numFeatures = inputVecLayer->GetFeatureCount();
				polyData = new RSGISPolygonData*[numFeatures];
				for(int i = 0; i < numFeatures; i++)
				{
					polyData[i] = new RSGISImageTileVector(this->filenameAttribute);
				}
				cout << "Reading in " << numFeatures << " features\n";
				vecIO.readPolygons(inputVecLayer, polyData, numFeatures);
				
				//Convert to RSGISImageTileVector
				data = new RSGISImageTileVector*[numFeatures];
				for(int i = 0; i < numFeatures; i++)
				{
					data[i] = dynamic_cast<RSGISImageTileVector*>(polyData[i]);
				}
				delete[] polyData;
				
				copyImage = new RSGISCopyImage(numImageBands);
				calcImage = new RSGISCalcImage(copyImage, "", true);
				
                unsigned int failCount = 0;
                
				for(int i = 0; i < numFeatures; i++)
				{
					outputFilePath = this->outputImage + data[i]->getFileName() + ".env";
					cout << i << ": " << outputFilePath << endl;
                    try
                    {
                        calcImage->calcImageWithinPolygon(dataset, 1, outputFilePath, data[i]->getBBox(), data[i]->getPolygon(), this->nodataValue, polyContainsPixelCenter);
                    }
                    catch (RSGISImageBandException e)
                    {
                        ++failCount;
                        if(failCount <= 100)
                        {
                            cerr << "RSGISException caught: " << e.what() << endl;
                            cerr << "Check output path exists and is writable and all polygons in shapefile:" << endl;
                            cerr << " " << this->inputVector << endl;
                            cerr << "Are completely within:" << endl;
                            cerr << " " << this->inputImage << endl;
                        }
                        else
                        {
                            cerr << "Over 100 exceptions have been caught, exiting" << endl;
                            throw e;
                        }
                    }
				}
				
				GDALClose(dataset[0]);
				delete[] dataset;
				OGRDataSource::DestroyDataSource(inputVecDS);
				OGRCleanupAll();
				GDALDestroyDriverManager();
				delete calcImage;
				delete copyImage;
			}
			catch(RSGISException e)
			{
				cerr << "RSGISException caught: " << e.what() << endl;
			}
		}
		else if(option == RSGISExeImageUtils::mask)
		{
			GDALAllRegister();
			RSGISMaskImage *maskImage = NULL;
			GDALDataset *dataset = NULL;
			GDALDataset *mask = NULL;
			try
			{
				cout << this->inputImage << endl;
				dataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(dataset == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				cout << this->imageMask << endl;
				mask = (GDALDataset *) GDALOpenShared(this->imageMask.c_str(), GA_ReadOnly);
				if(mask == NULL)
				{
					string message = string("Could not open image ") + this->imageMask;
					throw RSGISImageException(message.c_str());
				}
				
				maskImage = new RSGISMaskImage();
				maskImage->maskImage(dataset, mask, this->outputImage, this->imageFormat, this->outDataType, this->nodataValue);
				
				GDALClose(dataset);
				GDALClose(mask);
				delete maskImage;
				GDALDestroyDriverManager();
			}
			catch(RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageUtils::resample)
		{
			GDALAllRegister();
			
			RSGISImageInterpolation *interpolation = NULL;
			RSGISInterpolator *interpolator = NULL;
			
			GDALDataset *inDataset = NULL;
			
			int xOutResolution = 0;
			int yOutResolution = 0;
			
			try
			{
				if(this->interpolator == RSGISExeImageUtils::cubic)
				{
					cout << "Using a cubic interpolator\n";
					interpolator = new RSGISCubicInterpolator();
				}
				else if(this->interpolator == RSGISExeImageUtils::billinearArea)
				{
					cout << "Using a bilinear (area) interpolator\n";
					interpolator = new RSGISBilinearAreaInterpolator();
				}
				else if(this->interpolator == RSGISExeImageUtils::billinearPt)
				{
					cout << "Using a bilinear (point) interpolator\n";
					interpolator = new RSGISBilinearPointInterpolator();
				}
				else if(this->interpolator == RSGISExeImageUtils::nn)
				{
					cout << "Using a nearest neighbour interpolator\n";
					interpolator = new RSGISNearestNeighbourInterpolator();
				}
				else if(this->interpolator == RSGISExeImageUtils::trangular)
				{
					cout << "Using a triangular interpolator\n";
					interpolator = new RSGISTriangulationInterpolator();
				}
				else
				{
					throw RSGISException("Interpolator Option Not Reconised");
				}
				
				interpolation = new RSGISImageInterpolation(interpolator);
				
				cout << this->inputImage << endl;
				inDataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(inDataset == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				interpolation->findOutputResolution(inDataset, this->resampleScale, &xOutResolution, &yOutResolution);
				
				interpolation->interpolateNewImage(inDataset, xOutResolution, yOutResolution, this->outputImage);
				
				GDALClose(inDataset);

				delete interpolation;
				delete interpolator;
				GDALDestroyDriverManager();
								
			}
			catch(RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageUtils::rasterisedefiniens)
		{
			GDALAllRegister();
			RSGISFileUtils fileUtils;
			RSGISDefiniensCSVRasterise rasterisedefiniensCSV = RSGISDefiniensCSVRasterise(projFromImage, proj);
			cout << "Rasterise Definiens\n";
			
            if(this->definiensTiles)
            {
                cout << "Input DIR: " << this->inputDIR << endl;
                cout << "Output DIR: " << this->outputDIR << endl;
                string tif = string(".tif");
                string csv = string(".CSV");
                
                string *inputTIF = NULL;
                int numTIFs = 0;
                
                string *inputCSV = NULL;
                int numCSVs = 0;
                
                SortedGenericList<RSGISDefiniensWorkspaceFileName> *sortedTIFs = NULL;
                SortedGenericList<RSGISDefiniensWorkspaceFileName> *sortedCSVs = NULL;
                
                try
                {
                    inputTIF = fileUtils.getDIRList(this->inputDIR, tif, &numTIFs, false);
                    inputCSV = fileUtils.getDIRList(this->inputDIR, csv, &numCSVs, false);
                    
                    cout << "numTIFs = " << numTIFs << endl;
                    cout << "numCSVs = " << numCSVs << endl;
                    
                    if(numTIFs != numCSVs)
                    {
                        throw RSGISException("number of TIFs and CSV files found do not match.");
                    }
                    
                    if(numTIFs == 0)
                    {
                        throw RSGISException("No input files were found.");
                    }
                    
                    sortedTIFs = new SortedGenericList<RSGISDefiniensWorkspaceFileName>(numTIFs, numTIFs/2);
                    sortedCSVs = new SortedGenericList<RSGISDefiniensWorkspaceFileName>(numCSVs, numCSVs/2);
                    
                    for(int i = 0; i < numTIFs; i++)
                    {
                        //cout << i << ")\t" << inputTIF[i] << "\t" << inputCSV[i] << endl;
                        sortedTIFs->add(new RSGISDefiniensWorkspaceFileName(inputTIF[i]));
                        sortedCSVs->add(new RSGISDefiniensWorkspaceFileName(inputCSV[i]));
                    }
                    
                    sortedTIFs->printAsc();
                    sortedCSVs->printAsc();
                    
                    //Check basename is the same:
                    string basename = sortedTIFs->peekTop()->getBaseName();
                    //cout << "BaseName = " << basename << endl;
                    int size = sortedCSVs->getSize();
                    for(int i = 0; i < size; i++)
                    {
                        if(sortedTIFs->getAt(i)->getBaseName() != basename)
                        {
                            throw RSGISException("Base filenames are not the same.");
                        }
                        
                        if(sortedCSVs->getAt(i)->getBaseName() != basename)
                        {
                            throw RSGISException("Base filenames are not the same.");
                        }
                    }
                    
                    cout << "Base filenames match\n";
                    int oldTIFs = 0;

                    for(int i = size-1; i > 0; i--)
                    {
                        if(sortedTIFs->getAt(i)->getTile() == sortedTIFs->getAt(i-1)->getTile())
                        {
                            if(sortedTIFs->getAt(i)->getVersion() == sortedTIFs->getAt(i-1)->getVersion())
                            {
                                throw RSGISException("The tile has the same number and version.");
                            }
                            else
                            {
                                sortedTIFs->getAt(i)->setOldVersion(true);
                                oldTIFs++;
                            }
                        }
                    }
                    
                    int oldCSVs = 0;
                    for(int i = size-1; i > 0; i--)
                    {
                        if(sortedCSVs->getAt(i)->getTile() == sortedCSVs->getAt(i-1)->getTile())
                        {
                            if(sortedCSVs->getAt(i)->getVersion() == sortedCSVs->getAt(i-1)->getVersion())
                            {
                                throw RSGISException("The tile has the same number and version.");
                            }
                            else
                            {
                                sortedCSVs->getAt(i)->setOldVersion(true);
                                oldCSVs++;
                            }
                        }
                    }
                    
                    if(oldTIFs != oldCSVs)
                    {
                        throw RSGISException("A different number of old version tiles were identified in the TIF and CSV lists.");
                    }
                    
                    cout << oldTIFs << " old versions of tiles have been identified and will be ignored.\n";
                    // sortedTIFs->printAsc();
                    // sortedCSVs->printAsc();
                    
                    for(int i = size-1; i >= 0; i--)
                    {
                        if(sortedTIFs->getAt(i)->getTile() != sortedCSVs->getAt(i)->getTile())
                        {
                            throw RSGISException("TIF AND CSV lists are different. (tile number)");
                        }
                        else if(sortedTIFs->getAt(i)->getVersion() != sortedCSVs->getAt(i)->getVersion())
                        {
                            throw RSGISException("TIF AND CSV lists are different. (version number)");
                        }
                        else if(sortedTIFs->getAt(i)->getOldVersion() != sortedCSVs->getAt(i)->getOldVersion())
                        {
                            throw RSGISException("TIF AND CSV lists are different. (old version)");
                        }
                    }
                    cout << "Files have been checked and the corrsponding TIF and CSV files are present.\n";
                    cout << "Starting Rasterisation...\n";
                    
                    rasterisedefiniensCSV.rasteriseTiles(sortedTIFs, sortedCSVs, outputDIR);
                    
                    
                    sortedTIFs->clearListDelete();
                    delete sortedTIFs;
                    sortedCSVs->clearListDelete();
                    delete sortedCSVs;
                    
                    delete[] inputTIF;
                    delete[] inputCSV;
                }
                catch(RSGISException& e)
                {
                    throw e;
                }
            }
            else
            {
                cout << "Input Image: " << this->inputImage << endl;
                cout << "Input CSV: " << this->inputCSV << endl;
                cout << "Output Image: " << this->outputImage << endl;

                try
                {
                    rasterisedefiniensCSV.rasteriseFile(this->inputImage, this->inputCSV, this->outputImage);
                }
                catch(RSGISException& e)
                {
                    throw e;
                }
            }
            GDALDestroyDriverManager();
            cout << "Finished Rasterisation\n";
		}
		else if(option == RSGISExeImageUtils::printProj4)
		{
			cout << "Print Spatial Reference Proj4\n";
			cout << "Input Image: " << this->inputImage << endl;
			GDALAllRegister();
			GDALDataset *inDataset = NULL;
			
			try
			{
				inDataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(inDataset == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				const char *wtkSpatialRef = inDataset->GetProjectionRef();
				OGRSpatialReference ogrSpatial = OGRSpatialReference(wtkSpatialRef);
				
				char **proj4spatialref = new char*[1];
				proj4spatialref[0] = new char[1000];
				ogrSpatial.exportToProj4(proj4spatialref);
				cout << proj4spatialref[0] << endl;
				
				GDALClose(inDataset);
				GDALDestroyDriverManager();
				
			}
			catch(RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageUtils::printWKT)
		{
			cout << "Print Spatial Reference WKT\n";
			cout << "Input Image: " << this->inputImage << endl;
			
			GDALAllRegister();
			GDALDataset *inDataset = NULL;
			
			try
			{
				inDataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(inDataset == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				const char *wtkSpatialRef = inDataset->GetProjectionRef();
				
				cout << wtkSpatialRef << endl;
				
				GDALClose(inDataset);
				GDALDestroyDriverManager();
				
			}
			catch(RSGISException& e)
			{
				throw e;
			}
		}
		else if(option == RSGISExeImageUtils::extract2dscatterptxt)
		{
			cout << "Extract image data to create 2D scatter plot (exported as ptxt)\n";
			
			cout << "Input Images:\n";
			for(int i = 0; i < this->numImages; ++i)
			{
				cout << i << ") " << this->inputImages[i] << endl;
			}
			cout << "Image Band 1: " << this->imgBand1 << endl;
			cout << "Image Band 2: " << this->imgBand2 << endl;
			cout << "Output File: " << this->outputFile << endl;
			
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISExport2DScatterPTxt *export2DScatter = NULL;
			RSGISCalcImage *calcImage = NULL;
			
			try
			{
				datasets = new GDALDataset*[numImages];
				
				for(int i = 0; i < numImages; ++i)
				{
					datasets[i] = (GDALDataset *) GDALOpenShared(this->inputImages[i].c_str(), GA_ReadOnly);
					if(datasets[i] == NULL)
					{
						string message = string("Could not open image ") + this->inputImages[i];
						throw RSGISImageException(message.c_str());
					}
				}
					
				RSGISExportForPlottingIncremental *plotter = new RSGISExportForPlottingIncremental();
				plotter->openFile(this->outputFile, scatter2d);
				export2DScatter = new RSGISExport2DScatterPTxt(plotter, imgBand1, imgBand2);
				calcImage = new RSGISCalcImage(export2DScatter, "", true);
				
				calcImage->calcImage(datasets, numImages);
				
				plotter->close();
				delete plotter;
				
				
				for(int i = 0; i < numImages; ++i)
				{
					GDALClose(datasets[i]);
				}
				delete[] datasets;
				
				GDALDestroyDriverManager();
				delete calcImage;
				delete export2DScatter;
			}
			catch (RSGISException e) 
			{
				cout << "Exception occured: " << e.what() << endl;
			}
			
			
			
		}
		else if(option == RSGISExeImageUtils::sgsmoothing)
		{
			cout << "Apply Savitzky-Golay Smoothing Filters to data\n";
			
			cout << "Input Image = " << this->inputImage << endl;
			cout << "Output Image = " << this->outputImage << endl;
			cout << "Image Band Values = " << this->bandFloatValuesVector << endl;
			cout << "Order = " << this->order << endl;
			cout << "Window = " << this->window << endl;
			
			GDALAllRegister();
			
			RSGISVectors vectorUtils;
			
			GDALDataset **datasets = NULL;
			
			RSGISCalcImage *calcImage = NULL;
			RSGISCalcImageValue *calcImageValue = NULL;
			
			try 
			{
				if(order > window)
				{
					throw RSGISException("The window size needs to be at least as large as the order");
				}
				
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				Vector *bandValues = vectorUtils.readVectorFromTxt(this->bandFloatValuesVector);
				
				cout << "Input Image band values:\n";
				vectorUtils.printVector(bandValues);
				
				int numInBands = datasets[0]->GetRasterCount();
				
				calcImageValue = new RSGISSavitzkyGolaySmoothingFilters(numInBands, this->order, this->window, bandValues);
				calcImage = new RSGISCalcImage(calcImageValue, "", true);
				
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				delete calcImage;
				delete calcImageValue;
				
				vectorUtils.freeVector(bandValues);
				
				GDALClose(datasets[0]);
				delete[] datasets;
			}
			catch(RSGISException& e)
			{
				throw e;
			}
			GDALDestroyDriverManager();
		}
		else if(option == RSGISExeImageUtils::cumulativearea)
		{
			cout << "Calculate the cumulative area of the image profile/spectra\n";
			cout << "Input Image = " << this->inputImage << endl;
			cout << "Output Image = " << this->outputImage << endl;
			cout << "Image Band Widths = " << this->bandFloatValuesVector << endl;
			
			GDALAllRegister();
			
			RSGISMatrices matixUtils;
			
			GDALDataset **datasets = NULL;
			
			RSGISCalcImage *calcImage = NULL;
			RSGISCalcImageValue *calcImageValue = NULL;
			
			try 
			{
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				Matrix *bandValues = matixUtils.readMatrixFromTxt(this->inMatrixfile);
		
				int numInBands = datasets[0]->GetRasterCount();
				
				calcImageValue = new RSGISCumulativeArea(numInBands, bandValues);
				calcImage = new RSGISCalcImage(calcImageValue, "", true);
				
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				delete calcImage;
				delete calcImageValue;
				
				matixUtils.freeMatrix(bandValues);
				
				GDALClose(datasets[0]);
				delete[] datasets;
			}
			catch(RSGISException& e)
			{
				throw e;
			}
			GDALDestroyDriverManager();
		}
		else if(option == RSGISExeImageUtils::createimage)
		{
			cout << "Create a new blank image\n";
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Num. Image bands: " << this->numBands << endl;
			cout << "Size: [" << this->width << "," << this->height << "]\n";
			cout << "TL Geo: [" << this->eastings << "," << this->northings << "]\n";
			cout << "Resolution: " << this->resolution << endl;
			cout << "Default Value: " << this->outValue << endl;
			cout << "Projection: " << this->proj << endl;
			
			RSGISImageUtils imgUtils;
			try 
			{
				GDALAllRegister();
				double *transformation = new double[6];
				transformation[0] = eastings;
				transformation[1] = resolution;
				transformation[2] = 0;
				transformation[3] = northings;
				transformation[4] = 0;
				transformation[5] = resolution * (-1);
				
				string projection = "";
				if(proj != "")
				{
					OGRSpatialReference ogrSpatial = OGRSpatialReference();
					ogrSpatial.importFromProj4(proj.c_str());
					
					char **wktspatialref = new char*[1];
					wktspatialref[0] = new char[10000];
					ogrSpatial.exportToWkt(wktspatialref);			
					projection = string(wktspatialref[0]);
					OGRFree(wktspatialref);
				}
				
				GDALDataset* outImage = imgUtils.createBlankImage(outputImage, transformation, width, height, numBands, projection, outValue);
				GDALClose(outImage);
				GDALDestroyDriverManager();
			}
			catch(RSGISImageBandException &e)
			{
				throw RSGISException(e.what());
			}
			catch (RSGISImageException &e) 
			{
				throw RSGISException(e.what());
			}
		}
		else if(option == RSGISExeImageUtils::stretch)
		{
			cout << "Apply an enhancement stretch to the an input image - usually for visualisation\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			if(stretchType == linearMinMax)
			{
				cout << "Linear Min-Max stretch\n";
			}
			else if(stretchType == linearPercent)
			{
				cout << "Linear " << percent << " % stretch\n";
			}
			else if(stretchType == linearStdDev)
			{
				cout << "Linear " << stddev << " Standard Deviation stretch\n";
			}
			else if(stretchType == histogram)
			{
				cout << "Histogram stretch\n";
			}
			else if(stretchType == exponential)
			{
				cout << "Exponential stretch\n";
			}
			else if(stretchType == logarithmic)
			{
				cout << "Logarithmic stretch\n";
			}
			else if(stretchType == powerLaw)
			{
				cout << power << " Power Law stretch\n";
			}
			else
			{
				throw RSGISException("Stretch is not recognised.");
			}
            if(this->ignoreZeros)
            {
                cout << "Ignoring Zeros\n";
            }
			
			
			try
			{
				GDALAllRegister();
				RSGISStretchImage *stretchImg = NULL;
				GDALDataset *inDataset = NULL;
				
				inDataset = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(inDataset == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}

				stretchImg = new RSGISStretchImage(inDataset, this->outputImage, this->ignoreZeros, this->imageFormat, this->outDataType);
				if(stretchType == linearMinMax)
				{
					stretchImg->executeLinearMinMaxStretch();
				}
				else if(stretchType == linearPercent)
				{
					stretchImg->executeLinearPercentStretch(this->percent);
				}
				else if(stretchType == linearStdDev)
				{
					stretchImg->executeLinearStdDevStretch(this->stddev);
				}
				else if(stretchType == histogram)
				{
					stretchImg->executeHistogramStretch();
				}
				else if(stretchType == exponential)
				{
					stretchImg->executeExponentialStretch();
				}
				else if(stretchType == logarithmic)
				{
					stretchImg->executeLogrithmicStretch();
				}
				else if(stretchType == powerLaw)
				{
					stretchImg->executePowerLawStretch(power);
				}
				else
				{
					throw RSGISException("Stretch is not recognised.");
				}
				
				GDALClose(inDataset);
				GDALDestroyDriverManager();
				delete stretchImg;
			}
			catch(RSGISException& e)
			{
				throw e;
			}
			
		}
		else if(option == RSGISExeImageUtils::huecolour)
		{
			cout << "Generate a colour image (through Hue) representing an image band - usually for visualisation\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Band: " << this->imgBand1 << endl;
			cout << "Range: [" << this->lowerRangeValue << "," << this->upperRangeValue << "]\n";
			cout << "Background value: " << this->nodataValue << endl;
			
			GDALAllRegister();
			
			RSGISCalcImage *calcImage = NULL;
			RSGISCalcImageValue *calcImageValue = NULL;
			
			try
			{
				GDALDataset **datasets = NULL;
				
				datasets = new GDALDataset*[1];
				datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				
				if(this->imgBand1 >= datasets[0]->GetRasterCount())
				{
					throw RSGISImageException("Insufficient number of bands in the input image for select band.");
				}
				
				calcImageValue = new RSGISColourUsingHue(3, this->imgBand1, this->lowerRangeValue, this->upperRangeValue, this->nodataValue);
				calcImage = new RSGISCalcImage(calcImageValue, "", true);
				
				calcImage->calcImage(datasets, 1, this->outputImage);
				
				delete calcImage;
				delete calcImageValue;
								
				GDALClose(datasets[0]);
				delete[] datasets;
				GDALDestroyDriverManager();
			}
			catch(RSGISException& e)
			{
				throw e;
			}
			
		}
		else if(option == RSGISExeImageUtils::removespatialref)
		{
			cout << "Remove / define spatial reference to nothing and size set to pixel size\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
						
			try
			{
				RSGISImageUtils imgUtils;
				imgUtils.copyImageRemoveSpatialReference(inputImage, outputImage);
			}
			catch(RSGISException& e)
			{
				throw e;
			}
			
		}
		else if(option == RSGISExeImageUtils::addnoise)
		{
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			try
			{
				datasets = new GDALDataset*[1];
				cout << this->inputImage << endl;
				datasets[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(datasets[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
			}
			catch(RSGISException& e)
			{
				throw e;
			}
			RSGISCalcImage *calcImg = NULL;
			
			if (this->noise == percentGaussianNoise ) 
			{
				cout << "Adding " << this->scale * 100 << "% Gaussian Noise" << endl;
				RSGISAddRandomGaussianNoisePercent *addNoise = NULL;
				try
				{
					addNoise = new RSGISAddRandomGaussianNoisePercent(datasets[0]->GetRasterCount(),this->scale);
					calcImg = new RSGISCalcImage(addNoise, "", true);
					calcImg->calcImage(datasets, 1, this->outputImage);
					delete addNoise;
				}
				catch(RSGISException& e)
				{
					throw e;
				}
			}
			
			else
			{
				cout << "Adding random noise" << endl;
				RSGISAddRandomNoise *addNoise = NULL;
				try
				{
					addNoise = new RSGISAddRandomNoise(datasets[0]->GetRasterCount(),this->scale);
					calcImg = new RSGISCalcImage(addNoise, "", true);
					calcImg->calcImage(datasets, 1, this->outputImage);
					delete addNoise;
				}
				catch(RSGISException& e)
				{
					throw e;
				}
			}
			
			if(datasets != NULL)
			{
				GDALClose(datasets[0]);
				delete[] datasets;
			}
			delete calcImg;
						
		}
		else if(option == RSGISExeImageUtils::definespatialref)
		{
			cout << "Make a copy of the input image and define the projection and spatial locations\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Projection: " << this->proj << endl;
			cout << "TL [" << this->eastings << "," << this->northings << "]\n";
			cout << "Resolution [" << this->xRes << "," << this->yRes << "]\n";
			
			try
			{
				RSGISImageUtils imgUtils;
				imgUtils.copyImageDefiningSpatialReference(inputImage, outputImage, this->proj, this->eastings, this->northings, this->xRes, this->yRes);
			}
			catch(RSGISException& e)
			{
				throw e;
			}
		}
        else if(option == RSGISExeImageUtils::subset)
		{
			cout << "Subset image to vector\n";
			GDALAllRegister();
			OGRRegisterAll();
			
			GDALDataset **dataset = NULL;
			OGRDataSource *inputVecDS = NULL;
			OGRLayer *inputVecLayer = NULL;
			
			RSGISCopyImage *copyImage = NULL;
			RSGISCalcImage *calcImage = NULL;
            
			RSGISVectorUtils vecUtils;
			
			string vectorLayerName = vecUtils.getLayerName(this->inputVector);
			int numImageBands = 0;
			
			try
			{
				// Open Image
				dataset = new GDALDataset*[1];
				cout << this->inputImage << endl;
				dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(dataset[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				numImageBands = dataset[0]->GetRasterCount();
				cout << "Raster Band Count = " << numImageBands << endl;
				
				// Open vector
				inputVecDS = OGRSFDriverRegistrar::Open(this->inputVector.c_str(), FALSE);
				if(inputVecDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputVecLayer = inputVecDS->GetLayerByName(vectorLayerName.c_str());
				if(inputVecLayer == NULL)
				{
					string message = string("Could not open vector layer ") + vectorLayerName;
					throw RSGISFileException(message.c_str());
				}
                OGREnvelope ogrExtent;
                inputVecLayer->GetExtent(&ogrExtent);
                Envelope extent = Envelope(ogrExtent.MinX, ogrExtent.MaxX, ogrExtent.MinY, ogrExtent.MaxY);
				
				copyImage = new RSGISCopyImage(numImageBands);
				calcImage = new RSGISCalcImage(copyImage, "", true);
                calcImage->calcImageInEnv(dataset, 1, outputImage, &extent);
				
				GDALClose(dataset[0]);
				delete[] dataset;
				OGRDataSource::DestroyDataSource(inputVecDS);
				OGRCleanupAll();
				GDALDestroyDriverManager();
				delete calcImage;
				delete copyImage;
			}
			catch(RSGISException e)
			{
				cout << "RSGISException caught: " << e.what() << endl;
			}
		}
        else if(option == RSGISExeImageUtils::subset2polys)
		{
			cout << "Subset image to bounding box of polygons\n";
			GDALAllRegister();
			OGRRegisterAll();
			
			GDALDataset **dataset = NULL;
			OGRDataSource *inputVecDS = NULL;
			OGRLayer *inputVecLayer = NULL;
			
			RSGISCopyImage *copyImage = NULL;
			RSGISCalcImage *calcImage = NULL;
			
			RSGISVectorIO vecIO;
			RSGISPolygonData **polyData = NULL;
			RSGISImageTileVector **data = NULL;
			RSGISVectorUtils vecUtils;
			
			string vectorLayerName = vecUtils.getLayerName(this->inputVector);
			int numImageBands = 0;
			int numFeatures = 0;
			string outputFilePath;
			
			try
			{
				// Open Image
				dataset = new GDALDataset*[1];
				cout << this->inputImage << endl;
				dataset[0] = (GDALDataset *) GDALOpenShared(this->inputImage.c_str(), GA_ReadOnly);
				if(dataset[0] == NULL)
				{
					string message = string("Could not open image ") + this->inputImage;
					throw RSGISImageException(message.c_str());
				}
				numImageBands = dataset[0]->GetRasterCount();
				cout << "Raster Band Count = " << numImageBands << endl;
				
				// Open vector
				inputVecDS = OGRSFDriverRegistrar::Open(this->inputVector.c_str(), FALSE);
				if(inputVecDS == NULL)
				{
					string message = string("Could not open vector file ") + this->inputVector;
					throw RSGISFileException(message.c_str());
				}
				inputVecLayer = inputVecDS->GetLayerByName(vectorLayerName.c_str());
				if(inputVecLayer == NULL)
				{
					string message = string("Could not open vector layer ") + vectorLayerName;
					throw RSGISFileException(message.c_str());
				}
				
				// READ IN SHAPEFILE
				numFeatures = inputVecLayer->GetFeatureCount();
				polyData = new RSGISPolygonData*[numFeatures];
				for(int i = 0; i < numFeatures; i++)
				{
					polyData[i] = new RSGISImageTileVector(this->filenameAttribute);
				}
				cout << "Reading in " << numFeatures << " features\n";
				vecIO.readPolygons(inputVecLayer, polyData, numFeatures);
				
				//Convert to RSGISImageTileVector
				data = new RSGISImageTileVector*[numFeatures];
				for(int i = 0; i < numFeatures; i++)
				{
					data[i] = dynamic_cast<RSGISImageTileVector*>(polyData[i]);
				}
				delete[] polyData;
				
				copyImage = new RSGISCopyImage(numImageBands);
				calcImage = new RSGISCalcImage(copyImage, "", true);
                
				unsigned int failCount = 0;
				for(int i = 0; i < numFeatures; i++)
				{
					outputFilePath = this->outputImage + data[i]->getFileName() + ".env";
					cout << i << ": " << outputFilePath << endl;
                    try
                    {
                        calcImage->calcImageInEnv(dataset, 1, outputFilePath, data[i]->getBBox());
                    }
                    catch (RSGISImageBandException e)
                    {
                        ++failCount;
                        if(failCount <= 100)
                        {
                            cerr << "RSGISException caught: " << e.what() << endl;
                            cerr << "Check output path exists and is writable and all polygons in shapefile:" << endl;
                            cerr << " " << this->inputVector << endl;
                            cerr << "Are completely within:" << endl;
                            cerr << " " << this->inputImage << endl;
                        }
                        else
                        {
                            cerr << "Over 100 exceptions have been caught, exiting" << endl;
                            throw e;
                        }
                    }
				}
				
				GDALClose(dataset[0]);
				delete[] dataset;
				OGRDataSource::DestroyDataSource(inputVecDS);
				OGRCleanupAll();
				GDALDestroyDriverManager();
				delete calcImage;
				delete copyImage;
			}
			catch(RSGISException e)
			{
				cerr << "RSGISException caught: " << e.what() << endl;
			}
		}
		else if(option == RSGISExeImageUtils::pansharpen)
		{
			GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISCalcImage *calcImage = NULL;
			
			datasets = new GDALDataset*[1];
			
			datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
			if(datasets[0] == NULL)
			{
				string message = string("Could not open image ") + this->inputImage;
				throw RSGISImageException(message.c_str());
			}
			
			int numRasterBands = datasets[0]->GetRasterCount();
			
			// Calculate statistics
			float *imageStats = new float[4];  // Set up an array to hold image stats
			
			cout << "Calculating image mean.." << endl;
			RSGISHCSPanSharpenCalcMeanStats *panMean = new RSGISHCSPanSharpenCalcMeanStats(numRasterBands, imageStats);
			RSGISCalcImage *calcImageMean = new RSGISCalcImage(panMean, "", true);
			calcImageMean->calcImage(datasets, 1);
			panMean->returnStats();
			
			cout << "Calculating image standard deviation.." << endl;
			RSGISHCSPanSharpenCalcSDStats *panSD = new RSGISHCSPanSharpenCalcSDStats(numRasterBands, imageStats);
			RSGISCalcImage *calcImageSD = new RSGISCalcImage(panSD, "", true);
			calcImageSD->calcImage(datasets, 1);
			panSD->returnStats();
			
			/*cout << "meanMS = " << imageStats[0] << endl;
			cout << "meanPAN = "<< imageStats[1] << endl;
			cout << "sdMS = " << imageStats[2] << endl;
			cout << "sdPAN = "<< imageStats[3] << endl;*/
			
			cout << "Pan sharpening.." << endl;
			RSGISHCSPanSharpen *panSharpen = new RSGISHCSPanSharpen(numRasterBands - 1, imageStats);
			calcImage = new RSGISCalcImage(panSharpen, "", true);
			// naive mode
			//calcImage->calcImage(datasets, 1, this->outputImage);
			// smart mode 
			calcImage->calcImageWindowData(datasets, 1, this->outputImage, this->panWinSize);
			
			
			// Tidy up
			GDALClose(datasets[0]);
			delete[] datasets;
			
			delete calcImageMean;
			delete calcImageSD;
			delete calcImage;
			delete panMean;
			delete panSD;
			delete panSharpen;
			delete[] imageStats;
		}
        else if(option == RSGISExeImageUtils::createslices)
		{
            cout << "Create Image slices from a multiband input image\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image Base: " << this->outputImage << endl;
            
			GDALAllRegister();
			GDALDataset *dataset = NULL;
			
			dataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
			if(dataset == NULL)
			{
				string message = string("Could not open image ") + this->inputImage;
				throw RSGISImageException(message.c_str());
			}
            
            RSGISImageUtils imageUtils;
            try 
            {
                imageUtils.createImageSlices(dataset, outputImage);
            } 
            catch (RSGISImageException &e) 
            {
                throw e;
            }
			
			// Tidy up
			GDALClose(dataset);
            GDALDestroyDriverManager();
		}
        else if(option == RSGISExeImageUtils::clump)
        {
            cout << "Clump the input image for a given values.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            
            GDALAllRegister();
			GDALDataset *inDataset = NULL;
			inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
			if(inDataset == NULL)
			{
				string message = string("Could not open image ") + this->inputImage;
				throw RSGISImageException(message.c_str());
			}
            
            /*
            unsigned long width = inDataset->GetRasterXSize();
            unsigned long height = inDataset->GetRasterYSize();
            unsigned int numBands = inDataset->GetRasterCount();
            */
            RSGISImageUtils imgUtils;
            
            GDALDataset *processingDataset = NULL;
            
            cout << "Copying input dataset\n";
            if(this->processInMemory)
            {
                cout << "Processing in Memory\n";
                processingDataset = imgUtils.createCopy(inDataset, "", "MEM", GDT_UInt32, this->projFromImage, this->proj);
            }
            else
            {
                cout << "Processing using Disk\n";
                processingDataset = imgUtils.createCopy(inDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
            }
            imgUtils.copyUIntGDALDataset(inDataset, processingDataset);
            
            
            cout << "Performing clumping\n";
            RSGISCalcImgValueAlongsideOut *clumpImg = new RSGISClumpImage();
            RSGISCalcImgAlongsideOut calcImg = RSGISCalcImgAlongsideOut(clumpImg);
            calcImg.calcImageIterate(processingDataset);
            delete clumpImg;
            
            if(this->processInMemory)
            {
                cout << "Copying output to disk\n";
                GDALDataset *outDataset = imgUtils.createCopy(inDataset, this->outputImage, this->imageFormat, GDT_UInt32, this->projFromImage, this->proj);
                imgUtils.copyUIntGDALDataset(processingDataset, outDataset);
                GDALClose(outDataset);
            }
			
			// Tidy up
			GDALClose(inDataset);
            GDALClose(processingDataset);
            GDALDestroyDriverManager();
        }
		else if(option == RSGISExeImageUtils::imageComposite)
		{
            GDALAllRegister();
			GDALDataset **datasets = NULL;
			RSGISCalcImage *calcImage = NULL;
			
			datasets = new GDALDataset*[1];
			
			datasets[0] = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
			if(datasets[0] == NULL)
			{
				string message = string("Could not open image ") + this->inputImage;
				throw RSGISImageException(message.c_str());
			}
			
			int numRasterBands = datasets[0]->GetRasterCount();
			
            int numOutputBands = numRasterBands / this->compositeBands;
            
            cout << "Calculating statistics for every " << this->compositeBands << " bands of a " << numRasterBands << " band input image to create a " << numOutputBands << " band output image" << endl;
            
            
			RSGISImageComposite *compositeImage = new RSGISImageComposite(numOutputBands, this->compositeBands, this->outCompStat);
			calcImage = new RSGISCalcImage(compositeImage, "", true);
			calcImage->calcImage(datasets, 1, this->outputImage);
			
			// Tidy up
			GDALClose(datasets[0]);
			delete[] datasets;
			
			delete calcImage;
			delete compositeImage;
		}
        else if(option == RSGISExeImageUtils::relabel)
        {
            cout << "A command to relabel image pixel using a look up table from a gmtxt matrix file.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Look Up Table: " << this->lutMatrixFile << endl;
            
            try 
            {
                GDALAllRegister();
                GDALDataset *inDataset = NULL;
                inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_ReadOnly);
                if(inDataset == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                RSGISRelabelPixelValuesFromLUT relabelPixels;
                relabelPixels.relabelPixelValues(inDataset, this->outputImage, this->lutMatrixFile, this->imageFormat);
                
                GDALClose(inDataset);
                GDALDestroyDriverManager();
            } 
            catch (RSGISException &e) 
            {
                throw e;
            }
            
        }
        else if(option == RSGISExeImageUtils::assignproj)
        {
            cout << "Assign and update and image to a specific projection\n";
            cout << "Image: " << this->inputImage << endl;
            cout << "Projection File: " << this->projFile << endl;
            
            try 
            {
                GDALAllRegister();
                GDALDataset *inDataset = NULL;
                inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_Update);
                if(inDataset == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                RSGISTextUtils textUtils;
                string projWKTStr = textUtils.readFileToString(this->projFile);
                
                inDataset->SetProjection(projWKTStr.c_str());
                
                GDALClose(inDataset);
                GDALDestroyDriverManager();
            } 
            catch (RSGISException &e) 
            {
                throw e;
            }
        }
        else if(option == RSGISExeImageUtils::popimgstats)
        {
            cout << "Populate an image with image statistics and image pyramids\n";
            cout << "Image: " << this->inputImage << endl;
            if(this->useIgnoreVal)
            {
                cout << "Ignore Val: " << this->nodataValue << endl;
            }
            if(this->calcImgPyramids)
            {
                cout << "Calculating image pyramids\n";
            }
            
            try 
            {
                GDALAllRegister();
                GDALDataset *inDataset = NULL;
                inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_Update);
                if(inDataset == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                RSGISPopWithStats popWithStats;
                popWithStats.calcPopStats( inDataset, this->useIgnoreVal, this->nodataValue, this->calcImgPyramids );
                
                
                GDALClose(inDataset);
                GDALDestroyDriverManager();
            } 
            catch (RSGISException &e) 
            {
                throw e;
            }
        }
        else if(option == RSGISExeImageUtils::createcopy)
        {
            cout << "Create a new image from an existing image\n";
            cout << "Image: " << this->inputImage << endl;
            cout << "Output: " << this->outputImage << endl;
            cout << "Data Value: " << this->dataValue << endl;
            cout << "Num Image Bands: " << this->numBands << endl;
            cout << "Image format: " << this->imageFormat << endl;
            
            try 
            {
                GDALAllRegister();
                GDALDataset *inDataset = NULL;
                inDataset = (GDALDataset *) GDALOpen(this->inputImage.c_str(), GA_Update);
                if(inDataset == NULL)
                {
                    string message = string("Could not open image ") + this->inputImage;
                    throw RSGISImageException(message.c_str());
                }
                
                RSGISImageUtils imgUtils;
                GDALDataset *outDataset = imgUtils.createCopy(inDataset, this->numBands, this->outputImage, this->imageFormat, outDataType);
                imgUtils.assignValGDALDataset(outDataset, this->dataValue);
                
                GDALClose(inDataset);
                GDALClose(outDataset);
                GDALDestroyDriverManager();
            } 
            catch (RSGISException &e) 
            {
                throw e;
            }
        }
		else
		{
			cout << "Options not recognised\n";
		}
		
	}
}


void RSGISExeImageUtils::printParameters()
{
	if(parsed)
	{
		if(option == RSGISExeImageUtils::colour)
		{
			cout << "Colour Image\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			for(int i = 0; i < numClasses; i++)
			{
				cout << i <<  ") Class " << classColour[i]->className << " with ID ";
				cout << classColour[i]->classID << " uses image band " << classColour[i]->imgBand << "\n";
				cout << "Lower = " << classColour[i]->lower << endl;
				cout << "Upper = " << classColour[i]->upper << endl;
				cout << "Red = " << classColour[i]->red << endl;
				cout << "Green = " << classColour[i]->green << endl;
				cout << "Blue = " << classColour[i]->blue << endl;
			}
		}
        else if(option == RSGISExeImageUtils::colourimagebands)
		{
			cout << "Colour Image\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			for(int i = 0; i < numClasses; i++)
			{
				cout << i <<  ") Class " << classColour[i]->className << " with ID ";
                cout << classColour[i]->classID << endl;
                cout << "Lower = " << classColour[i]->lower << endl;
                cout << "Upper = " << classColour[i]->upper << endl;
                cout << "Red = " << classColour[i]->red << endl;
                cout << "Green = " << classColour[i]->green << endl;
                cout << "Blue = " << classColour[i]->blue << endl;
			}
		}
		else if(option == RSGISExeImageUtils::mosaic)
		{
			for(int i = 0; i < this->numImages; i++)
			{
				cout << "Input Image: " << this->inputImages[i] << endl;
			}
			cout << "Output Image: " << this->outputImage << endl;
			cout << "No Data Value: " << this->nodataValue << endl;
			
			if(projFromImage)
			{
				cout << "Projection is being taken from the first image in the list.\n";
			}
			else
			{
				cout << "Projection: " << proj << endl;
			}
		}
		else if(option == RSGISExeImageUtils::include)
		{
			cout << "Base Image: " << this->inputImage << endl;
			for(int i = 0; i < this->numImages; i++)
			{
				cout << "Input Image: " << this->inputImages[i] << endl;
			}
		}
		else if(option == RSGISExeImageUtils::cut2poly)
		{
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Input Vector: " << this->inputVector << endl;
			cout << "Filename attribute: " << this->filenameAttribute << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "No Data Value: " << this->nodataValue << endl;
		}
		else if(option == RSGISExeImageUtils::mask)
		{
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Image Mask: " << this->imageMask << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Mask Value: " << this->maskValue << endl;
		}
		else if(option == RSGISExeImageUtils::resample)
		{
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Scale Image: " << this->resampleScale << endl;
			if(this->interpolator == RSGISExeImageUtils::cubic)
			{
				cout << "Using a cubic interpolator\n";
			}
			else if(this->interpolator == RSGISExeImageUtils::billinearArea)
			{
				cout << "Using a bilinear (area) interpolator\n";
			}
			else if(this->interpolator == RSGISExeImageUtils::billinearPt)
			{
				cout << "Using a bilinear (point) interpolator\n";
			}
			else if(this->interpolator == RSGISExeImageUtils::nn)
			{
				cout << "Using a nearest neighbour interpolator\n";
			}
			else if(this->interpolator == RSGISExeImageUtils::trangular)
			{
				cout << "Using a triangular interpolator\n";
			}
		}
		else if(option == RSGISExeImageUtils::rasterisedefiniens)
		{
			cout << "Rasterise Definiens\n";
			cout << "Input DIR: " << this->inputDIR << endl;
			cout << "Output DIR: " << this->outputDIR << endl;
			if(projFromImage)
			{
				cout << "Projection is being taken from the first image in the list.\n";
			}
			else
			{
				cout << "Projection: " << proj << endl;
			}
		}
		else if(option == RSGISExeImageUtils::printProj4)
		{
			cout << "Print Spatial Reference Proj4\n";
			cout << "Input Image: " << this->inputImage << endl;
		}
		else if(option == RSGISExeImageUtils::printWKT)
		{
			cout << "Print Spatial Reference WKT\n";
			cout << "Input Image: " << this->inputImage << endl;
		}
		else if(option == RSGISExeImageUtils::extract2dscatterptxt)
		{
			cout << "Extract image data to create 2D scatter plot (exported as ptxt)\n";
			
			cout << "Input Images:\n";
			for(int i = 0; i < this->numImages; ++i)
			{
				cout << i << ") " << this->inputImages[i] << endl;
			}
			cout << "Image Band 1: " << this->imgBand1 << endl;
			cout << "Image Band 2: " << this->imgBand2 << endl;
			cout << "Output File: " << this->outputFile << endl;
		}
		else if(option == RSGISExeImageUtils::sgsmoothing)
		{
			cout << "Apply Savitzky-Golay Smoothing Filters to data\n";
			
			cout << "Input Image = " << this->inputImage << endl;
			cout << "Output Image = " << this->outputImage << endl;
			cout << "Image Band Values = " << this->bandFloatValuesVector << endl;
			cout << "Order = " << this->order << endl;
			cout << "Window = " << this->window << endl;
		}
		else if(option == RSGISExeImageUtils::cumulativearea)
		{
			cout << "Calculate the cumulative area of the image profile/spectra\n";
			cout << "Input Image = " << this->inputImage << endl;
			cout << "Output Image = " << this->outputImage << endl;
			cout << "Image Band Widths = " << this->bandFloatValuesVector << endl;
		}
		else if(option == RSGISExeImageUtils::createimage)
		{
			cout << "Create a new blank image\n";
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Num. Image bands: " << this->numBands << endl;
			cout << "Size: [" << this->width << "," << this->height << "]\n";
			cout << "TL Geo: [" << this->eastings << "," << this->northings << "]\n";
			cout << "Resolution: " << this->resolution << endl;
			cout << "Default Value: " << this->outValue << endl;
			cout << "Projection: " << this->proj << endl;
		}
		else if(option == RSGISExeImageUtils::stretch)
		{
			cout << "Apply an enhancement stretch to the an input image - usually for visualisation\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			if(stretchType == linearMinMax)
			{
				cout << "Linear Min-Max stretch\n";
			}
			else if(stretchType == linearPercent)
			{
				cout << "Linear " << percent << " % stretch\n";
			}
			else if(stretchType == linearStdDev)
			{
				cout << "Linear " << stddev << " Standard Deviation stretch\n";
			}			
			else if(stretchType == histogram)
			{
				cout << "Histogram stretch\n";
			}
			else if(stretchType == exponential)
			{
				cout << "Exponential stretch\n";
			}
			else if(stretchType == logarithmic)
			{
				cout << "Logarithmic stretch\n";
			}
			else if(stretchType == powerLaw)
			{
				cout << "Power Law stretch\n";
			}
			else
			{
				throw RSGISException("Stretch is not recognised.");
			}
            
            if(this->ignoreZeros)
            {
                cout << "Ignoring Zeros\n";
            }
		}
		else if(option == RSGISExeImageUtils::huecolour)
		{
			cout << "Generate a colour image (through Hue) representing an image band - usually for visualisation\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Band: " << this->imgBand1 << endl;
			cout << "Range: [" << this->lowerRangeValue << "," << this->upperRangeValue << "]\n";
			cout << "Background value: " << this->nodataValue << endl;
		}
		else if(option == RSGISExeImageUtils::definespatialref)
		{
			cout << "Make a copy of the input image and define the projection and spatial locations\n";
			cout << "Input Image: " << this->inputImage << endl;
			cout << "Output Image: " << this->outputImage << endl;
			cout << "Projection: " << this->proj << endl;
			cout << "TL [" << this->eastings << "," << this->northings << "]\n";
			cout << "Resolution [" << this->xRes << "," << this->yRes << "]\n";
		}
        else if(option == RSGISExeImageUtils::createslices)
		{
            cout << "Create Image slices from a multiband input image\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image Base: " << this->outputImage << endl;
        }
        else if(option == RSGISExeImageUtils::clump)
        {
            cout << "Clump the input image for a given values.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
        }
        else if(option == RSGISExeImageUtils::relabel)
        {
            cout << "A command to relabel image pixel using a look up table from a gmtxt matrix file.\n";
            cout << "Input Image: " << this->inputImage << endl;
            cout << "Output Image: " << this->outputImage << endl;
            cout << "Look Up Table: " << this->lutMatrixFile << endl;
        }
        else if(option == RSGISExeImageUtils::assignproj)
        {
            cout << "Assign and update and image to a specific projection\n";
            cout << "Image: " << this->inputImage << endl;
            cout << "Projection File: " << this->projFile << endl;
        }
        else if(option == RSGISExeImageUtils::popimgstats)
        {
            cout << "Populate an image with image statistics and image pyramids\n";
            cout << "Image: " << this->inputImage << endl;
            if(this->useIgnoreVal)
            {
                cout << "Ignore Val: " << this->nodataValue << endl;
            }
            if(this->calcImgPyramids)
            {
                cout << "Calculating image pyramids\n";
            }
        }
        else if(option == RSGISExeImageUtils::createcopy)
        {
            cout << "Create a new image from an existing image\n";
            cout << "Image: " << this->inputImage << endl;
            cout << "Output: " << this->outputImage << endl;
            cout << "Data Value: " << this->dataValue << endl;
            cout << "Num Image Bands: " << this->numBands << endl;
            cout << "Image format: " << this->imageFormat << endl;
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

void RSGISExeImageUtils::help()
{
    cout << "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">" << endl;
    cout << "<!-- A command that will generate an RGB colour image based on thresholds applied to the input image - useful for generating quicklooks and visualisation of classification -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"colourimage\" image=\"image.env\" output=\"image_out.env\">" << endl;
    cout << "    <rsgis:colour name=\"class_name_1\" id=\"int\" band=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << endl;
    cout << "    <rsgis:colour name=\"class_name_2\" id=\"int\" band=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << endl;
    cout << "    <rsgis:colour name=\"class_name_3\" id=\"int\" band=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command that will generate an RGB colour image based on thresholds applied to each input image band, generating a new image for each input band - useful for generating quicklooks and visualisation of classification.-->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"colourimagebands\" image=\"image.env\" output=\"image_out_base\">" << endl;
    cout << "    <rsgis:colour name=\"class_name_1\" id=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << endl;
    cout << "    <rsgis:colour name=\"class_name_2\" id=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << endl;
    cout << "    <rsgis:colour name=\"class_name_3\" id=\"int\" lower=\"double\" upper=\"double\" red=\"int\" green=\"int\" blue=\"int\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to mosaic a set of input images to generate a single output image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"mosaic\" output=\"image_out.env\" nodata=\"float=0\" skipValue=\"float (optional)\" skipLowerThreash=\"float (optional)\" skipUpperThreash=\"float (optional)\"  proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" setSkipBand=\"1\" >" << endl;
    cout << "    <rsgis:image file=\"image1\" />" << endl;
    cout << "    <rsgis:image file=\"image2\" />" << endl;
    cout << "    <rsgis:image file=\"image3\" />" << endl;
    cout << "    <rsgis:image file=\"image4\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to mosaic a set of input images from a directory to generate a single output image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"mosaic\" dir=\"directory\" ext=\"file_extension\" output=\"image_out.env\" nodata=\"float=0\" skipValue=\"float (optional)\" skipLowerThreash=\"float (optional)\" skipUpperThreash=\"float (optional)\"  proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command to include a set of input images into an existing image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"include\" image=\"base_image.env\" >" << endl;
    cout << "    <rsgis:image file=\"image1\" />" << endl;
    cout << "    <rsgis:image file=\"image2\" />" << endl;
    cout << "    <rsgis:image file=\"image3\" />" << endl;
    cout << "    <rsgis:image file=\"image4\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to include a set of input images from a directory into an existing image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"include\" image=\"base_image.env\" dir=\"directory\" ext=\"file_extension\" />" << endl;
    cout << "<!-- A command to cut an image to an input shapefile where each polygon geometry will created a new output image representing the region within the polygon -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"cut2poly\" image=\"image.env\" output=\"image_out_base\" vector=\"vector.shp\" outfilename=\"attribute\" nodata=\"float\" />" << endl;
    cout << "<!-- A command to mask the input image with a second 'mask' image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"mask\" image=\"image.env\" mask=\"mask.env\" output=\"image_out.env\" maskvalue=\"float\" />" << endl;
    cout << "<!-- A command resample an input image to another resolution -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"resample\" image=\"image.env\" output=\"image_out.env\" scale=\"float\" interpolation=\"Cubic | BilinearArea | BilinearPoint | NN | Triangular\" />" << endl;
    cout << "<!-- TODO A command to create a multiband raster image, based on thematic rasters exported from Definiens. The values for each band are held in a csv file (exported with raster image from Definiens -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"rasterisedefiniens\" [inDIR=\"/input/directory/\" outDIR=\"/output/directory\"] [image=\"image.env\" csv=\"input.csv\" output=\"image_out.env\"] proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command which prints (to the console) the proj4 string representing the projection of the inputted image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"printProj4\" image=\"image.env\" />" << endl;
    cout << "<!-- A command which prints (to the console) the WKT string representing the projection of the inputted image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"printWKT\" image=\"image.env\" />" << endl;
    cout << "<!-- A command extracts pixel values from two image bands and output them as 2D scatter ptxt file -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"extract2dscatterptxt\" image=\"image.env\" band1=\"int\" band2=\"int\" output=\"string\" />" << endl;
    cout << "<!-- A command extracts pixel values from two image bands (bands are numbered sequencially down the list of input files) and output them as 2D scatter ptxt file -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"extract2dscatterptxt\" band1=\"int\" band2=\"int\" output=\"string\" >" << endl;
    cout << "    <rsgis:image file=\"image1\" />" << endl;
    cout << "    <rsgis:image file=\"image2\" />" << endl;
    cout << "    <rsgis:image file=\"image3\" />" << endl;
    cout << "    <rsgis:image file=\"image4\" />" << endl;
    cout << "</rsgis:command>" << endl;
    cout << "<!-- A command to smooth an spectral profiles of the input image pixels -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"sgsmoothing\" image=\"image.env\" output=\"image_out.env\" order=\"int\" window=\"int\" imagebands=\"vector.mtxt\"/>" << endl;
    cout << "<!-- A command to generate a cumulativeAreaImage from a spectral curve (or profile) -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"cumulativearea\" image=\"image.env\" output=\"image_out.env\" imagebands=\"matrix.mtxt\"/>" << endl;
    cout << "<!-- A command to generate new image with a default value -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"createimage\" output=\"image_out.env\" numbands=\"int\" width=\"int\" height=\"int\" resolution=\"float\" eastings=\"double\" northings=\"double\" proj4=\"string\" value=\"float\"/>" << endl;
    cout << "<!-- A command to apply an enhancement stretch an images pixel values to a range of 0 to 255 - normally used for visualisation -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"stretch\" image=\"image.env\" output=\"image_out.env\" ignorezeros=\"yes | no\" stretch=\"LinearMinMax | LinearPercent | LinearStdDev | Histogram | Exponential | Logarithmic | PowerLaw\" percent=\"float - only LinearPercent\" stddev=\"float - only LinearStdDev\" power=\"float - only PowerLaw\"/>" << endl;
    cout << "<!-- A command to colour to generate a colour image, using the Hue of a HSV transformation, for a particular input image band -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"huecolour\" image=\"image.env\" output=\"image_out.env\" band=\"int\" lowvalue=\"float\" highvalue=\"float\" background=\"float\" />" << endl;
    cout << "<!-- A command to remove / define spatial reference to nothing and size set to pixel size -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"removespatialref\" image=\"image.env\" output=\"image_out.env\" />" << endl;
    cout << "<!-- A command to add noise to an image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"addnoise\" type=\"randomNoise | percentGaussianNoise\" scale=\"float\" image=\"image.env\" output=\"image_out.env\"/>" << endl;
    cout << "<!-- A command to subset an image to the same extent as a shapefile -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"subset\" image=\"image.env\" output=\"output_img.env\" vector=\"vector.shp\" />" << endl;
    cout << "<!-- A command to subset an image to polygons within shapefile -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"subset2polys\" image=\"image.env\" output=\"image_out_base\" vector=\"vector.shp\" outfilename=\"attribute\" />" << endl;
    cout << "<!-- A command to pan sharpen an image, takes stack of multispectral image (resampled to pan resolution) and panchromatic image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"pansharpen\" image=\"ms_pan_img.env\" output=\"ps_image.env\" />" << endl;
    cout << "<!-- A command to generate a set of slices from a multi-band image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"createslices\" image=\"image.env\" output=\"image_out_base\" />" << endl;
    cout << "<!-- A command to clump an image for a given pixel values -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"clump\" image=\"image.env\" output=\"image_out.env\" format=\"GDAL Format\" inmemory=\"yes | no\" proj=\"OSGB | NZ2000 | NZ1949 | IMAGE\" />" << endl;
    cout << "<!-- A command to create a composite image from a multi-band input image -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"composite\" image=\"image.env\" output=\"image_out.env\" compositeBands=\"int\" stats=\"mean | min | max | range\" />" << endl;
    cout << "<!-- A command to relabel image pixel using a look up table from a gmtxt matrix file (m=2 n=X) -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"relabel\" image=\"image.env\" output=\"image_out.env\" lut=\"matrix.gmtxt\" />" << endl;
    cout << "<!-- A command to assign and update and image to a specific projection -->" << endl;
    cout << "<rsgis:command algor=\"imageutils\" option=\"assignproj\" image=\"image.env\" projwkt=\"txt.wkt\" />" << endl;
	cout << "</rsgis:commands>\n";
}

string RSGISExeImageUtils::getDescription()
{
	return "Image utilities.";
}

string RSGISExeImageUtils::getXMLSchema()
{
	return "NOT DONE!";
}

RSGISExeImageUtils::~RSGISExeImageUtils()
{

}





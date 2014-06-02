/*
 *  imageutils.cpp
 *  RSGIS_LIB
 *
 *  Created by Sam Gillingham on 02/05/2013.
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

#include "rsgispy_common.h"
#include "cmds/RSGISCmdImageUtils.h"
#include <vector>

/* An exception object for this module */
/* created in the init function */
struct ImageUtilsState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct ImageUtilsState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct ImageUtilsState _state;
#endif

// Helper function to extract python sequence to array of strings
static std::string *ExtractStringArrayFromSequence(PyObject *sequence, int *nElements) {
    Py_ssize_t nFields = PySequence_Size(sequence);
    *nElements = nFields;
    std::string *stringsArray = new std::string[nFields];

    for(int i = 0; i < nFields; ++i) {
        PyObject *stringObj = PySequence_GetItem(sequence, i);

        if(!RSGISPY_CHECK_STRING(stringObj)) {
            PyErr_SetString(GETSTATE(sequence)->error, "Fields must be strings");
            Py_DECREF(stringObj);
            return stringsArray;
        }

        stringsArray[i] = RSGISPY_STRING_EXTRACT(stringObj);
        Py_DECREF(stringObj);
    }

    return stringsArray;
}

static std::vector<std::string> ExtractStringVectorFromSequence(PyObject *sequence, int *nElements) {
    Py_ssize_t nFields = PySequence_Size(sequence);
    *nElements = nFields;
    std::vector<std::string> stringsArray;
    stringsArray.reserve(*nElements);
    
    for(int i = 0; i < nFields; ++i)
    {
        PyObject *stringObj = PySequence_GetItem(sequence, i);
        
        if(!RSGISPY_CHECK_STRING(stringObj)) {
            PyErr_SetString(GETSTATE(sequence)->error, "Fields must be strings");
            Py_DECREF(stringObj);
            return stringsArray;
        }
        
        stringsArray.push_back(RSGISPY_STRING_EXTRACT(stringObj));
        Py_DECREF(stringObj);
    }
    
    return stringsArray;
}

// Helper function to extract python sequence to array of integers
/*static int *ExtractIntArrayFromSequence(PyObject *sequence, int *nElements) {
    Py_ssize_t nFields = PySequence_Size(sequence);
    *nElements = nFields;
    int *intArray = new int[nFields];

    for(int i = 0; i < nFields; ++i) {
        PyObject *intObj = PySequence_GetItem(sequence, i);

        if(!RSGISPY_CHECK_INT(intObj)) {
            PyErr_SetString(GETSTATE(sequence)->error, "Fields must be integers");
            Py_DECREF(intObj);
            return intArray;
        }

        intArray[i] = RSGISPY_INT_EXTRACT(intObj);
        Py_DECREF(intObj);
    }

    return intArray;
}*/

static PyObject *ImageUtils_StretchImage(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat, *pszOutStatsFile;
    int saveOutStats, ignoreZeros, onePassSD;
    int nOutDataType, nStretchType;
    float fStretchParam = 2;
    if( !PyArg_ParseTuple(args, "ssisiisii|f:stretchImage", &pszInputImage, &pszOutputFile, &saveOutStats, 
                                &pszOutStatsFile, &ignoreZeros, &onePassSD, &pszGDALFormat, &nOutDataType, &nStretchType,
                                &fStretchParam))
        return NULL;

    try
    {
        rsgis::cmds::executeStretchImage(pszInputImage, pszOutputFile, saveOutStats, pszOutStatsFile, ignoreZeros,
                    onePassSD, pszGDALFormat, (rsgis::RSGISLibDataType)nOutDataType, 
                    (rsgis::cmds::RSGISStretches)nStretchType, fStretchParam);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageUtils_StretchImageWithStats(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputFile, *pszGDALFormat, *pszInStatsFile;
    int nOutDataType, nStretchType;
    float fStretchParam = 2.0;
    if( !PyArg_ParseTuple(args, "ssssii|f:stretchImageWithStats", &pszInputImage, &pszOutputFile, 
                                &pszInStatsFile, &pszGDALFormat, &nOutDataType, &nStretchType,
                                &fStretchParam))
        return NULL;

    try
    {
        rsgis::cmds::executeStretchImageWithStats(pszInputImage, pszOutputFile, pszInStatsFile,
                    pszGDALFormat, (rsgis::RSGISLibDataType)nOutDataType, 
                    (rsgis::cmds::RSGISStretches)nStretchType, fStretchParam);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageUtils_maskImage(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszImageMask, *pszOutputImage, *pszGDALFormat;
    int nDataType;
    float outValue, maskValue;
    if( !PyArg_ParseTuple(args, "ssssiff:maskImage", &pszInputImage, &pszImageMask, &pszOutputImage, 
                                &pszGDALFormat, &nDataType, &outValue, &maskValue ))
        return NULL;

    try
    {
        rsgis::cmds::executeMaskImage(pszInputImage, pszImageMask, pszOutputImage, pszGDALFormat, 
                            (rsgis::RSGISLibDataType)nDataType, outValue, maskValue);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageUtils_createTiles(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszImageBase, *pszGDALFormat, *pszExt;
    float width, height, tileOverlap; // Command actually needs ints. Won't complain if floats are provided
    int offsetTiling, nDataType;
    /* Check if required parameters are present and of the correct type
       The type and order are specified by ssfffisis.
       I.e., string, string, float ...etc
    */
    if( !PyArg_ParseTuple(args, "ssfffisis:createTiles", &pszInputImage, &pszImageBase,
                                &width, &height, &tileOverlap, &offsetTiling, &pszGDALFormat, 
                                &nDataType, &pszExt ))
        return NULL;

    PyObject *pOutList;
    try
    {
        std::vector<std::string> outFileNames;
        rsgis::cmds::executeCreateTiles(pszInputImage, pszImageBase, width, height, tileOverlap,
                        offsetTiling, pszGDALFormat, (rsgis::RSGISLibDataType)nDataType, 
                        pszExt, &outFileNames);

        pOutList = PyList_New(outFileNames.size());
        Py_ssize_t nIndex = 0;
        for( std::vector<std::string>::iterator itr = outFileNames.begin(); itr != outFileNames.end(); itr++)
        {
            PyObject *pVal = RSGISPY_CREATE_STRING((*itr).c_str());
            PyList_SetItem(pOutList, nIndex, pVal ); // steals a reference
            nIndex++;
        }

    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    return pOutList;
}

static PyObject *ImageUtils_createImageMosaic(PyObject *self, PyObject *args)
{
    const char *pszOutputImage, *pszGDALFormat;
    float backgroundVal, skipVal;
    int skipBand, nDataType, overlapBehaviour;
    PyObject *pInputImages; // List of input images

    // Check parameters are present and of correct type
    if( !PyArg_ParseTuple(args, "Osffiisi:createImageMosaic", &pInputImages, &pszOutputImage,
                                &backgroundVal, &skipVal, &skipBand, &overlapBehaviour,&pszGDALFormat, &nDataType))
        return NULL;

    // TODO: Look into this function - doesn't seem to catch when only a single image is provided.
    if(!PySequence_Check(pInputImages)) {
        PyErr_SetString(GETSTATE(self)->error, "First argument must be a sequence");
        return NULL;
    }

    // Extract list of images to array of strings.
    int numImages = 0;
    std::string *inputImages = ExtractStringArrayFromSequence(pInputImages, &numImages);
    if(numImages == 0) 
    { 
        PyErr_SetString(GETSTATE(self)->error, "No input images provided");
        return NULL; 
    }
    
    try
    {
        rsgis::cmds::executeImageMosaic(inputImages, numImages, pszOutputImage, backgroundVal, 
                    skipVal, skipBand-1, overlapBehaviour, pszGDALFormat, (rsgis::RSGISLibDataType)nDataType);

    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageUtils_IncludeImages(PyObject *self, PyObject *args)
{
    const char *pszBaseImage;
    int bandsDefined = false;
    PyObject *pInputImages; // List of input images
    PyObject *pInputBands = Py_None; // List of bands

    // Check parameters are present and of correct type
    if( !PyArg_ParseTuple(args, "sO|O:includeImages", &pszBaseImage, &pInputImages, &pInputBands))
        return NULL;

    // TODO: Look into this function - doesn't seem to catch when only a single image is provided.
    if(!PySequence_Check(pInputImages)) {
        PyErr_SetString(GETSTATE(self)->error, "Second argument must be a list of images");
        return NULL;
    }

    // Extract list of images to array of strings.
    int numImages = 0;
    std::string *inputImages = ExtractStringArrayFromSequence(pInputImages, &numImages);
    if(numImages == 0) 
    { 
        PyErr_SetString(GETSTATE(self)->error, "No input images provided");
        return NULL; 
    }

    if(pInputBands == Py_None){bandsDefined = false;}
    // Extract bands to an array (if using)
    std::vector<int> imgBands;
    if(bandsDefined)
    {
        Py_ssize_t nFields = PySequence_Size(pInputBands);
        
        for(int i = 0; i < nFields; ++i)
        {
            PyObject *intObj = PySequence_GetItem(pInputBands, i);
            
            if(!RSGISPY_CHECK_INT(intObj))
            {
                PyErr_SetString(GETSTATE(pInputBands)->error, "Bands must be integers");
                Py_DECREF(intObj);
                return NULL;
            }
            
            imgBands.push_back(RSGISPY_INT_EXTRACT(intObj));
            Py_DECREF(intObj);
        }
    }
    try
    {
        rsgis::cmds::executeImageInclude(inputImages, numImages, pszBaseImage, bandsDefined, imgBands);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ImageUtils_PopImageStats(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char *pszInputImage;
    int useNoDataValue = true;
    int buildPyramids = true;
    float noDataValue = 0;
    static char *kwlist[] = {"image", "usenodataval","nodataval", "calcpyramids", NULL};

    if( !PyArg_ParseTupleAndKeywords(args, keywds, "s|ifi:popImageStats", kwlist, &pszInputImage, 
                    &useNoDataValue, &noDataValue, &buildPyramids))
        return NULL;
    
    try
    {
        rsgis::cmds::executePopulateImgStats(pszInputImage, useNoDataValue, noDataValue, buildPyramids);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_AssignProj(PyObject *self, PyObject *args)
{
    const char *pszInputImage;
    std::string pszInputProj = "";
    std::string pszInputProjFile = "";
    bool readWKTFromFile = false;
    PyObject *pszInputProjObj;
    PyObject *pszInputProjFileObj;
    
    if( !PyArg_ParseTuple(args, "sOO:assignProj", &pszInputImage, &pszInputProjObj, &pszInputProjFileObj))
        return NULL;
    
    if(pszInputProjObj == Py_None)
    {
        pszInputProj = "";
        if(pszInputProjFileObj == Py_None)
        {
            pszInputProjFile = "";
            PyErr_SetString(GETSTATE(self)->error, "Must specify either a wkt string or a file from which it can be read." );
            return NULL;
        }
        else
        {
            readWKTFromFile = true;
            pszInputProjFile = RSGISPY_STRING_EXTRACT(pszInputProjFileObj);
        }
    }
    else
    {
        pszInputProj = RSGISPY_STRING_EXTRACT(pszInputProjObj);
        readWKTFromFile = false;
    }
    
    try
    {
        rsgis::cmds::executeAssignProj(pszInputImage, pszInputProj, readWKTFromFile, pszInputProjFile);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_CopyProjFromImage(PyObject *self, PyObject *args)
{
    const char *pszInputImage;
    const char *pszInputRefImage;
    
    if( !PyArg_ParseTuple(args, "ss:copyProjFromImage", &pszInputImage, &pszInputRefImage))
        return NULL;
    
    try
    {
        rsgis::cmds::executeCopyProj(pszInputImage, pszInputRefImage);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_CopySpatialAndProjFromImage(PyObject *self, PyObject *args)
{
    const char *pszInputImage;
    const char *pszInputRefImage;
    
    if( !PyArg_ParseTuple(args, "ss:copySpatialAndProjFromImage", &pszInputImage, &pszInputRefImage))
        return NULL;
    
    try
    {
        rsgis::cmds::executeCopyProjSpatial(pszInputImage, pszInputRefImage);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_AssignSpatialInfo(PyObject *self, PyObject *args)
{
    const char *pszInputImage;
    PyObject *xTLObj;
    PyObject *yTLObj;
    PyObject *xResObj;
    PyObject *yResObj;
    PyObject *xRotObj;
    PyObject *yRotObj;
        
    if( !PyArg_ParseTuple(args, "sOOOOOO:assignSpatialInfo", &pszInputImage, &xTLObj, &yTLObj, &xResObj, &yResObj, &xRotObj, &yRotObj))
        return NULL;
    
    
    double xTL = 0.0;
    double yTL = 0.0;
    double xRes = 0.0;
    double yRes = 0.0;
    double xRot = 0.0;
    double yRot = 0.0;
    
    bool xTLDef = false;
    bool yTLDef = false;
    bool xResDef = false;
    bool yResDef = false;
    bool xRotDef = false;
    bool yRotDef = false;
    
    if(xTLObj != Py_None)
    {
        if(RSGISPY_CHECK_FLOAT(xTLObj) | RSGISPY_CHECK_INT(xTLObj))
        {
            xTL = RSGISPY_FLOAT_EXTRACT(xTLObj);
            xTLDef = true;
        }
    }
    
    if(yTLObj != Py_None)
    {
        if(RSGISPY_CHECK_FLOAT(yTLObj) | RSGISPY_CHECK_INT(yTLObj))
        {
            yTL = RSGISPY_FLOAT_EXTRACT(yTLObj);
            yTLDef = true;
        }
    }
    
    if(xResObj != Py_None)
    {
        if(RSGISPY_CHECK_FLOAT(xResObj) | RSGISPY_CHECK_INT(xResObj))
        {
            xRes = RSGISPY_FLOAT_EXTRACT(xResObj);
            xResDef = true;
        }
    }
    
    if(yResObj != Py_None)
    {
        if(RSGISPY_CHECK_FLOAT(yResObj) | RSGISPY_CHECK_INT(yResObj))
        {
            yRes = RSGISPY_FLOAT_EXTRACT(yResObj);
            yResDef = true;
        }
    }
    
    if(xRotObj != Py_None)
    {
        if(RSGISPY_CHECK_FLOAT(xRotObj) | RSGISPY_CHECK_INT(xRotObj))
        {
            xRot = RSGISPY_FLOAT_EXTRACT(xRotObj);
            xRotDef = true;
        }
    }
    
    if(yRotObj != Py_None)
    {
        if(RSGISPY_CHECK_FLOAT(yRotObj) | RSGISPY_CHECK_INT(yRotObj))
        {
            yRot = RSGISPY_FLOAT_EXTRACT(yRotObj);
            yRotDef = true;
        }
    }
    
    try
    {
        rsgis::cmds::executeAssignSpatialInfo(pszInputImage, xTL, yTL, xRes, yRes, xRot, yRot, xTLDef, yTLDef, xResDef, yResDef, xRotDef, yRotDef);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_ExtractZoneImageValues2HDF(PyObject *self, PyObject *args)
{
    const char *pszInputImage;
    const char *pszInputMaskImage;
    const char *pszOutputFile;
    float maskValue = 0;
    
    if( !PyArg_ParseTuple(args, "sssf:extractZoneImageValues2HDF", &pszInputImage, &pszInputMaskImage, &pszOutputFile, &maskValue))
        return NULL;
    
    try
    {
        rsgis::cmds::executeImageRasterZone2HDF(std::string(pszInputImage), std::string(pszInputMaskImage), std::string(pszOutputFile), maskValue);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_SelectImageBands(PyObject *self, PyObject *args)
{
    const char *pszInputImage;
    const char *pszOutputFile;
    const char *pszGDALFormat;
    int nDataType;
    PyObject *pImageBands = NULL;
    
    if( !PyArg_ParseTuple(args, "sssiO:selectImageBands", &pszInputImage, &pszOutputFile, &pszGDALFormat, &nDataType, &pImageBands))
        return NULL;
    
    if(!PySequence_Check(pImageBands)) {
        PyErr_SetString(GETSTATE(self)->error, "Last argument must be a sequence of image bands (int)");
        return NULL;
    }
    
    std::vector<unsigned int> imgBands;
    Py_ssize_t nFields = PySequence_Size(pImageBands);
    
    for(int i = 0; i < nFields; ++i)
    {
        PyObject *intObj = PySequence_GetItem(pImageBands, i);
        
        if(!RSGISPY_CHECK_INT(intObj))
        {
            PyErr_SetString(GETSTATE(pImageBands)->error, "Fields must be integers");
            Py_DECREF(intObj);
            return NULL;
        }
        
        imgBands.push_back(RSGISPY_INT_EXTRACT(intObj));
        Py_DECREF(intObj);
    }
    
    try
    {
        rsgis::cmds::executeSubsetImageBands(std::string(pszInputImage), std::string(pszOutputFile), imgBands, std::string(pszGDALFormat), (rsgis::RSGISLibDataType)nDataType);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_Subset(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszInputVector, *pszOutputImage, *pszGDALFormat;
    int nOutDataType;

    if( !PyArg_ParseTuple(args, "ssssi:subset", &pszInputImage, &pszInputVector, &pszOutputImage, &pszGDALFormat, &nOutDataType))
        return NULL;
    
    try
    {
        rsgis::cmds::excecuteSubset(pszInputImage, pszInputVector, pszOutputImage, pszGDALFormat, (rsgis::RSGISLibDataType)nOutDataType);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_Subset2Polys(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszInputVector, *pszOutputImageBase, *pszAttribute, *pszGDALFormat, *pszOutputExt;
    int nOutDataType;

    if( !PyArg_ParseTuple(args, "sssssis:subset2polys", &pszInputImage, &pszInputVector, &pszAttribute, &pszOutputImageBase, &pszGDALFormat, &nOutDataType, &pszOutputExt))
        return NULL;
    
    PyObject *pOutList;
    try
    {
        std::vector<std::string> outFileNames;
        
        rsgis::cmds::excecuteSubset2Polys(pszInputImage, pszInputVector, pszAttribute, pszOutputImageBase, pszGDALFormat, (rsgis::RSGISLibDataType)nOutDataType, pszOutputExt, &outFileNames);
     
        pOutList = PyList_New(outFileNames.size());
        Py_ssize_t nIndex = 0;
        for( std::vector<std::string>::iterator itr = outFileNames.begin(); itr != outFileNames.end(); itr++)
        {
            PyObject *pVal = RSGISPY_CREATE_STRING((*itr).c_str());
            PyList_SetItem(pOutList, nIndex, pVal ); // steals a reference
            nIndex++;
        }
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    return pOutList;
}

static PyObject *ImageUtils_Subset2Img(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszInputROI, *pszOutputImage, *pszGDALFormat;
    int nOutDataType;

    if( !PyArg_ParseTuple(args, "ssssi:subset2img", &pszInputImage, &pszInputROI, &pszOutputImage, &pszGDALFormat, &nOutDataType))
        return NULL;
    
    try
    {
        rsgis::cmds::excecuteSubset2Img(pszInputImage, pszInputROI, pszOutputImage, pszGDALFormat, (rsgis::RSGISLibDataType)nOutDataType);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_StackImageBands(PyObject *self, PyObject *args)
{
    const char *pszOutputFile;
    const char *pszGDALFormat;
    int nDataType;
    float noDataValue;
    PyObject *skipValueObj = NULL;
    PyObject *pInputImages = NULL;
    PyObject *pimageBandNames = NULL;
    
    if( !PyArg_ParseTuple(args, "OOsOfsi:stackImageBands", &pInputImages, &pimageBandNames, &pszOutputFile, &skipValueObj, &noDataValue, &pszGDALFormat, &nDataType))
        return NULL;
    
    bool skipPixels = false;
    float skipValue = 0.0;
    if(skipValueObj != Py_None)
    {
        if(RSGISPY_CHECK_FLOAT(skipValueObj) | RSGISPY_CHECK_INT(skipValueObj))
        {
            skipValue = RSGISPY_FLOAT_EXTRACT(skipValueObj);
            skipPixels = true;
        }
    }
    
    if(!PySequence_Check(pInputImages)) {
        PyErr_SetString(GETSTATE(self)->error, "First argument must be a sequence with a list of band names.");
        return NULL;
    }
    
    // Extract list of images to array of strings.
    int numImages = 0;
    std::string *inputImages = ExtractStringArrayFromSequence(pInputImages, &numImages);
    if(numImages == 0)
    {
        PyErr_SetString(GETSTATE(self)->error, "No input images were provided");
        return NULL;
    }
    
    bool replaceBandNames = false;
    std::string *imageBandNames = NULL;
    if(PySequence_Check(pimageBandNames))
    {
        // Extract list of images to array of strings.
        int numBandName = 0;
        imageBandNames = ExtractStringArrayFromSequence(pimageBandNames, &numBandName);
        if(numBandName == 0)
        {
            replaceBandNames = false;
        }
        else if(numBandName != numImages)
        {
            PyErr_SetString(GETSTATE(self)->error, "The number of band names must match the number of input images.");
            return NULL;
        }
        else
        {
            replaceBandNames = true;
        }
    }
    
    
    
    try
    {
        rsgis::cmds::executeStackImageBands(inputImages, imageBandNames, numImages, std::string(pszOutputFile), skipPixels, skipValue, noDataValue, std::string(pszGDALFormat), (rsgis::RSGISLibDataType)nDataType, replaceBandNames);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


static PyObject *ImageUtils_CreateBlankImage(PyObject *self, PyObject *args)
{
    const char *pszOutputImage, *pszGDALFormat, *wktFile, *wktString;
    int nOutDataType;
    unsigned int numBands, width, height = 0;
    double tlX, tlY, res = 0;
    float pxlVal = 0;
    
    if( !PyArg_ParseTuple(args, "sIIIdddfsssi:createBlankImage", &pszOutputImage, &numBands, &width, &height, &tlX, &tlY, &res, &pxlVal, &wktFile, &wktString, &pszGDALFormat, &nOutDataType))
        return NULL;
    
    try
    {
        rsgis::cmds::executeCreateBlankImage(std::string(pszOutputImage), numBands, width, height, tlX, tlY, res, pxlVal, std::string(wktFile), std::string(wktString), std::string(pszGDALFormat), (rsgis::RSGISLibDataType)nOutDataType);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_CreateCopyImage(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputImage, *pszGDALFormat;
    int nOutDataType;
    unsigned int numBands;
    float pxlVal = 0;
    
    if( !PyArg_ParseTuple(args, "ssIfsi:createCopyImage", &pszInputImage, &pszOutputImage, &numBands, &pxlVal, &pszGDALFormat, &nOutDataType))
        return NULL;
    
    try
    {
        rsgis::cmds::executeCreateCopyBlankImage(std::string(pszInputImage), std::string(pszOutputImage), numBands, pxlVal, std::string(pszGDALFormat), (rsgis::RSGISLibDataType)nOutDataType);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}


static PyObject *ImageUtils_StackStats(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszOutputImage, *pszGDALFormat, *pszCalcStat;
    int nOutDataType;
    PyObject *nBandsObj;
    unsigned int numBands = 0;
    bool allBands = true;
    
    if( !PyArg_ParseTuple(args, "ssOssi:stackStats", &pszInputImage, &pszOutputImage, &nBandsObj, &pszCalcStat, &pszGDALFormat, &nOutDataType))
        return NULL;

    // If an integer has been passes in for bands, extract it, otherwise assume all bands are needed.
    if(nBandsObj != Py_None)
    {
        if(RSGISPY_CHECK_INT(nBandsObj))
        {
            numBands = RSGISPY_INT_EXTRACT(nBandsObj);
            allBands = false;
        }
    }
    
    try
    {
        rsgis::cmds::executeStackStats(pszInputImage, pszOutputImage, pszCalcStat, allBands, numBands, std::string(pszGDALFormat), (rsgis::RSGISLibDataType)nOutDataType);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyObject *ImageUtils_OrderImagesUsingPropValidData(PyObject *self, PyObject *args)
{
    float noDataValue;
    PyObject *pInputImages; // List of input images
    
    // Check parameters are present and of correct type
    if( !PyArg_ParseTuple(args, "Of:orderImageUsingPropValidPxls", &pInputImages, &noDataValue))
        return NULL;
    
    // TODO: Look into this function - doesn't seem to catch when only a single image is provided.
    if(!PySequence_Check(pInputImages)) {
        PyErr_SetString(GETSTATE(self)->error, "First argument must be a sequence");
        return NULL;
    }
    
    // Extract list of images to array of strings.
    int numImages = 0;
    std::vector<std::string> inputImages = ExtractStringVectorFromSequence(pInputImages, &numImages);
    if(numImages == 0)
    {
        PyErr_SetString(GETSTATE(self)->error, "No input images provided");
        return NULL;
    }

    PyObject *outImagesList = NULL;
    try
    {
        std::vector<std::string> orderedInputImages = rsgis::cmds::executeOrderImageUsingValidDataProp(inputImages, noDataValue);
        
        outImagesList = PyTuple_New(orderedInputImages.size());
        
        if(outImagesList == NULL)
        {
            throw rsgis::cmds::RSGISCmdException("Could not create a python list...");
        }
        
        for(unsigned int i = 0; i < orderedInputImages.size(); ++i)
        {
            //std::cout << i << " = " << orderedInputImages.at(i) << std::endl;
            if(PyTuple_SetItem(outImagesList, i, Py_BuildValue("s", orderedInputImages.at(i).c_str())) == -1)
            {
                throw rsgis::cmds::RSGISCmdException("Failed to add a value to the list...");
            }
        }
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }
    
    return outImagesList;
}

// Our list of functions in this module
static PyMethodDef ImageUtilsMethods[] = {
    {"stretchImage", ImageUtils_StretchImage, METH_VARARGS, 
"imageutils.stretchImage(inputimage, outputimage, saveoutstats, outstatsfile, ignorezeros, onepasssd, gdalformat, outtype, stretchtype, stretchparam)\n"
"Stretches (scales) pixel values from 0 - 255, normally for display although also used for normalisation.\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input file\n"
"* outputImage is a string containing the name of the output file\n"
"* saveoutstats is a bool specifying if stats should be saved to a text file.\n"
"* outstatsfile is a string providing the name of the file to save stats to.\n"
"* ignorezeros is a bool specifying if pixels with a value of zero should be ignored.\n"
"* onepasssd is a bool specifying if is single pass should be used for calculating standard deviation (faster but less accurate)\n"
"* gdalformat is a string providing the output gdalformat of the tiles (e.g., KEA).\n"
"* outtype is a rsgislib.TYPE_* value providing the output data type.\n"
"* stretchtype is a STRETCH_* value providing the type of stretch, options are:\n"
"\n"
"   * imageutils.STRETCH_LINEARMINMAX - Stretches between min and max.\n"
"   * imageutils.STRETCH_LINEARPERCENT - Stretches between percentage of image range. Parameter defines percent.\n"
"   * imageutils.STRETCH_LINEARSTDDEV - Stretches between mean - sd to mean + sd. Parameter defines number of standard deviations.\n"
"   * imageutils.STRETCH_EXPONENTIAL - Exponential stretch between mean - 2*sd to mean + 2*sd. No parameter.\n"
"   * imageutils.STRETCH_LOGARITHMIC - Logarithmic stretch between mean - 2*sd to mean + 2*sd. No parameter.\n"
"   * imageutils.STRETCH_POWERLAW - Power law stretch between mean - 2*sd to mean + 2*sd. Parameter defines power.\n"
"\n"
"* stretchparam is a float, providing the input parameter to the stretch (if required).\n"
"\nExample::\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imageutils\n"
"   inputImage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
"   outputImage = './TestOutputs/injune_p142_casi_sub_utm_2sd.kea'\n"
"   gdalformat = 'KEA'\n"
"   dataType = rsgislib.TYPE_8INT\n"
"   imageutils.stretchImage(inputImage, outputImage, False, '', True, False, gdalformat, dataType, imageutils.STRETCH_LINEARSTDDEV, 2)\n"
"\n"},

    {"stretchImageWithStats", ImageUtils_StretchImageWithStats, METH_VARARGS, 
"imageutils.stretchImageWithStats(inputimage, outputimage, instatsfile, gdalformat, outtype, stretchtype, stretchparam)\n"
"Stretches (scales) pixel values from 0 - 255, normally for display although also used for normalisation. Users pre-calculated statistics.\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input file\n"
"* outputImage is a string containing the name of the output file\n"
"* instatsfile is a string providing the name of the file to read stats from.\n"
"* ignorezeros is a bool specifying if pixels with a value of zero should be ignored.\n"
"* onepasssd is a bool specifying if is single pass should be used for calculating standard deviation (faster but less accurate)\n"
"* gdalformat is a string providing the output gdalformat of the tiles (e.g., KEA).\n"
"* outtype is a rsgislib.TYPE_* value providing the output data type.\n"
"* stretchtype is a STRETCH_* value providing the type of stretch, options are:\n"
"\n"
"   * imageutils.STRETCH_LINEARMINMAX - Stretches between min and max.\n"
"   * imageutils.STRETCH_LINEARPERCENT - Stretches between percentage of image range. Parameter defines percent.\n"
"   * imageutils.STRETCH_LINEARSTDDEV - Stretches between mean - sd to mean + sd. Parameter defines number of standard deviations.\n"
"   * imageutils.STRETCH_EXPONENTIAL - Exponential stretch between mean - 2*sd to mean + 2*sd. No parameter.\n"
"   * imageutils.STRETCH_LOGARITHMIC - Logarithmic stretch between mean - 2*sd to mean + 2*sd. No parameter.\n"
"   * imageutils.STRETCH_POWERLAW - Power law stretch between mean - 2*sd to mean + 2*sd. Parameter defines power.\n"
"\n"
"* stretchparam is a float, providing the input parameter to the stretch (if required).\n"
"\nExample::\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imageutils\n"
"   inputImage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
"   inputImageStats = './Rasters/injune_p142_casi_sub_utm_stats.txt'\n"
"   outputImage = './TestOutputs/injune_p142_casi_sub_utm_2sd.kea'\n"
"   gdalformat = 'KEA'\n"
"   dataType = rsgislib.TYPE_8INT\n"
"   imageutils.stretchImageWithStats(inputImage, outputImage, inputImageStats, True, False, gdalformat, dataType, imageutils.STRETCH_LINEARSTDDEV, 2)\n"
"\n"},

    {"maskImage", ImageUtils_maskImage, METH_VARARGS,
"imageutils.maskImage(inputimage, imagemask, outputimage, gdalformat, type, outvalue, maskvalue)"
"Mask image.\n"
"Where:\n"
"\n"
"* inputImage is a string containing the name of the input file\n"
"* imagemask is a string\n"
"* outputimage is a string\n"
"* gdalformat is a string\n"
"* type is a rsgislib.TYPE_* value\n"
"* outvalue is a float\n"
"* maskvalue is a float\n"
"\n"},

    {"createTiles", ImageUtils_createTiles, METH_VARARGS,
"imageutils.createTiles(inputimage, baseimage, width, height, overlap, offsettiling, gdalformat, type, ext)\n"     
"Create tiles from a larger image, useful for splitting a large image into multiple smaller ones for processing.\n"
"Where\n"
"\n"
"* inputImage is a string containing the name of the input file\n"
"* baseimage is a string containing the base name of the output file\n    the number of the tile and file extension will be appended.\n"
"* width is the width of each tile, in pixels.\n"
"* height is the height of each tile, in pixels.\n"
"* overlap is the overlap between tiles, in pixels\n"
"* offsettiling is a bool, determining if tiles should start halfway into the image\n    useful for generating overlapping sets of tiles.\n"
"* gdalformat is a string providing the output gdalformat of the tiles (e.g., KEA).\n"
"* type is a rsgislib.TYPE_* value providing the output data type of the tiles.\n"
"* ext is a string providing the extension for the tiles (as required by the specified data type).\n"
"\nA list of strings containing the filenames is returned.\n"
"Example::\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imageutils\n"
"   inputImage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
"   outBase = './TestOutputs/Tiles/injune_p142_casi_sub_utm'\n"
"   width = 100\n"
"   height = width\n"
"   overlap = 5\n"
"   offsettiling = 0\n"
"   format = 'KEA'\n"
"   dataType = rsgislib.TYPE_32INT\n"
"   ext='kea'\n"
"   imageutils.createTiles(inputImage, outBase, width, height, overlap, offsettiling, gdalformat, dataType, ext)\n"
"\n"},
    
    {"createImageMosaic", ImageUtils_createImageMosaic, METH_VARARGS,
"imageutils.createImageMosaic(inputimagelist, outputimage, backgroundVal, skipVal, skipBand, overlapBehaviour, gdalformat, type)\n"  
"Create mosaic from list of input images.\n"
"\n"
"* inputimagelist is a list of input images.\n"
"* outputimage is a string containing the name of the output mosaic\n"
"* backgroundVal is a float providing the background (nodata) value for the mosaic\n"
"* skipVal is a float providing the value to be skipped (nodata values) in the input images\n"
"* skipBand is an integer providing the band to check for skipVal\n"
"* overlapBehaviour is an integer specifying the behaviour for overlaping regions\n"
"\n"
"      * 0 - Overwrite\n"
"      * 1 - Overwrite if value of new pixel is lower (minimum)\n"
"      * 2 - Overwrite if value of new pixel is higher (maximum)\n"
"\n"
"* gdalformat is a string providing the gdalformat of the output image (e.g., KEA).\n"
"* type is a rsgislib.TYPE_* value providing the data type of the output image.\n"
"\nExample::\n"
"\n"
"	import rsgislib\n"
"	from rsgislib import imageutils\n"
"	import glob\n"
"	# Search for all files with the extension 'kea'\n"
"	inputList = glob.glob('./TestOutputs/Tiles/*.kea')\n"
"	outImage = './TestOutputs/injune_p142_casi_sub_utm_mosaic.kea'\n"
"	backgroundVal = 0.\n"
"	skipVal = 0.\n"
"	skipBand = 1\n"
"	overlapBehaviour = 0\n"
"	format = 'KEA'\n"
"	dataType = rsgislib.TYPE_32FLOAT\n"
"	imageutils.createImageMosaic(inputList, outImage, backgroundVal, skipVal, skipBand, overlapBehaviour, gdalformat, dataType)\n"
"\n"},
 
    {"includeImages", ImageUtils_IncludeImages, METH_VARARGS,
"imageutils.includeImages(baseImage, inputImages, inputBands=None)\n"  
"Create mosaic from list of input images.\n"
"\n"
"* baseImage is a string containing the name of the input image to add image to\n"
"* inputimagelist is a list of input images\n"
"* inputBands is a subset of input bands to use (optional)\n"
"\nExample::\n"
"\n"
"	import rsgislib\n"
"	from rsgislib import imageutils\n"
"	import glob\n"
"	# Search for all files with the extension 'kea'\n"
"	baseImage = './TestOutputs/injune_p142_casi_sub_utm_mosaic.kea'\n"
"	inputList = glob.glob('./TestOutputs/Tiles/*.kea')\n"
"	imageutils.includeImages(baseImage, inputList)\n"
"\n"},
 
  
    {"popImageStats", (PyCFunction)ImageUtils_PopImageStats, METH_VARARGS | METH_KEYWORDS,
"rsgislib.imageutils.popImageStats(image, usenodataval=True,nodataval=0, calcpyramids=True)\n"
"Calculate the image statistics and build image pyramids populating the image file.\n"
"\n"
"* image is a string containing the name of the input file\n"
"* usenodataval is a boolean stating whether the no data value is to be used (default=True).\n"
"* nodataval is a floating point value to be used as the no data value (default=0.0).\n"
"* calcpyramids is a boolean stating whether image pyramids should be calculated (default=True).\n"
"\nExample::\n"
"\n"
"   from rsgislib import imageutils\n"
"   inputImage = './TestOutputs/injune_p142_casi_sub_utm.kea'\n"
"   imageutils.popImageStats(inputImage,True,0.,True)\n"
"\n"},
    
    {"assignProj", ImageUtils_AssignProj, METH_VARARGS,
"rsgislib.imageutils.assignProj(inputImage, wktString, wktStringFile)\n"
"Assign a projection to the input GDAL image file.\n"
"\n"
"* inputImage is a string containing the name of the input file\n"
"* wktString is the wkt string to be assigned to the image. If None then it will be read from the wktStringFile.\n"
"* wktStringFile is a file path to a text file containing the WKT string to be assigned. This is ignored if wktString is not None.\n"
"\n"},
    
    {"copyProjFromImage", ImageUtils_CopyProjFromImage, METH_VARARGS,
"rsgislib.imageutils.copyProjFromImage(inputImage, refImage)\n"
"Copy the projection from a reference image to an input GDAL image file.\n"
"\n"
"* inputImage is a string containing the name and path of the input file\n"
"* refImage is a string containing the name and path of the reference image.\n"
"\n"},
    
    {"copySpatialAndProjFromImage", ImageUtils_CopySpatialAndProjFromImage, METH_VARARGS,
"rsgislib.imageutils.copySpatialAndProjFromImage(inputImage, refImage)\n"
"Copy the spatial information and projection from a reference image to an input GDAL image file.\n"
"\n"
"* inputImage is a string containing the name and path of the input file\n"
"* refImage is a string containing the name and path of the reference image.\n"
"\n"},

    {"assignSpatialInfo", ImageUtils_AssignSpatialInfo, METH_VARARGS,
"rsgislib.imageutils.assignSpatialInfo(inputImage, tlX, tlY, resX, resY, rotX, rotY)\n"
"Assign the spatial information to an input GDAL image file.\n"
"\n"
"* inputImage is a string containing the name and path of the input file\n"
"* tlX is a double representing the top left X coordinate of the image.\n"
"* tlY is a double representing the top left Y coordinate of the image.\n"
"* resX is a double representing X resolution of the image.\n"
"* resY is a double representing Y resolution of the image.\n"
"* rotX is a double representing X rotation of the image.\n"
"* rotY is a double representing Y rotation of the image.\n"
"\n"},
    
    {"extractZoneImageValues2HDF", ImageUtils_ExtractZoneImageValues2HDF, METH_VARARGS,
"rsgislib.imageutils.extractZoneImageValues2HDF(inputImage, imageMask, outputHDF, maskValue)\n"
"Extract the all the pixel values for raster regions to a HDF5 file (1 column for each image band).\n"
"\n"
"* inputImage is a string containing the name and path of the input file\n"
"* imageMask is a string containing the name and path of the input image mask file; the mask file must have only 1 image band.\n"
"* outputHDF is a string containing the name and path of the output HDF5 file\n"
"* maskValue is a float containing the value of the pixel within the mask for which values are to be extracted\n"
"\n"},

    {"selectImageBands", ImageUtils_SelectImageBands, METH_VARARGS,
"rsgislib.imageutils.selectImageBands(inputImage, outputImage, gdalformat, type, bands)\n"
"Copy selected image bands from an image to a new image.\n"
"\n"
"* inputImage is a string containing the name and path of the input file\n"
"* outputImage is a string containing the name and path of the output file.\n"
"* gdalformat is a string providing the gdalformat of the output image (e.g., KEA).\n"
"* type is a rsgislib.TYPE_* value providing the data type of the output image.\n"
"* bands is a list of integers for the bands in the input image to exported to the output image (Note band count starts at 1)."
"\nExample::\n"
"\n"
"   import rsgislib.imageutils\n"
"   import rsgislib\n"
"   bands = [1,2]\n"
"   rsgislib.imageutils.selectImageBands('N06W053_07_ALL_sl_sub.kea', 'N06W053_07_ALL_sl_sub_HHVV.kea', 'KEA', rsgislib.TYPE_32INT, bands)\n"
"\n"},

    {"subset", ImageUtils_Subset, METH_VARARGS,
"imageutils.subset(inputimage, inputvector, outputimage, gdalformat, type)"
"Subset an image to the bounding box of a vector.\n"
"Where:\n"
"\n"
"* inputimage is a string providing the name of the input file.\n"
"* inputvector is a string providing the vector which the image is to be clipped to. \n"
"* outputimage is a string providing the output image. \n"
"* gdalformat is a string providing the gdalformat of the output image (e.g., KEA).\n"
"* type is a rsgislib.TYPE_* value providing the data type of the output image.\n"
"Example::\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imageutils\n"
"   inputImage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
"   inputVector = './Vectors/injune_p142_plot_location_utm.shp'\n"
"   outputImage = './TestOutputs/injune_p142_casi_sub_utm_subset.kea'\n"
"   gdalformat = 'KEA'\n"
"   dataType = rsgislib.TYPE_32FLOAT\n"
"   imageutils.subset(inputImage, inputVector, outputImage, gdalformat, dataType)\n"
"\n"},

    {"subset2polys", ImageUtils_Subset2Polys, METH_VARARGS,
"imageutils.subset(inputimage, inputvector, attribute, baseimage, gdalformat, type, ext)"
"Subset an image to the bounding box of a each polygon in an input vector.\n"
"Useful for splitting an image into tiles of unequal sizes.\n"
"Where:\n"
"\n"
"* inputimage is a string providing the name of the input file.\n"
"* inputvector is a string providing the vector which the image is to be clipped to. \n"
"* attribute is a string providing the attribute in the vector to use for the ouput name\n"
"* baseimage is a string providing the base name of the output file. The specified attribute of each polygon and extension will be appended."
"* gdalformat is a string providing the output gdalformat of the subsets (e.g., KEA).\n"
"* type is a rsgislib.TYPE_* value providing the output data type of the subsets.\n"
"* ext is a string providing the extension for the tiles (as required by the specified data gdalformat).\n"
"\nA list of strings containing the filenames is returned.\n"
"Example::\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imageutils\n"
"   inputImage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
"   inputVector = './Vectors/injune_p142_plot_location_utm.shp'\n"
"   attribute = 'PLOTNO'\n"
"   outputImageBase = './TestOutputs/injune_p142_casi_sub_utm_subset_polys_'\n"
"   gdalformat = 'KEA'\n"
"   dataType = rsgislib.TYPE_32FLOAT\n"
"   ext = 'kea'\n"
"   imageutils.subset2polys(inputImage, inputVector, attribute, outputImageBase, gdalformat, dataType, ext)\n"
"\n"},

    {"subset2img", ImageUtils_Subset2Img, METH_VARARGS,
"imageutils.subset2img(inputimage, inputROIimage, outputimage, gdalformat, type)"
"Subset an image to the bounding box of an image.\n"
"Where:\n"
"\n"
"* inputimage is a string providing the name of the input file.\n"
"* inputvector is a string providing the image which the 'inputimage' is to be clipped to. \n"
"* outputimage is a string providing the output image. \n"
"* gdalformat is a string providing the gdalformat of the output image (e.g., KEA).\n"
"* type is a rsgislib.TYPE_* value providing the data type of the output image.\n"
"Example::\n"
"\n"
"   import rsgislib\n"
"   from rsgislib import imageutils\n"
"   inputImage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
"   inputVector = './Vectors/injune_p142_plot_location_utm.shp'\n"
"   outputImage = './TestOutputs/injune_p142_casi_sub_utm_subset.kea'\n"
"   gdalformat = 'KEA'\n"
"   dataType = rsgislib.TYPE_32FLOAT\n"
"   imageutils.subset(inputImage, inputVector, outputImage, gdalformat, dataType)\n"
"\n"},
    
    
{"stackImageBands", ImageUtils_StackImageBands, METH_VARARGS,
    "imageutils.stackImageBands(inputImages, imageBandNames, outputImage, skipValue, noDataValue, gdalformat, type)\n"
    "Create a single image from list of input images through band stacking.\n"
	"\n"
    "* inputImages is a list of input images.\n"
    "* imageBandNames is a list of band names (one for each input image). If None then ignored.\n"
    "* outputImage is a string containing the name and path for the outputted image.\n"
    "* skipVal is a float providing the value to be skipped (nodata values) in the input images (If None then ignored)\n"
    "* noDataValue is float specifying a no data value.\n"
    "* gdalformat is a string providing the gdalformat of the output image (e.g., KEA).\n"
    "* type is a rsgislib.TYPE_* value providing the data type of the output image.\n"
    "\nExample::\n"
    "\n"
    "   import rsgislib\n"
    "   from rsgislib import imageutils\n"
    "   imageList = ['./Rasters/injune_p142_casi_sub_utm_single_band.vrt','./Rasters/injune_p142_casi_sub_utm_single_band.vrt']\n"
    "   bandNamesList = ['Image1','Image2']\n"
    "   outputImage = './TestOutputs/injune_p142_casi_sub_stack.kea'\n"
    "   gdalformat = 'KEA'\n"
    "   dataType = rsgislib.TYPE_32FLOAT\n"
    "   imageutils.stackImageBands(imageList, bandNamesList, outputImage, None, 0, gdalformat, dataType)\n"
    "\n"},
    
{"createBlankImage", ImageUtils_CreateBlankImage, METH_VARARGS,
    "imageutils.createBlankImage(outputImage, numBands, width, height, tlX, tlY, res, pxlVal, wktFile, wktString, gdalformat, type)\n"
    "Create a new blank image with the parameters specified.\n"
	"\n"
    "* outputImage is a string containing the name and path for the outputted image.\n"
    "* numBands is an integer specifying the number of image bands in the output image.\n"
    "* width is an integer specifying the width of the output image.\n"
    "* height is an integer specifying the height of the output image.\n"
    "* tlX is a double specifying the Top Left pixel X coordinate (eastings) of the output image.\n"
    "* tlY is a double specifying the Top Left pixel Y coordinate (northings) of the output image.\n"
    "* res is a double specifying the pixel resolution of the output image.\n"
    "* pxlVal is a float specifying the pixel value of the output image.\n"
    "* wktFile is a string specifying the location of a file containing the WKT string representing the coordinate system and projection of the output image (if specified this parameter overrides the wktString parameter).\n"
    "* wktString is a string specifying the WKT string representing the coordinate system and projection of the output image.\n"
    "* gdalformat is a string providing the gdalformat of the output image (e.g., KEA).\n"
    "* type is a rsgislib.TYPE_* value providing the data type of the output image.\n"
    "\nExample::\n"
    "\n"
    "\n"},

{"createCopyImage", ImageUtils_CreateCopyImage, METH_VARARGS,
    "imageutils.createCopyImage(inputImage, outputImage, numBands, pxlVal, gdalformat, type)\n"
    "Create a new blank image with the parameters specified.\n"
	"\n"
    "* inputImage is a string containing the name and path for the input image, which is to be copied.\n"
    "* outputImage is a string containing the name and path for the outputted image.\n"
    "* numBands is an integer specifying the number of image bands in the output image.\n"
    "* pxlVal is a float specifying the pixel value of the output image.\n"
    "* gdalformat is a string providing the gdalformat of the output image (e.g., KEA).\n"
    "* type is a rsgislib.TYPE_* value providing the data type of the output image.\n"
    "\nExample::\n"
    "\n"
    "   inputImage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
    "   outputImage = './TestOutputs/injune_p142_casi_sub_utm_blank.kea'\n"
    "   gdalformat = 'KEA'\n"
    "   dataType = rsgislib.TYPE_32FLOAT\n"
    "   imageutils.createCopyImage(inputImage, outputImage, 1, 3, gdalformat, dataType)\n"
    "\n"},

{"stackStats", ImageUtils_StackStats, METH_VARARGS,
    "imageutils.stackStats(inputImage, outputImage, numBands, stat, gdalformat, type)\n"
    "Calculate statistics for every pixel in a stack of image. If all bands are used a single band image is produced with the specified statistics.\n"
    "If a number of bands are specified statistics are taken over every n bands to provide an image with B / n bands (where B is the number of input bands. \
    For example, can be used to produce monthly composite images from a stack with images from every day.\n"
    "\n"
    "* inputImage is a string containing the name and path for the input image.\n"
    "* outputImage is a string containing the name and path for the output image.\n"
    "* numBands is an integer specifying the number of image bands in the output image, pass 'None' to use all bands.\n"
    "* stat is a string providing the statistics to calculate, options are 'mean', 'min', 'max', and 'range'.\n"
    "* gdalformat is a string providing the gdalformat of the output image (e.g., KEA).\n"
    "* type is a rsgislib.TYPE_* value providing the data type of the output image.\n"
    "\n"
    "Example::\n"
    "\n"
    "   import rsgislib\n"
    "   from rsgislib import imageutils\n"
    "   inputImage = './Rasters/injune_p142_casi_sub_utm.kea'\n"
    "   outputImage = './TestOutputs/injune_p142_casi_sub_utm_stackStats.kea'\n"
    "   gdalformat = 'KEA'\n"
    "   dataType = rsgislib.TYPE_32FLOAT\n"
    "   imageutils.stackStats(inputImage, outputImage, None, 'mean', gdalformat, dataType)\n"
    "\n"},
    
{"orderImageUsingValidPxls", ImageUtils_OrderImagesUsingPropValidData, METH_VARARGS,
    "imageutils.orderImageUsingValidPxls(inputImages, noDataVal)\n"
    "Order the list of input images based on the their proportion of valid image pixels.\n"
    "The primary use of this function is expected to be order (rank) images ahead of mosaicing.\n"
    "\n"
    "* inputImages is a list of string containing the name and path for the input images.\n"
    "* noDataVal is a float which specifies the no data value used to defined \'invalid\' pixels.\n"
    "\n"
    "Returns: a list of images ordered, from low to high (i.e., the first image will be the image \n"
    "         with the smallest number of valid image pixels).\n"
    "\n"},


    {NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static int ImageUtils_traverse(PyObject *m, visitproc visit, void *arg) 
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int ImageUtils_clear(PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_imageutils",
        NULL,
        sizeof(struct ImageUtilsState),
        ImageUtilsMethods,
        NULL,
        ImageUtils_traverse,
        ImageUtils_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit__imageutils(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_imageutils(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_imageutils", ImageUtilsMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct ImageUtilsState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_imageutils.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

    // add constants
    PyModule_AddIntConstant(pModule, "STRETCH_LINEARMINMAX", rsgis::cmds::linearMinMax);
    PyModule_AddIntConstant(pModule, "STRETCH_LINEARPERCENT", rsgis::cmds::linearPercent);
    PyModule_AddIntConstant(pModule, "STRETCH_LINEARSTDDEV", rsgis::cmds::linearStdDev);
    PyModule_AddIntConstant(pModule, "STRETCH_HISTOGRAM", rsgis::cmds::histogram);
    PyModule_AddIntConstant(pModule, "STRETCH_EXPONENTIAL", rsgis::cmds::exponential);
    PyModule_AddIntConstant(pModule, "STRETCH_LOGARITHMIC", rsgis::cmds::logarithmic);
    PyModule_AddIntConstant(pModule, "STRETCH_POWERLAW", rsgis::cmds::powerLaw);

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}

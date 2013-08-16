/*
 *  zonalstats.cpp
 *  RSGIS_LIB
 *
 *  Created by Dan Clewley on 09/08/2013.
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
#include "cmds/RSGISCmdZonalStats.h"

/* An exception object for this module */
/* created in the init function */
struct ZonalStatsState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct ZonalStatsState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct ZonalStatsState _state;
#endif

static void FreePythonObjects(std::vector<PyObject*> toFree) {
    std::vector<PyObject*>::iterator iter;
    for(iter = toFree.begin(); iter != toFree.end(); ++iter) {
        Py_XDECREF(*iter);
    }
}

static PyObject *ZonalStats_PointValue2SHP(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszInputVector, *pszOutputVector;
    int force, useBandNames;
    if( !PyArg_ParseTuple(args, "sssii:pointValue2SHP", &pszInputImage, &pszInputVector, &pszOutputVector, 
                                &force, &useBandNames))
        return NULL;

    try
    {
        rsgis::cmds::executePointValue(pszInputImage, pszInputVector, pszOutputVector, false, force, useBandNames);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ZonalStats_PointValue2TXT(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszInputVector, *pszOutputTxt;
    int useBandNames;
    if( !PyArg_ParseTuple(args, "sssi:pointValue2TXT", &pszInputImage, &pszInputVector, &pszOutputTxt, 
                                &useBandNames))
        return NULL;

    try
    {
        rsgis::cmds::executePointValue(pszInputImage, pszInputVector, pszOutputTxt, true, false, useBandNames);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ZonalStats_PixelVals2TXT(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszInputVector, *pszOutputTextBase, *pzsPolyAttribute;
    int pixelInPolyMethod;
    if( !PyArg_ParseTuple(args, "ssssi:pixelVals2TXT", &pszInputImage, &pszInputVector, &pszOutputTextBase, &pzsPolyAttribute, &pixelInPolyMethod))
        return NULL;
    try
    {
        rsgis::cmds::executePixelVals2txt(pszInputImage, pszInputVector, pszOutputTextBase, pzsPolyAttribute, "csv", false, pixelInPolyMethod);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ZonalStats_PixelStats2SHP(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszInputVector, *pszOutputVector;
    int force, useBandNames, pixelInPolyMethod;
    PyObject *pBandAttZonalStatsCmds;
    if( !PyArg_ParseTuple(args, "sssOiii:pixelStats2SHP", &pszInputImage, &pszInputVector, &pszOutputVector, 
                                &pBandAttZonalStatsCmds, &force, &useBandNames, &pixelInPolyMethod))
        return NULL;
    
    rsgis::cmds::RSGISBandAttZonalStatsCmds *zonalAtts = new rsgis::cmds::RSGISBandAttZonalStatsCmds();   // the c++ struct

    // declare and initialise pointers for all the attributes of the struct
    PyObject *pMinThreshold, *pMaxThreshold, *pCalcCount, *pCalcMin, *pCalcMax, *pCalcMean, *pCalcStdDev, *pCalcMode, *pCalcSum;
    pMinThreshold = pMaxThreshold = pCalcCount = pCalcMin = pCalcMax = pCalcMean = pCalcStdDev = pCalcMode = pCalcSum = NULL;

    std::vector<PyObject*> extractedAttributes;     // store a list of extracted pyobjects to dereference
    extractedAttributes.push_back(pBandAttZonalStatsCmds);

    
    /* Check if values have been set. 
        If not set assume false.
        If set, assume true for now - will check the value later
    */
    pCalcCount = PyObject_GetAttrString(pBandAttZonalStatsCmds, "calcCount");
    extractedAttributes.push_back(pCalcCount);
    zonalAtts->calcCount = !(pCalcCount == NULL || !RSGISPY_CHECK_INT(pCalcCount));
    
    pCalcMin = PyObject_GetAttrString(pBandAttZonalStatsCmds, "calcMin");
    extractedAttributes.push_back(pCalcMin);
    zonalAtts->calcMin = !(pCalcMin == NULL || !RSGISPY_CHECK_INT(pCalcMin));

    pCalcMax = PyObject_GetAttrString(pBandAttZonalStatsCmds, "calcMax");
    extractedAttributes.push_back(pCalcMax);
    zonalAtts->calcMax = !(pCalcMax == NULL || !RSGISPY_CHECK_INT(pCalcMax));
    
    pCalcMean = PyObject_GetAttrString(pBandAttZonalStatsCmds, "calcMean");
    extractedAttributes.push_back(pCalcMean);
    zonalAtts->calcMean = !(pCalcMean == NULL || !RSGISPY_CHECK_INT(pCalcMean));
    
    pCalcStdDev = PyObject_GetAttrString(pBandAttZonalStatsCmds, "calcStdDev");
    extractedAttributes.push_back(pCalcStdDev);
    zonalAtts->calcStdDev= !(pCalcStdDev == NULL || !RSGISPY_CHECK_INT(pCalcStdDev));
    
    pCalcMode = PyObject_GetAttrString(pBandAttZonalStatsCmds, "calcMode");
    extractedAttributes.push_back(pCalcMode);
    zonalAtts->calcMode = !(pCalcMode == NULL || !RSGISPY_CHECK_INT(pCalcMode));
    
    pCalcSum = PyObject_GetAttrString(pBandAttZonalStatsCmds, "calcSum");
    extractedAttributes.push_back(pCalcSum);
    zonalAtts->calcSum = !(pCalcSum == NULL || !RSGISPY_CHECK_INT(pCalcSum));

    // check the calcValue and extract fields if required
    if(zonalAtts->calcCount) {zonalAtts->calcCount = RSGISPY_INT_EXTRACT(pCalcCount);}
    if(zonalAtts->calcMin) {zonalAtts->calcMin = RSGISPY_INT_EXTRACT(pCalcMin);}
    if(zonalAtts->calcMax) {zonalAtts->calcMax = RSGISPY_INT_EXTRACT(pCalcMax);}
    if(zonalAtts->calcMean) {zonalAtts->calcMean = RSGISPY_INT_EXTRACT(pCalcMean);}
    if(zonalAtts->calcStdDev) {zonalAtts->calcStdDev = RSGISPY_INT_EXTRACT(pCalcStdDev);}
    if(zonalAtts->calcMode) {zonalAtts->calcMode = RSGISPY_INT_EXTRACT(pCalcMode);}
    if(zonalAtts->calcSum) {zonalAtts->calcSum = RSGISPY_INT_EXTRACT(pCalcSum);}
    
    // Check if thresholds have been set - use default values (+/- Inf) if not.
    // Check for float or int (want float but don't complain if we get int.)
    pMinThreshold = PyObject_GetAttrString(pBandAttZonalStatsCmds, "minThreshold");
    extractedAttributes.push_back(pMinThreshold);
    if( ( pMinThreshold == NULL ) || ( pMinThreshold == Py_None ) || !(RSGISPY_CHECK_FLOAT(pMinThreshold) || RSGISPY_CHECK_INT(pMinThreshold))) 
    {
        zonalAtts->minThreshold = -std::numeric_limits<double>::infinity();
    }
    else{zonalAtts->minThreshold = RSGISPY_FLOAT_EXTRACT(pMinThreshold);}
    
    pMaxThreshold = PyObject_GetAttrString(pBandAttZonalStatsCmds, "maxThreshold");
    extractedAttributes.push_back(pMaxThreshold);
    if( ( pMaxThreshold == NULL ) || ( pMaxThreshold == Py_None ) || !(RSGISPY_CHECK_FLOAT(pMaxThreshold) || RSGISPY_CHECK_INT(pMaxThreshold)))
    {
        zonalAtts->maxThreshold = +std::numeric_limits<double>::infinity();
    }
    else{zonalAtts->maxThreshold = RSGISPY_FLOAT_EXTRACT(pMaxThreshold);}
    
    FreePythonObjects(extractedAttributes);
  
    try
    {
        rsgis::cmds::executePixelStats(pszInputImage, pszInputVector, pszOutputVector, zonalAtts, 
            "", false, force, useBandNames, pixelInPolyMethod);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *ZonalStats_PixelStats2TXT(PyObject *self, PyObject *args)
{
    const char *pszInputImage, *pszInputVector, *pszOutputTxt;
    int useBandNames, pixelInPolyMethod;
    PyObject *pBandAttZonalStatsCmds;
    if( !PyArg_ParseTuple(args, "sssOii:pixelStats2SHP", &pszInputImage, &pszInputVector, &pszOutputTxt, 
                                &pBandAttZonalStatsCmds, &useBandNames, &pixelInPolyMethod))
        return NULL;
    
    rsgis::cmds::RSGISBandAttZonalStatsCmds *zonalAtts = new rsgis::cmds::RSGISBandAttZonalStatsCmds();   // the c++ struct

    // declare and initialise pointers for all the attributes of the struct
    PyObject *pMinThreshold, *pMaxThreshold, *pCalcCount, *pCalcMin, *pCalcMax, *pCalcMean, *pCalcStdDev, *pCalcMode, *pCalcSum;
    pMinThreshold = pMaxThreshold = pCalcCount = pCalcMin = pCalcMax = pCalcMean = pCalcStdDev = pCalcMode = pCalcSum = NULL;

    std::vector<PyObject*> extractedAttributes;     // store a list of extracted pyobjects to dereference
    extractedAttributes.push_back(pBandAttZonalStatsCmds);

    
    /* Check if values have been set. 
        If not set assume false.
        If set, assume true for now - will check the value later
    */
    pCalcCount = PyObject_GetAttrString(pBandAttZonalStatsCmds, "calcCount");
    extractedAttributes.push_back(pCalcCount);
    zonalAtts->calcCount = !(pCalcCount == NULL || !RSGISPY_CHECK_INT(pCalcCount));
    
    pCalcMin = PyObject_GetAttrString(pBandAttZonalStatsCmds, "calcMin");
    extractedAttributes.push_back(pCalcMin);
    zonalAtts->calcMin = !(pCalcMin == NULL || !RSGISPY_CHECK_INT(pCalcMin));

    pCalcMax = PyObject_GetAttrString(pBandAttZonalStatsCmds, "calcMax");
    extractedAttributes.push_back(pCalcMax);
    zonalAtts->calcMax = !(pCalcMax == NULL || !RSGISPY_CHECK_INT(pCalcMax));
    
    pCalcMean = PyObject_GetAttrString(pBandAttZonalStatsCmds, "calcMean");
    extractedAttributes.push_back(pCalcMean);
    zonalAtts->calcMean = !(pCalcMean == NULL || !RSGISPY_CHECK_INT(pCalcMean));
    
    pCalcStdDev = PyObject_GetAttrString(pBandAttZonalStatsCmds, "calcStdDev");
    extractedAttributes.push_back(pCalcStdDev);
    zonalAtts->calcStdDev= !(pCalcStdDev == NULL || !RSGISPY_CHECK_INT(pCalcStdDev));
    
    pCalcMode = PyObject_GetAttrString(pBandAttZonalStatsCmds, "calcMode");
    extractedAttributes.push_back(pCalcMode);
    zonalAtts->calcMode = !(pCalcMode == NULL || !RSGISPY_CHECK_INT(pCalcMode));
    
    pCalcSum = PyObject_GetAttrString(pBandAttZonalStatsCmds, "calcSum");
    extractedAttributes.push_back(pCalcSum);
    zonalAtts->calcSum = !(pCalcSum == NULL || !RSGISPY_CHECK_INT(pCalcSum));

    // check the calcValue and extract fields if required
    if(zonalAtts->calcCount) {zonalAtts->calcCount = RSGISPY_INT_EXTRACT(pCalcCount);}
    if(zonalAtts->calcMin) {zonalAtts->calcMin = RSGISPY_INT_EXTRACT(pCalcMin);}
    if(zonalAtts->calcMax) {zonalAtts->calcMax = RSGISPY_INT_EXTRACT(pCalcMax);}
    if(zonalAtts->calcMean) {zonalAtts->calcMean = RSGISPY_INT_EXTRACT(pCalcMean);}
    if(zonalAtts->calcStdDev) {zonalAtts->calcStdDev = RSGISPY_INT_EXTRACT(pCalcStdDev);}
    if(zonalAtts->calcMode) {zonalAtts->calcMode = RSGISPY_INT_EXTRACT(pCalcMode);}
    if(zonalAtts->calcSum) {zonalAtts->calcSum = RSGISPY_INT_EXTRACT(pCalcSum);}
    
    // Check if thresholds have been set - use default values (+/- Inf) if not.
    // Check for float or int (want float but don't complain if we get int.)
    pMinThreshold = PyObject_GetAttrString(pBandAttZonalStatsCmds, "minThreshold");
    extractedAttributes.push_back(pMinThreshold);
    if( ( pMinThreshold == NULL ) || ( pMinThreshold == Py_None ) || !(RSGISPY_CHECK_FLOAT(pMinThreshold) || RSGISPY_CHECK_INT(pMinThreshold))) 
    {
        zonalAtts->minThreshold = -std::numeric_limits<double>::infinity();
    }
    else{zonalAtts->minThreshold = RSGISPY_FLOAT_EXTRACT(pMinThreshold);}
    
    pMaxThreshold = PyObject_GetAttrString(pBandAttZonalStatsCmds, "maxThreshold");
    extractedAttributes.push_back(pMaxThreshold);
    if( ( pMaxThreshold == NULL ) || ( pMaxThreshold == Py_None ) || !(RSGISPY_CHECK_FLOAT(pMaxThreshold) || RSGISPY_CHECK_INT(pMaxThreshold)))
    {
        zonalAtts->maxThreshold = +std::numeric_limits<double>::infinity();
    }
    else{zonalAtts->maxThreshold = RSGISPY_FLOAT_EXTRACT(pMaxThreshold);}
    
    FreePythonObjects(extractedAttributes);
  
    try
    {
        rsgis::cmds::executePixelStats(pszInputImage, pszInputVector, pszOutputTxt, zonalAtts, 
            "", true, false, useBandNames, pixelInPolyMethod);
    }
    catch(rsgis::cmds::RSGISCmdException &e)
    {
        PyErr_SetString(GETSTATE(self)->error, e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}


// Our list of functions in this module
static PyMethodDef ZonalStatsMethods[] = {
    {"pointValue2SHP", ZonalStats_PointValue2SHP, METH_VARARGS, 
"Extract pixel value for each point in a shape file and output as a shapefile.\n"
"call signature: zonalstats.pointValue2SHP(inputimage, inputvector, outputvector, force, useBandNames=True)\n"
"where:\n"
"  * inputimage is a string containing the name of the input image\n"
"  * inputvector is a string containing the name of the input vector\n"
"  * outputvector is a string containing the name of the output vector\n"
"  * force is a bool, specifying whether to force removal of the output vector if it exists\n"
"  * useBandNames is a bool, specifying whether to use the band names of the input dataset in the output file (if not uses b1, b2, etc.,\n"},

    {"pointValue2TXT", ZonalStats_PointValue2TXT, METH_VARARGS, 
"Extract pixel value for each point in a shape file and output as a CSV.\n"
"call signature: zonalstats.pointValue2TXT(inputimage, inputvector, outputtxt, useBandNames=True)\n"
"where:\n"
"  * inputimage is a string containing the name of the input image\n"
"  * inputvector is a string containing the name of the input vector\n"
"  * outputtxt is a string containing the name of the output text file\n"
"  * useBandNames is a bool, specifying whether to use the band names of the input dataset in the output file (if not uses b1, b2, etc.,\n"},

    {"pixelVals2TXT", ZonalStats_PixelVals2TXT, METH_VARARGS, 
"Extract pixel value for all pixels within a polygon and save a seperate CSV for each polygon in the shapefile.\n"
"call signature: zonalstats.pixelVals2TXT(inputimage, inputvector, outputtxtBase, attribute, pixelInPolyMethod)\n"
"where:\n"
"  * inputimage is a string containing the name of the input image.\n"
"  * inputvector is a string containing the name of the input vector.\n"
"  * outputtxtBase is a string containing the base name for output text files.\n"
"  * attribute is a string specifying an identifier for each polygon to be used for the name of each output text file.\n"
"  * pixelInPolyMethod is the method for determining if a pixel is included with a polygon of type rsgislib.zonalstats.METHOD_*.\n"},

    {"pixelStats2SHP", ZonalStats_PixelStats2SHP, METH_VARARGS, 
"Calculate statistics for pixels falling within each polygon in a shapefile output as a shapefile.\n"
"call signature: zonalstats.pixelStats2SHP(inputimage, inputvector, outputvector, zonalattributes, force, useBandNames, pixelInPolyMethod)\n"
"where:\n"
"  * inputimage is a string containing the name of the input image\n"
"  * inputvector is a string containing the name of the input vector\n"
"  * outputvector is a string containing the name of the output vector\n"
"  * ZonalAttributes is an rsgislib.rastergis.zonalattributes object that has attributes in line with rsgis::cmds::RSGISBandAttZStatsCmds\n"
"\n"
"       * minThreshold, a float providing the minimum pixel value to include when calculating statistics.\n"
"       * maxThreshold, a float providing the maximum pixel value to include when calculating statistics.\n"
"       * calcCount, a bool specifying whether to report a count of pixels between thresholds.\n"
"       * calcMin, a bool specifying whether to report the minimum of pixels between thresholds.\n"
"       * calcMax, a bool specifying whether to report the maximum of pixels between thresholds.\n"
"       * calcMean, a bool specifying whether to report the mean of pixels between thresholds.\n"
"       * calcStdDev, a bool specifying whether to report the standard deviation of pixels between thresholds.\n"
"       * calcMode, a bool specifying whether to report the mode of pixels between thresholds (for integer datasets only).\n"
"       * calcSum, a bool specifying whether to report the sum of pixels between thresholds.\n"
"\n"
"  * force is a bool, specifying whether to force removal of the output vector if it exists\n"
"  * useBandNames is a bool, specifying whether to use the band names of the input dataset in the output file (if not uses b1, b2, etc.,\n"
"  * pixelInPolyMethod is the method for determining if a pixel is included with a polygon of type rsgislib.zonalstats.METHOD_*.\n"},

    {"pixelStats2TXT", ZonalStats_PixelStats2TXT, METH_VARARGS, 
"Calculate statistics for pixels falling within each polygon in a shapefile output as a CSV.\n"
"call signature: zonalstats.pixelStats2SHP(inputimage, inputvector, outputtxt, zonalattributes, useBandNames, pixelInPolyMethod)\n"
"where:\n"
"  * inputimage is a string containing the name of the input image\n"
"  * inputvector is a string containing the name of the input vector\n"
"  * outputtxt is a string containing the name of the output text file\n"
"  * ZonalAttributes is an rsgislib.rastergis.zonalattributes object that has attributes in line with rsgis::cmds::RSGISBandAttZStatsCmds\n"
"\n"
"       * minThreshold, a float providing the minimum pixel value to include when calculating statistics.\n"
"       * maxThreshold, a float providing the maximum pixel value to include when calculating statistics.\n"
"       * calcCount, a bool specifying whether to report a count of pixels between thresholds.\n"
"       * calcMin, a bool specifying whether to report the minimum of pixels between thresholds.\n"
"       * calcMax, a bool specifying whether to report the maximum of pixels between thresholds.\n"
"       * calcMean, a bool specifying whether to report the mean of pixels between thresholds.\n"
"       * calcStdDev, a bool specifying whether to report the standard deviation of pixels between thresholds.\n"
"       * calcMode, a bool specifying whether to report the mode of pixels between thresholds (for integer datasets only).\n"
"       * calcSum, a bool specifying whether to report the sum of pixels between thresholds.\n"
"\n"
"  * useBandNames is a bool, specifying whether to use the band names of the input dataset in the output file (if not uses b1, b2, etc.,\n"
"  * pixelInPolyMethod is the method for determining if a pixel is included with a polygon of type rsgislib.zonalstats.METHOD_*.\n"},

    {NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static int ZonalStats_traverse(PyObject *m, visitproc visit, void *arg) 
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int ZonalStats_clear(PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_zonalstats",
        NULL,
        sizeof(struct ZonalStatsState),
        ZonalStatsMethods,
        NULL,
        ZonalStats_traverse,
        ZonalStats_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit__zonalstats(void)

#else
#define INITERROR return

PyMODINIT_FUNC
init_zonalstats(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *pModule = PyModule_Create(&moduledef);
#else
    PyObject *pModule = Py_InitModule("_zonalstats", ZonalStatsMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    struct ZonalStatsState *state = GETSTATE(pModule);

    // Create and add our exception type
    state->error = PyErr_NewException("_zonalstats.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}

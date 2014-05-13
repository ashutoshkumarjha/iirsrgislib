"""
This namespace contains rsgislib Python bindings 

Please be aware that the following variables have been 
defined to match enums within RSGISLib.

Data Types for images:

    * TYPE_UNDEFINED = 0
    * TYPE_8INT = 1
    * TYPE_16INT = 2
    * TYPE_32INT = 3
    * TYPE_64INT = 4
    * TYPE_8UINT = 5
    * TYPE_16UINT = 6
    * TYPE_32UINT = 7
    * TYPE_64UINT = 8
    * TYPE_32FLOAT = 9
    * TYPE_64FLOAT = 10

Methods for the Maximum Likelihood Classifier:

    * METHOD_SAMPLES = 0        # as calculated by ML
    * METHOD_AREA = 1           # priors set by the relative area
    * METHOD_EQUAL = 2          # priors all equal
    * METHOD_USERDEFINED = 3    # priors passed in to function
    * METHOD_WEIGHTED = 4       # priors by area but with a weight applied

Shape indexes used with RasterGIS:

    * SHAPE_SHAPENA = 0
    * SHAPE_SHAPEAREA = 1
    * SHAPE_ASYMMETRY = 2
    * SHAPE_BORDERINDEX = 3
    * SHAPE_BORDERLENGTH = 4
    * SHAPE_COMPACTNESS = 5
    * SHAPE_DENSITY = 6
    * SHAPE_ELLIPTICFIT = 7
    * SHAPE_LENGTH = 8
    * SHAPE_LENGTHWIDTH = 9
    * SHAPE_WIDTH = 10
    * SHAPE_MAINDIRECTION = 11
    * SHAPE_RADIUSLARGESTENCLOSEDELLIPSE = 12
    * SHAPE_RADIUSSMALLESTENCLOSEDELLIPSE = 13
    * SHAPE_RECTANGULARFIT = 14
    * SHAPE_ROUNDNESS = 15
    * SHAPE_SHAPEINDEX = 16

Methods of initialising KMEANS:

    * INITCLUSTER_RANDOM = 0
    * INITCLUSTER_DIAGONAL_FULL = 1
    * INITCLUSTER_DIAGONAL_STDDEV = 2
    * INITCLUSTER_DIAGONAL_FULL_ATTACH = 3
    * INITCLUSTER_DIAGONAL_STDDEV_ATTACH = 4
    * INITCLUSTER_KPP = 5

"""
import os.path
import os
import subprocess
import fnmatch
import time

TYPE_UNDEFINED = 0
TYPE_8INT = 1
TYPE_16INT = 2
TYPE_32INT = 3
TYPE_64INT = 4
TYPE_8UINT = 5
TYPE_16UINT = 6
TYPE_32UINT = 7
TYPE_64UINT = 8
TYPE_32FLOAT = 9
TYPE_64FLOAT = 10

METHOD_SAMPLES = 0      # as calculated by ML
METHOD_AREA = 1         # priors set by the relative area
METHOD_EQUAL = 2        # priors all equal
METHOD_USERDEFINED = 3  # priors passed in to function
METHOD_WEIGHTED = 4     # priors by area but with a weight applied

SHAPE_SHAPENA = 0
SHAPE_SHAPEAREA = 1
SHAPE_ASYMMETRY = 2
SHAPE_BORDERINDEX = 3
SHAPE_BORDERLENGTH = 4
SHAPE_COMPACTNESS = 5
SHAPE_DENSITY = 6
SHAPE_ELLIPTICFIT = 7
SHAPE_LENGTH = 8
SHAPE_LENGTHWIDTH = 9
SHAPE_WIDTH = 10
SHAPE_MAINDIRECTION = 11
SHAPE_RADIUSLARGESTENCLOSEDELLIPSE = 12
SHAPE_RADIUSSMALLESTENCLOSEDELLIPSE = 13
SHAPE_RECTANGULARFIT = 14
SHAPE_ROUNDNESS = 15
SHAPE_SHAPEINDEX = 16

INITCLUSTER_RANDOM = 0
INITCLUSTER_DIAGONAL_FULL = 1
INITCLUSTER_DIAGONAL_STDDEV = 2
INITCLUSTER_DIAGONAL_FULL_ATTACH = 3
INITCLUSTER_DIAGONAL_STDDEV_ATTACH = 4
INITCLUSTER_KPP = 5

def getRSGISLibVersion():
    """ Calls rsgis-config to get the version number. """

    # Try calling rsgis-config to get minor version number
    try:
        out = subprocess.Popen('rsgis-config --version',shell=True,stdin=subprocess.PIPE, stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        (stdout, stderr) = out.communicate()
        versionStr = stdout.decode()
        versionStr = versionStr.split('\n')[0]
    except Exception:
        versionStr = '2.2'
    return(versionStr)

__version__ = getRSGISLibVersion()

class RSGISPyException(Exception):
    """
    A class representing the RSGIS exception.
    """
    
    def __init__(self, value):
        """
        Init for the RSGISPyException class
        """
        self.value = value
        
    def __str__(self):
        """
        Return a string representation of the exception
        """
        return repr(self.value)

class RSGISPyUtils (object):
    """
    A class with useful utilities within RSGISLib.
    """
    
    def getFileExtension(self, format):
        """
        A function to get the extension for a given file format 
        (NOTE, currently only KEA, GTIFF, HFA, PCI and ENVI are supported).
        """
        ext = ".NA"
        if format.lower() == "kea":
            ext = ".kea"
        elif format.lower() == "gtiff":
            ext = ".tif"
        elif format.lower() == "hfa":
            ext = ".img"
        elif format.lower() == "envi":
            ext = ".env"
        elif format.lower() == "pcidsk":
            ext = ".pix"
        else:
            raise RSGISPyException("The extension for the format specified is unknown.")
        return ext
    
    def getGDALFormatFromExt(self, fileName):
        """ Get GDAL format, based on filename """
        gdalStr = ''
        extension = os.path.splitext(fileName)[-1] 
        if extension == '.env':
            gdalStr = 'ENVI'
        elif extension == '.kea':
            gdalStr = 'KEA'
        elif extension == '.tif' or extension == '.tiff':
            gdalStr = 'GTiff'
        elif extension == '.img':
            gdalStr = 'HFA'
        elif extension == '.pix':
            gdalStr = 'PCIDSK'
        else:
            raise Exception('Type not recognised')
        
        return gdalStr

    def deleteFileWithBasename(self, filePath):
        """
        Function to delete all the files which have a path
        and base name defined in the filePath attribute.
        """
        fileDIR = os.path.split(filePath)[0]
        fileName = os.path.split(filePath)[1]
        
        if os.path.isdir(fileDIR):
            basename = os.path.splitext(fileName)[0]
            for file in os.listdir(fileDIR):
                if fnmatch.fnmatch(file, basename+str('.*')):
                    print("Deleting file: " + str(os.path.join(fileDIR, file)))
                    os.remove(os.path.join(fileDIR, file))
                
    def deleteDIR(self, dirPath):
        """
        A function which will delete a directory, if files and other directories
        are within the path specified they will be recursively deleted as well.
        So becareful you don't delete things within meaning it.
        """
        for root, dirs, files in os.walk(dirPath, topdown=False):
            for name in files:
                os.remove(os.path.join(root, name))
            for name in dirs:
                os.rmdir(os.path.join(root, name))
        os.rmdir(dirPath)
        print("Deleted " + dirPath)

    def getRSGISLibDataType(self, gdaltype):
        """ Convert from GDAL data type string to 
            RSGISLib data type int.
        """
        gdaltype = gdaltype.lower()
        if gdaltype == 'byte' or gdaltype == 'int8':
            return TYPE_8INT
        elif gdaltype == 'int16':
            return TYPE_16INT
        elif gdaltype == 'int32':
            return TYPE_32INT
        elif gdaltype == 'int64':
            return TYPE_64INT
        elif gdaltype == 'uint8':
            return TYPE_8UINT
        elif gdaltype == 'uint16':
            return TYPE_16UINT
        elif gdaltype == 'uint32':
            return TYPE_32UINT
        elif gdaltype == 'uint64':
            return TYPE_64UINT
        elif gdaltype == 'float32':
            return TYPE_32FLOAT
        elif gdaltype == 'float64':
            return TYPE_64FLOAT
        else:
            raise RSGISPyException("The data type '%s' is unknown / not supported."%(gdaltype))

class RSGISTime (object):
    """ Class to calculate run time for a function, format and print out (similar to for XML interface).

        Need to call start before running function and end immediately after.
        Example::

            t = RSGISTime()
            t.start()
            rsgislib.segmentation.clump(kMeansFileZonesNoSgls, initClumpsFile, gdalFormat, False, 0) 
            t.end()
        
        Note, this is only designed to provide some general feedback, for benchmarking the timeit module
        is better suited."""

    def __init__(self):
        self.startTime = time.time()
        self.endTime = time.time()

    def start(self, printStartTime=False):
        """ Start timer, optionally printing start time"""
        self.startTime = time.time()
        if printStartTime:
            print(time.strftime('Start Time: %H:%M:%S, %a %b %m %Y.'))


    def end(self,reportDiff = True):
        """ End timer and optionally print difference."""
        self.endTime = time.time()
        if reportDiff:
            self.calcDiff()

    def calcDiff(self):
        """ Calculate time difference, format and print. """
        timeDiff = self.endTime - self.startTime

        if timeDiff <= 1:
            print("Algorithm Completed in less than a second.")
        elif timeDiff > 3600:
            timeDiff = timeDiff/3600.;
            timeDiffStr = str(round(timeDiff,2))
            print('''Algorithm Completed in %s hours.'''%(timeDiffStr))
        elif timeDiff > 60:
            timeDiff = timeDiff/60.;
            timeDiffStr = str(round(timeDiff,2))
            print('''Algorithm Completed in %s minutes.'''%(timeDiffStr))
        else:
            timeDiffStr = str(round(timeDiff,2))
            print('''Algorithm Completed in %s seconds.'''%(timeDiffStr))
        
        




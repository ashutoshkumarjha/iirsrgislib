#!/usr/bin/env python
"""
The tools.visualisation module contains functions for aiding visualisation of data.
"""

# Import modules
import rsgislib
import rsgislib.imageutils

import os.path
import os
import shutil
import subprocess
import math


def createKMZImg(inputImg, outputFile, bands, reprojLatLong=True, finiteMsk=False):
    """
    A function to convert an input image to a KML/KMZ file, where the input image
    is stretched and bands sub-selected / ordered as required for visualisation.
    
    Where:
    
    :param inputImg: input image file (any format that gdal supports)
    :param outputFile: output image file (extension kmz for KMZ output / kml for KML output)
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param reprojLatLong: specify whether the image should be explicitly reprojected to WGS84 Lat/Long before transformation to KML.
    :param finiteMsk: specify whether the image data should be masked so all values are finite before stretching.
    
    """
    
    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')
    rsgisUtils = rsgislib.RSGISPyUtils()
    nImgBands = rsgisUtils.getImageBandCount(inputImg)

    uid_str = rsgisUtils.uidGenerator()
    tmpDIR = os.path.join(os.path.dirname(inputImg), uid_str)
    if not os.path.exists(tmpDIR):
        os.makedirs(tmpDIR)
    baseName = os.path.splitext(os.path.basename(inputImg))[0]
    
    selImgBandsImg = ''
    if (nImgBands == 1) and (not multiBand):
        selImgBandsImg = inputImg
    elif (nImgBands == 3) and (multiBand) and (bandLst[0] == '1') and (bandLst[1] == '2') and (bandLst[2] == '3'):
        selImgBandsImg = inputImg
    else:
        sBands = []
        for strBand in bandLst:
            sBands.append(int(strBand))
        selImgBandsImg = os.path.join(tmpDIR, baseName+'_sband.kea')
        rsgislib.imageutils.selectImageBands(inputImg, selImgBandsImg, 'KEA', rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), sBands)
    
    img2Stch = selImgBandsImg
    if finiteMsk:
        finiteMskImg = os.path.join(tmpDIR, baseName+'_FiniteMsk.kea')
        rsgislib.imageutils.genFiniteMask(selImgBandsImg, finiteMskImg, 'KEA')
        img2Stch = os.path.join(tmpDIR, baseName+'_Msk2FiniteRegions.kea')
        rsgislib.imageutils.maskImage(selImgBandsImg, finiteMskImg, img2Stch, 'KEA', rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), 0, 0)
    
    stretchImg = os.path.join(tmpDIR, baseName+'_stretch.kea')
    rsgislib.imageutils.stretchImage(img2Stch, stretchImg, False, '', True, False, 'KEA', rsgislib.TYPE_8UINT, rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)
    
    gdalInFile = stretchImg
    if reprojLatLong:
        latLongImg = os.path.join(tmpDIR, baseName+'_latlong.kea')
        outWKT = os.path.join(tmpDIR, baseName+'_latlong.wkt')
        rsgisUtils.writeList2File(['GEOGCS["WGS_1984",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.2572235630016],TOWGS84[0,0,0,0,0,0,0]],PRIMEM["Greenwich",0],UNIT["degree",0.0174532925199433],AUTHORITY["EPSG","4326"]]'], outWKT)
        rsgislib.imageutils.reprojectImage(stretchImg, latLongImg, outWKT, gdalformat='KEA', interp='cubic', inWKT=None, noData=0.0, outPxlRes='auto', snap2Grid=True)
        gdalInFile = latLongImg
    
    cmd = 'gdal_translate -of KMLSUPEROVERLAY ' + gdalInFile + ' ' + outputFile
    print(cmd)
    try:
        subprocess.check_call(cmd, shell=True)
    except OSError as e:
       raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
        
    shutil.rmtree(tmpDIR)


def createWebTilesImg(inputImg, outputDIR, bands, zoomLevels='2-10', resample='average', finiteMsk=False):
    """
    A function to convert an input image to a tile cache for web map servers, where the input image
    is stretched and bands sub-selected / ordered as required for visualisation.
    
    Where:
    
    :param inputImg: input image file (any format that gdal supports)
    :param outputDIR: output directory within which the cache will be created.
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param zoomLevels: The zoom levels to be created for the web tile cache.
    :param resample: Method of resampling (average,near,bilinear,cubic,cubicspline,lanczos,antialias)
    :param finiteMsk: specify whether the image data should be masked so all values are finite before stretching.
    
    """
    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')
    rsgisUtils = rsgislib.RSGISPyUtils()
    nImgBands = rsgisUtils.getImageBandCount(inputImg)
    
    tmpDIR = os.path.join(os.path.dirname(inputImg), rsgisUtils.uidGenerator())
    os.makedirs(tmpDIR)
    baseName = os.path.splitext(os.path.basename(inputImg))[0]
    
    selImgBandsImg = ''
    if (nImgBands == 1) and (not multiBand):
        selImgBandsImg = inputImg
    elif (nImgBands == 3) and (multiBand) and (bandLst[0] == '1') and (bandLst[1] == '2') and (bandLst[2] == '3'):
        selImgBandsImg = inputImg
    else:
        sBands = []
        for strBand in bandLst:
            sBands.append(int(strBand))
        selImgBandsImg = os.path.join(tmpDIR, baseName+'_sband.kea')
        rsgislib.imageutils.selectImageBands(inputImg, selImgBandsImg, 'KEA', rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), sBands)
    
    img2Stch = selImgBandsImg
    if finiteMsk:
        finiteMskImg = os.path.join(tmpDIR, baseName+'_FiniteMsk.kea')
        rsgislib.imageutils.genFiniteMask(selImgBandsImg, finiteMskImg, 'KEA')
        img2Stch = os.path.join(tmpDIR, baseName+'_Msk2FiniteRegions.kea')
        rsgislib.imageutils.maskImage(selImgBandsImg, finiteMskImg, img2Stch, 'KEA', rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), 0, 0)
    
    stretchImg = os.path.join(tmpDIR, baseName+'_stretch.kea')
    rsgislib.imageutils.stretchImage(img2Stch, stretchImg, False, '', True, False, 'KEA', rsgislib.TYPE_8UINT, rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)
    
    cmd = 'gdal2tiles.py -r ' + resample + ' -z ' + zoomLevels + ' -a  0 ' + stretchImg + ' ' + outputDIR
    print(cmd)
    try:
        subprocess.check_call(cmd, shell=True)
    except OSError as e:
       raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
        
    shutil.rmtree(tmpDIR)


def createWebTilesImg(inputImg, bands, outputDIR, zoomLevels='2-10', img_stats_msk=None, img_msk_vals=1, tmp_dir=None,
                      webview=True):
    """
    A function to produce a web cache for the input image.

    :param inputImg: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param outputDIR: output directory within which the cache will be created.
    :param zoomLevels: The zoom levels to be created for the web tile cache.
    :param img_stats_msk: Optional (default=None) input image which is used to define regions calculate
                          the image stats for stretch.
    :param img_msk_vals: The pixel(s) value define the region of interest in the image mask
                        (can be list of values or single value).
    :param tmp_dir: an input directory which can be used to write tempory files/directories. If not provided
                    (i.e., input is None) then a local directory will be define in the same folder as the input
                    image.
    :param webview: Provide default GDAL leaflet web viewer.

    """
    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')
    rsgisUtils = rsgislib.RSGISPyUtils()
    nImgBands = rsgisUtils.getImageBandCount(inputImg)
    img_no_data_val = rsgisUtils.getImageNoDataValue(inputImg)

    uid_str = rsgisUtils.uidGenerator()
    tmpDIR = os.path.join(os.path.dirname(inputImg), uid_str)
    if tmp_dir is not None:
        tmpDIR = os.path.join(tmp_dir, uid_str)
    if not os.path.exists(tmpDIR):
        os.makedirs(tmpDIR)
    baseName = os.path.splitext(os.path.basename(inputImg))[0]

    selImgBandsImg = ''
    if (nImgBands == 1) and (not multiBand):
        selImgBandsImg = inputImg
    elif (nImgBands == 3) and (multiBand) and (bandLst[0] == '1') and (bandLst[1] == '2') and (bandLst[2] == '3'):
        selImgBandsImg = inputImg
    else:
        sBands = []
        for strBand in bandLst:
            sBands.append(int(strBand))
        selImgBandsImg = os.path.join(tmpDIR, baseName + '_sband.kea')
        rsgislib.imageutils.selectImageBands(inputImg, selImgBandsImg, 'KEA',
                                             rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), sBands)

    img2Stch = selImgBandsImg
    stretchImg = os.path.join(tmpDIR, baseName + '_stretch.kea')
    if img_stats_msk is not None:
        img2StchMskd = os.path.join(tmpDIR, baseName + '_MskdImg.kea')
        rsgislib.imageutils.maskImage(selImgBandsImg, img_stats_msk, img2StchMskd, 'KEA',
                                      rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), img_no_data_val, img_msk_vals)
        stretchImgStats = os.path.join(tmpDIR, baseName + '_stretch_statstmp.txt')
        stretchImgTmp = os.path.join(tmpDIR, baseName + '_stretch_tmp.kea')
        rsgislib.imageutils.stretchImageNoData(img2StchMskd, stretchImgTmp, True, stretchImgStats, img_no_data_val,
                                               False, 'KEA', rsgislib.TYPE_8UINT,
                                               rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

        rsgislib.imageutils.stretchImageWithStatsNoData(img2Stch, stretchImg, stretchImgStats, 'KEA', rsgislib.TYPE_8UINT,
                                                  img_no_data_val, rsgislib.imageutils.STRETCH_LINEARMINMAX, 2)
    else:
        rsgislib.imageutils.stretchImageNoData(img2Stch, stretchImg, False, '', img_no_data_val, False, 'KEA', rsgislib.TYPE_8UINT,
                                         rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

    webview_opt = 'none'
    if webview:
        webview_opt = 'leaflet'

    cmd = 'gdal2tiles.py -r average -z {0} -a 0 -w {1} {2} {3}'.format(zoomLevels, webview_opt, stretchImg, outputDIR)
    print(cmd)
    try:
        subprocess.check_call(cmd, shell=True)
    except OSError as e:
        raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
    shutil.rmtree(tmpDIR)


def createQuicklookImgs(inputImg, bands, outputImgs='quicklook.jpg', output_img_sizes=250, scale_axis='auto', img_stats_msk=None,
                            img_msk_vals=1, tmp_dir=None):
    """
    A function to produce

    :param inputImg: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param outputImgs: a single output image or list of output images. The same size as output_img_sizes.
    :param output_img_sizes: the output image size (in pixels) or list of output image sizes.
    :param scale_axis: the axis to which the output_img_sizes refer. Options: width, height or auto.
                       Auto applies the output_img_sizes to the longest of the two axes.
    :param img_stats_msk: Optional (default=None) input image which is used to define regions calculate
                          the image stats for stretch.
    :param img_msk_vals: The pixel(s) value define the region of interest in the image mask
                        (can be list of values or single value).
    :param tmp_dir: an input directory which can be used to write tempory files/directories. If not provided
                    (i.e., input is None) then a local directory will be define in the same folder as the input
                    image.

    """
    if scale_axis not in ['width', 'height', 'auto']:
        raise rsgislib.RSGISPyException("Input parameter 'scale_axis' must have the value 'width', 'height' or 'auto'.")

    n_out_imgs = 1
    if type(outputImgs) is list:
        n_out_imgs = len(outputImgs)
        if type(output_img_sizes) is not list:
            raise rsgislib.RSGISPyException("If the outputImgs input is a list so must output_img_sizes.")
        if len(output_img_sizes) != n_out_imgs:
            raise rsgislib.RSGISPyException("outputImgs and output_img_sizes must be the same length")

        if n_out_imgs == 1:
            outputImgs = outputImgs[0]
            output_img_sizes = output_img_sizes[0]

    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')
    rsgisUtils = rsgislib.RSGISPyUtils()
    nImgBands = rsgisUtils.getImageBandCount(inputImg)
    img_no_data_val = rsgisUtils.getImageNoDataValue(inputImg)

    tmpDIR = os.path.join(os.path.dirname(inputImg), rsgisUtils.uidGenerator())
    if tmp_dir is not None:
        tmpDIR = os.path.join(tmp_dir, rsgisUtils.uidGenerator())
    os.makedirs(tmpDIR)
    baseName = os.path.splitext(os.path.basename(inputImg))[0]

    selImgBandsImg = ''
    if (nImgBands == 1) and (not multiBand):
        selImgBandsImg = inputImg
    elif (nImgBands == 3) and (multiBand) and (bandLst[0] == '1') and (bandLst[1] == '2') and (bandLst[2] == '3'):
        selImgBandsImg = inputImg
    else:
        sBands = []
        for strBand in bandLst:
            sBands.append(int(strBand))
        selImgBandsImg = os.path.join(tmpDIR, baseName + '_sband.kea')
        rsgislib.imageutils.selectImageBands(inputImg, selImgBandsImg, 'KEA',
                                             rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), sBands)

    img2Stch = selImgBandsImg
    stretchImg = os.path.join(tmpDIR, baseName + '_stretch.kea')
    if img_stats_msk is not None:
        img2StchMskd = os.path.join(tmpDIR, baseName + '_MskdImg.kea')
        rsgislib.imageutils.maskImage(selImgBandsImg, img_stats_msk, img2StchMskd, 'KEA',
                                      rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), img_no_data_val, img_msk_vals)
        stretchImgStats = os.path.join(tmpDIR, baseName + '_stretch_statstmp.txt')
        stretchImgTmp = os.path.join(tmpDIR, baseName + '_stretch_tmp.kea')
        rsgislib.imageutils.stretchImageNoData(img2StchMskd, stretchImgTmp, True, stretchImgStats, img_no_data_val,
                                               False, 'KEA', rsgislib.TYPE_8UINT,
                                               rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

        rsgislib.imageutils.stretchImageWithStatsNoData(img2Stch, stretchImg, stretchImgStats, 'KEA', rsgislib.TYPE_8UINT,
                                                        img_no_data_val, rsgislib.imageutils.STRETCH_LINEARMINMAX, 2)
    else:
        rsgislib.imageutils.stretchImageNoData(img2Stch, stretchImg, False, '', img_no_data_val, False, 'KEA',
                                               rsgislib.TYPE_8UINT, rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

    if scale_axis == 'auto':
        x_size, y_size = rsgisUtils.getImageSize(stretchImg)
        if x_size > y_size:
            scale_axis = 'width'
        else:
            scale_axis = 'height'

    if n_out_imgs == 1:
        if scale_axis == 'width':
            out_size = '-outsize {0} 0'.format(output_img_sizes)
        else:
            out_size = '-outsize 0 {0}'.format(output_img_sizes)

        cmd = 'gdal_translate -of JPEG -ot Byte -scale {0}} -r average {1} {2}'.format(out_size, stretchImg, outputImgs)
        print(cmd)
        try:
            subprocess.check_call(cmd, shell=True)
        except OSError as e:
            raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)

    elif n_out_imgs > 1:
        for i in range(n_out_imgs):
            if scale_axis == 'width':
                out_size = '-outsize {0} 0'.format(output_img_sizes[i])
            else:
                out_size = '-outsize 0 {0}'.format(output_img_sizes[i])

            cmd = 'gdal_translate -of JPEG -ot Byte -scale {0} -r average {1} {2}'.format(out_size, stretchImg,
                                                                                    outputImgs[i])
            print(cmd)
            try:
                subprocess.check_call(cmd, shell=True)
            except OSError as e:
                raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
    shutil.rmtree(tmpDIR)


def createMBTileFile(input_img, bands, output_mbtiles, scale_input_img=50, img_stats_msk=None, img_msk_vals=1,
                     tmp_dir=None, tile_format='PNG'):
    """
    A function to produce

    :param input_img: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param output_mbtiles: output MBTiles file which will be created.
    :param scale_input_img: The scale of the output image with respect to the input as a percentage (e.g., 50% reduction in size).
    :param img_stats_msk: Optional (default=None) input image which is used to define regions calculate
                          the image stats for stretch.
    :param img_msk_vals: The pixel(s) value define the region of interest in the image mask
                        (can be list of values or single value).
    :param tmp_dir: an input directory which can be used to write tempory files/directories. If not provided
                    (i.e., input is None) then a local directory will be define in the same folder as the input
                    image.
    :param tile_format: Specify the tile file format to use, options: PNG, PNG8 and JPG. Default: PNG

    """
    from osgeo import gdal
    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')
    rsgisUtils = rsgislib.RSGISPyUtils()
    nImgBands = rsgisUtils.getImageBandCount(input_img)
    img_no_data_val = rsgisUtils.getImageNoDataValue(input_img)

    uid_str = rsgisUtils.uidGenerator()
    tmpDIR = os.path.join(os.path.dirname(input_img), uid_str)
    if tmp_dir is not None:
        tmpDIR = os.path.join(tmp_dir, uid_str)
    if not os.path.exists(tmpDIR):
        os.makedirs(tmpDIR)
    baseName = os.path.splitext(os.path.basename(input_img))[0]

    selImgBandsImg = ''
    if (nImgBands == 1) and (not multiBand):
        selImgBandsImg = input_img
    elif (nImgBands == 3) and (multiBand) and (bandLst[0] == '1') and (bandLst[1] == '2') and (bandLst[2] == '3'):
        selImgBandsImg = input_img
    else:
        sBands = []
        for strBand in bandLst:
            sBands.append(int(strBand))
        selImgBandsImg = os.path.join(tmpDIR, baseName + '_sband.kea')
        rsgislib.imageutils.selectImageBands(input_img, selImgBandsImg, 'KEA',
                                             rsgisUtils.getRSGISLibDataTypeFromImg(input_img), sBands)

    img2Stch = selImgBandsImg
    stretchImg = os.path.join(tmpDIR, baseName + '_stretch.kea')
    if img_stats_msk is not None:
        img2StchMskd = os.path.join(tmpDIR, baseName + '_MskdImg.kea')
        rsgislib.imageutils.maskImage(selImgBandsImg, img_stats_msk, img2StchMskd, 'KEA',
                                      rsgisUtils.getRSGISLibDataTypeFromImg(input_img), img_no_data_val, img_msk_vals)
        stretchImgStats = os.path.join(tmpDIR, baseName + '_stretch_statstmp.txt')
        stretchImgTmp = os.path.join(tmpDIR, baseName + '_stretch_tmp.kea')
        rsgislib.imageutils.stretchImageNoData(img2StchMskd, stretchImgTmp, True, stretchImgStats, img_no_data_val,
                                               False, 'KEA', rsgislib.TYPE_8UINT,
                                               rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)
        rsgislib.imageutils.stretchImageWithStatsNoData(img2Stch, stretchImg, stretchImgStats, 'KEA',
                                                        rsgislib.TYPE_8UINT, img_no_data_val,
                                                        rsgislib.imageutils.STRETCH_LINEARMINMAX, 2)
    else:
        rsgislib.imageutils.stretchImageNoData(img2Stch, stretchImg, False, '', img_no_data_val, False, 'KEA',
                                               rsgislib.TYPE_8UINT, rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

    stretchImgReProj = os.path.join(tmpDIR, baseName + '_stretch_epsg3857.kea')
    rsgislib.imageutils.gdal_warp(stretchImg, stretchImgReProj, 3857, interp='near', gdalformat='KEA', options=[])

    try:
        import tqdm
        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    trans_opt = gdal.TranslateOptions(format='MBTiles', widthPct=scale_input_img, heightPct=scale_input_img, noData=0,
                                      options=["TILE_FORMAT={}".format(tile_format)], callback=callback)
    gdal.Translate(output_mbtiles, stretchImgReProj, options=trans_opt)
    rsgislib.imageutils.popImageStats(output_mbtiles, usenodataval=True, nodataval=0, calcpyramids=True)
    shutil.rmtree(tmpDIR)


def createWebTilesVisGTIFFImg(input_img, bands, output_dir, scaled_gtiff_img, zoomLevels='2-10', img_stats_msk=None,
                              img_msk_vals=1, tmp_dir=None, webview=True, scale=0):
    """
    A function to produce web cache and scaled and stretched geotiff.

    :param input_img: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param output_dir: output directory within which the cache will be created.
    :param scaled_gtiff_img: output geotiff image path and file name.
    :param zoomLevels: The zoom levels to be created for the web tile cache.
    :param img_stats_msk: Optional (default=None) input image which is used to define regions calculate
                          the image stats for stretch.
    :param img_msk_vals: The pixel(s) value define the region of interest in the image mask
                        (can be list of values or single value).
    :param tmp_dir: an input directory which can be used to write tempory files/directories. If not provided
                    (i.e., input is None) then a local directory will be define in the same folder as the input
                    image.
    :param webview: Provide default GDAL leaflet web viewer.
    :param scale: the scale output geotiff. Input is percentage in the x-axis. If zero (default) then no scaling
                  will be applied.

    """
    from osgeo import gdal
    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')
    rsgisUtils = rsgislib.RSGISPyUtils()
    nImgBands = rsgisUtils.getImageBandCount(input_img)
    img_no_data_val = rsgisUtils.getImageNoDataValue(input_img)

    uid_str = rsgisUtils.uidGenerator()
    tmpDIR = os.path.join(os.path.dirname(input_img), uid_str)
    if tmp_dir is not None:
        tmpDIR = os.path.join(tmp_dir, uid_str)
    if not os.path.exists(tmpDIR):
        os.makedirs(tmpDIR)
    baseName = os.path.splitext(os.path.basename(input_img))[0]

    selImgBandsImg = ''
    if (nImgBands == 1) and (not multiBand):
        selImgBandsImg = input_img
    elif (nImgBands == 3) and (multiBand) and (bandLst[0] == '1') and (bandLst[1] == '2') and (bandLst[2] == '3'):
        selImgBandsImg = input_img
    else:
        sBands = []
        for strBand in bandLst:
            sBands.append(int(strBand))
        selImgBandsImg = os.path.join(tmpDIR, baseName + '_sband.kea')
        rsgislib.imageutils.selectImageBands(input_img, selImgBandsImg, 'KEA',
                                             rsgisUtils.getRSGISLibDataTypeFromImg(input_img), sBands)

    img2Stch = selImgBandsImg
    stretchImg = os.path.join(tmpDIR, baseName + '_stretch.kea')
    if img_stats_msk is not None:
        img2StchMskd = os.path.join(tmpDIR, baseName + '_MskdImg.kea')
        rsgislib.imageutils.maskImage(selImgBandsImg, img_stats_msk, img2StchMskd, 'KEA',
                                      rsgisUtils.getRSGISLibDataTypeFromImg(input_img), img_no_data_val, img_msk_vals)
        stretchImgStats = os.path.join(tmpDIR, baseName + '_stretch_statstmp.txt')
        stretchImgTmp = os.path.join(tmpDIR, baseName + '_stretch_tmp.kea')
        rsgislib.imageutils.stretchImageNoData(img2StchMskd, stretchImgTmp, True, stretchImgStats, img_no_data_val,
                                               False, 'KEA', rsgislib.TYPE_8UINT,
                                               rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

        rsgislib.imageutils.stretchImageWithStatsNoData(img2Stch, stretchImg, stretchImgStats, 'KEA',
                                                        rsgislib.TYPE_8UINT,
                                                        img_no_data_val, rsgislib.imageutils.STRETCH_LINEARMINMAX, 2)
    else:
        rsgislib.imageutils.stretchImageNoData(img2Stch, stretchImg, False, '', img_no_data_val, False, 'KEA',
                                               rsgislib.TYPE_8UINT,
                                               rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

    webview_opt = 'none'
    if webview:
        webview_opt = 'leaflet'

    cmd = 'gdal2tiles.py -r average -z {0} -a 0 -w {1} {2} {3}'.format(zoomLevels, webview_opt, stretchImg, output_dir)
    print(cmd)
    try:
        subprocess.check_call(cmd, shell=True)
    except OSError as e:
        raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)

    if scale > 0:
        cmd = 'gdal_translate -of GTIFF -co TILED=YES -co COMPRESS=JPEG -co BIGTIFF=NO -ot Byte -outsize {0}% 0 -r average {1} {2}'.format(scale, stretchImg, scaled_gtiff_img)
    else:
        cmd = 'gdal_translate -of GTIFF -co TILED=YES -co COMPRESS=JPEG -co BIGTIFF=NO -ot Byte -r average {0} {1}'.format(stretchImg, scaled_gtiff_img)

    try:
        subprocess.check_call(cmd, shell=True)
    except OSError as e:
        raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
    rsgislib.imageutils.popImageStats(scaled_gtiff_img, usenodataval=True, nodataval=0, calcpyramids=True)
    shutil.rmtree(tmpDIR)

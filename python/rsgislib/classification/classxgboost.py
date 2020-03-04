#! /usr/bin/env python
############################################################################
#  classimgutils.py
#
#  Copyright 2016 RSGISLib.
#
#  RSGISLib: 'The remote sensing and GIS Software Library'
#
#  RSGISLib is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  RSGISLib is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
#
#
# Purpose:  Provide a set of utilities which combine commands to create
#           useful extra functionality and make it more easily available
#           to be reused.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 17/12/2016
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################

from __future__ import print_function

import rsgislib
import rsgislib.imageutils
import rsgislib.imagecalc
import rsgislib.rastergis

import numpy

import h5py

import osgeo.gdal as gdal

from rios import applier
from rios import cuiprogress
from rios import rat

import xgboost as xgb

from skopt.space import Real, Integer
from skopt import gp_minimize
from sklearn.metrics import roc_auc_score
from sklearn.metrics import accuracy_score

import gc


def train_xgboost_binary_classifer(out_mdl_file, cls1_train_file, cls1_valid_file, cls1_test_file, cls2_train_file,
                                   cls2_valid_file, cls2_test_file, nthread=2, scale_pos_weight=None):
    """
    A function which performs a bayesian optimisation of the hyper-parameters for a binary xgboost
    classifier. Class 1 is the class which you are interested in and Class 2 is the 'other class'.

    This function requires that xgboost and skopt modules to be installed.

    :param out_mdl_file: The output model which can be loaded to perform a classification.
    :param cls1_train_file: Training samples HDF5 file for the primary class (i.e., the one being classified)
    :param cls1_valid_file: Validation samples HDF5 file for the primary class (i.e., the one being classified)
    :param cls1_test_file: Testing samples HDF5 file for the primary class (i.e., the one being classified)
    :param cls2_train_file: Training samples HDF5 file for the 'other' class
    :param cls2_valid_file: Validation samples HDF5 file for the 'other' class
    :param cls2_test_file: Testing samples HDF5 file for the 'other' class
    :param nthread: The number of threads to use for the training.
    :param scale_pos_weight: Optional, default is None. If None then a value will automatically be calculated.
                             Parameter used to balance imbalanced training data.

    """
    print("Reading Class 1 Training")
    f = h5py.File(cls1_train_file, 'r')
    num_cls1_train_rows = f['DATA/DATA'].shape[0]
    print("num_cls1_train_rows = {}".format(num_cls1_train_rows))
    train_cls1 = numpy.array(f['DATA/DATA'])
    train_cls1_lbl = numpy.ones(num_cls1_train_rows, dtype=int)

    print("Reading Class 1 Validation")
    f = h5py.File(cls1_valid_file, 'r')
    num_cls1_valid_rows = f['DATA/DATA'].shape[0]
    print("num_cls1_valid_rows = {}".format(num_cls1_valid_rows))
    valid_cls1 = numpy.array(f['DATA/DATA'])
    valid_cls1_lbl = numpy.ones(num_cls1_valid_rows, dtype=int)

    print("Reading Class 1 Testing")
    f = h5py.File(cls1_test_file, 'r')
    num_cls1_test_rows = f['DATA/DATA'].shape[0]
    print("num_cls1_test_rows = {}".format(num_cls1_test_rows))
    test_cls1 = numpy.array(f['DATA/DATA'])
    test_cls1_lbl = numpy.ones(num_cls1_test_rows, dtype=int)

    print("Reading Class 2 Training")
    f = h5py.File(cls2_train_file, 'r')
    num_cls2_train_rows = f['DATA/DATA'].shape[0]
    print("num_cls2_train_rows = {}".format(num_cls2_train_rows))
    train_cls2 = numpy.array(f['DATA/DATA'])
    train_cls2_lbl = numpy.zeros(num_cls2_train_rows, dtype=int)

    print("Reading Class 2 Validation")
    f = h5py.File(cls2_valid_file, 'r')
    num_cls2_valid_rows = f['DATA/DATA'].shape[0]
    print("num_cls2_valid_rows = {}".format(num_cls2_valid_rows))
    valid_cls2 = numpy.array(f['DATA/DATA'])
    valid_cls2_lbl = numpy.zeros(num_cls2_valid_rows, dtype=int)

    print("Reading Class 2 Testing")
    f = h5py.File(cls2_test_file, 'r')
    num_cls2_test_rows = f['DATA/DATA'].shape[0]
    print("num_cls2_test_rows = {}".format(num_cls2_test_rows))
    test_cls2 = numpy.array(f['DATA/DATA'])
    test_cls2_lbl = numpy.zeros(num_cls2_test_rows, dtype=int)

    print("Finished Reading Data")

    vaild_np = numpy.concatenate((valid_cls2, valid_cls1))
    vaild_lbl_np = numpy.concatenate((valid_cls2_lbl, valid_cls1_lbl))
    d_valid = xgb.DMatrix(vaild_np, label=vaild_lbl_np)

    d_train = xgb.DMatrix(numpy.concatenate((train_cls2, train_cls1)),
                          label=numpy.concatenate((train_cls2_lbl, train_cls1_lbl)))

    test_np = numpy.concatenate((test_cls2, test_cls1))
    test_lbl_np = numpy.concatenate((test_cls2_lbl, test_cls1_lbl))
    d_test = xgb.DMatrix(test_np, label=test_lbl_np)

    space = [Real(0.01, 0.9, name='eta'),
             Integer(0, 100, name='gamma'),
             Integer(2, 20, name='max_depth'),
             Integer(1, 10, name='min_child_weight'),
             Integer(0, 10, name='max_delta_step'),
             Real(0.5, 1, name='subsample'),
             Integer(2, 100, name='num_boost_round')
             ]

    if scale_pos_weight is None:
        scale_pos_weight = num_cls2_train_rows / num_cls1_train_rows
        if scale_pos_weight < 1:
            scale_pos_weight = 1
    print("scale_pos_weight = {}".format(scale_pos_weight))

    def _objective(values):
        params = {'eta'             : values[0],
                  'gamma'           : values[1],
                  'max_depth'       : values[2],
                  'min_child_weight': values[3],
                  'max_delta_step'  : values[4],
                  'subsample'       : values[5],
                  'nthread'         : nthread,
                  'eval_metric'     : 'auc',
                  'objective'       : 'binary:logistic'}

        print('\nNext set of params.....', params)

        num_boost_round = values[6]
        print("num_boost_round = {}.".format(num_boost_round))

        watchlist = [(d_train, 'train'), (d_valid, 'validation')]
        evals_results = {}
        model_xgb = xgb.train(params, d_train, num_boost_round, evals=watchlist, evals_result=evals_results,
                              verbose_eval=False)

        auc = -roc_auc_score(vaild_lbl_np, model_xgb.predict(d_valid))
        print('\nAUROC.....', -auc, ".....iter.....")
        gc.collect()
        return auc

    res_gp = gp_minimize(_objective, space, n_calls=20, random_state=0, n_random_starts=10)

    print("Best score={}".format(res_gp.fun))

    best_params = res_gp.x

    print("Best Params:\n{}".format(best_params))

    print("Start Training Find Classifier")

    params = {'eta'             : best_params[0],
              'gamma'           : best_params[1],
              'max_depth'       : best_params[2],
              'min_child_weight': best_params[3],
              'max_delta_step'  : best_params[4],
              'subsample'       : best_params[5],
              'nthread'         : nthread,
              'eval_metric'     : 'auc',
              'objective'       : 'binary:logistic'}

    num_boost_round = best_params[6]

    evals_results = {}
    watchlist = [(d_train, 'train'), (d_valid, 'validation')]
    model = xgb.train(params, d_train, num_boost_round, evals=watchlist, evals_result=evals_results, verbose_eval=False)
    test_auc = roc_auc_score(test_lbl_np, model.predict(d_test))
    print("Testing AUC: {}".format(test_auc))
    print("Finish Training")

    model.save_model(out_mdl_file)

    pred_test = model.predict(d_test)
    for i in range(test_np.shape[0]):
        if (pred_test[i] >= 0.5):
            pred_test[i] = 1
        else:
            pred_test[i] = 0
    len(pred_test)

    test_acc = accuracy_score(test_lbl_np, pred_test)
    print("Testing Accuracy: {}".format(test_acc))


def apply_xgboost_binary_classifier(model_file, imgMask, imgMaskVal, imgFileInfo, outProbImg, gdalformat,
                                    outClassImg=None, class_thres=5000, nthread=1):
    """
This function applies a trained binary (i.e., two classes) xgboost model. The function train_xgboost_binary_classifer
can be used to train such as model. The output image will contain the probability of membership to the class of
interest. You will need to threshold this image to get a final hard classification. Alternative, a hard class output
image and threshold can be applied to this image.

:param model_file: a trained xgboost binary model which can be loaded with lgb.Booster(model_file=model_file).
:param imgMask: is an image file providing a mask to specify where should be classified. Simplest mask is all the
                valid data regions (rsgislib.imageutils.genValidMask)
:param imgMaskVal: the pixel value within the imgMask to limit the region to which the classification is applied.
                   Can be used to create a heirachical classification.
:param imgFileInfo: a list of rsgislib.imageutils.ImageBandInfo objects (also used within
                    rsgislib.imageutils.extractZoneImageBandValues2HDF) to identify which images and bands are to
                    be used for the classification so it adheres to the training data.
:param outProbImg: output image file with the classification probabilities - this image is scaled by
                   multiplying by 10000.
:param gdalformat: is the output image format - all GDAL supported formats are supported.
:param outClassImg: Optional output image which will contain the hard classification, defined with a threshold on the
                    probability image.
:param class_thres: The threshold used to define the hard classification. Default is 5000 (i.e., probability of 0.5).
:param nthread: The number of threads to use for the classifier.

    """

    def _applyXGBClassifier(info, inputs, outputs, otherargs):
        outClassVals = numpy.zeros_like(inputs.imageMask, dtype=numpy.uint16)
        if numpy.any(inputs.imageMask == otherargs.mskVal):
            outClassVals = outClassVals.flatten()
            imgMaskVals = inputs.imageMask.flatten()
            classVars = numpy.zeros((outClassVals.shape[0], otherargs.numClassVars), dtype=numpy.float)
            # Array index which can be used to populate the output array following masking etc.
            ID = numpy.arange(imgMaskVals.shape[0])
            classVarsIdx = 0
            for imgFile in otherargs.imgFileInfo:
                imgArr = inputs.__dict__[imgFile.name]
                for band in imgFile.bands:
                    classVars[..., classVarsIdx] = imgArr[(band - 1)].flatten()
                    classVarsIdx = classVarsIdx + 1
            classVars = classVars[imgMaskVals == otherargs.mskVal]
            ID = ID[imgMaskVals == otherargs.mskVal]
            predClass = numpy.around(otherargs.classifier.predict(xgb.DMatrix(classVars)) * 10000)
            outClassVals[ID] = predClass
            outClassVals = numpy.expand_dims(
                    outClassVals.reshape((inputs.imageMask.shape[1], inputs.imageMask.shape[2])), axis=0)
        outputs.outimage = outClassVals

    classifier = xgb.Booster({'nthread': nthread})
    classifier.load_model(model_file)

    infiles = applier.FilenameAssociations()
    infiles.imageMask = imgMask
    numClassVars = 0
    for imgFile in imgFileInfo:
        infiles.__dict__[imgFile.name] = imgFile.fileName
        numClassVars = numClassVars + len(imgFile.bands)

    outfiles = applier.FilenameAssociations()
    outfiles.outimage = outProbImg
    otherargs = applier.OtherInputs()
    otherargs.classifier = classifier
    otherargs.mskVal = imgMaskVal
    otherargs.numClassVars = numClassVars
    otherargs.imgFileInfo = imgFileInfo

    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        progress_bar = cuiprogress.GDALProgressBar()

    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False
    print("Applying the Classifier")
    applier.apply(_applyXGBClassifier, infiles, outfiles, otherargs, controls=aControls)
    print("Completed")
    rsgislib.imageutils.popImageStats(outProbImg, usenodataval=True, nodataval=0, calcpyramids=True)

    if outClassImg is not None:
        rsgislib.imagecalc.imageMath(outProbImg, outClassImg, 'b1>{}?1:0'.format(class_thres), gdalformat,
                                     rsgislib.TYPE_8UINT)
        rsgislib.rastergis.populateStats(outClassImg, addclrtab=True, calcpyramids=True, ignorezero=True)


def train_xgboost_multiclass_classifer(out_mdl_file, clsinfodict, nthread=1):
    """
    A function which performs a bayesian optimisation of the hyper-parameters for a multiclass xgboost
    classifier. A dict of class information, as ClassInfoObj objects, is defined with the training data.

    This function requires that xgboost and skopt modules to be installed.

    :param out_mdl_file: The output model which can be loaded to perform a classification.
    :param clsinfodict: dict (key is string with class name) of ClassInfoObj objects defining the training data.
    :param nthread: The number of threads to use to train the classifier.

    """
    n_classes = len(clsinfodict)
    for clsname in clsinfodict:
        if clsinfodict[clsname].id >= n_classes:
            raise ("ClassInfoObj '{}' id ({}) is not consecutive starting from 0.".format(clsname,
                                                                                          clsinfodict[clsname].id))

    cls_data_dict = {}
    train_data_lst = []
    train_lbls_lst = []
    valid_data_lst = []
    valid_lbls_lst = []
    test_data_lst = []
    test_lbls_lst = []
    cls_ids = []
    n_classes = 0
    for clsname in clsinfodict:
        sgl_cls_info = {}
        print("Reading Class {} Training".format(clsname))
        f = h5py.File(clsinfodict[clsname].trainfileH5, 'r')
        sgl_cls_info['train_n_rows'] = f['DATA/DATA'].shape[0]
        sgl_cls_info['train_data'] = numpy.array(f['DATA/DATA'])
        sgl_cls_info['train_data_lbls'] = numpy.zeros(sgl_cls_info['train_n_rows'], dtype=int)
        sgl_cls_info['train_data_lbls'][...] = clsinfodict[clsname].id
        f.close()
        train_data_lst.append(sgl_cls_info['train_data'])
        train_lbls_lst.append(sgl_cls_info['train_data_lbls'])

        print("Reading Class {} Validation".format(clsname))
        f = h5py.File(clsinfodict[clsname].validfileH5, 'r')
        sgl_cls_info['valid_n_rows'] = f['DATA/DATA'].shape[0]
        sgl_cls_info['valid_data'] = numpy.array(f['DATA/DATA'])
        sgl_cls_info['valid_data_lbls'] = numpy.zeros(sgl_cls_info['valid_n_rows'], dtype=int)
        sgl_cls_info['valid_data_lbls'][...] = clsinfodict[clsname].id
        f.close()
        valid_data_lst.append(sgl_cls_info['valid_data'])
        valid_lbls_lst.append(sgl_cls_info['valid_data_lbls'])

        print("Reading Class {} Testing".format(clsname))
        f = h5py.File(clsinfodict[clsname].testfileH5, 'r')
        sgl_cls_info['test_n_rows'] = f['DATA/DATA'].shape[0]
        sgl_cls_info['test_data'] = numpy.array(f['DATA/DATA'])
        sgl_cls_info['test_data_lbls'] = numpy.zeros(sgl_cls_info['test_n_rows'], dtype=int)
        sgl_cls_info['test_data_lbls'][...] = clsinfodict[clsname].id
        f.close()
        test_data_lst.append(sgl_cls_info['test_data'])
        test_lbls_lst.append(sgl_cls_info['test_data_lbls'])

        cls_data_dict[clsname] = sgl_cls_info
        cls_ids.append(clsinfodict[clsname].id)
        n_classes = n_classes + 1

    print("Finished Reading Data")

    vaild_np = numpy.concatenate(valid_data_lst)
    vaild_lbl_np = numpy.concatenate(valid_lbls_lst)
    d_valid = xgb.DMatrix(vaild_np, label=vaild_lbl_np)

    d_train = xgb.DMatrix(numpy.concatenate(train_data_lst), label=numpy.concatenate(train_lbls_lst))

    test_np = numpy.concatenate(test_data_lst)
    test_lbl_np = numpy.concatenate(test_lbls_lst)
    d_test = xgb.DMatrix(test_np, label=test_lbl_np)

    space = [Real(0.01, 0.9, name='eta'),
             Integer(0, 100, name='gamma'),
             Integer(2, 20, name='max_depth'),
             Integer(1, 10, name='min_child_weight'),
             Integer(0, 10, name='max_delta_step'),
             Real(0.5, 1, name='subsample'),
             Integer(2, 100, name='num_boost_round')
             ]

    def _objective(values):
        params = {'eta'             : values[0],
                  'gamma'           : values[1],
                  'max_depth'       : values[2],
                  'min_child_weight': values[3],
                  'max_delta_step'  : values[4],
                  'subsample'       : values[5],
                  'nthread'         : nthread,
                  'eval_metric'     : 'merror',
                  'objective'       : 'multi:softmax',
                  'num_class'       : n_classes}

        print('\nNext set of params.....', params)

        num_boost_round = values[6]
        print("num_boost_round = {}.".format(num_boost_round))

        watchlist = [(d_train, 'train'), (d_valid, 'validation')]
        evals_results = {}
        model_xgb = xgb.train(params, d_train, num_boost_round, evals=watchlist, evals_result=evals_results,
                              verbose_eval=False)

        vld_preds_idxs = model_xgb.predict(d_valid)

        acc_score = -accuracy_score(vaild_lbl_np, vld_preds_idxs)
        print('\nAccScore.....', -acc_score, ".....iter.....")
        gc.collect()
        return acc_score

    res_gp = gp_minimize(_objective, space, n_calls=20, random_state=0, n_random_starts=10)

    print("Best score={}".format(res_gp.fun))
    best_params = res_gp.x
    print("Best Params:\n{}".format(best_params))

    print("Start Training Find Classifier")
    params = {'eta'             : best_params[0],
              'gamma'           : best_params[1],
              'max_depth'       : best_params[2],
              'min_child_weight': best_params[3],
              'max_delta_step'  : best_params[4],
              'subsample'       : best_params[5],
              'nthread'         : nthread,
              'eval_metric'     : 'merror',
              'objective'       : 'multi:softmax',
              'num_class'       : n_classes}

    num_boost_round = best_params[6]

    watchlist = [(d_train, 'train'), (d_valid, 'validation')]
    evals_results = {}
    model_xgb = xgb.train(params, d_train, num_boost_round, evals=watchlist, evals_result=evals_results,
                          verbose_eval=False)
    model_xgb.save_model(out_mdl_file)

    vld_preds_idxs = model_xgb.predict(d_valid)
    valid_acc_scr = accuracy_score(vaild_lbl_np, vld_preds_idxs)
    print("Validate Accuracy: {}".format(valid_acc_scr))

    test_preds_idxs = model_xgb.predict(d_test)
    test_acc_scr = accuracy_score(test_lbl_np, test_preds_idxs)
    print("Testing Accuracy: {}".format(test_acc_scr))


def apply_xgboost_multiclass_classifier(classTrainInfo, model_file, imgMask, imgMaskVal, imgFileInfo,
                                        outClassImg, gdalformat, classClrNames=True, nthread=1):
    """
This function applies a trained multiple classes xgboost model. The function train_xgboost_multiclass_classifer
can be used to train such as model. The output image will contain the probability of membership to the class of
interest. You will need to threshold this image to get a final hard classification. Alternative, a hard class
output image and threshold can be applied to this image.

:param classTrainInfo: dict (where the key is the class name) of rsgislib.classification.ClassInfoObj
                       objects which will be used to train the classifier (i.e., train_xgboost_multiclass_classifer()),
                       provide pixel value id and RGB class values.
:param model_file: a trained xgboost multiclass model which can be loaded with lgb.Booster(model_file=model_file).
:param imgMask: is an image file providing a mask to specify where should be classified. Simplest mask is all the
                valid data regions (rsgislib.imageutils.genValidMask)
:param imgMaskVal: the pixel value within the imgMask to limit the region to which the classification is applied.
                   Can be used to create a heirachical classification.
:param imgFileInfo: a list of rsgislib.imageutils.ImageBandInfo objects (also used within
                    rsgislib.imageutils.extractZoneImageBandValues2HDF) to identify which images and bands are to
                    be used for the classification so it adheres to the training data.
:param outClassImg: Output image which will contain the hard classification defined as the maximum probability.
:param gdalformat: is the output image format - all GDAL supported formats are supported.
:param classClrNames: default is True and therefore a colour table will the colours specified in ClassInfoObj
                      and a ClassName (from classTrainInfo) column will be added to the output file.
:param nthread: The number of threads to use for the classifier.

    """

    def _applyXGBMClassifier(info, inputs, outputs, otherargs):
        outClassIdVals = numpy.zeros_like(inputs.imageMask, dtype=numpy.uint16)
        if numpy.any(inputs.imageMask == otherargs.mskVal):
            n_pxls = inputs.imageMask.shape[1] * inputs.imageMask.shape[2]
            outClassIdVals = outClassIdVals.flatten()
            imgMaskVals = inputs.imageMask.flatten()
            classVars = numpy.zeros((n_pxls, otherargs.numClassVars), dtype=numpy.float)
            # Array index which can be used to populate the output array following masking etc.
            ID = numpy.arange(imgMaskVals.shape[0])
            classVarsIdx = 0
            for imgFile in otherargs.imgFileInfo:
                imgArr = inputs.__dict__[imgFile.name]
                for band in imgFile.bands:
                    classVars[..., classVarsIdx] = imgArr[(band - 1)].flatten()
                    classVarsIdx = classVarsIdx + 1
            classVars = classVars[imgMaskVals == otherargs.mskVal]
            ID = ID[imgMaskVals == otherargs.mskVal]
            preds_idxs = otherargs.classifier.predict(xgb.DMatrix(classVars))
            preds_cls_ids = numpy.zeros_like(preds_idxs, dtype=numpy.uint16)
            for cld_id, idx in zip(otherargs.cls_id_lut, numpy.arange(0, len(otherargs.cls_id_lut))):
                preds_cls_ids[preds_idxs == idx] = cld_id

            outClassIdVals[ID] = preds_cls_ids
            outClassIdVals = numpy.expand_dims(
                    outClassIdVals.reshape((inputs.imageMask.shape[1], inputs.imageMask.shape[2])), axis=0)

        outputs.outclsimage = outClassIdVals

    classifier = xgb.Booster({'nthread': nthread})
    classifier.load_model(model_file)

    infiles = applier.FilenameAssociations()
    infiles.imageMask = imgMask
    numClassVars = 0
    for imgFile in imgFileInfo:
        infiles.__dict__[imgFile.name] = imgFile.fileName
        numClassVars = numClassVars + len(imgFile.bands)

    n_classes = len(classTrainInfo)
    cls_id_lut = numpy.zeros(n_classes)
    for clsname in classTrainInfo:
        if classTrainInfo[clsname].id >= n_classes:
            raise ("ClassInfoObj '{}' id ({}) is not consecutive starting from 0.".format(clsname,
                                                                                          classTrainInfo[clsname].id))
        cls_id_lut[classTrainInfo[clsname].id] = classTrainInfo[clsname].out_id

    outfiles = applier.FilenameAssociations()
    outfiles.outclsimage = outClassImg
    otherargs = applier.OtherInputs()
    otherargs.classifier = classifier
    otherargs.mskVal = imgMaskVal
    otherargs.numClassVars = numClassVars
    otherargs.imgFileInfo = imgFileInfo
    otherargs.n_classes = n_classes
    otherargs.cls_id_lut = cls_id_lut

    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        progress_bar = cuiprogress.GDALProgressBar()

    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False
    print("Applying the Classifier")
    applier.apply(_applyXGBMClassifier, infiles, outfiles, otherargs, controls=aControls)
    print("Completed Classification")

    if classClrNames:
        rsgislib.rastergis.populateStats(outClassImg, addclrtab=True, calcpyramids=True, ignorezero=True)
        ratDataset = gdal.Open(outClassImg, gdal.GA_Update)
        red = rat.readColumn(ratDataset, 'Red')
        green = rat.readColumn(ratDataset, 'Green')
        blue = rat.readColumn(ratDataset, 'Blue')
        ClassName = numpy.empty_like(red, dtype=numpy.dtype('a255'))
        ClassName[...] = ""

        for classKey in classTrainInfo:
            print("Apply Colour to class \'" + classKey + "\'")
            red[classTrainInfo[classKey].out_id] = classTrainInfo[classKey].red
            green[classTrainInfo[classKey].out_id] = classTrainInfo[classKey].green
            blue[classTrainInfo[classKey].out_id] = classTrainInfo[classKey].blue
            ClassName[classTrainInfo[classKey].out_id] = classKey

        rat.writeColumn(ratDataset, "Red", red)
        rat.writeColumn(ratDataset, "Green", green)
        rat.writeColumn(ratDataset, "Blue", blue)
        rat.writeColumn(ratDataset, "ClassName", ClassName)
        ratDataset = None



import os

DATA_DIR = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "data", "imagemorphology"
)


def test_createCircularOp(tmp_path):
    import rsgislib.imagemorphology

    out_file = os.path.join(tmp_path, "CircularOp.gmtxt")
    op_size = 5
    rsgislib.imagemorphology.createCircularOp(out_file, op_size)

    assert os.path.exists(out_file)


def test_imageDilate(tmp_path):
    import rsgislib.imagemorphology
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_NDVI_lt5_bin.kea")
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_imgDilate_OpFile.kea")
    morph_op_file = os.path.join(DATA_DIR, "CircularOp.gmtxt")
    use_op_file = True
    op_size = 5
    gdalformat = "KEA"
    datatype = rsgislib.TYPE_8UINT
    rsgislib.imagemorphology.imageDilate(
        input_img, output_img, morph_op_file, use_op_file, op_size, gdalformat, datatype
    )

    ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_imgDilate_OpFile.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_img, output_img)
    assert img_eq


def test_imageDilateCombinedOut(tmp_path):
    import rsgislib.imagemorphology
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_NDVI_lt5_bin.kea")
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_imgDilate_combined.kea")
    morph_op_file = os.path.join(DATA_DIR, "CircularOp.gmtxt")
    use_op_file = True
    op_size = 5
    gdalformat = "KEA"
    datatype = rsgislib.TYPE_8UINT
    rsgislib.imagemorphology.imageDilateCombinedOut(
        input_img, output_img, morph_op_file, use_op_file, op_size, gdalformat, datatype
    )

    ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_imgDilate_combined.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_img, output_img)
    assert img_eq


def test_imageErode(tmp_path):
    import rsgislib.imagemorphology
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_NDVI_lt5_bin.kea")
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_imgErode.kea")
    morph_op_file = os.path.join(DATA_DIR, "CircularOp.gmtxt")
    use_op_file = True
    op_size = 5
    gdalformat = "KEA"
    datatype = rsgislib.TYPE_8UINT
    rsgislib.imagemorphology.imageErode(
        input_img, output_img, morph_op_file, use_op_file, op_size, gdalformat, datatype
    )

    ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_imgErode.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_img, output_img)
    assert img_eq


def test_imageErodeCombinedOut(tmp_path):
    import rsgislib.imagemorphology
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_NDVI_lt5_bin.kea")
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_imgErode_combined.kea")
    morph_op_file = os.path.join(DATA_DIR, "CircularOp.gmtxt")
    use_op_file = True
    op_size = 5
    gdalformat = "KEA"
    datatype = rsgislib.TYPE_8UINT
    rsgislib.imagemorphology.imageErodeCombinedOut(
        input_img, output_img, morph_op_file, use_op_file, op_size, gdalformat, datatype
    )

    ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_imgErode_combined.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_img, output_img)
    assert img_eq


def test_imageGradiant(tmp_path):
    import rsgislib.imagemorphology
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_NDVI_lt5_bin.kea")
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_imgGradiant.kea")
    morph_op_file = os.path.join(DATA_DIR, "CircularOp.gmtxt")
    use_op_file = True
    op_size = 5
    gdalformat = "KEA"
    datatype = rsgislib.TYPE_32FLOAT
    rsgislib.imagemorphology.imageGradiant(
        input_img, output_img, morph_op_file, use_op_file, op_size, gdalformat, datatype
    )

    ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_imgGradiant.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_img, output_img)
    assert img_eq


def test_imageGradiantCombinedOut(tmp_path):
    import rsgislib.imagemorphology
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_NDVI_lt5_bin.kea")
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_imgGradiant_combined.kea")
    morph_op_file = os.path.join(DATA_DIR, "CircularOp.gmtxt")
    use_op_file = True
    op_size = 5
    gdalformat = "KEA"
    datatype = rsgislib.TYPE_32FLOAT
    rsgislib.imagemorphology.imageGradiantCombinedOut(
        input_img, output_img, morph_op_file, use_op_file, op_size, gdalformat, datatype
    )

    ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_imgGradiant_combined.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_img, output_img)
    assert img_eq


def test_imageLocalMinima(tmp_path):
    import rsgislib.imagemorphology
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_NDVI_lt5_bin.kea")
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_imgLocalMinima.kea")
    sequencial_out = True
    allow_equal = True
    morph_op_file = os.path.join(DATA_DIR, "CircularOp.gmtxt")
    use_op_file = True
    op_size = 5
    gdalformat = "KEA"
    datatype = rsgislib.TYPE_8UINT
    rsgislib.imagemorphology.imageLocalMinima(
        input_img,
        output_img,
        sequencial_out,
        allow_equal,
        morph_op_file,
        use_op_file,
        op_size,
        gdalformat,
        datatype,
    )

    ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_imgLocalMinima.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_img, output_img)
    assert img_eq


def test_imageLocalMinimaCombinedOut(tmp_path):
    import rsgislib.imagemorphology
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_NDVI_lt5_bin.kea")
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_imgLocalMinimaCombined.kea")
    sequencial_out = True
    allow_equal = True
    morph_op_file = os.path.join(DATA_DIR, "CircularOp.gmtxt")
    use_op_file = True
    op_size = 5
    gdalformat = "KEA"
    datatype = rsgislib.TYPE_8UINT
    rsgislib.imagemorphology.imageLocalMinimaCombinedOut(
        input_img,
        output_img,
        sequencial_out,
        allow_equal,
        morph_op_file,
        use_op_file,
        op_size,
        gdalformat,
        datatype,
    )

    ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_imgLocalMinimaCombined.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_img, output_img)
    assert img_eq


def test_imageOpening(tmp_path):
    import rsgislib.imagemorphology
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_NDVI_lt5_bin.kea")
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_imgOpening.kea")
    tmp_img = os.path.join(tmp_path, "sen2_20210527_aber_imgOpening_tmp.kea")
    morph_op_file = os.path.join(DATA_DIR, "CircularOp.gmtxt")
    use_op_file = True
    op_size = 5
    gdalformat = "KEA"
    datatype = rsgislib.TYPE_8UINT
    niters = 1
    rsgislib.imagemorphology.imageOpening(
        input_img,
        output_img,
        tmp_img,
        morph_op_file,
        use_op_file,
        op_size,
        gdalformat,
        datatype,
        niters,
    )

    ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_imgOpening.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_img, output_img)
    print(prop_match)
    assert img_eq


def test_imageClosing(tmp_path):
    import rsgislib.imagemorphology
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_NDVI_lt5_bin.kea")
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_imgClosing.kea")
    tmp_img = os.path.join(tmp_path, "sen2_20210527_aber_imgClosing_tmp.kea")
    morph_op_file = os.path.join(DATA_DIR, "CircularOp.gmtxt")
    use_op_file = True
    op_size = 5
    gdalformat = "KEA"
    datatype = rsgislib.TYPE_8UINT
    niters = 1
    rsgislib.imagemorphology.imageClosing(
        input_img,
        output_img,
        tmp_img,
        morph_op_file,
        use_op_file,
        op_size,
        gdalformat,
        datatype,
        niters,
    )

    ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_imgClosing.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_img, output_img)
    print(prop_match)
    assert img_eq


def test_imageBlackTopHat(tmp_path):
    import rsgislib.imagemorphology
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_NDVI_lt5_bin.kea")
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_imgBlackTopHat.kea")
    tmp_img = os.path.join(tmp_path, "sen2_20210527_aber_imgBlackTopHat_tmp.kea")
    morph_op_file = os.path.join(DATA_DIR, "CircularOp.gmtxt")
    use_op_file = True
    op_size = 5
    gdalformat = "KEA"
    datatype = rsgislib.TYPE_8UINT
    rsgislib.imagemorphology.imageBlackTopHat(
        input_img,
        output_img,
        tmp_img,
        morph_op_file,
        use_op_file,
        op_size,
        gdalformat,
        datatype,
    )

    ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_imgBlackTopHat.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_img, output_img)
    print(prop_match)
    assert img_eq


def test_imageWhiteTopHat(tmp_path):
    import rsgislib.imagemorphology
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_NDVI_lt5_bin.kea")
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_imgWhiteTopHat.kea")
    tmp_img = os.path.join(tmp_path, "sen2_20210527_aber_imgWhiteTopHat_tmp.kea")
    morph_op_file = os.path.join(DATA_DIR, "CircularOp.gmtxt")
    use_op_file = True
    op_size = 5
    gdalformat = "KEA"
    datatype = rsgislib.TYPE_8UINT
    rsgislib.imagemorphology.imageWhiteTopHat(
        input_img,
        output_img,
        tmp_img,
        morph_op_file,
        use_op_file,
        op_size,
        gdalformat,
        datatype,
    )

    ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_imgWhiteTopHat.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(ref_img, output_img)
    print(prop_match)
    assert img_eq
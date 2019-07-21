#-------------------------------------------------
#
# Project created by QtCreator 2014-02-01T20:30:52
#
#-------------------------------------------------
QMAKE_CFLAGS += -std=gnu++11 -std=c99

QMAKE_CXXFLAGS += -O2 -fPIC -ltcg

include(3rdparty/qtxlsx/src/xlsx/qtxlsx.pri)

QT       += core gui webenginewidgets multimedia webchannel

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = intontrainer
TEMPLATE = app

win32{
    INCLUDEPATH += "c:/Dev/MathGL/include"
    LIBS += -L"c:/Dev/MathGL/lib" -llibmgl -llibmgl-qt5

    INCLUDEPATH += "c:/Dev/openal-soft-1.17.2-bin/include"
    LIBS += -L"c:/Dev/openal-soft-1.17.2-bin/lib/Win32" -llibOpenAL32

    INCLUDEPATH += "c:/Dev/Libs/gsl-2.1-bin/include"
    LIBS += -L"c:/Dev/Libs/gsl-2.1-bin/lib" -llibgsl -llibgslcblas
}

unix:!mac {
    QMAKE_CFLAGS += -U__STRICT_ANSI__

    INCLUDEPATH += "/usr/include/AL"
    INCLUDEPATH += "/usr/local/include/mgl2/"

    LIBS += `pkg-config --libs gsl openal`
    LIBS += -L"/usr/local/lib" -lmgl -lmgl-qt5
}

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    recorder/soundrecorder.cpp \
    graphswindow.cpp \
    settingsdialog.cpp \
    recorder/autosoundrecorder.cpp \
    recorder/timesoundrecorder.cpp \
    recorder/buffer.cpp \
    drawer.cpp \
    soundplayer.cpp \
    graphsevalwindow.cpp \
    dp/vectordp.cpp \
    dp/spectrdp.cpp \
    webwindow.cpp \
    recorder/dpsoundrecorder.cpp \
    dp/continuousdp.cpp \
    drawerdp.cpp \
    dataprocessing.cpp \
    utils.cpp \
    databaseeditor/databasemanager.cpp \
    databaseeditor/sandboxitemmodel.cpp \
    analysis/curve_similarity.cpp \
    processing/filemarkout.cpp \
    analysis/ump.cpp \
    analysis/derivative.c \
    analysis/metrics.c \
    analysis/functions.c \
    dp/multidp.cpp

HEADERS  += \
    mainwindow.h \
    recorder/soundrecorder.h \
    graphswindow.h \
    settingsdialog.h \
    recorder/autosoundrecorder.h \
    recorder/timesoundrecorder.h \
    recorder/buffer.h \
    drawer.h \
    soundplayer.h \
    graphsevalwindow.h \
    abstractdrawer.h \
    dp/dp.h \
    dp/vectordp.h \
    dp/spectrdp.h \
    webwindow.h \
    recorder/dpsoundrecorder.h \
    dp/continuousdp.h \
    drawerdp.h \
    dataprocessing.h \
    defines.h \
    utils.h \
    databaseeditor/databasemanager.h \
    databaseeditor/sandboxitemmodel.h \
    analysis/curve_similarity.h \
    processing/filemarkout.h \
    analysis/ump.h \
    analysis/mask_data.h \
    analysis/derivative.h \
    analysis/metrics.h \
    analysis/functions.h \
    dp/multidp.h

# OpenAL
SOURCES += \
    openal/openal_wrapper.c\
    openal/wavFile.c

HEADERS  += \
    openal/openal_wrapper.h \
    openal/wavFile.h

# SPTK
INCLUDEPATH += ./SPTK

SOURCES +=  \
    sptk/sptk.c\
    sptk/agexp.c\
    sptk/cholesky.c \
    sptk/fileio.c \
    sptk/fillz.c \
    sptk/getfp.c \
    sptk/getmem.c \
    sptk/gexp.c \
    sptk/glog.c \
    sptk/invert.c \
    sptk/matrix.c \
    sptk/movem.c \
    sptk/mseq.c \
    sptk/theq.c \
    sptk/toeplitz.c \
    sptk/vector.c \
#    sptk/acep/acep.c \
#    sptk/acep/_acep.c \
#    sptk/acorr/acorr.c \
    sptk/acorr/_acorr.c \
#    sptk/agcep/agcep.c \
#    sptk/agcep/_agcep.c \
#    sptk/amcep/amcep.c \
#    sptk/amcep/_amcep.c \
#    sptk/average/average.c \
#    sptk/average/_average.c \
#    sptk/b2mc/b2mc.c \
#    sptk/b2mc/_b2mc.c \
#    sptk/bcp/bcp.c \
#    sptk/bcut/bcut.c \
#    sptk/c2acr/c2acr.c \
#    sptk/c2acr/_c2acr.c \
#    sptk/c2ir/c2ir.c \
#    sptk/c2ir/_c2ir.c \
#    sptk/c2sp/c2sp.c \
#    sptk/c2sp/_c2sp.c \
#    sptk/cat2/cat2.c \
#    sptk/cdist/cdist.c \
#    sptk/clip/clip.c \
#    sptk/clip/_clip.c \
#    sptk/da/dawrite.c \
#    sptk/da/winplay.c \
#    sptk/dct/dct.c \
#    sptk/dct/_dct.c \
#    sptk/decimate/decimate.c \
#    sptk/delay/delay.c \
#    sptk/delta/delta.c \
#    sptk/df2/df2.c \
#    sptk/df2/_df2.c \
#    sptk/dfs/dfs.c \
#    sptk/dfs/_dfs.c \
#    sptk/dmp/dmp.c \
#    sptk/ds/ds.c \
#    sptk/dtw/dtw.c \
#    sptk/echo2/echo2.c \
#    sptk/excite/excite.c \
#    sptk/extract/extract.c \
#    sptk/fd/fd.c \
#    sptk/fft2/fft2.c \
#    sptk/fft2/_fft2.c \
#    sptk/fft/fft.c \
    sptk/fft/_fft.c \
#    sptk/fftcep/fftcep.c \
#    sptk/fftcep/_fftcep.c \
#    sptk/fftr2/fftr2.c \
#    sptk/fftr2/_fftr2.c \
#    sptk/fftr/fftr.c \
    sptk/fftr/_fftr.c \
#    sptk/fig+fdrw/fdrw.c \
#    sptk/fig+fdrw/fig.c \
#    sptk/fig+fdrw/fig0.c \
#    sptk/fig+fdrw/fig1.c \
#    sptk/fig+fdrw/plot.c \
#    sptk/fig+fdrw/plsub.c \
    sptk/frame/frame.c \
#    sptk/freqt/freqt.c \
#    sptk/freqt/_freqt.c \
#    sptk/gc2gc/gc2gc.c \
#    sptk/gc2gc/_gc2gc.c \
#    sptk/gcep/gcep.c \
#    sptk/gcep/_gcep.c \
#    sptk/glsadf/glsadf.c \
#    sptk/glsadf/_glsadf.c \
#    sptk/gmm/gmm.c \
#    sptk/gmm/_gmm.c \
#    sptk/gmm/gmmp.c \
#    sptk/gnorm/gnorm.c \
#    sptk/gnorm/_gnorm.c \
#    sptk/grpdelay/grpdelay.c \
#    sptk/grpdelay/_grpdelay.c \
#    sptk/histogram/histogram.c \
#    sptk/histogram/_histogram.c \
#    sptk/idct/idct.c \
#    sptk/ifft2/ifft2.c \
#    sptk/ifft2/_ifft2.c \
#    sptk/ifft/ifft.c \
#    sptk/ifft/_ifft.c \
#    sptk/ifftr/ifftr.c \
#    sptk/ifftr/_ifftr.c \
#    sptk/ignorm/ignorm.c \
#    sptk/ignorm/_ignorm.c \
#    sptk/impulse/impulse.c \
#    sptk/imsvq/imsvq.c \
#    sptk/imsvq/_imsvq.c \
#    sptk/interpolate/interpolate.c \
#    sptk/ivq/ivq.c \
#    sptk/ivq/_ivq.c \
#    sptk/lbg/lbg.c \
#    sptk/lbg/_lbg.c \
#    sptk/levdur/levdur.c \
    sptk/levdur/_levdur.c \
#    sptk/linear_intpl/linear_intpl.c \
#    sptk/lmadf/lmadf.c \
#    sptk/lmadf/_lmadf.c \
    sptk/lpc2c/lpc2c.c \
    sptk/lpc2c/_lpc2c.c \
#    sptk/lpc2lsp/lpc2lsp.c \
#    sptk/lpc2lsp/_lpc2lsp.c \
#    sptk/lpc2par/lpc2par.c \
#    sptk/lpc2par/_lpc2par.c \
    sptk/lpc/lpc.c \
    sptk/lpc/_lpc.c \
#    sptk/lsp2lpc/lsp2lpc.c \
#    sptk/lsp2lpc/_lsp2lpc.c \
#    sptk/lsp2sp/lsp2sp.c \
#    sptk/lsp2sp/_lsp2sp.c \
#    sptk/lspcheck/lspcheck.c \
#    sptk/lspcheck/_lspcheck.c \
#    sptk/lspdf/lspdf.c \
#    sptk/lspdf/_lspdf.c \
#    sptk/ltcdf/ltcdf.c \
#    sptk/ltcdf/_ltcdf.c \
#    sptk/mc2b/mc2b.c \
#    sptk/mc2b/_mc2b.c \
#    sptk/mcep/mcep.c \
#    sptk/mcep/_mcep.c \
#    sptk/merge/merge.c \
#    sptk/mfcc/mfcc.c \
#    sptk/mfcc/_mfcc.c \
#    sptk/mgc2mgc/mgc2mgc.c \
#    sptk/mgc2mgc/_mgc2mgc.c \
#    sptk/mgc2sp/mgc2sp.c \
#    sptk/mgc2sp/_mgc2sp.c \
#    sptk/mgcep/mgcep.c \
#    sptk/mgcep/_mgcep.c \
#    sptk/mglsadf/mglsadf.c \
#    sptk/mglsadf/_mglsadf.c \
#    sptk/minmax/minmax.c \
#    sptk/mlpg/mlpg.c \
#    sptk/mlpg/_mlpg.c \
#    sptk/mlsacheck/mlsacheck.c \
#    sptk/mlsadf/mlsadf.c \
#    sptk/mlsadf/_mlsadf.c \
#    sptk/msvq/msvq.c \
#    sptk/msvq/_msvq.c \
#    sptk/nan/nan.c \
#    sptk/norm0/norm0.c \
#    sptk/norm0/_norm0.c \
#    sptk/nrand/nrand.c \
    sptk/nrand/_nrand.c \
#    sptk/par2lpc/par2lpc.c \
#    sptk/par2lpc/_par2lpc.c \
#    sptk/pca/pca.c \
#    sptk/pca/pcas.c \
#    sptk/phase/phase.c \
#    sptk/phase/_phase.c \
    sptk/pitch/pitch.c \
    sptk/pitch/snack/jkGetF0.c \
    sptk/pitch/snack/sigproc.c \
    sptk/pitch/swipe/swipe.c \
#    sptk/poledf/poledf.c \
#    sptk/poledf/_poledf.c \
#    sptk/psgr/dict.c \
#    sptk/psgr/eps.c \
#    sptk/psgr/plot.c \
#    sptk/psgr/psgr.c \
#    sptk/ramp/ramp.c \
#    sptk/rawtowav/rawtowav.c \
#    sptk/reverse/reverse.c \
#    sptk/reverse/_reverse.c \
#    sptk/rmse/rmse.c \
#    sptk/rmse/_rmse.c \
#    sptk/root_pol/root_pol.c \
#    sptk/root_pol/_root_pol.c \
#    sptk/sin/sin.c \
#    sptk/smcep/smcep.c \
#    sptk/smcep/_smcep.c \
#    sptk/snr/snr.c \
#    sptk/sopr/sopr.c \
    sptk/spec/spec.c \
#    sptk/step/step.c \
#    sptk/swab/swab.c \
#    sptk/symmetrize/symmetrize.c \
#    sptk/train/train.c \
#    sptk/transpose/transpose.c \
#    sptk/transpose/_transpose.c \
#    sptk/uels/uels.c \
#    sptk/uels/_uels.c \
#    sptk/ulaw/ulaw.c \
#    sptk/ulaw/_ulaw.c \
#    sptk/us/us.c \
#    sptk/vc/vc.c \
#    sptk/vc/_vc.c \
#    sptk/vc/hts_engine_API/HTS_misc.c \
#    sptk/vc/hts_engine_API/HTS_pstream.c \
#    sptk/vc/hts_engine_API/HTS_sstream.c \
#    sptk/vopr/vopr.c \
#    sptk/vq/vq.c \
#    sptk/vq/_vq.c \
#    sptk/vstat/vstat.c \
#    sptk/vsum/vsum.c \
    sptk/window/window.c \
    sptk/window/_window.c \
    sptk/x2x/x2x.c \
#    sptk/xgr/plot.c \
#    sptk/xgr/window.c \
#    sptk/xgr/xgr.c \
#    sptk/zcross/zcross.c \
#    sptk/zcross/_zcross.c \
#    sptk/zerodf/zerodf.c \
#    sptk/zerodf/_zerodf.c \
    sptk/others/func.c \
    sptk/others/interpolation.c \

HEADERS  += \
    sptk/SPTK.h \
    sptk/vector.h \
#    sptk/da/da.h \
#    sptk/da/winplay.h \
#    sptk/fig+fdrw/fig.h \
#    sptk/fig+fdrw/plot.h \
    sptk/lpc/lpc.h \
    sptk/lpc2c/lpc2c.h \
    sptk/pitch/pitch.h \
    sptk/pitch/snack/jkGetF0.h \
    sptk/frame/frame.h \
#    sptk/psgr/psgr.h \
#    sptk/vc/hts_engine_API/HTS_engine.h \
#    sptk/vc/hts_engine_API/HTS_hidden.h \
    sptk/spec/spec.h \
    sptk/window/window.h \
    sptk/x2x/x2x.h \
#    sptk/xgr/config.h \
#    sptk/xgr/gcdata.h \
#    sptk/xgr/xgr.h \
    sptk/others/func.h \
    sptk/others/interpolation.h \

FORMS += \
    mainwindow.ui \
    graphswindow.ui \
    settingsdialog.ui \
    webwindow.ui

RESOURCES += \
    icons.qrc \
    sounds.qrc

DISTFILES += \
    html/rus.html \
    3rdparty/qtxlsx/src/xlsx/qtxlsx.pri \
    3rdparty/qtxlsx/examples/xlsx/demo/doc/images/xlsx_demo.gif \
    3rdparty/qtxlsx/examples/xlsx/documentproperty/doc/images/doc_property.png \
    3rdparty/qtxlsx/examples/xlsx/hello/doc/images/hello.png \
    3rdparty/qtxlsx/examples/xlsx/mergecells/doc/images/xlsx-mergecells.png \
    3rdparty/qtxlsx/examples/xlsx/richtext/doc/images/richtext.png \
    3rdparty/qtxlsx/README.md \
    3rdparty/qtxlsx/src/xlsx/doc/qtxlsx.qdocconf \
    3rdparty/qtxlsx/examples/xlsx/CMakeLists.txt \
    3rdparty/qtxlsx/tests/auto/cmake/CMakeLists.txt \
    3rdparty/qtxlsx/tests/auto/CMakeLists.txt \
    3rdparty/qtxlsx/tests/CMakeLists.txt \
    3rdparty/qtxlsx/CMakeLists.txt \
    3rdparty/qtxlsx/QtXlsxWriterConfig.cmake.in \
    3rdparty/qtxlsx/sync.profile \
    3rdparty/qtxlsx/examples/xlsx/conditionalformatting/doc/src/conditionalformatting.qdoc \
    3rdparty/qtxlsx/examples/xlsx/demo/doc/src/demo.qdoc \
    3rdparty/qtxlsx/examples/xlsx/documentproperty/doc/src/documentproperty.qdoc \
    3rdparty/qtxlsx/examples/xlsx/extractdata/doc/src/extractdata.qdoc \
    3rdparty/qtxlsx/examples/xlsx/formulas/doc/src/formulas.qdoc \
    3rdparty/qtxlsx/examples/xlsx/hello/doc/src/hello.qdoc \
    3rdparty/qtxlsx/examples/xlsx/mergecells/doc/src/mergecells.qdoc \
    3rdparty/qtxlsx/examples/xlsx/richtext/doc/src/richtext.qdoc \
    3rdparty/qtxlsx/examples/xlsx/style/doc/src/style.qdoc \
    3rdparty/qtxlsx/examples/xlsx/worksheetoperations/doc/src/worksheetoperations.qdoc \
    3rdparty/qtxlsx/src/xlsx/doc/src/examples.qdoc \
    3rdparty/qtxlsx/src/xlsx/doc/src/qtxlsx-index.qdoc \
    3rdparty/qtxlsx/src/xlsx/doc/src/qtxlsx.qdoc \
    3rdparty/qtxlsx/src/xlsx/doc/src/usage.qdoc

# Copy external files post build
win32{
    PWD_WIN = $${PWD}
    DESTDIR_WIN = $${OUT_PWD}
    PWD_WIN ~= s,/,\\,g
    DESTDIR_WIN ~= s,/,\\,g

    CONFIG(release, debug|release) {
        data-html.commands = $(COPY_DIR) $$quote($$PWD_WIN\\html) $$quote($$DESTDIR_WIN\\release\\html)
    } else {
        data-html.commands = $(COPY_DIR) $$quote($$PWD_WIN\\html) $$quote($$DESTDIR_WIN\\debug\\html)
    }

    CONFIG(release, debug|release) {
        data-wav.commands = $(COPY_DIR) $$quote($$PWD_WIN\\data) $$quote($$DESTDIR_WIN\\release\\data)
    } else {
        data-wav.commands = $(COPY_DIR) $$quote($$PWD_WIN\\data) $$quote($$DESTDIR_WIN\\debug\\data)
    }
}
unix{
    data-html.commands = $(COPY_DIR) $$quote($$PWD/html) $$quote($$OUT_PWD)
    data-wav.commands = $(COPY_DIR) $$quote($$PWD/data) $$quote($$OUT_PWD)
}

QMAKE_EXTRA_TARGETS += data-html data-wav
POST_TARGETDEPS += data-html data-wav

SUBDIRS += \
    3rdparty/qtxlsx/examples/xlsx/calendar/calendar.pro \
    3rdparty/qtxlsx/examples/xlsx/chart/chart.pro \
    3rdparty/qtxlsx/examples/xlsx/chartsheet/chartsheet.pro \
    3rdparty/qtxlsx/examples/xlsx/conditionalformatting/conditionalformatting.pro \
    3rdparty/qtxlsx/examples/xlsx/datavalidation/datavalidation.pro \
    3rdparty/qtxlsx/examples/xlsx/definename/definename.pro \
    3rdparty/qtxlsx/examples/xlsx/demo/demo.pro \
    3rdparty/qtxlsx/examples/xlsx/documentproperty/documentproperty.pro \
    3rdparty/qtxlsx/examples/xlsx/extractdata/extractdata.pro \
    3rdparty/qtxlsx/examples/xlsx/formulas/formulas.pro \
    3rdparty/qtxlsx/examples/xlsx/hello/hello.pro \
    3rdparty/qtxlsx/examples/xlsx/hyperlinks/hyperlinks.pro \
    3rdparty/qtxlsx/examples/xlsx/image/image.pro \
    3rdparty/qtxlsx/examples/xlsx/mergecells/mergecells.pro \
    3rdparty/qtxlsx/examples/xlsx/numberformat/numberformat.pro \
    3rdparty/qtxlsx/examples/xlsx/richtext/richtext.pro \
    3rdparty/qtxlsx/examples/xlsx/rowcolumn/rowcolumn.pro \
    3rdparty/qtxlsx/examples/xlsx/style/style.pro \
    3rdparty/qtxlsx/examples/xlsx/worksheetoperations/worksheetoperations.pro \
    3rdparty/qtxlsx/examples/xlsx/xlsxwidget/xlsxwidget.pro \
    3rdparty/qtxlsx/examples/xlsx/xlsx.pro \
    3rdparty/qtxlsx/examples/examples.pro \
    3rdparty/qtxlsx/src/xlsx/doc/snippets/doc_src_qtxlsx.pro \
    3rdparty/qtxlsx/src/xlsx/xlsx.pro \
    3rdparty/qtxlsx/src/src.pro \
    3rdparty/qtxlsx/tests/auto/cellreference/cellreference.pro \
    3rdparty/qtxlsx/tests/auto/cmake/cmake.pro \
    3rdparty/qtxlsx/tests/auto/document/document.pro \
    3rdparty/qtxlsx/tests/auto/format/format.pro \
    3rdparty/qtxlsx/tests/auto/propsapp/propsapp.pro \
    3rdparty/qtxlsx/tests/auto/propscore/propscore.pro \
    3rdparty/qtxlsx/tests/auto/relationships/relationships.pro \
    3rdparty/qtxlsx/tests/auto/richstring/richstring.pro \
    3rdparty/qtxlsx/tests/auto/sharedstrings/sharedstrings.pro \
    3rdparty/qtxlsx/tests/auto/styles/styles.pro \
    3rdparty/qtxlsx/tests/auto/utility/utility.pro \
    3rdparty/qtxlsx/tests/auto/worksheet/worksheet.pro \
    3rdparty/qtxlsx/tests/auto/xlsxconditionalformatting/xlsxconditionalformatting.pro \
    3rdparty/qtxlsx/tests/auto/zipreader/zipreader.pro \
    3rdparty/qtxlsx/tests/auto/auto.pro \
    3rdparty/qtxlsx/tests/benchmarks/xmlspace/xmlspace.pro \
    3rdparty/qtxlsx/tests/benchmarks/benchmarks.pro \
    3rdparty/qtxlsx/tests/tests.pro \
    3rdparty/qtxlsx/qtxlsx.pro

#-------------------------------------------------
#
# Project created by QtCreator 2014-02-01T20:30:52
#
#-------------------------------------------------
QMAKE_CFLAGS += -std=gnu++11 -std=c99

QT       += core gui webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sptk-analyzer
TEMPLATE = app

win32{
    INCLUDEPATH += "c:/Dev/MathGL/include"
    LIBS += -L"c:/Dev/MathGL/lib" -llibmgl -llibmgl-qt5

#    INCLUDEPATH += "c:/Dev/openal-soft-1.15.1-bin/include"
#    LIBS += -L"c:/Dev/openal-soft-1.15.1-bin/lib/Win32" -llibOpenAL32
    INCLUDEPATH += "c:/Dev/openal-soft-1.17.2-bin/include"
    LIBS += -L"c:/Dev/openal-soft-1.17.2-bin/lib/Win32" -llibOpenAL32

    INCLUDEPATH += "c:/Dev/Libs/gsl-2.1-bin/include"
    LIBS += -L"c:/Dev/Libs/gsl-2.1-bin/lib" -llibgsl -llibgslcblas
}

unix:!mac {
    QMAKE_CFLAGS += -U__STRICT_ANSI__

    INCLUDEPATH += "/usr/include"

    LIBS += `pkg-config --libs gsl openal`
}

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    Recorder/soundrecorder.cpp \
    graphswindow.cpp \
    settingsdialog.cpp \
    Recorder/autosoundrecorder.cpp \
    Recorder/timesoundrecorder.cpp \
    Recorder/buffer.cpp \
    drawer.cpp \
    soundplayer.cpp \
    graphsevalwindow.cpp \
    DP/vectordp.cpp \
    drawerevalpitch.cpp \
    drawerevalenergy.cpp \
    drawerevalpitchbyspectr.cpp \
    drawerevalenergybyspectr.cpp \
    DP/spectrdp.cpp \
    drawerevalspectr.cpp \
    webwindow.cpp \
    Recorder/dpsoundrecorder.cpp \
    DP/continuousdp.cpp \
    drawerdp.cpp \
    dataprocessing.cpp

HEADERS  += \
    mainwindow.h \
    Recorder/soundrecorder.h \
    graphswindow.h \
    settingsdialog.h \
    Recorder/autosoundrecorder.h \
    Recorder/timesoundrecorder.h \
    Recorder/buffer.h \
    drawer.h \
    soundplayer.h \
    graphsevalwindow.h \
    abstractdrawer.h \
    DP/dp.h \
    drawerevalpitch.h \
    drawerevalenergy.h \
    drawerevalpitchbyspectr.h \
    drawerevalenergybyspectr.h \
    DP/vectordp.h \
    DP/spectrdp.h \
    drawerevalspectr.h \
    webwindow.h \
    Recorder/dpsoundrecorder.h \
    DP/continuousdp.h \
    drawerdp.h \
    dataprocessing.h

# OpenAL
SOURCES += \
    OpenAL/openal_wrapper.c\
    OpenAL/wavFile.c

HEADERS  += \
    OpenAL/openal_wrapper.h \
    OpenAL/wavFile.h

# SPTK
INCLUDEPATH += ./SPTK

SOURCES +=  \
    SPTK/sptk.c\
    SPTK/agexp.c\
    SPTK/cholesky.c \
    SPTK/fileio.c \
    SPTK/fillz.c \
    SPTK/getfp.c \
    SPTK/getmem.c \
    SPTK/gexp.c \
    SPTK/glog.c \
    SPTK/invert.c \
    SPTK/matrix.c \
    SPTK/movem.c \
    SPTK/mseq.c \
    SPTK/theq.c \
    SPTK/toeplitz.c \
    SPTK/vector.c \
#    SPTK/acep/acep.c \
#    SPTK/acep/_acep.c \
#    SPTK/acorr/acorr.c \
    SPTK/acorr/_acorr.c \
#    SPTK/agcep/agcep.c \
#    SPTK/agcep/_agcep.c \
#    SPTK/amcep/amcep.c \
#    SPTK/amcep/_amcep.c \
#    SPTK/average/average.c \
#    SPTK/average/_average.c \
#    SPTK/b2mc/b2mc.c \
#    SPTK/b2mc/_b2mc.c \
#    SPTK/bcp/bcp.c \
#    SPTK/bcut/bcut.c \
#    SPTK/c2acr/c2acr.c \
#    SPTK/c2acr/_c2acr.c \
#    SPTK/c2ir/c2ir.c \
#    SPTK/c2ir/_c2ir.c \
#    SPTK/c2sp/c2sp.c \
#    SPTK/c2sp/_c2sp.c \
#    SPTK/cat2/cat2.c \
#    SPTK/cdist/cdist.c \
#    SPTK/clip/clip.c \
#    SPTK/clip/_clip.c \
#    SPTK/da/dawrite.c \
#    SPTK/da/winplay.c \
#    SPTK/dct/dct.c \
#    SPTK/dct/_dct.c \
#    SPTK/decimate/decimate.c \
#    SPTK/delay/delay.c \
#    SPTK/delta/delta.c \
#    SPTK/df2/df2.c \
#    SPTK/df2/_df2.c \
#    SPTK/dfs/dfs.c \
#    SPTK/dfs/_dfs.c \
#    SPTK/dmp/dmp.c \
#    SPTK/ds/ds.c \
#    SPTK/dtw/dtw.c \
#    SPTK/echo2/echo2.c \
#    SPTK/excite/excite.c \
#    SPTK/extract/extract.c \
#    SPTK/fd/fd.c \
#    SPTK/fft2/fft2.c \
#    SPTK/fft2/_fft2.c \
#    SPTK/fft/fft.c \
    SPTK/fft/_fft.c \
#    SPTK/fftcep/fftcep.c \
#    SPTK/fftcep/_fftcep.c \
#    SPTK/fftr2/fftr2.c \
#    SPTK/fftr2/_fftr2.c \
#    SPTK/fftr/fftr.c \
    SPTK/fftr/_fftr.c \
#    SPTK/fig+fdrw/fdrw.c \
#    SPTK/fig+fdrw/fig.c \
#    SPTK/fig+fdrw/fig0.c \
#    SPTK/fig+fdrw/fig1.c \
#    SPTK/fig+fdrw/plot.c \
#    SPTK/fig+fdrw/plsub.c \
    SPTK/frame/frame.c \
#    SPTK/freqt/freqt.c \
#    SPTK/freqt/_freqt.c \
#    SPTK/gc2gc/gc2gc.c \
#    SPTK/gc2gc/_gc2gc.c \
#    SPTK/gcep/gcep.c \
#    SPTK/gcep/_gcep.c \
#    SPTK/glsadf/glsadf.c \
#    SPTK/glsadf/_glsadf.c \
#    SPTK/gmm/gmm.c \
#    SPTK/gmm/_gmm.c \
#    SPTK/gmm/gmmp.c \
#    SPTK/gnorm/gnorm.c \
#    SPTK/gnorm/_gnorm.c \
#    SPTK/grpdelay/grpdelay.c \
#    SPTK/grpdelay/_grpdelay.c \
#    SPTK/histogram/histogram.c \
#    SPTK/histogram/_histogram.c \
#    SPTK/idct/idct.c \
#    SPTK/ifft2/ifft2.c \
#    SPTK/ifft2/_ifft2.c \
#    SPTK/ifft/ifft.c \
#    SPTK/ifft/_ifft.c \
#    SPTK/ifftr/ifftr.c \
#    SPTK/ifftr/_ifftr.c \
#    SPTK/ignorm/ignorm.c \
#    SPTK/ignorm/_ignorm.c \
#    SPTK/impulse/impulse.c \
#    SPTK/imsvq/imsvq.c \
#    SPTK/imsvq/_imsvq.c \
#    SPTK/interpolate/interpolate.c \
#    SPTK/ivq/ivq.c \
#    SPTK/ivq/_ivq.c \
#    SPTK/lbg/lbg.c \
#    SPTK/lbg/_lbg.c \
#    SPTK/levdur/levdur.c \
    SPTK/levdur/_levdur.c \
#    SPTK/linear_intpl/linear_intpl.c \
#    SPTK/lmadf/lmadf.c \
#    SPTK/lmadf/_lmadf.c \
    SPTK/lpc2c/lpc2c.c \
    SPTK/lpc2c/_lpc2c.c \
#    SPTK/lpc2lsp/lpc2lsp.c \
#    SPTK/lpc2lsp/_lpc2lsp.c \
#    SPTK/lpc2par/lpc2par.c \
#    SPTK/lpc2par/_lpc2par.c \
    SPTK/lpc/lpc.c \
    SPTK/lpc/_lpc.c \
#    SPTK/lsp2lpc/lsp2lpc.c \
#    SPTK/lsp2lpc/_lsp2lpc.c \
#    SPTK/lsp2sp/lsp2sp.c \
#    SPTK/lsp2sp/_lsp2sp.c \
#    SPTK/lspcheck/lspcheck.c \
#    SPTK/lspcheck/_lspcheck.c \
#    SPTK/lspdf/lspdf.c \
#    SPTK/lspdf/_lspdf.c \
#    SPTK/ltcdf/ltcdf.c \
#    SPTK/ltcdf/_ltcdf.c \
#    SPTK/mc2b/mc2b.c \
#    SPTK/mc2b/_mc2b.c \
#    SPTK/mcep/mcep.c \
#    SPTK/mcep/_mcep.c \
#    SPTK/merge/merge.c \
#    SPTK/mfcc/mfcc.c \
#    SPTK/mfcc/_mfcc.c \
#    SPTK/mgc2mgc/mgc2mgc.c \
#    SPTK/mgc2mgc/_mgc2mgc.c \
#    SPTK/mgc2sp/mgc2sp.c \
#    SPTK/mgc2sp/_mgc2sp.c \
#    SPTK/mgcep/mgcep.c \
#    SPTK/mgcep/_mgcep.c \
#    SPTK/mglsadf/mglsadf.c \
#    SPTK/mglsadf/_mglsadf.c \
#    SPTK/minmax/minmax.c \
#    SPTK/mlpg/mlpg.c \
#    SPTK/mlpg/_mlpg.c \
#    SPTK/mlsacheck/mlsacheck.c \
#    SPTK/mlsadf/mlsadf.c \
#    SPTK/mlsadf/_mlsadf.c \
#    SPTK/msvq/msvq.c \
#    SPTK/msvq/_msvq.c \
#    SPTK/nan/nan.c \
#    SPTK/norm0/norm0.c \
#    SPTK/norm0/_norm0.c \
#    SPTK/nrand/nrand.c \
    SPTK/nrand/_nrand.c \
#    SPTK/par2lpc/par2lpc.c \
#    SPTK/par2lpc/_par2lpc.c \
#    SPTK/pca/pca.c \
#    SPTK/pca/pcas.c \
#    SPTK/phase/phase.c \
#    SPTK/phase/_phase.c \
    SPTK/pitch/pitch.c \
    SPTK/pitch/snack/jkGetF0.c \
    SPTK/pitch/snack/sigproc.c \
    SPTK/pitch/swipe/swipe.c \
#    SPTK/poledf/poledf.c \
#    SPTK/poledf/_poledf.c \
#    SPTK/psgr/dict.c \
#    SPTK/psgr/eps.c \
#    SPTK/psgr/plot.c \
#    SPTK/psgr/psgr.c \
#    SPTK/ramp/ramp.c \
#    SPTK/rawtowav/rawtowav.c \
#    SPTK/reverse/reverse.c \
#    SPTK/reverse/_reverse.c \
#    SPTK/rmse/rmse.c \
#    SPTK/rmse/_rmse.c \
#    SPTK/root_pol/root_pol.c \
#    SPTK/root_pol/_root_pol.c \
#    SPTK/sin/sin.c \
#    SPTK/smcep/smcep.c \
#    SPTK/smcep/_smcep.c \
#    SPTK/snr/snr.c \
#    SPTK/sopr/sopr.c \
    SPTK/spec/spec.c \
#    SPTK/step/step.c \
#    SPTK/swab/swab.c \
#    SPTK/symmetrize/symmetrize.c \
#    SPTK/train/train.c \
#    SPTK/transpose/transpose.c \
#    SPTK/transpose/_transpose.c \
#    SPTK/uels/uels.c \
#    SPTK/uels/_uels.c \
#    SPTK/ulaw/ulaw.c \
#    SPTK/ulaw/_ulaw.c \
#    SPTK/us/us.c \
#    SPTK/vc/vc.c \
#    SPTK/vc/_vc.c \
#    SPTK/vc/hts_engine_API/HTS_misc.c \
#    SPTK/vc/hts_engine_API/HTS_pstream.c \
#    SPTK/vc/hts_engine_API/HTS_sstream.c \
#    SPTK/vopr/vopr.c \
#    SPTK/vq/vq.c \
#    SPTK/vq/_vq.c \
#    SPTK/vstat/vstat.c \
#    SPTK/vsum/vsum.c \
    SPTK/window/window.c \
    SPTK/window/_window.c \
    SPTK/x2x/x2x.c \
#    SPTK/xgr/plot.c \
#    SPTK/xgr/window.c \
#    SPTK/xgr/xgr.c \
#    SPTK/zcross/zcross.c \
#    SPTK/zcross/_zcross.c \
#    SPTK/zerodf/zerodf.c \
#    SPTK/zerodf/_zerodf.c \
    SPTK/others/func.c \
    SPTK/others/interpolation.c \

HEADERS  += \
    SPTK/SPTK.h \
    SPTK/vector.h \
#    SPTK/da/da.h \
#    SPTK/da/winplay.h \
#    SPTK/fig+fdrw/fig.h \
#    SPTK/fig+fdrw/plot.h \
    SPTK/lpc/lpc.h \
    SPTK/lpc2c/lpc2c.h \
    SPTK/pitch/pitch.h \
    SPTK/pitch/snack/jkGetF0.h \
    SPTK/frame/frame.h \
#    SPTK/psgr/psgr.h \
#    SPTK/vc/hts_engine_API/HTS_engine.h \
#    SPTK/vc/hts_engine_API/HTS_hidden.h \
    SPTK/spec/spec.h \
    SPTK/window/window.h \
    SPTK/x2x/x2x.h \
#    SPTK/xgr/config.h \
#    SPTK/xgr/gcdata.h \
#    SPTK/xgr/xgr.h \
    SPTK/others/func.h \
    SPTK/others/interpolation.h \

FORMS += \
    mainwindow.ui \
    graphswindow.ui \
    settingsdialog.ui \
    webwindow.ui

RESOURCES += \
    icons.qrc

DISTFILES += \
    html/rus.html

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

include(GenerateExportHeader)

if(enable-qt5)
	include(../../scripts/qt5.cmake)
	set(MGL_QT5_FILES ../qt.cpp ../../include/mgl2/qt.h ../../include/mgl2/qmathgl.h)
	mgl_add_lib(qt5 ${MGL_QT5_FILES})
	target_link_libraries(mgl-qt5 mgl ${MGL_QT5_LIBS})
	target_link_libraries(mgl-qt5-static mgl-static ${MGL_QT5_LIBS})
	if(enable-qt5asqt)
		mgl_add_lib(qt ${MGL_QT5_FILES})
		target_link_libraries(mgl-qt mgl ${MGL_QT5_LIBS})
		target_link_libraries(mgl-qt-static mgl-static ${MGL_QT5_LIBS})
	endif(enable-qt5asqt)
	if(MGL_HAVE_FLTK)
		mgl_add_lib(wnd ${MGL_QT5_FILES} ../fltk.cpp ../../include/mgl2/fltk.h ../image.cpp ../image.h)
		target_include_directories(mgl-wnd SYSTEM PUBLIC ${FLTK_INCLUDE_DIR})
		target_include_directories(mgl-wnd-static SYSTEM PUBLIC ${FLTK_INCLUDE_DIR})
		target_link_libraries(mgl-wnd mgl ${FLTK_LIBRARIES} ${MGL_QT5_LIBS})
		target_link_libraries(mgl-wnd-static mgl-static ${FLTK_LIBRARIES} ${MGL_QT5_LIBS})
	endif(MGL_HAVE_FLTK)
endif(enable-qt5)

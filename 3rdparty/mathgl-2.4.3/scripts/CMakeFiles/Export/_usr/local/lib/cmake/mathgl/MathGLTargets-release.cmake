#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "mgl" for configuration "Release"
set_property(TARGET mgl APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(mgl PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/local/lib/libmgl.so.7.5.0"
  IMPORTED_SONAME_RELEASE "libmgl.so.7.5.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS mgl )
list(APPEND _IMPORT_CHECK_FILES_FOR_mgl "/usr/local/lib/libmgl.so.7.5.0" )

# Import target "mgl-static" for configuration "Release"
set_property(TARGET mgl-static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(mgl-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LOCATION_RELEASE "/usr/local/lib/libmgl.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS mgl-static )
list(APPEND _IMPORT_CHECK_FILES_FOR_mgl-static "/usr/local/lib/libmgl.a" )

# Import target "mgl-qt5" for configuration "Release"
set_property(TARGET mgl-qt5 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(mgl-qt5 PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/local/lib/libmgl-qt5.so.7.5.0"
  IMPORTED_SONAME_RELEASE "libmgl-qt5.so.7.5.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS mgl-qt5 )
list(APPEND _IMPORT_CHECK_FILES_FOR_mgl-qt5 "/usr/local/lib/libmgl-qt5.so.7.5.0" )

# Import target "mgl-qt5-static" for configuration "Release"
set_property(TARGET mgl-qt5-static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(mgl-qt5-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "/usr/local/lib/libmgl-qt5.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS mgl-qt5-static )
list(APPEND _IMPORT_CHECK_FILES_FOR_mgl-qt5-static "/usr/local/lib/libmgl-qt5.a" )

# Import target "mgl-qt" for configuration "Release"
set_property(TARGET mgl-qt APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(mgl-qt PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/local/lib/libmgl-qt.so.7.5.0"
  IMPORTED_SONAME_RELEASE "libmgl-qt.so.7.5.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS mgl-qt )
list(APPEND _IMPORT_CHECK_FILES_FOR_mgl-qt "/usr/local/lib/libmgl-qt.so.7.5.0" )

# Import target "mgl-qt-static" for configuration "Release"
set_property(TARGET mgl-qt-static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(mgl-qt-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "/usr/local/lib/libmgl-qt.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS mgl-qt-static )
list(APPEND _IMPORT_CHECK_FILES_FOR_mgl-qt-static "/usr/local/lib/libmgl-qt.a" )

# Import target "udav" for configuration "Release"
set_property(TARGET udav APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(udav PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/local/bin/udav"
  )

list(APPEND _IMPORT_CHECK_TARGETS udav )
list(APPEND _IMPORT_CHECK_FILES_FOR_udav "/usr/local/bin/udav" )

# Import target "mgltask" for configuration "Release"
set_property(TARGET mgltask APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(mgltask PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/local/bin/mgltask"
  )

list(APPEND _IMPORT_CHECK_TARGETS mgltask )
list(APPEND _IMPORT_CHECK_FILES_FOR_mgltask "/usr/local/bin/mgltask" )

# Import target "mglconv" for configuration "Release"
set_property(TARGET mglconv APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(mglconv PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/local/bin/mglconv"
  )

list(APPEND _IMPORT_CHECK_TARGETS mglconv )
list(APPEND _IMPORT_CHECK_FILES_FOR_mglconv "/usr/local/bin/mglconv" )

# Import target "mgl.cgi" for configuration "Release"
set_property(TARGET mgl.cgi APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(mgl.cgi PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/local/lib/cgi-bin/mgl.cgi"
  )

list(APPEND _IMPORT_CHECK_TARGETS mgl.cgi )
list(APPEND _IMPORT_CHECK_FILES_FOR_mgl.cgi "/usr/local/lib/cgi-bin/mgl.cgi" )

# Import target "mglview" for configuration "Release"
set_property(TARGET mglview APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(mglview PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/local/bin/mglview"
  )

list(APPEND _IMPORT_CHECK_TARGETS mglview )
list(APPEND _IMPORT_CHECK_FILES_FOR_mglview "/usr/local/bin/mglview" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

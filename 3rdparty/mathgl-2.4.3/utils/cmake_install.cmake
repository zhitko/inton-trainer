# Install script for directory: /home/vzhitko/Dev/UIIP/inton-trainer/3rdparty/mathgl-2.4.3/utils

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/mgltask" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/mgltask")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/local/bin/mgltask"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/bin/mgltask")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/bin" TYPE EXECUTABLE FILES "/home/vzhitko/Dev/UIIP/inton-trainer/3rdparty/mathgl-2.4.3/utils/mgltask")
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/mgltask" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/mgltask")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/bin/mgltask")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/mglconv" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/mglconv")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/local/bin/mglconv"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/bin/mglconv")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/bin" TYPE EXECUTABLE FILES "/home/vzhitko/Dev/UIIP/inton-trainer/3rdparty/mathgl-2.4.3/utils/mglconv")
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/mglconv" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/mglconv")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/usr/local/bin/mglconv"
         OLD_RPATH "/home/vzhitko/Dev/UIIP/inton-trainer/3rdparty/mathgl-2.4.3/src:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/bin/mglconv")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/usr/local/lib/cgi-bin/mgl.cgi" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/lib/cgi-bin/mgl.cgi")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/local/lib/cgi-bin/mgl.cgi"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/cgi-bin/mgl.cgi")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib/cgi-bin" TYPE EXECUTABLE FILES "/home/vzhitko/Dev/UIIP/inton-trainer/3rdparty/mathgl-2.4.3/utils/mgl.cgi")
  if(EXISTS "$ENV{DESTDIR}/usr/local/lib/cgi-bin/mgl.cgi" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/lib/cgi-bin/mgl.cgi")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/lib/cgi-bin/mgl.cgi")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/mglview" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/mglview")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/local/bin/mglview"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/bin/mglview")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/bin" TYPE EXECUTABLE FILES "/home/vzhitko/Dev/UIIP/inton-trainer/3rdparty/mathgl-2.4.3/utils/mglview")
  if(EXISTS "$ENV{DESTDIR}/usr/local/bin/mglview" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/mglview")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/usr/local/bin/mglview"
         OLD_RPATH "/home/vzhitko/Dev/UIIP/inton-trainer/3rdparty/mathgl-2.4.3/widgets/qt5:/home/vzhitko/opt/Qt/5.12.2/gcc_64/lib:/home/vzhitko/Dev/UIIP/inton-trainer/3rdparty/mathgl-2.4.3/src:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/bin/mglview")
    endif()
  endif()
endif()


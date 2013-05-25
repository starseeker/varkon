# Minimum required version of CMake
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
if(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
endif(COMMAND CMAKE_POLICY)

# Set CMake project name
PROJECT(VARKON)

#set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/CMake")
find_package(X11)
find_package(OpenGL)
find_package(BLAS)

# Handle Math Library
find_library(HAVE_M_LIBRARY NAMES m)
if(HAVE_M_LIBRARY)
  set(M_LIBRARY m)
endif(HAVE_M_LIBRARY)

#---------------------------------------------------------------------
# The following logic is what allows binaries to run successfully in
# the build directory AND install directory.  Thanks to plplot for
# identifying the necessity of setting CMAKE_INSTALL_NAME_DIR on OSX.
# Documentation of these options is available at
# http://www.cmake.org/Wiki/CMake_RPATH_handling

# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# the RPATH/INSTALL_NAME_DIR to be used when installing
if (NOT APPLE)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LIB_DIR}:\$ORIGIN/../${LIB_DIR}")
endif(NOT APPLE)
# On OSX, we need to set INSTALL_NAME_DIR instead of RPATH
# http://www.cmake.org/cmake/help/cmake-2-8-docs.html#variable:CMAKE_INSTALL_NAME_DIR
set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${LIB_DIR}")

# add the automatically determined parts of the RPATH which point to
# directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


#---------------------------------------------------------------------
# Set directory variables
if(NOT DEFINED BIN_DIR)
  set(BIN_DIR bin)
endif(NOT DEFINED BIN_DIR)
if(NOT DEFINED LIB_DIR)
  set(LIB_DIR lib)
endif(NOT DEFINED LIB_DIR)

#--------------------------------------------------------------------- 
# Output directories - this is where built library and executable 
# files will be placed after building but prior to install.  The 
# necessary variables change between single and multi configuration 
# build systems, so it is necessary to handle both cases on a 
# conditional basis. 
 
if(NOT CMAKE_CONFIGURATION_TYPES) 
  # If we're not doing multi-configuration, just set the three main 
  # variables to the correct values. 
  if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY) 
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${VARKON_BINARY_DIR}/${LIB_DIR} CACHE INTERNAL "Single output directory for building all libraries.") 
  endif(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY) 
  if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY) 
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${VARKON_BINARY_DIR}/${LIB_DIR} CACHE INTERNAL "Single output directory for building all archives.") 
  endif(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY) 
  if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY) 
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${VARKON_BINARY_DIR}/${BIN_DIR} CACHE INTERNAL "Single output directory for building all executables.") 
  endif(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY) 
else(NOT CMAKE_CONFIGURATION_TYPES) 
  # Multi-configuration is more difficult.  Not only do we need to 
  # properly set the output directories, but we also need to 
  # identify the "toplevel" directory for each configuration so 
  # we can place files, documentation, etc. in the correct 
  # relative positions.  Because files may be placed by CMake 
  # without a build target to put them in their proper relative build 
  # directory position using these paths, we must fully qualify them 
  # without using CMAKE_CFG_INTDIR. 
  # 
  # We define directories that may not be quite "standard" 
  # for a particular build tool - for example, native VS2010 projects use 
  # another directory to denote CPU type being compiled for - but CMake only 
  # supports multi-configuration setups having multiple configurations, 
  # not multiple compilers. 
  # 
  # One additional wrinkle we must watch for here is the case where 
  # a multi-configuration setup uses "." for its internal directory - 
  # if that's the case, we need to just set the various config output 
  # directories to the same value. 
  set(CFG_ROOT ${VARKON_BINARY_DIR})
  foreach(CFG_TYPE ${CMAKE_CONFIGURATION_TYPES})
    if(NOT "${CMAKE_CFG_INTDIR}" STREQUAL ".")
      set(CFG_ROOT ${VARKON_BINARY_DIR}/${CFG_TYPE})
    endif(NOT "${CMAKE_CFG_INTDIR}" STREQUAL ".")
    string(TOUPPER "${CFG_TYPE}" CFG_TYPE_UPPER)
    if(NOT "CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CFG_TYPE_UPPER}")
      set("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CFG_TYPE_UPPER}" ${CFG_ROOT}/${LIB_DIR} CACHE INTERNAL "Single output directory for building ${CFG_TYPE} libraries.")
    endif(NOT "CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CFG_TYPE_UPPER}")
    if(NOT "CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CFG_TYPE_UPPER}")
      set("CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CFG_TYPE_UPPER}" ${CFG_ROOT}/${LIB_DIR} CACHE INTERNAL "Single output directory for building ${CFG_TYPE} archives.")
    endif(NOT "CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CFG_TYPE_UPPER}")
    if(NOT "CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CFG_TYPE_UPPER}")
      set("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CFG_TYPE_UPPER}" ${CFG_ROOT}/${BIN_DIR} CACHE INTERNAL "Single output directory for building ${CFG_TYPE} executables.")
    endif(NOT "CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CFG_TYPE_UPPER}")
    if(NOT "CMAKE_BINARY_DIR_${CFG_TYPE_UPPER}")
      set("CMAKE_BINARY_DIR_${CFG_TYPE_UPPER}" ${CFG_ROOT} CACHE INTERNAL "Toplevel binary dir for ${CFG_TYPE} building.")
    endif(NOT "CMAKE_BINARY_DIR_${CFG_TYPE_UPPER}")
    if(NOT "VARKON_BINARY_DIR_${CFG_TYPE_UPPER}")
      set("VARKON_BINARY_DIR_${CFG_TYPE_UPPER}" ${CFG_ROOT} CACHE INTERNAL "Toplevel binary dir for ${CFG_TYPE} building.")
    endif(NOT "VARKON_BINARY_DIR_${CFG_TYPE_UPPER}")
  endforeach()
endif(NOT CMAKE_CONFIGURATION_TYPES)

#---------------------------------------------------------------------
if(NOT DEFINED ENABLE_TIFF_SUPPORT)
  set(ENABLE_TIFF_SUPPORT OFF CACHE BOOL "Support for TIFF image format")
else(NOT DEFINED ENABLE_TIFF_SUPPORT)
  set(ENABLE_TIFF_SUPPORT ${ENABLE_TIFF_SUPPORT} CACHE BOOL "Support for TIFF image format")
endif(NOT DEFINED ENABLE_TIFF_SUPPORT)

# Support for EIGASYM() function in MBS - needs ATLAS library installed
if(NOT DEFINED ENABLE_ATLAS_SUPPORT)
  set(ENABLE_ATLAS_SUPPORT OFF CACHE BOOL "Support for ATLAS")
else(NOT DEFINED ENABLE_ATLAS_SUPPORT)
  set(ENABLE_ATLAS_SUPPORT ${ENABLE_ATLAS_SUPPORT} CACHE BOOL "Support for ATLAS")
endif(NOT DEFINED ENABLE_ATLAS_SUPPORT)

if(NOT DEFINED ENABLE_ODBC_SUPPORT)
  set(ENABLE_ODBC_SUPPORT OFF CACHE BOOL "Support for ODBC")
else(NOT DEFINED ENABLE_ODBC_SUPPORT)
  set(ENABLE_ODBC_SUPPORT ${ENABLE_ODBC_SUPPORT} CACHE BOOL "Support for ODBC")
endif(NOT DEFINED ENABLE_ODBC_SUPPORT)

if(NOT DEFINED ENABLE_RAP_SUPPORT)
  set(ENABLE_RAP_SUPPORT OFF CACHE BOOL "Support for Robot Application Protocol")
else(NOT DEFINED ENABLE_RAP_SUPPORT)
  set(ENABLE_RAP_SUPPORT ${ENABLE_RAP_SUPPORT} CACHE BOOL "Support for Robot Application Protocol")
endif(NOT DEFINED ENABLE_RAP_SUPPORT)

if(NOT DEFINED ENABLE_ORU_SUPPORT)
  set(ENABLE_ORU_SUPPORT OFF CACHE BOOL "Support for Örebro University specific routines")
else(NOT DEFINED ENABLE_ORU_SUPPORT)
  set(ENABLE_ORU_SUPPORT ${ENABLE_ORU_SUPPORT} CACHE BOOL "Support for Örebro University specific routines")
endif(NOT DEFINED ENABLE_ORU_SUPPORT)

#---------------------------------------------------------------------
#set(STDC_HEADERS 1)
#include(CheckIncludeFile)
#CHECK_INCLUDE_FILE(inttypes.h HAVE_INTTYPES_H)
#CHECK_INCLUDE_FILE(memory.h HAVE_MEMORY_H)
#CHECK_INCLUDE_FILE(stdbool.h HAVE_STDBOOL_H)
#CHECK_INCLUDE_FILE(stdint.h HAVE_STDINT_H)
#CHECK_INCLUDE_FILE(stdlib.h HAVE_STDLIB_H)
#CHECK_INCLUDE_FILE(string.h HAVE_STRING_H)
#CHECK_INCLUDE_FILE(strings.h HAVE_STRINGS_H)
#CHECK_INCLUDE_FILE(sys/stat.h HAVE_SYS_STAT_H)
#CHECK_INCLUDE_FILE(sys/types.h HAVE_SYS_TYPES_H)
#CHECK_INCLUDE_FILE(unistd.h HAVE_UNISTD_H)
#include(CheckFunctionExists)
#CHECK_FUNCTION_EXISTS(memset HAVE_MEMSET)
#CHECK_FUNCTION_EXISTS(mmap HAVE_MMAP)
#CHECK_FUNCTION_EXISTS(munmap HAVE_MUNMAP)
#CHECK_FUNCTION_EXISTS(strdup HAVE_STRDUP)
#CHECK_FUNCTION_EXISTS(strndup HAVE_STRNDUP)
#include(CheckTypeSize)
#CHECK_TYPE_SIZE(char SIZEOF_CHAR)
#CHECK_TYPE_SIZE(int SIZEOF_INT)
#CHECK_TYPE_SIZE(long SIZEOF_LONG)
#CHECK_TYPE_SIZE(short SIZEOF_SHORT)

#configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
#add_definitions(-DHAVE_CONFIG_H)

#---------------------------------------------------------------------
add_subdirectory(sources)

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8

# Generated from StepLabeller.pro.

cmake_minimum_required(VERSION 3.16)
project(StepLabeller VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

SET(OPENCV_DIR $ENV{OPENCV_DIR})

if(NOT DEFINED INSTALL_EXAMPLEDIR)
    set(INSTALL_EXAMPLEDIR "bin")
endif()

include(CMakePrintHelpers)
cmake_print_variables(INSTALL_EXAMPLEDIR)
cmake_print_variables(OPENCV_DIR)

# Stop if OPENCV_DIR isn't set
if(NOT DEFINED OPENCV_DIR)
  message(FATAL_ERROR "OPENCV_DIR not found!
     Set the OPENCV_DIR environment variable or pass in to cmake with -DOPENCV_DIR={path_to_opencv_root_build_dir}")
endif(NOT DEFINED OPENCV_DIR)

add_subdirectory(src)

install(TARGETS StepLabeller
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)

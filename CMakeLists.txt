# Generated from StepLabeller.pro.

cmake_minimum_required(VERSION 3.16)
project(StepLabeller VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Try to used passed in OPENCV_DIR, or ENV variable if not passed in
if(NOT DEFINED OPENCV_DIR)
    SET(OPENCV_DIR $ENV{OPENCV_DIR})
    if(NOT DEFINED OPENCV_DIR)
        message(FATAL_ERROR "OPENCV_DIR not found!
        Set the OPENCV_DIR environment variable or pass in to cmake with -DOPENCV_DIR={path_to_opencv_root_build_dir}")
     endif()
endif(NOT DEFINED OPENCV_DIR)

if(NOT DEFINED INSTALL_EXAMPLEDIR)
    set(INSTALL_EXAMPLEDIR "bin")
endif()


include(CMakePrintHelpers)
cmake_print_variables(INSTALL_EXAMPLEDIR)
cmake_print_variables(OPENCV_DIR)
cmake_print_variables(CMAKE_MODULE_PATH)
set(OpenCV_STATIC OFF)
find_package(OpenCV REQUIRED PATHS ${OPENCV_DIR} NO_DEFAULT_PATH)

add_subdirectory(src)

install(TARGETS StepLabeller
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)

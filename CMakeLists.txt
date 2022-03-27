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

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Multimedia)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

cmake_print_variables(QT_VERSION_MAJOR)

add_executable(StepLabeller WIN32 MACOSX_BUNDLE
    main.cpp
    capture_thread.cpp capture_thread.h
    filetable.cpp filetable.h
    mainwindow.cpp mainwindow.h
    playercontrols.cpp playercontrols.h
    popupwindow.cpp popupwindow.h popupwindow.ui
    steptable.cpp steptable.h
)

target_include_directories(StepLabeller PUBLIC
    ${OPENCV_DIR}/install/include
)

target_link_libraries(StepLabeller PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Multimedia
    Qt::Widgets
    ${OPENCV_DIR}/bin/libopencv_world451.dll
)

install(TARGETS StepLabeller
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)

# Generated from StepLabeller.pro.

cmake_minimum_required(VERSION 3.16)
project(StepLabeller VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Multimedia)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

add_executable(StepLabeller WIN32 MACOSX_BUNDLE
    capture_thread.cpp capture_thread.h
    filetable.cpp filetable.h
    main.cpp
    mainwindow.cpp mainwindow.h
    playercontrols.cpp playercontrols.h
    popupwindow.cpp popupwindow.h popupwindow.ui
    steptable.cpp steptable.h
)
target_include_directories(StepLabeller PUBLIC
    c:/opencv_build_release/install/include
)

target_link_libraries(StepLabeller PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Multimedia
    c:/opencv_build_release/bin/libopencv_world451.dll
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(StepLabeller PUBLIC
        Qt::Widgets
    )
endif()

install(TARGETS StepLabeller
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)

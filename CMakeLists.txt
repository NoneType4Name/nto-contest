cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
  nto-contest
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${CMAKE_SOURCE_DIR}/deps/sqlite3")

find_package(sqlite3 REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES nto-contest_ru_RU.ts)

set(PROJECT_SOURCES main.cxx ${TS_FILES})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(
    nto-contest
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    commercial.hxx
    commercial.cxx
    commercial.ui
    order.hxx
    order.cxx
    order.ui
    login.hxx
    login.cxx
    login.ui
    neworder.hxx
    neworder.cxx
    neworder.ui
    technical.hxx technical.cxx technical.ui
    production.hxx production.cxx production.ui
  )

  # Define target properties for Android with Qt 6 as: set_property(TARGET
  # nto-contest APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
  # ${CMAKE_CURRENT_SOURCE_DIR}/android) For more information, see
  # https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
  qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
  if(ANDROID)
    add_library(nto-contest SHARED ${PROJECT_SOURCES})

  # Define properties for Android with Qt 5 after find_package() calls as:
  # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
  else()
    add_executable(nto-contest ${PROJECT_SOURCES})
  endif()

  qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_include_directories(nto-contest PRIVATE ${SQLite3_INCLUDE_DIRS})
target_link_libraries(nto-contest PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  ${SQLite3_LIBRARIES})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1. If
# you are developing for iOS or macOS you should consider setting an explicit,
# fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.nto-contest)
endif()

set_target_properties(
  nto-contest
  PROPERTIES ${BUNDLE_ID_OPTION} MACOSX_BUNDLE_BUNDLE_VERSION
  ${PROJECT_VERSION} MACOSX_BUNDLE_SHORT_VERSION_STRING
  ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} MACOSX_BUNDLE
  TRUE WIN32_EXECUTABLE
  TRUE)

include(GNUInstallDirs)
install(
  TARGETS nto-contest
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

file(GLOB_RECURSE SQLITE3_LIBS ${CMAKE_SOURCE_DIR}/deps/sqlite3/lib/*)

foreach(SQLITE3_LIB IN LISTS SQLITE3_LIBS)
  file(COPY ${SQLITE3_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(nto-contest)
endif()

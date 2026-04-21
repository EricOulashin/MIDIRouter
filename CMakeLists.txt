cmake_minimum_required(VERSION 3.16)
project(MIDIRouter VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
  pkg_check_modules(RTMIDI_PC IMPORTED_TARGET rtmidi)
endif()

if(TARGET PkgConfig::RTMIDI_PC)
  set(RTMIDI_LINK_LIB PkgConfig::RTMIDI_PC)
else()
  find_library(RTMIDI_LIBRARY NAMES rtmidi librtmidi)
  find_path(RTMIDI_INCLUDE_DIR RtMidi.h PATH_SUFFIXES "" rtmidi)
  if(NOT RTMIDI_LIBRARY OR NOT RTMIDI_INCLUDE_DIR)
    message(FATAL_ERROR "RtMidi not found. Install librtmidi-dev (Linux), brew install rtmidi (macOS), or set RTMIDI_LIBRARY / RTMIDI_INCLUDE_DIR.")
  endif()
  add_library(rtmidi_imported INTERFACE)
  target_include_directories(rtmidi_imported INTERFACE ${RTMIDI_INCLUDE_DIR})
  target_link_libraries(rtmidi_imported INTERFACE ${RTMIDI_LIBRARY})
  set(RTMIDI_LINK_LIB rtmidi_imported)
endif()

set(SOURCES
    code/src/main.cpp
    code/src/MainWindow.cpp
    code/src/MainWindow.h
    code/src/MidiClientWorker.cpp
    code/src/MidiClientWorker.h
    code/src/MidiLocalWorker.cpp
    code/src/MidiLocalWorker.h
    code/src/MidiPlayoutEngine.cpp
    code/src/MidiPlayoutEngine.h
    code/src/MidiServerController.cpp
    code/src/MidiServerController.h
    code/src/NetworkMidiProtocol.h
    code/src/modes.h
)

add_executable(midirouter ${SOURCES})

target_include_directories(midirouter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/code/src)

target_link_libraries(midirouter PRIVATE
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::Network
  ${RTMIDI_LINK_LIB}
)

if(WIN32)
  set_target_properties(midirouter PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if(APPLE)
  set_target_properties(midirouter PROPERTIES MACOSX_BUNDLE TRUE)
endif()

cmake_minimum_required(VERSION 3.28)

project(ayin
        VERSION 0.1.0
        LANGUAGES CXX
        DESCRIPTION "Ayin Photo Editor")

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(FLTK CONFIG REQUIRED)

add_executable(ayin)
target_sources(ayin
  PRIVATE
    FILE_SET cxx_modules TYPE CXX_MODULES
    FILES
      ayin.cxx
)

target_link_libraries(ayin PRIVATE fltk)

if(APPLE)
    target_link_libraries(ayin PRIVATE "-framework Cocoa")
endif()

if(WIN32)
  target_link_libraries(ayin PRIVATE gdiplus)
endif()

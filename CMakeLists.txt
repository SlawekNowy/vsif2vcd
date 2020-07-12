
﻿cmake_minimum_required(VERSION 3.17)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

include(cmake/locate_vcpkg.cmake)
setup_vcpkg_before_project()
#TODO: stop if we use vs2015 or older


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
project(Vsif2Vcd VERSION 0.1.0.0
                  DESCRIPTION "Cross-platform decompiler for scenes.image file."
                  LANGUAGES CXX)
                  
				  
configure_file (
    "${PROJECT_SOURCE_DIR}/include/version.hpp.in"
    "${PROJECT_BINARY_DIR}/include/version.hpp"
)

find_package(Boost 1.70.0 REQUIRED
    COMPONENTS archive iostreams serialization tokenizer filesys crc boost
    lexical_cast

    )

set(SOURCES 
    "src/Vsif2vcd_c++.cpp"
    "src/map_bsp.cpp"
    "src/gameinfo.cpp"
    "include/VSIF.hpp"
    "include/map_bsp.hpp"
    "include/enum_bitmask.hpp"
    "include/program.h" "include/BVCD.hpp" 
    "include/gameinfo.hpp" 
    "include/thirdparty/vdf_parser.hpp" "src/VSIF.cpp"
    )
# Dodaj źródło do pliku wykonywalnego tego projektu.
add_executable (Vsif2vcd ${SOURCES})
target_precompile_headers(Vsif2vcd PUBLIC "include/pch.hpp")

set_property(TARGET Vsif2vcd PROPERTY CXX_STANDARD 11)
target_link_libraries(Vsif2vcd PUBLIC ${Boost_LIBRARIES} )
target_include_directories(Vsif2vcd PUBLIC 
     ${Boost_INCLUDE_DIR} 
    "${PROJECT_SOURCE_DIR}/include/" 
    "${PROJECT_BINARY_DIR}/include/")



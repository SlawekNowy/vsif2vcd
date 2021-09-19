cmake_minimum_required(VERSION 3.15)

include_guard(GLOBAL)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})
#find_package(Sanitizers)
#include(cmake/locate_vcpkg.cmake)
#include(cmake/asan.cmake)
#setup_vcpkg_before_project()

#TODO: stop if we use vs2015 or older

project(Vsif2Vcd VERSION 0.1.0.0
                  DESCRIPTION "Cross-platform decompiler for scenes.image file."
                  LANGUAGES CXX)
                  
configure_file (
    "${PROJECT_SOURCE_DIR}/include/version.hpp.in"
    "${PROJECT_BINARY_DIR}/include/version.hpp"
)

find_package(Boost REQUIRED
    COMPONENTS iostreams filesystem)

find_package(fmt CONFIG REQUIRED)


find_package (Bitsery CONFIG REQUIRED)







set(PRIVATE_INCLUDES

    "include/VSIF.hpp"
    "include/map_bsp.hpp"
    "include/enum_bitmask.hpp"
    "include/gameinfoKV.hpp"
    "include/program.h"
    "include/BVCD.hpp"
    "include/gameinfo.hpp"
    "include/hardcoded_entries.h"
    "include/CRC.h"
    "include/pch.hpp"
    "include/response_system.h"
    "include/split_serialization.h"
    "include/helper.hpp"
    )
set(SOURCES 
    "src/Vsif2vcd_c++.cpp"
    "src/BVCD.cpp"
    "src/map_bsp.cpp"
    "src/gameinfo.cpp"   "src/response_system.cpp"
      "src/VSIF.cpp")
# Dodaj źródło do pliku wykonywalnego tego projektu.
add_executable (Vsif2vcd ${SOURCES} ${PRIVATE_INCLUDES})
target_precompile_headers(Vsif2vcd PUBLIC "include/pch.hpp")


add_subdirectory(thirdparty/valve-bsp-parser)


add_dependencies(Vsif2vcd valve-bsp-parser lzma)
set_property(TARGET Vsif2vcd PROPERTY CXX_STANDARD 17)
target_link_libraries(Vsif2vcd PUBLIC Boost::iostreams Boost::filesystem Bitsery::bitsery lzma valve-bsp-parser)

target_link_libraries(Vsif2vcd PRIVATE fmt::fmt fmt::fmt-header-only)

find_path(BOOST_CRC_INCLUDE_DIRS "boost/crc.hpp")
target_include_directories(Vsif2vcd PRIVATE ${BOOST_CRC_INCLUDE_DIRS})

target_include_directories(Vsif2vcd PRIVATE fmt::fmt-header-only)

find_path(BOOST_LEXICAL_CAST_INCLUDE_DIRS "boost/detail/basic_pointerbuf.hpp")
target_include_directories(Vsif2vcd PRIVATE ${BOOST_LEXICAL_CAST_INCLUDE_DIRS})

find_path(BOOST_TOKENIZER_INCLUDE_DIRS "boost/token_functions.hpp")
target_include_directories(Vsif2vcd PRIVATE ${BOOST_TOKENIZER_INCLUDE_DIRS})

find_path(BOOST_ALGORITHM_INCLUDE_DIRS "boost/algorithm/algorithm.hpp")
target_include_directories(Vsif2vcd PRIVATE ${BOOST_ALGORITHM_INCLUDE_DIRS})



target_include_directories(Vsif2vcd PUBLIC 
     ${Boost_INCLUDE_DIR} 
    "${PROJECT_SOURCE_DIR}/include/"
    "${PROJECT_SOURCE_DIR}/thirdparty/vdf_parser/include"
    "${PROJECT_SOURCE_DIR}/thirdparty/liblzma/include"
    "${PROJECT_SOURCE_DIR}/thirdparty/valve-bsp-parser/include"
    "${PROJECT_BINARY_DIR}/include/"
    ${Bitsery_INCLUDE_DIR})




        #Setup CMake to run tests
enable_testing()
#Prep ourselves for compiling boost
find_package(Boost COMPONENTS unit_test_framework iostreams filesystem REQUIRED)

find_package (Bitsery CONFIG REQUIRED)
#I like to keep test files in a separate source directory called test

set(TEST_SRCS
"src/gameinfo.cpp"
"src/VSIF.cpp"
"src/map_bsp.cpp"

"src/BVCD.cpp"
)
#Run through each source

#foreach(testSrc ${TEST_SRCS})
        #Extract the filename without an extension (NAME_WE)
 #       get_filename_component(testName ${testSrc} NAME_WE)
#endforeach(testSrc)


#Add compile target
add_executable(gameinfo "src/gameinfo.cpp" ${PRIVATE_INCLUDES})
#link to Boost libraries AND your targets and dependencies
target_link_libraries(gameinfo ${Boost_LIBRARIES} Bitsery::bitsery lzma)


#I like to move testing binaries into a testBin directory

target_compile_definitions(gameinfo PRIVATE ENABLE_TESTING TESTING_GAMEINFO)
target_include_directories(gameinfo PUBLIC
    ${Boost_INCLUDE_DIR}
   "${PROJECT_SOURCE_DIR}/include/"
   "${PROJECT_SOURCE_DIR}/thirdparty/vdf_parser/include"
   "${PROJECT_SOURCE_DIR}/thirdparty/liblzma/include"
   "${PROJECT_SOURCE_DIR}/thirdparty/valve-bsp-parser/include"
   "${PROJECT_BINARY_DIR}/include/"
   ${Bitsery_INCLUDE_DIR})

set_property(TARGET gameinfo PROPERTY CXX_STANDARD 17)
set_target_properties(gameinfo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/testBin)
#Finally add it to test execution -
#Notice the WORKING_DIRECTORY and COMMAND
add_test(NAME gameinfo
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testBin
COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testBin/gameinfo)


#Add compile target
add_executable(VSIF src/VSIF.cpp ${PRIVATE_INCLUDES})
#link to Boost libraries AND your targets and dependencies
target_link_libraries(VSIF ${Boost_LIBRARIES} Bitsery::bitsery lzma)


#I like to move testing binaries into a testBin directory

target_compile_definitions(VSIF PRIVATE ENABLE_TESTING TESTING_VSIF)
target_include_directories(VSIF PUBLIC
    ${Boost_INCLUDE_DIR}
   "${PROJECT_SOURCE_DIR}/include/"
   "${PROJECT_SOURCE_DIR}/thirdparty/vdf_parser/include"
   "${PROJECT_SOURCE_DIR}/thirdparty/liblzma/include"
   "${PROJECT_SOURCE_DIR}/thirdparty/valve-bsp-parser/include"
   "${PROJECT_BINARY_DIR}/include/"
   ${Bitsery_INCLUDE_DIR})

set_property(TARGET VSIF PROPERTY CXX_STANDARD 17)
set_target_properties(VSIF PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/testBin)
#Finally add it to test execution -
#Notice the WORKING_DIRECTORY and COMMAND
add_test(NAME VSIF
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testBin
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testBin/VSIF)


     #Add compile target
     add_executable(BVCD "src/BVCD.cpp" src/VSIF.cpp ${PRIVATE_INCLUDES})
     #link to Boost libraries AND your targets and dependencies
     target_link_libraries(BVCD ${Boost_LIBRARIES} Bitsery::bitsery lzma fmt::fmt fmt::fmt-header-only)


     #I like to move testing binaries into a testBin directory

     target_include_directories(BVCD PRIVATE fmt::fmt-header-only)

     target_compile_definitions(BVCD PRIVATE ENABLE_TESTING TESTING_BVCD)
     target_include_directories(BVCD PUBLIC
         ${Boost_INCLUDE_DIR}
        "${PROJECT_SOURCE_DIR}/include/"
        "${PROJECT_SOURCE_DIR}/thirdparty/vdf_parser/include"
        "${PROJECT_SOURCE_DIR}/thirdparty/liblzma/include"
        "${PROJECT_SOURCE_DIR}/thirdparty/valve-bsp-parser/include"
        "${PROJECT_BINARY_DIR}/include/"
        ${Bitsery_INCLUDE_DIR})


     set_property(TARGET BVCD PROPERTY CXX_STANDARD 17)
     set_target_properties(BVCD PROPERTIES
         RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/testBin)
     #Finally add it to test execution -
     #Notice the WORKING_DIRECTORY and COMMAND
     add_test(NAME BVCD WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testBin COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testBin/BVCD)#Add compile target
     add_executable(map_bsp "src/map_bsp.cpp" "src/response_system.cpp" ${PRIVATE_INCLUDES})
     #link to Boost libraries AND your targets and dependencies

     target_link_libraries(map_bsp ${Boost_LIBRARIES} Bitsery::bitsery fmt::fmt fmt::fmt-header-only valve-bsp-parser)


     #I like to move testing binaries into a testBin directory

     target_include_directories(map_bsp PRIVATE fmt::fmt-header-only)

     target_compile_definitions(map_bsp PRIVATE ENABLE_TESTING TESTING_BVCD)
     target_include_directories(map_bsp PUBLIC
         ${Boost_INCLUDE_DIR}
        "${PROJECT_SOURCE_DIR}/include/"
        "${PROJECT_SOURCE_DIR}/thirdparty/vdf_parser/include"
        "${PROJECT_SOURCE_DIR}/thirdparty/liblzma/include"
        "${PROJECT_SOURCE_DIR}/thirdparty/valve-bsp-parser/include"
        "${PROJECT_BINARY_DIR}/include/"
        ${Bitsery_INCLUDE_DIR})


     set_property(TARGET map_bsp PROPERTY CXX_STANDARD 17)
     set_target_properties(map_bsp PROPERTIES
         RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/testBin)
     #Finally add it to test execution -
     #Notice the WORKING_DIRECTORY and COMMAND
     add_test(NAME map_bsp WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testBin COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testBin/BVCD)


cmake_minimum_required(VERSION 3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()


include(cmake/locate_vcpkg.cmake)
setup_vcpkg_before_project()

#TODO: stop if we use vs2015 or older

project(Vsif2Vcd VERSION 0.1.0.0
                  DESCRIPTION "Cross-platform decompiler for scenes.image file."
                  LANGUAGES CXX)
                  
configure_file (
    "${PROJECT_SOURCE_DIR}/include/version.hpp.in"
    "${PROJECT_BINARY_DIR}/include/version.hpp"
)

find_package(boost 1.73.0 REQUIRED
    COMPONENTS iostreams filesystem )

find_package (Bitsery CONFIG REQUIRED)





set(SOURCES 
    "src/Vsif2vcd_c++.cpp"
    "src/BVCD.cpp"
    "src/map_bsp.cpp"
    "src/gameinfo.cpp"
    "include/VSIF.hpp"
    "include/map_bsp.hpp"
    "include/enum_bitmask.hpp"
    "include/gameinfoKV.hpp"
    "include/program.h" "include/BVCD.hpp" 
    "include/gameinfo.hpp" 
    "include/hardcoded_entries.h" 
    "include/CRC.h"
     
      "src/VSIF.cpp" "include/split_serialization.h" "include/helper.hpp")
# Dodaj źródło do pliku wykonywalnego tego projektu.
add_executable (Vsif2vcd ${SOURCES})
target_precompile_headers(Vsif2vcd PUBLIC "include/pch.hpp")

set_property(TARGET Vsif2vcd PROPERTY CXX_STANDARD 17)
target_link_libraries(Vsif2vcd PUBLIC boost::iostreams boost::filesystem Bitsery::bitsery)

find_path(BOOST_CRC_INCLUDE_DIRS "boost/crc.hpp")
target_include_directories(Vsif2vcd PRIVATE ${BOOST_CRC_INCLUDE_DIRS})

find_path(BOOST_LEXICAL_CAST_INCLUDE_DIRS "boost/detail/basic_pointerbuf.hpp")
target_include_directories(Vsif2vcd PRIVATE ${BOOST_LEXICAL_CAST_INCLUDE_DIRS})

find_path(BOOST_TOKENIZER_INCLUDE_DIRS "boost/token_functions.hpp")
target_include_directories(Vsif2vcd PRIVATE ${BOOST_TOKENIZER_INCLUDE_DIRS})
target_include_directories(Vsif2vcd PUBLIC 
     ${Boost_INCLUDE_DIR} 
    "${PROJECT_SOURCE_DIR}/include/" 
    "${PROJECT_BINARY_DIR}/include/"
    ${Bitsery_INCLUDE_DIR})




        #Setup CMake to run tests
enable_testing()
#Prep ourselves for compiling boost
find_package(boost COMPONENTS unit_test_framework iostreams filesystem REQUIRED)

find_package (Bitsery CONFIG REQUIRED)
#I like to keep test files in a separate source directory called test

set(TEST_SRCS
"src/gameinfo.cpp"
"src/VSIF.cpp"
"src/map_bsp.cpp"

"src/BVCD.cpp"
)
#Run through each source
foreach(testSrc ${TEST_SRCS})
        #Extract the filename without an extension (NAME_WE)
        get_filename_component(testName ${testSrc} NAME_WE)
        #Add compile target
        add_executable(${testName} ${testSrc})
        #link to Boost libraries AND your targets and dependencies
        target_link_libraries(${testName} ${Boost_LIBRARIES} Bitsery::bitsery )
        #I like to move testing binaries into a testBin directory

        target_compile_definitions(${testName} PRIVATE ENABLE_TESTING)
        target_include_directories(${testName} PUBLIC 
     ${Boost_INCLUDE_DIR} 
    "${PROJECT_SOURCE_DIR}/include/" 
    "${PROJECT_BINARY_DIR}/include/"
    ${Bitsery_INCLUDE_DIR})

    set_property(TARGET ${testName} PROPERTY CXX_STANDARD 17)
        set_target_properties(${testName} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/testBin)
        #Finally add it to test execution -
        #Notice the WORKING_DIRECTORY and COMMAND
        add_test(NAME ${testName}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testBin
                 COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testBin/${testName} )
endforeach(testSrc)


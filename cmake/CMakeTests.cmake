
        #Setup CMake to run tests
enable_testing()
#Prep ourselves for compiling boost
find_package(Boost COMPONENTS unit_test_framework iostreams REQUIRED)

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
add_executable(gameinfo "src/gameinfo.cpp"
    src/filesystem_internal/cvpkmountpath.h src/filesystem_internal/cvpkinfile.h src/filesystem_internal/cvpkinfile.cpp src/filesystem_internal/cvpkmountpath.cpp
    src/filesystem_internal/cloosemountpath.h src/filesystem_internal/cloosemountpath.cpp src/filesystem_internal/cloosefile.h src/filesystem_internal/cloosefile.cpp
    src/filesystem_internal/imountpath_internal.cpp ${PRIVATE_INCLUDES})
#link to Boost libraries AND your targets and dependencies
target_link_libraries(gameinfo ${Boost_LIBRARIES} Bitsery::bitsery lzma hllib spdlog::spdlog spdlog::spdlog_header_only)


#I like to move testing binaries into a testBin directory

target_compile_definitions(gameinfo PRIVATE ENABLE_TESTING TESTING_GAMEINFO)
target_include_directories(gameinfo PUBLIC
    ${Boost_INCLUDE_DIR}
   "${PROJECT_SOURCE_DIR}/include/"
   "${PROJECT_SOURCE_DIR}/thirdparty/vdf_parser/include"
   "${PROJECT_SOURCE_DIR}/thirdparty/liblzma/include"
   "${PROJECT_SOURCE_DIR}/thirdparty/valve-bsp-parser/include"
   "${PROJECT_SOURCE_DIR}/thirdparty/hllib/HLLib"
   "${PROJECT_BINARY_DIR}/include/"
   ${Bitsery_INCLUDE_DIR})


set_property(TARGET gameinfo PROPERTY CXX_STANDARD 17)
set_target_properties(gameinfo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/testBin)
    set_target_properties(gameinfo PROPERTIES EXCLUDE_FROM_ALL TRUE)
#Finally add it to test execution -
#Notice the WORKING_DIRECTORY and COMMAND
add_test(NAME gameinfo
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testBin
COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testBin/gameinfo)


#Add compile target
add_executable(VSIF src/VSIF.cpp ${PRIVATE_INCLUDES})
#link to Boost libraries AND your targets and dependencies
target_link_libraries(VSIF ${Boost_LIBRARIES} Bitsery::bitsery lzma spdlog::spdlog spdlog::spdlog_header_only)

    set_target_properties(VSIF PROPERTIES EXCLUDE_FROM_ALL TRUE)

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
     target_link_libraries(BVCD ${Boost_LIBRARIES} Bitsery::bitsery lzma fmt::fmt fmt::fmt-header-only spdlog::spdlog spdlog::spdlog_header_only)


     #I like to move testing binaries into a testBin directory
     
    set_target_properties(BVCD PROPERTIES EXCLUDE_FROM_ALL TRUE)
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

     target_link_libraries(map_bsp ${Boost_LIBRARIES} Bitsery::bitsery fmt::fmt fmt::fmt-header-only valve-bsp-parser spdlog::spdlog spdlog::spdlog_header_only)


     #I like to move testing binaries into a testBin directory

     target_include_directories(map_bsp PRIVATE fmt::fmt-header-only)
     
    set_target_properties(map_bsp PROPERTIES EXCLUDE_FROM_ALL TRUE)
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


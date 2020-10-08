#Manually adding vcpkg to CMake environment.
#Credit to Qt Company for this script

function(setup_vcpkg_before_project)
        if(DEFINED ENV{VCPKG_ROOT})
                set(vcpkg_toolchain_path "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
		set(CMAKE_TOOLCHAIN_FILE ${vcpkg_toolchain_path} CACHE STRING "" FORCE)
	endif()



endfunction()

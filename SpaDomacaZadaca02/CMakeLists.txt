cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# -DCMAKE_POLICY_VERSION_MINIMUM=3.5
# System dependant, if SFML is having issues.

project(rvs19-spa-dz-02 LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(default_build_type "Debug")
	message(STATUS "Set the build type to `${default_build_type}` as none was specified.")
	set(CMAKE_BUILD_TYPE ${default_build_type} CACHE STRING "Chooce the build type." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
	"Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

# Fetch and configure SFML version 2.5.1
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.5.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

file(GLOB PROJECT_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

file(COPY ${CMAKE_SOURCE_DIR}/../resources DESTINATION ${CMAKE_BINARY_DIR})

# --- Main executable ---
add_executable(main Source.cpp ${PROJECT_SOURCES})

target_include_directories(main PRIVATE src)

target_link_libraries(
	main
	PRIVATE
	sfml-system
	sfml-window
	sfml-graphics
)

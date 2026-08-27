# PCANNON CMAKELISTS.TXT v1.3 - FROM PCANNON PROJECT STANDARDS
# STANDARD: 20260216
# https://github.com/pcannon09/pcannonProjectStandards

cmake_minimum_required(VERSION 3.24)

# Read project metadata

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/.private/project.json" projectInfo)

string(JSON projectName	GET "${projectInfo}" exeName)
string(JSON projectVersion GET "${projectInfo}" version)
string(JSON projectPrefix GET "${projectInfo}" projectPrefix)
string(JSON projectVersionState GET "${projectInfo}" versionState)
string(JSON projectVersionSTD GET "${projectInfo}" versionSTD)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

project(${projectName}
	VERSION ${projectVersion}
	LANGUAGES CXX
)

# C++ Standard

set(CXX_STD 17)
set(CMAKE_CXX_STANDARD ${CXX_STD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_COLOR_MAKEFILE ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_CROSSCOMPILING)
	set(WINDOWS_PLATFORM YES)
else()
	set(WINDOWS_PLATFORM NO)
endif()

message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")

# Directories

set(SRC_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(VENDOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor")
set(INC_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/inc/${projectName}")
set(INC_ROOT_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/inc")

# Sources

set(HEADERS
	${INC_DIR}/RAMPpredefines.h
	${INC_DIR}/RAMPconfig.h

	${INC_DIR}/lib/RAMP.hpp
)

set(SOURCES
		${SRC_DIR}/lib/RAMP.cpp
)

# Library Targets

add_library(${projectName} STATIC ${SOURCES} ${HEADERS})
add_library(${projectName}::${projectName} ALIAS ${projectName})

target_include_directories(${projectName}
	PRIVATE ${VENDOR_INC_PATHS}

	PUBLIC
		$<BUILD_INTERFACE:${INC_ROOT_DIR}>
)

target_compile_features(${projectName} PUBLIC cxx_std_${CXX_STD})

# Compiler Warnings

if(${WINDOWS_PLATFORM})
	target_compile_options(${projectName} PRIVATE)
	target_link_libraries(${projectName} bcrypt)
else()
	target_compile_options(${projectName} PRIVATE
		-Wall -Wextra -Wpedantic
	)
endif()

# Debug / Release handling

target_compile_definitions(${projectName}
	PUBLIC
		$<$<CONFIG:Debug>:${projectPrefix}_DEV=1>
		$<$<CONFIG:Release>:${projectPrefix}_DEV=0>
)

add_executable(${projectName}_exec ${SRC_DIR}/main.cpp)
target_link_libraries(${projectName}_exec PRIVATE
		${projectName}
)

if(NOT ${WINDOWS_PLATFORM})
	target_compile_options(${projectName} PRIVATE
		$<$<CONFIG:Debug>:-g -fno-omit-frame-pointer>
		$<$<CONFIG:Release>:-O3>
	)

	target_link_options(${projectName} PRIVATE
		$<$<CONFIG:Debug>:-g -fno-omit-frame-pointer>
	)

	target_compile_options(${projectName}_exec PRIVATE
		$<$<CONFIG:Debug>:-g -fno-omit-frame-pointer>
		$<$<CONFIG:Release>:-O3>
	)

	target_link_options(${projectName}_exec PRIVATE
		$<$<CONFIG:Debug>:-g>
	)
endif()

# Optional sanitizers (only for Debug and only on supported compilers)
option(ENABLE_SANITIZER "Enable address sanitizer in Debug builds" ON)

if(ENABLE_SANITIZER AND NOT ${WINDOWS_PLATFORM})
	target_compile_options(${projectName} PRIVATE
		$<$<CONFIG:Debug>:-fsanitize=address,leak>
	)

	target_link_options(${projectName} PRIVATE
		$<$<CONFIG:Debug>:-fsanitize=address,leak>
	)

	target_compile_options(${projectName}_exec PRIVATE
		$<$<CONFIG:Debug>:-fsanitize=address,leak>
	)

	target_link_options(${projectName}_exec PRIVATE
		$<$<CONFIG:Debug>:-fsanitize=address,leak>
	)
endif()

# Executables

set(VENDOR_INC_PATHS
)

include_directories(${VENDOR_INC_PATHS})

set_target_properties(
	${projectName}
	PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# CPack

set(CPACK_PACKAGE_NAME ${projectName})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "pcannon")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "pcannon project standards TEMPLATE")

include(CPack)

# Build Info

message(STATUS "Project:		${projectName}")
message(STATUS "Version:		${PROJECT_VERSION}-${projectVersionState}")
message(STATUS "C Standard:		C${C_STD}")
message(STATUS "Compiler:		${CMAKE_C_COMPILER}")


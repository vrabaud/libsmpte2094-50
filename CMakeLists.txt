# Copyright 2026 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

cmake_minimum_required(VERSION 3.22)

project(libsmpte2094-50 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SMPTE2094_ENABLE_TESTS "Enable tests." OFF)

set(RUST_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/rust_lib")

# Define the main library.
add_library(smpte2094_50 INTERFACE include/smpte2094_50/smpte2094_50.h)
target_include_directories(smpte2094_50 INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(smpte2094_50
                      INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/librust_lib.a)

include(FetchContent)

# Install dependencies.
FetchContent_Declare(
  abseil-cpp
        EXCLUDE_FROM_ALL
  GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
  GIT_TAG        20260107.1
        GIT_SHALLOW ON
)
FetchContent_MakeAvailable(abseil-cpp)

FetchContent_Declare(  
        Corrosion
        EXCLUDE_FROM_ALL
        GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
        GIT_TAG v0.6.1
        GIT_SHALLOW ON
)
FetchContent_MakeAvailable(Corrosion)

# Install Cargo C.
find_program(CARGO_CINSTALL cargo-cinstall HINTS "$ENV{HOME}/.cargo/bin" HINTS ${CMAKE_BINARY_DIR})

if(CARGO_CINSTALL)
    add_executable(cargo-cinstall IMPORTED GLOBAL)
    set_property(TARGET cargo-cinstall PROPERTY IMPORTED_LOCATION ${CARGO_CINSTALL})
endif()

if(NOT TARGET cargo-cinstall)
    FetchContent_Declare(
        cargoc
        EXCLUDE_FROM_ALL
        GIT_REPOSITORY https://github.com/lu-zero/cargo-c.git
        GIT_TAG "${AVIF_CARGOC_GIT_TAG}"
        GIT_SHALLOW ON
    )
    FetchContent_MakeAvailable(cargoc)

    corrosion_import_crate(
        MANIFEST_PATH ${cargoc_SOURCE_DIR}/Cargo.toml PROFILE release IMPORTED_CRATES MYVAR_IMPORTED_CRATES FEATURES
        vendored-openssl
    )

    set(CARGO_CINSTALL $<TARGET_FILE:cargo-cinstall>)
endif()

set(SMPTE2094_50_RS_ENVVARS)
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin" AND CMAKE_OSX_SYSROOT)
    list(APPEND SMPTE2094_50_RS_ENVVARS "SDKROOT=${CMAKE_OSX_SYSROOT}")
endif()
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin" AND CMAKE_OSX_DEPLOYMENT_TARGET)
    list(APPEND SMPTE2094_50_RS_ENVVARS "MACOSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}")
endif()
    
add_custom_target(
    utils_cargo_c
    COMMAND ${CMAKE_COMMAND} -E env ${SMPTE2094_50_RS_ENVVARS} ${CARGO_CINSTALL} cinstall -v --release --library-type=staticlib
            --prefix=/usr --target ${Rust_CARGO_TARGET_CACHED} --destdir ${CMAKE_CURRENT_BINARY_DIR}/smpte2094-50_rs
    DEPENDS cargo-cinstall
    BYPRODUCTS ${RAV1E_LIB_FILENAME}
    USES_TERMINAL
    WORKING_DIRECTORY ${rav1e_SOURCE_DIR}
)

set(SMPTE2094_50_RS_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/smpte2094-50_rs/include/smpte2094-50")
file(MAKE_DIRECTORY ${SMPTE2094_50_RS_INCLUDE_DIR})
add_library(smpte2094-50_rs STATIC IMPORTED)
add_dependencies(smpte2094-50_rs utils_cargo_c)
target_link_libraries(smpte2094-50_rs INTERFACE "${Rust_CARGO_TARGET_LINK_NATIVE_LIBS}")
target_link_options(smpte2094-50_rs INTERFACE "${Rust_CARGO_TARGET_LINK_OPTIONS}")
set_target_properties(smpte2094-50_rs PROPERTIES IMPORTED_LOCATION ${SMPTE2094_50_RS_INCLUDE_DIR} FOLDER smpte2094-50_rs
)
target_include_directories(smpte2094-50_rs INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/smpte2094-50_rs/usr/include/smpte2094-50)

# gloop is assumed to be available in the include path or as a package.
# find_package(gloop REQUIRED)

# Compile the Rust Library.
corrosion_import_crate(MANIFEST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/rust_lib/Cargo.toml)

# -----------------------------------------------------------------------------
# Library Definition
# -----------------------------------------------------------------------------
add_library(smpte2094-50_utils
    src/utils.cc
    include/smpte2094_50/utils.h
)

target_link_libraries(smpte2094-50_utils PUBLIC
    absl::status
    absl::statusor
    absl::strings
    absl::span
    smpte2094-50
    smpte2094-50_rs # If using Corrosion, link the Cargo imported target
)
target_include_directories(smpte2094-50_utils PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)


if(SMPTE2094_ENABLE_TESTS)
  enable_testing() # Allow ctest to be called from top-level directory.
  add_subdirectory(tests)
endif()


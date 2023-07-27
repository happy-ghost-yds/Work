cmake_minimum_required (VERSION 2.6)

#------------------------------------------------------------------------------
# module

if (MDP_LN_QUOTER)
  set (MODULE_NAME mdp_api_ln)
else ()
  set (MODULE_NAME mdp_api_l1)
endif()

#------------------------------------------------------------------------------
# config

if (WIN32)
  add_definitions(-DMDP_EXPORTS)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
else ()
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g3 -O0  -m64 -Wall -std=c++11")
endif ()

#------------------------------------------------------------------------------
# include

include_directories(${PROJECT_SOURCE_DIR}/3rd_party/spdlog/include)
include_directories(${PROJECT_SOURCE_DIR}/3rd_party/json)
if (WIN32)
  include_directories(${PROJECT_SOURCE_DIR}/3rd_party/openssl/x64/include)
endif ()
include_directories(${PROJECT_SOURCE_DIR}/3rd_party/rang/include)
include_directories(${PROJECT_SOURCE_DIR}/3rd_party/libuv/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

#------------------------------------------------------------------------------
# src

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ SRCS)

#------------------------------------------------------------------------------
# lib

link_directories(${PROJECT_SOURCE_DIR}/lib)
if (WIN32)
  link_directories(${PROJECT_SOURCE_DIR}/3rd_party/openssl/x64/lib)
endif ()

#------------------------------------------------------------------------------
# output

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(ARCHIVE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
add_library(${MODULE_NAME} SHARED ${SRCS})

#------------------------------------------------------------------------------
# link

if (WIN32)
  target_link_libraries (
    ${MODULE_NAME}
    uv_a
    advapi32
    iphlpapi
    psapi
    libssl
    libcrypto
    shell32
    user32
    userenv
    ws2_32
  )
else ()
  target_link_libraries (
    ${MODULE_NAME}
    "-Wl,--whole-archive"
    uv_a
    pthread
    dl
    rt
    crypto
    "-Wl,--no-whole-archive"
  )
endif ()

#------------------------------------------------------------------------------
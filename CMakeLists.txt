cmake_minimum_required(VERSION 3.5)

project(bce3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

aux_source_directory(src SRC_LIST)

include_directories(src)

add_executable(${PROJECT_NAME} ${SRC_LIST})

# Boost
#find_package(Boost REQUIRED)
#include_directories(${Boost_INCLUDE_DIR})
#target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARY})

# libbitcoin
find_package(PkgConfig REQUIRED)
pkg_check_modules(libbitcoin REQUIRED IMPORTED_TARGET libbitcoin)
target_link_libraries(${PROJECT_NAME} PkgConfig::libbitcoin)

## kyotocabinet
find_package(PkgConfig REQUIRED)
pkg_check_modules(kyotocabinet REQUIRED IMPORTED_TARGET kyotocabinet)
target_link_libraries(${PROJECT_NAME} PkgConfig::kyotocabinet)

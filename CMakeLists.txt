cmake_minimum_required(VERSION 3.16)
project(KDTree)

include(FetchContent)
include(cmake/Doxygen.cmake)

# defining options which can be enabled during building
option(KDTREE_ENABLE_DOXYGEN "Enable doxygen" ON)
option(KDTREE_ENABLE_TESTS "Enable tests" ON)

# enables testing
if(KDTREE_ENABLE_TESTS)
    find_package(Catch2 REQUIRED)
    include(Catch)
    include(CTest)
    add_subdirectory(tests)
    enable_testing()
endif()

# generates Doxygen documentation
if(KDTREE_ENABLE_DOXYGEN)
  KDTree_enable_doxygen("awesome-sidebar")
endif()

add_executable(target main.cxx)
target_include_directories(target PUBLIC "${CMAKE_SOURCE_DIR}/include")
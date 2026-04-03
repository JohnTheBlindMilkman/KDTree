cmake_minimum_required(VERSION 3.16)
project(KDTree)

# defining options which can be enabled during building
option(KDTREE_ENABLE_DOXYGEN "Enable doxygen" OFF)
option(KDTREE_ENABLE_TESTS "Enable tests" ON)
option(KDTREE_ENABLE_ASAN "Enable address sanitiser during testing" OFF)

# enables testing
if(KDTREE_ENABLE_TESTS)
    find_package(Catch2 3 REQUIRED)
    include(Catch)
    include(CTest)
    add_subdirectory(tests)
    enable_testing()
endif()

# generates Doxygen documentation
if(KDTREE_ENABLE_DOXYGEN)
    include(FetchContent)
    include(cmake/Doxygen.cmake)
    KDTree_enable_doxygen("awesome-sidebar")
endif()

# add some extra compile flags
add_compile_options(-Wall -Wextra -Wformat -Wformat=2 -Wconversion -Wsign-conversion -Wtrampolines -Wimplicit-fallthrough -Wbidi-chars=any -Werror -Werror=format-security)

add_executable(target main.cxx)
target_include_directories(target PUBLIC "${CMAKE_SOURCE_DIR}/include")
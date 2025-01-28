if(KDTREE_ENABLE_TESTS)

    add_executable(testPoint testPoint.cxx)
    target_link_libraries(testPoint PRIVATE Catch2::Catch2WithMain)
    target_include_directories(testPoint PRIVATE "${CMAKE_SOURCE_DIR}/include")
    catch_discover_tests(testPoint)

    add_executable(testDist testDistance.cxx)
    target_link_libraries(testDist PRIVATE Catch2::Catch2WithMain)
    target_include_directories(testDist PRIVATE "${CMAKE_SOURCE_DIR}/include")
    catch_discover_tests(testDist)

    add_executable(testNode testNode.cxx)
    target_link_libraries(testNode PRIVATE Catch2::Catch2WithMain)
    target_include_directories(testNode PRIVATE "${CMAKE_SOURCE_DIR}/include")
    catch_discover_tests(testNode)

    add_executable(testActions testActions.cxx)
    target_link_libraries(testActions PRIVATE Catch2::Catch2WithMain)
    target_include_directories(testActions PRIVATE "${CMAKE_SOURCE_DIR}/include")
    catch_discover_tests(testActions)

    add_executable(testTree testTree.cxx)
    target_link_libraries(testTree PRIVATE Catch2::Catch2WithMain)
    target_include_directories(testTree PRIVATE "${CMAKE_SOURCE_DIR}/include")
    catch_discover_tests(testTree)

endif()
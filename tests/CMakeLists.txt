find_package(PkgConfig)
pkg_search_module(GTEST REQUIRED gtest_main)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_subdirectory(heaps)
#add_subdirectory(trees)

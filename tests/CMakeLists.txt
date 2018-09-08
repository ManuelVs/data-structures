find_package(PkgConfig)
pkg_search_module(GTEST REQUIRED gtest_main)

add_subdirectory(heaps)
add_subdirectory(trees)

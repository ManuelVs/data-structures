# ADD EXECUTABLES
add_executable(AVLTreeTest "AVLTreeTest.cpp")
add_executable(RedBlackTreeTest "RedBlackTree.cpp")

# LINK LIBRARIES
target_link_libraries(AVLTreeTest DATA_STRUCTURES)
target_link_libraries(AVLTreeTest ${GTEST_LDFLAGS})
target_compile_options(AVLTreeTest PUBLIC ${GTEST_CFLAGS})

target_link_libraries(RedBlackTreeTest DATA_STRUCTURES)
target_link_libraries(RedBlackTreeTest ${GTEST_LDFLAGS})
target_compile_options(RedBlackTreeTest PUBLIC ${GTEST_CFLAGS})

# ADD TESTS
add_test(NAME AVLTreeTest
         COMMAND AVLTreeTest)

add_test(NAME RedBlackTreeTest
         COMMAND RedBlackTreeTest)

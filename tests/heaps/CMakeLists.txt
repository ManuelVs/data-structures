# ADD EXECUTABLES
add_executable(BinomialHeapTest "BinomialHeapTest.cpp")
add_executable(FibonacciHeapTest "FibonacciHeapTest.cpp")
add_executable(STLHeapTest "STLHeapTest.cpp")
add_executable(WilliamHeapTest "WilliamHeapTest.cpp")

# LINK LIBRARIES
target_link_libraries(BinomialHeapTest DATA_STRUCTURES)
target_link_libraries(BinomialHeapTest ${GTEST_LDFLAGS})
target_compile_options(BinomialHeapTest PUBLIC ${GTEST_CFLAGS})

target_link_libraries(FibonacciHeapTest DATA_STRUCTURES)
target_link_libraries(FibonacciHeapTest ${GTEST_LDFLAGS})
target_compile_options(FibonacciHeapTest PUBLIC ${GTEST_CFLAGS})

target_link_libraries(STLHeapTest DATA_STRUCTURES)
target_link_libraries(STLHeapTest ${GTEST_LDFLAGS})
target_compile_options(STLHeapTest PUBLIC ${GTEST_CFLAGS})

target_link_libraries(WilliamHeapTest DATA_STRUCTURES)
target_link_libraries(WilliamHeapTest ${GTEST_LDFLAGS})
target_compile_options(WilliamHeapTest PUBLIC ${GTEST_CFLAGS})

# ADD TESTS
add_test(NAME BinomialHeapTest
         COMMAND BinomialHeapTest)

add_test(NAME FibonacciHeapTest
         COMMAND FibonacciHeapTest)

add_test(NAME STLHeapTest
         COMMAND STLHeapTest)

add_test(NAME WilliamHeapTest
         COMMAND WilliamHeapTest)

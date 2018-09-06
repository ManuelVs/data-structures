# ADD EXECUTABLES
ADD_EXECUTABLE(BinomialHeapTest "BinomialHeapTest.cpp")
ADD_EXECUTABLE(FibonacciHeapTest "FibonacciHeapTest.cpp")
ADD_EXECUTABLE(STLHeapTest "STLHeapTest.cpp")
ADD_EXECUTABLE(WilliamHeapTest "WilliamHeapTest.cpp")

# LINK LIBRARIES
target_link_libraries(BinomialHeapTest INCLUDE_HEAPS)
target_link_libraries(BinomialHeapTest ${GTEST_LDFLAGS})
target_compile_options(BinomialHeapTest PUBLIC ${GTEST_CFLAGS})

target_link_libraries(FibonacciHeapTest INCLUDE_HEAPS)
target_link_libraries(FibonacciHeapTest ${GTEST_LDFLAGS})
target_compile_options(FibonacciHeapTest PUBLIC ${GTEST_CFLAGS})

target_link_libraries(STLHeapTest INCLUDE_HEAPS)
target_link_libraries(STLHeapTest ${GTEST_LDFLAGS})
target_compile_options(STLHeapTest PUBLIC ${GTEST_CFLAGS})

target_link_libraries(WilliamHeapTest INCLUDE_HEAPS)
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

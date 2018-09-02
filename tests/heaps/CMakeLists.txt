ADD_EXECUTABLE(BinomialHeapTest "BinomialHeapTest.cpp")
ADD_EXECUTABLE(STLHeapTest "STLHeapTest.cpp")
ADD_EXECUTABLE(WilliamHeapTest "WilliamHeapTest.cpp")

TARGET_LINK_LIBRARIES(
    BinomialHeapTest
    INCLUDE_HEAPS
    libgtest
    libgmock
)
TARGET_LINK_LIBRARIES(
    STLHeapTest
    INCLUDE_HEAPS
    libgtest
    libgmock
)
TARGET_LINK_LIBRARIES(
    WilliamHeapTest
    INCLUDE_HEAPS
    libgtest
    libgmock
)


add_test(NAME BinomialHeapTest
         COMMAND BinomialHeapTest)

add_test(NAME STLHeapTest
         COMMAND STLHeapTest)

add_test(NAME WilliamHeapTest
         COMMAND WilliamHeapTest)

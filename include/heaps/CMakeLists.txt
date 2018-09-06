set(INCLUDE_HEAPS_SRCS
    BinomialHeap.hpp
    FibonacciHeap.hpp
    WilliamHeap.hpp
)

# Declare the library
add_library(INCLUDE_HEAPS STATIC
    ${INCLUDE_HEAPS_SRCS}
)

# Specify here the include directories exported by this library
target_include_directories(INCLUDE_HEAPS PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(INCLUDE_HEAPS PROPERTIES LINKER_LANGUAGE CXX)
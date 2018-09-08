add_subdirectory(heaps)
#add_subdirectory(simple)
add_subdirectory(trees)

# Declare the library
add_library(DATA_STRUCTURES SHARED
    ${INCLUDE_HEAPS_SRCS}
    ${INCLUDE_TREES_SRCS}
)

# Specify here the include directories exported by this library
target_include_directories(DATA_STRUCTURES
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(DATA_STRUCTURES PROPERTIES LINKER_LANGUAGE CXX)

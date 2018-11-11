cmake_minimum_required(VERSION 3.10)

project( PrimitivePolynomials  CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
 -O2 -mfpmath=sse -msse2 -msse3 -msse4.2 -ffast-math \
 -mfma -mfma4 -mavx \
 -fvariable-expansion-in-unroller -Wall"
)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS}" )
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS}" )

file( GLOB HEADERS ${PROJECT_SOURCE_DIR}
    *.h
)

# no effect
source_group("HEADERS" FILES ${HEADERS})

file( GLOB SOURCES ${PROJECT_SOURCE_DIR}
    *.cpp
)

file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/FactorTables/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )

# no effect
source_group("SOURCES" FILES ${SOURCES})

include_directories(
	./
)

# set(CMAKE_BUILD_TYPE RELEASE CACHE STRING "" FORCE)

add_executable( ${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

# target_link_libraries( ${PROJECT_NAME}
# )


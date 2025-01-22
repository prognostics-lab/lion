file(GLOB MATH_ROOT_SOURCE *.c)
file(GLOB MATH_ROOT_HEADER *.h)
file(GLOB MATH_DYN_SOURCE dynamics/*.c)
file(GLOB MATH_DYN_HEADER dynamics/*.h)

add_library(${PROJECT_MATH_NAME} ${MATH_ROOT_HEADER} ${MATH_ROOT_SOURCE}
                                 ${MATH_DYN_HEADER} ${MATH_DYN_SOURCE})
find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
message(STATUS "Found math library")
target_link_libraries(${PROJECT_MATH_NAME} PUBLIC ${PROJECT_UTILS_NAME}
                      ${GSL_LIBRARIES} ${MATH_LIBRARY})
else()
message(STATUS "Did not find math library")
target_link_libraries(${PROJECT_MATH_NAME} PUBLIC ${PROJECT_UTILS_NAME}
    ${GSL_LIBRARIES})
endif()
target_include_directories(
  ${PROJECT_MATH_NAME}
  PRIVATE ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}
         ${PROJECT_SOURCE_DIR_LOCATION} ${PROJECT_HEADERS} ${CMAKE_SOURCE_DIR}
         ${GSL_INCLUDE_DIRS})

set_target_properties(${PROJECT_MATH_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

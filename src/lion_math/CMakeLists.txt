file(GLOB MATH_ROOT_SOURCE *.c)
file(GLOB MATH_ROOT_HEADER *.h)
file(GLOB MATH_DYN_SOURCE dynamics/*.c)
file(GLOB MATH_DYN_HEADER dynamics/*.h)

add_library(${PROJECT_MATH_NAME} ${MATH_ROOT_HEADER} ${MATH_ROOT_SOURCE}
                                 ${MATH_DYN_HEADER} ${MATH_DYN_SOURCE})
find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
target_link_libraries(${PROJECT_MATH_NAME} PUBLIC ${PROJECT_UTILS_NAME}
                      ${GSL_LIBRARIES} ${MATH_LIBRARY})
else()
target_link_libraries(${PROJECT_MATH_NAME} PUBLIC ${PROJECT_UTILS_NAME}
    ${GSL_LIBRARIES})
endif()
target_include_directories(
  ${PROJECT_MATH_NAME}
  PUBLIC ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}
         ${PROJECT_SOURCE_DIR_LOCATION} ${PROJECT_HEADERS} ${CMAKE_SOURCE_DIR}
         ${GSL_INCLUDE_DIRS})


file(GLOB MATH_ROOT_SOURCE *.c)
file(GLOB MATH_ROOT_HEADER *.h)

add_library(${PROJECT_MATH_NAME} ${MATH_ROOT_HEADER} ${MATH_ROOT_SOURCE})

target_link_libraries(${PROJECT_MATH_NAME} PUBLIC ${PROJECT_UTILS_NAME}
                                                  ${GSL_LIBRARIES})
target_include_directories(
  ${PROJECT_MATH_NAME}
  PRIVATE ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}
          ${PROJECT_SOURCE_DIR_LOCATION} ${PROJECT_HEADERS} ${CMAKE_SOURCE_DIR}
          ${GSL_INCLUDE_DIRS})

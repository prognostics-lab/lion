file(GLOB APP_ROOT_SOURCE *.c)
file(GLOB APP_ROOT_HEADER *.h)

add_library(${PROJECT_APP_NAME} ${APP_ROOT_HEADER} ${APP_ROOT_SOURCE})

target_link_libraries(${PROJECT_APP_NAME} PUBLIC ${PROJECT_UTILS_NAME}
                                                 ${GSL_LIBRARIES})
target_include_directories(
  ${PROJECT_APP_NAME}
  PRIVATE ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}
          ${PROJECT_SOURCE_DIR_LOCATION} ${PROJECT_HEADERS} ${CMAKE_SOURCE_DIR}
          ${GSL_INCLUDE_DIRS})

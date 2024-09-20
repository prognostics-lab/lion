file(GLOB CPP_ROOT_SOURCE *.cpp)
file(GLOB CPP_ROOT_HEADER *.hpp)
file(GLOB CPP_SLVR_SOURCE solver/*.cpp)
file(GLOB CPP_SLVR_HEADER solver/*.hpp)

add_library(${PROJECT_CPP_NAME} ${CPP_ROOT_HEADER} ${CPP_ROOT_SOURCE}
                                ${CPP_SLVR_HEADER} ${CPP_SLVR_SOURCE})

target_link_libraries(
  ${PROJECT_CPP_NAME} PUBLIC ${PROJECT_APP_NAME})
target_include_directories(
  ${PROJECT_CPP_NAME}
  PUBLIC ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}
         ${PROJECT_SOURCE_DIR_LOCATION} ${PROJECT_HEADERS} ${CMAKE_SOURCE_DIR}
       )

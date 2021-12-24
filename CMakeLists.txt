cmake_minimum_required(VERSION 3.0.0)

project(openGlApplication)

add_subdirectory(glfw-3.3.5)
add_subdirectory(glad)
add_subdirectory(glm)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC glad/include)
target_include_directories(${PROJECT_NAME} PUBLIC glm)


target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} glad)
target_link_libraries(${PROJECT_NAME} glm)
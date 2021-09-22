cmake_minimum_required(VERSION 3.20)
project(DynamicWall)

set(CMAKE_CXX_STANDARD 17)

find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_path(STB_INCLUDE_DIRS "stb.h")
find_package(glm CONFIG REQUIRED)

add_executable(DynamicWall main.cpp)
add_executable(WallWindow WallWindow.cpp)

add_custom_target(ALL
        DEPENDS DynamicWall WallWindow)

#target_include_directories(DynamicWall PRIVATE ${STB_INCLUDE_DIRS})
#target_link_libraries(DynamicWall PRIVATE glad::glad)
#target_link_libraries(DynamicWall PRIVATE glfw)
#target_link_libraries(DynamicWall PRIVATE glm::glm)

target_link_libraries(WallWindow PRIVATE glfw)

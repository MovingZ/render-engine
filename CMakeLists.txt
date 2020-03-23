# This will become a big project one day.

cmake_minimum_required(VERSION 3.14)
project(render-engine)

set(CMAKE_CXX_STANDARD 17)

file(GLOB engine_src
        "src/app/main.cpp"
        "src/basic/*.cpp"
        "src/core/*.cpp"
        "src/components/*.cpp"
        "src/core/uniform-blocks/*cpp"
        "src/core/shadow/*cpp"
        )

# excutables:
add_executable(engine ${engine_src})
add_executable(test src/app/test.cpp)
add_executable(bullet3test src/app/bullet3test/main.cpp)

target_include_directories(engine PRIVATE
        ${PROJECT_SOURCE_DIR}/src/core
        ${PROJECT_SOURCE_DIR}/src/components
        ${PROJECT_SOURCE_DIR}/src/core/uniform-blocks
        ${PROJECT_SOURCE_DIR}/src/core/shadow
        )

add_compile_definitions(ENGINE_DEBUG)

#------------------------------------------------------------------------------#
# Below is the settings for 3rd party libraries

# glm
target_include_directories(engine PRIVATE ${PROJECT_SOURCE_DIR}/3rd-party/glm-0.9.9.5/include/)
add_subdirectory(${PROJECT_SOURCE_DIR}/3rd-party/glm-0.9.9.5/)

# stb_image
target_include_directories(engine PRIVATE ${PROJECT_SOURCE_DIR}/3rd-party/stb_image/include/)
add_subdirectory(${PROJECT_SOURCE_DIR}/3rd-party/stb_image/)

# glfw
target_include_directories(engine PRIVATE ${PROJECT_SOURCE_DIR}/3rd-party/glfw-3.3/include/)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${PROJECT_SOURCE_DIR}/3rd-party/glfw-3.3/)

# glad
target_include_directories(engine PRIVATE ${PROJECT_SOURCE_DIR}/3rd-party/glad/include/)
add_subdirectory(${PROJECT_SOURCE_DIR}/3rd-party/glad/)

# assimp
target_include_directories(engine PRIVATE ${PROJECT_SOURCE_DIR}/3rd-party/assimp-5.0/include/)
add_subdirectory(${PROJECT_SOURCE_DIR}/3rd-party/assimp-5.0/)

# imgui
target_include_directories(engine PRIVATE ${PROJECT_SOURCE_DIR}/3rd-party/imgui/)
add_subdirectory(${PROJECT_SOURCE_DIR}/3rd-party/imgui/)

# rapidJSON
target_include_directories(engine PRIVATE ${PROJECT_SOURCE_DIR}/3rd-party/rapidjson/include/)
add_subdirectory(${PROJECT_SOURCE_DIR}/3rd-party/rapidjson/)

# bullet3
include_directories(${PROJECT_SOURCE_DIR}/3rd-party/bullet3/src/)
add_subdirectory(${PROJECT_SOURCE_DIR}/3rd-party/bullet3/)

# linking:
target_link_libraries(imgui glad glfw)
target_link_libraries(engine assimp imgui glad glfw stb_image glm)

target_link_libraries(bullet3test BulletInverseDynamics BulletSoftBody BulletCollision BulletDynamics LinearMath Bullet3Common)

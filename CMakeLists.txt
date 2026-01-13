cmake_minimum_required(VERSION 4.1)
project(Renderer LANGUAGES CXX C)

# C++23 标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包管理
find_package(SDL2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# 搜集源文件
file(GLOB_RECURSE SOURCES "Src/*.cpp")

add_executable(Renderer ${SOURCES})

target_include_directories(Renderer PUBLIC
        ${CMAKE_SOURCE_DIR}/Include
)

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL2::SDL2
        SDL2::SDL2main
        imgui::imgui
        glm::glm
)

# 这种方式在构建配置阶段就会创建目录并拷贝，更健壮
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets")

add_custom_command(TARGET Renderer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Renderer>/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Renderer>/assets"
        COMMENT "Copying assets to build directory..."
)


if(MSVC)
    add_compile_options(/utf-8)
endif()
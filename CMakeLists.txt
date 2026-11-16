cmake_minimum_required(VERSION 3.10)
project(GameCosmo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_SOURCE_DIR}/GameCosmo/libs/glad/include
    ${CMAKE_SOURCE_DIR}/GameCosmo/libs/glfw/include
    ${CMAKE_SOURCE_DIR}/GameCosmo/libs/glm
    ${CMAKE_SOURCE_DIR}/GameCosmo/libs/stb
    ${CMAKE_SOURCE_DIR}/GameCosmo/libs/freetype/include
)

add_executable(${PROJECT_NAME} 
    GameCosmo/src/main.cpp 
    GameCosmo/src/game.cpp
    GameCosmo/src/glad.c
    GameCosmo/src/game.h
    GameCosmo/src/shader.cpp
    GameCosmo/src/shader.h
    GameCosmo/src/sprite_renderer.cpp
    GameCosmo/src/sprite_renderer.h
 "GameCosmo/src/texture.cpp" "GameCosmo/src/texture.h" "GameCosmo/src/resource_manager.cpp" "GameCosmo/src/resource_manager.h" "GameCosmo/src/game_object.h" "GameCosmo/src/game_object.cpp" "GameCosmo/src/player.h" "GameCosmo/src/bullet.h" "GameCosmo/src/player.cpp" "GameCosmo/src/enemy.h" "GameCosmo/src/text_renderer.h" "GameCosmo/src/text_renderer.cpp" "GameCosmo/src/boss.h" "GameCosmo/src/boss.cpp" "GameCosmo/src/enemy_bullet.h" "GameCosmo/src/power_up.h")

target_link_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/GameCosmo/libs/glfw/lib-vc2022
    ${CMAKE_SOURCE_DIR}/GameCosmo/libs/freetype/lib
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw3.lib 
    opengl32.lib
    freetype.lib
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/GameCosmo/shaders/sprite.vert"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/sprite.vert"
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/GameCosmo/shaders/sprite.frag"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/sprite.frag"
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/GameCosmo/shaders/text.vert"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/text.vert"
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/GameCosmo/shaders/text.frag"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/text.frag"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/textures"
)

file(GLOB TEXTURE_FILES "${CMAKE_SOURCE_DIR}/GameCosmo/textures/*.*")

foreach(TEXTURE_FILE ${TEXTURE_FILES})
    get_filename_component(TEXTURE_NAME ${TEXTURE_FILE} NAME)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${TEXTURE_FILE}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/textures/${TEXTURE_NAME}"
    )
endforeach()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/GameCosmo/libs/freetype/bin/freetype.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/fonts"
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy

    "${CMAKE_SOURCE_DIR}/GameCosmo/fonts/Nasalization.otf"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/fonts/Nasalization.otf"
)
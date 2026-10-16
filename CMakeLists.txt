cmake_minimum_required(VERSION 3.28)

project(ObjectProject VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include(FetchContent)

# Build SFML as static libraries so the M0 executable can run without copying
# SFML DLL files beside it.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "Build SFML audio module" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "Build SFML network module" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "Build SFML examples" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "Build SFML documentation" FORCE)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

add_library(object_project_core
    src/core/AppConfig.cpp
    src/game/Board.cpp
    src/game/CardSystem.cpp
    src/game/GameSession.cpp
    src/game/SunSystem.cpp
)
target_include_directories(object_project_core PUBLIC include)
target_compile_features(object_project_core PUBLIC cxx_std_17)

add_executable(pvz_game
    src/main.cpp
    src/ui/GameScreen.cpp
)
target_link_libraries(pvz_game PRIVATE object_project_core SFML::Graphics)

if(MSVC)
    target_compile_options(object_project_core PRIVATE /W4 /permissive-)
    target_compile_options(pvz_game PRIVATE /W4 /permissive-)
else()
    target_compile_options(object_project_core PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(pvz_game PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_custom_command(
    TARGET pvz_game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:pvz_game>/assets"
    COMMENT "Copying runtime assets"
)

include(CTest)
if(BUILD_TESTING)
    add_executable(app_config_tests
        tests/AppConfigTests.cpp
    )
    add_executable(board_tests tests/BoardTests.cpp)
    add_executable(card_system_tests tests/CardSystemTests.cpp)
    add_executable(sun_system_tests tests/SunSystemTests.cpp)
    add_executable(game_session_tests tests/GameSessionTests.cpp)

    foreach(test_target IN ITEMS
        app_config_tests
        board_tests
        card_system_tests
        sun_system_tests
        game_session_tests
    )
        target_link_libraries(${test_target} PRIVATE object_project_core)
        if(MSVC)
            target_compile_options(${test_target} PRIVATE /W4 /permissive-)
        else()
            target_compile_options(${test_target} PRIVATE -Wall -Wextra -Wpedantic)
        endif()
        add_test(NAME ${test_target} COMMAND ${test_target})
    endforeach()
endif()

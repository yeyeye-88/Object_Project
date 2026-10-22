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
set(SFML_BUILD_AUDIO ON CACHE BOOL "Build SFML audio module" FORCE)
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
    src/core/AppSettings.cpp
    src/game/AlmanacData.cpp
    src/game/Board.cpp
    src/game/CardSystem.cpp
    src/game/CombatSystem.cpp
    src/game/GameSession.cpp
    src/game/LevelDefinition.cpp
    src/game/LevelSystem.cpp
    src/game/Plant.cpp
    src/game/SunSystem.cpp
    src/game/Zombie.cpp
)
target_include_directories(object_project_core PUBLIC include)
target_compile_features(object_project_core PUBLIC cxx_std_17)

add_executable(pvz_game
    src/audio/AudioSystem.cpp
    src/main.cpp
    src/ui/AlmanacScreen.cpp
    src/ui/GameScreen.cpp
    src/ui/HelpScreen.cpp
    src/ui/IntroScreen.cpp
    src/ui/LevelSelectScreen.cpp
    src/ui/MainMenuScreen.cpp
    src/ui/SettingsScreen.cpp
    src/ui/ZombieNoteScreen.cpp
)
target_link_libraries(pvz_game PRIVATE object_project_core SFML::Graphics SFML::Audio)

if(MSVC)
    target_compile_options(object_project_core PRIVATE /W4 /permissive-)
    target_compile_options(pvz_game PRIVATE /W4 /permissive- /utf-8)
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
    add_executable(app_settings_tests tests/AppSettingsTests.cpp)
    add_executable(board_tests tests/BoardTests.cpp)
    add_executable(card_system_tests tests/CardSystemTests.cpp)
    add_executable(sun_system_tests tests/SunSystemTests.cpp)
    add_executable(game_session_tests tests/GameSessionTests.cpp)
    add_executable(plant_tests tests/PlantTests.cpp)
    add_executable(level_definition_tests tests/LevelDefinitionTests.cpp)
    add_executable(level_system_tests tests/LevelSystemTests.cpp)
    add_executable(combat_system_tests tests/CombatSystemTests.cpp)
    add_executable(zombie_tests tests/ZombieTests.cpp)
    add_executable(almanac_data_tests tests/AlmanacDataTests.cpp)
    add_executable(visual_asset_tests tests/VisualAssetTests.cpp)
    target_link_libraries(visual_asset_tests PRIVATE SFML::Graphics)
    target_compile_definitions(
        visual_asset_tests
        PRIVATE OBJECT_PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    )

    foreach(test_target IN ITEMS
        app_config_tests
        app_settings_tests
        board_tests
        card_system_tests
        sun_system_tests
        game_session_tests
        plant_tests
        level_definition_tests
        level_system_tests
        combat_system_tests
        zombie_tests
        almanac_data_tests
    )
        target_link_libraries(${test_target} PRIVATE object_project_core)
        if(MSVC)
            target_compile_options(${test_target} PRIVATE /W4 /permissive-)
        else()
            target_compile_options(${test_target} PRIVATE -Wall -Wextra -Wpedantic)
        endif()
        add_test(NAME ${test_target} COMMAND ${test_target})
    endforeach()
    add_test(NAME visual_asset_tests COMMAND visual_asset_tests)
endif()

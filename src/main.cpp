#include "core/AppConfig.hpp"
#include "game/GameSession.hpp"
#include "ui/AlmanacScreen.hpp"
#include "ui/GameScreen.hpp"
#include "ui/LevelSelectScreen.hpp"
#include "ui/MainMenuScreen.hpp"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <filesystem>

namespace {

enum class AppScreen {
    MainMenu,
    LevelSelect,
    Game,
    Almanac,
};

}  // namespace

int main() {
    const std::string title = object_project::loadWindowTitle(
        std::filesystem::path{"assets/config/window_title.txt"},
        "Object Project - M4 Rich Gameplay"
    );

    sf::RenderWindow window(sf::VideoMode({960, 600}), title);
    window.setFramerateLimit(60);

    object_project::GameSession session;
    object_project::GameScreen screen(session);
    object_project::AlmanacScreen almanac;
    object_project::MainMenuScreen mainMenu;
    object_project::LevelSelectScreen levelSelect;
    AppScreen activeScreen = AppScreen::MainMenu;
    AppScreen almanacReturnScreen = AppScreen::MainMenu;
    sf::Clock frameClock;

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (activeScreen == AppScreen::MainMenu) {
                const object_project::MainMenuAction action =
                    mainMenu.handleEvent(*event, window);
                if (action == object_project::MainMenuAction::Play) {
                    activeScreen = AppScreen::LevelSelect;
                } else if (action == object_project::MainMenuAction::Almanac) {
                    almanacReturnScreen = AppScreen::MainMenu;
                    activeScreen = AppScreen::Almanac;
                } else if (action == object_project::MainMenuAction::Exit) {
                    window.close();
                }
            } else if (activeScreen == AppScreen::LevelSelect) {
                const object_project::LevelSelectAction action =
                    levelSelect.handleEvent(*event, window);
                if (action.type == object_project::LevelSelectActionType::StartLevel) {
                    session.startLevel(action.level);
                    screen.resetInteraction();
                    activeScreen = AppScreen::Game;
                } else if (action.type == object_project::LevelSelectActionType::Back) {
                    activeScreen = AppScreen::MainMenu;
                }
            } else if (activeScreen == AppScreen::Game) {
                screen.handleEvent(*event, window);
            } else if (activeScreen == AppScreen::Almanac &&
                       almanac.handleEvent(*event, window)) {
                activeScreen = almanacReturnScreen;
            }
        }

        if (activeScreen == AppScreen::Game && screen.takeAlmanacRequest()) {
            almanacReturnScreen = AppScreen::Game;
            activeScreen = AppScreen::Almanac;
        } else if (activeScreen == AppScreen::Game && screen.takeMenuRequest()) {
            activeScreen = AppScreen::LevelSelect;
        }

        const float deltaSeconds = std::min(frameClock.restart().asSeconds(), 0.1F);
        if (activeScreen == AppScreen::Game) {
            session.update(deltaSeconds);
        }

        window.clear(sf::Color(35, 71, 39));
        switch (activeScreen) {
            case AppScreen::MainMenu:
                mainMenu.draw(window);
                break;
            case AppScreen::LevelSelect:
                levelSelect.draw(window);
                break;
            case AppScreen::Game:
                screen.draw(window);
                break;
            case AppScreen::Almanac:
                almanac.draw(window);
                break;
        }
        window.display();
    }

    return 0;
}

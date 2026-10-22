#include "audio/AudioSystem.hpp"
#include "core/AppConfig.hpp"
#include "core/AppSettings.hpp"
#include "game/GameSession.hpp"
#include "ui/AlmanacScreen.hpp"
#include "ui/GameScreen.hpp"
#include "ui/HelpScreen.hpp"
#include "ui/IntroScreen.hpp"
#include "ui/LevelSelectScreen.hpp"
#include "ui/MainMenuScreen.hpp"
#include "ui/SettingsScreen.hpp"
#include "ui/ZombieNoteScreen.hpp"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <filesystem>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace {

enum class AppScreen {
    Intro,
    MainMenu,
    LevelSelect,
    ZombieNote,
    Game,
    Almanac,
    Settings,
    Help,
};

void applyWindowSettings(
    sf::RenderWindow& window,
    const object_project::AppSettings& settings,
    const sf::String& title
) {
    const object_project::WindowResolution resolution = settings.resolution();
    if (settings.fullscreen()) {
        window.create(sf::VideoMode::getDesktopMode(), title, sf::State::Fullscreen);
    } else {
        window.create(sf::VideoMode({resolution.width, resolution.height}), title);
    }
    window.setView(sf::View(sf::FloatRect({0.0F, 0.0F}, {960.0F, 600.0F})));
    window.setFramerateLimit(60);
}

}  // namespace

int main() {
#ifdef _WIN32
    SetProcessDPIAware();
#endif
    const std::string titleText = object_project::loadWindowTitle(
        std::filesystem::path{"assets/config/window_title.txt"},
        "植物大战僵尸 - M4 丰富玩法"
    );
    const sf::String title = sf::String::fromUtf8(titleText.begin(), titleText.end());

    object_project::AppSettings settings;
    sf::RenderWindow window;
    applyWindowSettings(window, settings, title);
    object_project::AudioSystem audio;
    audio.applySettings(settings);

    object_project::GameSession session;
    object_project::GameScreen screen(session);
    object_project::IntroScreen intro;
    object_project::AlmanacScreen almanac;
    object_project::MainMenuScreen mainMenu;
    object_project::LevelSelectScreen levelSelect;
    object_project::SettingsScreen settingsScreen(settings);
    object_project::ZombieNoteScreen zombieNote;
    object_project::HelpScreen help;
    object_project::LevelId pendingLevel = object_project::LevelId::Tutorial;
    AppScreen activeScreen = AppScreen::Intro;
    AppScreen almanacReturnScreen = AppScreen::MainMenu;
    sf::Clock frameClock;

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (activeScreen == AppScreen::Intro) {
                if (intro.handleEvent(*event)) {
                    audio.play(object_project::SoundCue::Click);
                    activeScreen = AppScreen::MainMenu;
                }
            } else if (activeScreen == AppScreen::MainMenu) {
                const object_project::MainMenuAction action =
                    mainMenu.handleEvent(*event, window);
                if (action == object_project::MainMenuAction::Play) {
                    audio.play(object_project::SoundCue::Click);
                    activeScreen = AppScreen::LevelSelect;
                } else if (action == object_project::MainMenuAction::Almanac) {
                    audio.play(object_project::SoundCue::Click);
                    almanacReturnScreen = AppScreen::MainMenu;
                    activeScreen = AppScreen::Almanac;
                } else if (action == object_project::MainMenuAction::Settings) {
                    audio.play(object_project::SoundCue::Click);
                    activeScreen = AppScreen::Settings;
                } else if (action == object_project::MainMenuAction::Help) {
                    audio.play(object_project::SoundCue::Click);
                    activeScreen = AppScreen::Help;
                } else if (action == object_project::MainMenuAction::Exit) {
                    window.close();
                }
            } else if (activeScreen == AppScreen::LevelSelect) {
                const object_project::LevelSelectAction action =
                    levelSelect.handleEvent(*event, window);
                if (action.type == object_project::LevelSelectActionType::StartLevel) {
                    audio.play(object_project::SoundCue::Click);
                    pendingLevel = action.level;
                    if (pendingLevel == object_project::LevelId::EndlessDefense) {
                        session.startLevel(pendingLevel);
                        screen.resetInteraction();
                        activeScreen = AppScreen::Game;
                    } else {
                        zombieNote.setLevel(pendingLevel);
                        activeScreen = AppScreen::ZombieNote;
                    }
                } else if (action.type == object_project::LevelSelectActionType::Back) {
                    audio.play(object_project::SoundCue::Click);
                    activeScreen = AppScreen::MainMenu;
                }
            } else if (activeScreen == AppScreen::ZombieNote) {
                if (zombieNote.handleEvent(*event, window)) {
                    audio.play(object_project::SoundCue::Click);
                    session.startLevel(pendingLevel);
                    screen.resetInteraction();
                    activeScreen = AppScreen::Game;
                }
            } else if (activeScreen == AppScreen::Game) {
                screen.handleEvent(*event, window);
            } else if (activeScreen == AppScreen::Almanac &&
                       almanac.handleEvent(*event, window)) {
                audio.play(object_project::SoundCue::Click);
                activeScreen = almanacReturnScreen;
            } else if (activeScreen == AppScreen::Settings) {
                const object_project::SettingsAction action =
                    settingsScreen.handleEvent(*event, window);
                if (action != object_project::SettingsAction::None) {
                    audio.applySettings(settings);
                    audio.play(object_project::SoundCue::Click);
                }
                if (action == object_project::SettingsAction::WindowChanged) {
                    applyWindowSettings(window, settings, title);
                } else if (action == object_project::SettingsAction::Back) {
                    activeScreen = AppScreen::MainMenu;
                }
            } else if (activeScreen == AppScreen::Help && help.handleEvent(*event, window)) {
                audio.play(object_project::SoundCue::Click);
                activeScreen = AppScreen::MainMenu;
            }
        }

        if (activeScreen == AppScreen::Game && screen.takeAlmanacRequest()) {
            almanacReturnScreen = AppScreen::Game;
            activeScreen = AppScreen::Almanac;
        } else if (activeScreen == AppScreen::Game && screen.takeMenuRequest()) {
            activeScreen = AppScreen::LevelSelect;
        }

        const float deltaSeconds = std::min(frameClock.restart().asSeconds(), 0.1F);
        if (activeScreen == AppScreen::Intro) {
            intro.update(deltaSeconds);
        } else if (activeScreen == AppScreen::Game) {
            session.update(deltaSeconds);
            screen.update(deltaSeconds);
            for (const object_project::SoundCue cue : screen.takeSoundCues()) {
                audio.play(cue);
            }
        }

        window.clear(sf::Color(35, 71, 39));
        switch (activeScreen) {
            case AppScreen::Intro:
                intro.draw(window);
                break;
            case AppScreen::MainMenu:
                mainMenu.draw(window);
                break;
            case AppScreen::LevelSelect:
                levelSelect.draw(window);
                break;
            case AppScreen::ZombieNote:
                zombieNote.draw(window);
                break;
            case AppScreen::Game:
                screen.draw(window);
                break;
            case AppScreen::Almanac:
                almanac.draw(window);
                break;
            case AppScreen::Settings:
                settingsScreen.draw(window);
                break;
            case AppScreen::Help:
                help.draw(window);
                break;
        }
        window.display();
    }

    return 0;
}

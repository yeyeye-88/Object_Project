#pragma once

#include "core/AppSettings.hpp"

#include <SFML/Graphics.hpp>

#include <string_view>

namespace object_project {

enum class SettingsAction {
    None,
    Changed,
    WindowChanged,
    Back,
};

class SettingsScreen {
public:
    explicit SettingsScreen(AppSettings& settings);

    SettingsAction handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const;

private:
    void drawText(
        sf::RenderWindow& window,
        std::string_view value,
        unsigned int size,
        sf::Vector2f position,
        sf::Color color = sf::Color::White
    ) const;
    void drawButton(sf::RenderWindow& window, sf::FloatRect bounds, std::string_view label) const;
    void drawRow(
        sf::RenderWindow& window,
        float y,
        std::string_view label,
        const std::string& value
    ) const;

    AppSettings& settings_;
    sf::Font font_;
    bool fontLoaded_{};
    sf::Texture backgroundTexture_;
    sf::Texture panelTexture_;
    sf::Texture buttonTexture_;
    bool backgroundLoaded_{};
    bool panelLoaded_{};
    bool buttonLoaded_{};
};

}  // namespace object_project

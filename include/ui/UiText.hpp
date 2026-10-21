#pragma once

#include <SFML/Graphics.hpp>

#include <array>
#include <string_view>

namespace object_project::ui {

inline bool loadChineseFont(sf::Font& font) {
    constexpr std::array<std::string_view, 4> candidates{
        "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/msyhbd.ttc",
        "C:/Windows/Fonts/simhei.ttf",
        "assets/fonts/tuffy.ttf",
    };
    for (const std::string_view path : candidates) {
        if (font.openFromFile(std::string(path))) {
            return true;
        }
    }
    return false;
}

inline sf::String utf8(const std::string_view value) {
    return sf::String::fromUtf8(value.begin(), value.end());
}

inline void centerText(sf::Text& text, const sf::FloatRect bounds) {
    const sf::FloatRect local = text.getLocalBounds();
    text.setPosition({
        bounds.position.x + (bounds.size.x - local.size.x) * 0.5F - local.position.x,
        bounds.position.y + (bounds.size.y - local.size.y) * 0.5F - local.position.y,
    });
}

}  // namespace object_project::ui

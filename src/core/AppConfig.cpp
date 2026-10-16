#include "core/AppConfig.hpp"

#include <fstream>

namespace object_project {

std::string loadWindowTitle(
    const std::filesystem::path& path,
    const std::string& fallbackTitle
) {
    std::ifstream input(path);
    if (!input) {
        return fallbackTitle;
    }

    std::string title;
    std::getline(input, title);
    return title.empty() ? fallbackTitle : title;
}

}  // namespace object_project


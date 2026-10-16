#pragma once

#include <filesystem>
#include <string>

namespace object_project {

std::string loadWindowTitle(
    const std::filesystem::path& path,
    const std::string& fallbackTitle
);

}  // namespace object_project


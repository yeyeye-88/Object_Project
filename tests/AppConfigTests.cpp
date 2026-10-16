#include "core/AppConfig.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace {

bool expectEqual(
    const std::string& actual,
    const std::string& expected,
    const std::string& testName
) {
    if (actual == expected) {
        std::cout << "[PASS] " << testName << '\n';
        return true;
    }

    std::cerr << "[FAIL] " << testName << ": expected '" << expected
              << "' but got '" << actual << "'\n";
    return false;
}

}  // namespace

int main() {
    bool allPassed = true;
    const std::string fallback = "Fallback Title";

    allPassed &= expectEqual(
        object_project::loadWindowTitle("missing-window-title.txt", fallback),
        fallback,
        "missing file uses fallback title"
    );

    const std::filesystem::path testFile =
        std::filesystem::temp_directory_path() / "object-project-window-title.txt";
    {
        std::ofstream output(testFile);
        output << "Test Window Title\n";
    }

    allPassed &= expectEqual(
        object_project::loadWindowTitle(testFile, fallback),
        "Test Window Title",
        "configured title is loaded"
    );

    std::error_code error;
    std::filesystem::remove(testFile, error);
    if (error) {
        std::cerr << "[FAIL] could not remove temporary test file: "
                  << error.message() << '\n';
        allPassed = false;
    }

    return allPassed ? 0 : 1;
}


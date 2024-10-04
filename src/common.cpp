#include "common.h"
#include <fstream>
#include <sstream>

std::optional<std::string> LoadTextFile(const std::string& filename) {
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return {};
    }
    std::stringstream text;
    text << fin.rdbuf();
    return text.str();
}

std::optional<int> ParseInt(const std::string& str) {
    if (str.empty()) return {};

    size_t index = 0;

    if (str[0] == '+' || str[0] == '-') {
        index++;
        if (index == str.size()) return {};
    }

    for (; index < str.size(); index++) {
        if (!std::isdigit(str[index])) {
            return {};
        }
    }

    try {
        return std::stoi(str);
    } catch (const std::exception&) {
        return {};
    }
}

std::optional<float> ParseFloat(const std::string& str) {
    if (str.empty()) return {};

    size_t index = 0;
    bool dotFound = false;

    if (str[0] == '+' || str[0] == '-') {
        index++;
        if (index == str.size()) return {};
    }

    for (; index < str.size(); index++) {
        if (str[index] == '.') {
            if (dotFound) return {};
            dotFound = true;
        }
        else if (!std::isdigit(str[index])) {
            return {};
        }
    }

    try {
        return std::stof(str);
    } catch (const std::exception&) {
        return {};
    }
}
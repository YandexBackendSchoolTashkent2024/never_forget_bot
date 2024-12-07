#include "env_parser.h"
#include <iostream>
#include <fstream>
#include <algorithm>

namespace EnvParser {

static inline std::string trim(const std::string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::unordered_map<std::string, std::string> load(const std::string &filePath) {
    std::unordered_map<std::string, std::string> envVariables;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Warning: Could not open file: " << filePath << "\n";
        return envVariables;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);

        if (line.empty() || line.front() == '#')
            continue;

        size_t delimiterPos = line.find('=');
        if (delimiterPos == std::string::npos) {
            continue;
        }

        std::string key = trim(line.substr(0, delimiterPos));
        std::string value = trim(line.substr(delimiterPos + 1));

        if (!value.empty() && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }

        size_t pos = 0;
        while ((pos = value.find("\\n", pos)) != std::string::npos) {
            value.replace(pos, 2, "\n");
            pos += 1;
        }

        envVariables[key] = value;
    }

    return envVariables;
}

} // namespace EnvParser

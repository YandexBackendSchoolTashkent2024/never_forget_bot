#ifndef ENV_PARSER_H
#define ENV_PARSER_H

#include <string>
#include <unordered_map>

namespace EnvParser {
    std::unordered_map<std::string, std::string> load(const std::string &filePath);
}

#endif

// parse_request.h
#ifndef PARSE_REQUEST_H
#define PARSE_REQUEST_H

#include <unordered_map>
#include <string>

std::unordered_map<std::string, std::string> sendPostRequest(const std::string& msg);

#endif

#pragma once
#include <string>
#include <string_view>
#include <vector>

bool string_is_number(std::string_view);
std::vector<std::string> split_string(std::string_view, const char delim = ' ');
#pragma once
#include <string>
#include <string_view>
#include <vector>

std::vector<std::string> split_string(std::string_view, const char delim = ' ');
#pragma once
#include <string>
#include <string_view>
#include <vector>

std::string remove_extra_whitespaces(std::string_view);

bool string_is_number(std::string_view);

bool starts_with(std::string_view, std::string_view);

std::vector<std::string> split_string(std::string_view, const char delim = ' ');
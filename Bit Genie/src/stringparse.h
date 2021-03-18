#pragma once
#include <string>
#include <string_view>
#include <vector>

std::string& ltrim(std::string&, const char* t = " \t\n\r\f\v");
void trim(std::string&);

std::string& tolower(std::string&);

bool string_is_number(std::string_view);

bool starts_with(std::string_view, std::string_view);

std::vector<std::string> split_string(std::string_view);
std::vector<std::string> split_string(std::string_view, char);
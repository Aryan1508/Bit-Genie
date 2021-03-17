#include "stringparse.h"
#include <sstream>
#include <algorithm>
#include <regex>

std::string& ltrim(std::string& s, const char* t)
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}

bool string_is_number(std::string_view s)
{
	std::string_view::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

std::string& tolower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(),
						  [](unsigned char c) { return std::tolower(c); });
	return str;
}

std::vector<std::string> split_string(std::string_view str)
{
	std::vector<std::string> parts;
	std::stringstream stream(str.data());
	std::string temp;
	while (stream >> temp)
	{
		parts.push_back(std::move(temp));
	}
	return parts;
}


std::vector<std::string> split_string(std::string_view str, char delim)
{
	std::vector<std::string> parts;
	std::stringstream stream(str.data());
	std::string temp;

	while (std::getline(stream, temp, delim))
	{
		parts.push_back(std::move(temp));
	}
	return parts;
}

bool starts_with(std::string_view str, std::string_view comp)
{
	return str.rfind(comp, 0) == 0;
}
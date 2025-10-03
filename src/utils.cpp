//
// Created by bvasseur on 10/3/25.
//

#include "utils.hpp"

float roundFloat(const float value, const int precision) {
	const float multiplier = std::pow(10.0f, precision);
	return std::floor(value * multiplier) / multiplier;
}

std::string roundStringFloat(const std::string& str, const int precision) {
	if (str.rfind('.') == std::string::npos || str.rfind('.') + precision + 1 >= str.size())
		return str;
	return str.substr(0, str.rfind('.') + precision + 1);
}

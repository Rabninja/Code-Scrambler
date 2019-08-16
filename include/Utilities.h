#pragma once

#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <algorithm>
#include <sstream>
#include <optional>
#include <iostream>
#include <SFML/Graphics/Rect.hpp>

inline bool endsWith(const std::string& data, const std::string& ending) {
	if (ending.size() > data.size()) {
		return false;
	}
	return std::equal(data.rbegin(), data.rbegin() + ending.size(), ending.rbegin());
}

inline bool beginsWith(const std::string& data, const std::string& beginning) {
	if (beginning.size() > data.size()) {
		return false;
	}
	return std::equal(data.begin(), data.begin() + beginning.size(), beginning.begin());
}

inline sf::Vector2f getCenter(sf::FloatRect rect) {
	return{ std::round(rect.left + rect.width * 0.5f), std::round(rect.top + rect.height * 0.5f) };
}

template<typename T>
inline void centerX(T& item) {
	item.setOrigin(getCenter(item.getLocalBounds()).x, item.getOrigin().y);
}

inline void ltrim(std::string& str) {
	str.erase(str.begin(), std::find_if_not(str.begin(), str.end(), [](auto c) {return std::isblank(c); }));
}

template<typename T>
inline void centerY(T& item) {
	item.setOrigin(item.getOrigin().x, getCenter(item.getLocalBounds()).y);
}

template<typename T>
inline void outputContainer(const std::string& location, const T& container)
{
	std::ofstream output(location, std::ofstream::trunc | std::ofstream::out);

	for (auto& value : container) {
		output << value << '\n';
	}

	output.flush();
}



inline std::string getCmdOption(const char** begin, const char** end, const std::string& option)
{
	const char** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
	{
		return *itr;
	}
	return {};
}

template<typename T>
std::optional<T> parseType(const std::string& data)
{
	T value;
	std::stringstream converter(data);
	if (converter >> value) {
		return value;
	}
	
	return std::nullopt;
}

inline void exitPrompt()
{
	std::cout << "Press enter to exit...";
	std::cin.get();
	std::exit(EXIT_FAILURE);
}


#endif

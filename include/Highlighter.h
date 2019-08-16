#pragma once

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include "SmartText.h"
#include <unordered_map>

struct Detail
{
	using Validator = bool(*)(char ch);
	Validator validator;
	sf::Color color;
	sf::Text::Style style;

	static bool any(char ch)
	{
		return true;
	}

	static bool userType(char ch)
	{
		return !std::isalnum(ch) && ch != '_';
	}

	static bool fundamentalType(char ch)
	{
		return !std::isalnum(ch) && ch != '_' && ch != ':';
	}

	Detail()
		: Detail(any, sf::Color::Black, sf::Text::Style::Regular) {
		
	}

	Detail(sf::Color color, Validator validator)
		 : Detail(validator, color, sf::Text::Style::Regular) {
		
	}

	Detail(Validator validator, sf::Color color, sf::Text::Style style)
		: validator(validator),
		color(color),
		style(style) {
		
	}

	bool validate(const char ch) const {
		return validator(ch);
	}
};

class Highlighter
{
private:
	const sf::Font* m_font;
	sf::Uint32 m_characterSize;
	std::unordered_map<std::string, Detail> m_keywords;
public:
	Highlighter();

	void setFont(const sf::Font& font);

	void setCharacterSize(sf::Uint32 characterSize);

	const sf::Font& getFont() const;

	void addKeyword(const std::string& key, Detail detail);

	bool removeKeyword(const std::string& key);

	Detail getKeyword(const std::string& key) const;

	SmartText buildText(const std::string& line) const;

private:
	void highlightComments(SmartText& text) const;

	void highlightKeywords(SmartText& text) const;

	void highlightMacros(SmartText& text) const;

	void highlightQuotes(SmartText& text) const;
};

#endif

#include "Highlighter.h"
#include <fstream>
#include <cctype>
#include "Utilities.h"

namespace
{
	bool isEscaped(const std::string& string, std::size_t boundary, std::size_t index) {
		bool escaped = false;
		while (index > boundary && string[--index] == '\\') {
			escaped = !escaped;
		}
		return escaped;
	}
}

Highlighter::Highlighter()
{
	std::ifstream file("keywords.txt", std::ifstream::in);

	std::string keyword;
	while (std::getline(file, keyword)) {
		addKeyword(keyword, { sf::Color::Blue, &Detail::fundamentalType });
	}

	std::string stdtype;
	file.close();
	file.clear();
	file.open("stdtypes.txt", std::ifstream::in);
	while (std::getline(file, stdtype)) {
		addKeyword(stdtype, { {32, 100, 148}, &Detail::userType });
	}

	file.close();
	file.clear();
	file.open("operators.txt", std::ifstream::in);
	while (std::getline(file, keyword)) {
		addKeyword(keyword, { {80, 170, 192}, &Detail::any });
	}
}

void Highlighter::setFont(const sf::Font& font)
{
	m_font = &font;
}

void Highlighter::setCharacterSize(sf::Uint32 characterSize)
{
	m_characterSize = characterSize;
}

const sf::Font& Highlighter::getFont() const
{
	return *m_font;
}

void Highlighter::addKeyword(const std::string& key, Detail detail)
{
	m_keywords.emplace(key, detail);
}

bool Highlighter::removeKeyword(const std::string& key)
{
	auto it = m_keywords.find(key);

	if (it != m_keywords.end()) {
		m_keywords.erase(it);
	}
	return it != m_keywords.end();
}

Detail Highlighter::getKeyword(const std::string& key) const
{
	return m_keywords.at(key);
}

SmartText Highlighter::buildText(const std::string& line) const
{
	SmartText text(line, *m_font);
	text.setCharacterSize(m_characterSize);
	text.setFillColor(sf::Color::Black);
	text.setStyle(sf::Text::Style::Regular);
	highlightKeywords(text);
	highlightMacros(text);
	highlightQuotes(text);
	highlightComments(text);

	return text;
}

void Highlighter::highlightComments(SmartText& text) const
{
	std::string line = text.getString();
	std::size_t comment = std::string::npos;

	const sf::Color commentColor(5, 115, 11);
	while ((comment = line.find("//", comment + 1)) != std::string::npos) {
		std::size_t opening = std::string::npos;

		for (std::size_t index = 0; index != line.size(); ++index) {
			if (line[index] != '\"') {
				continue;
			}

			if (opening == std::string::npos) {
				opening = index;
			}
			else if (!isEscaped(line, opening, index)) {
				if (comment >= opening && comment < index) {
					return;
				}
			}
		}
		text.setFillColor(comment, commentColor);
	}
}

void Highlighter::highlightKeywords(SmartText& text) const
{
	std::string line = text.getString();

	const auto begin = line.begin();
	const auto end = line.end();

	const auto validInstance = [](char ch) -> bool {
		return !std::isalnum(ch) && ch != '_' && ch != ':';
	};

	for (auto& keyword : m_keywords) {
		auto key = keyword.first;
		auto iter = begin; 
		while ((iter = std::search(iter, end, key.begin(), key.end())) != end) {
			const std::size_t index = iter - line.begin();
			const bool before = index == 0 || keyword.second.validate(*std::prev(iter));
			const bool after = index + key.size() == line.size() || keyword.second.validate(*(iter + key.size()));
			if (before && after) {
				text.setFillColor(index, key.size(), keyword.second.color);
				text.setStyle(index, key.size(), keyword.second.style);
			}
			std::advance(iter, key.size());
		}
	}
}

void Highlighter::highlightMacros(SmartText& text) const
{
	std::string line = text.getString();

	for (auto ch : line) {
		if (ch == '#') {
			text.setFillColor({ 128, 128, 128 });
			break;
		}
		else if (!std::isspace(ch)) {
			return;
		}
	}

	std::size_t start = std::string::npos;
	std::size_t end = std::string::npos;
	while ((start = line.find('<', start + 1)) != std::string::npos && (end = line.find('>', start + 1)) != std::string::npos) {
		const sf::Color color(137, 16, 8);
		text.setFillColor(start, end - start + 1, { color });
		start = end;
	}
}

void Highlighter::highlightQuotes(SmartText& text) const
{
	std::string line = text.getString();

	std::size_t start = std::string::npos; 
	std::size_t end   = std::string::npos;

	while ((start = line.find_first_of("\"\'", start + 1)) != std::string::npos) {
		const std::size_t marking = start;

		while ((end = line.find_first_of("\"\'", start + 1)) != std::string::npos) {
			const bool escaped = isEscaped(line, start, end);
			start = end;

			if (!escaped) {
				const sf::Color color(187, 41, 21);
				text.setFillColor(marking, end - marking + 1, { color });
				break;
			}
		}
	}
}


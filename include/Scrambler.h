#pragma once

#ifndef SCRAMBLER_H
#define SCRAMBLER_H

#include <vector>
#include <random>
#include <string>
#include <istream>

class Scrambler
{
private:
	mutable std::mt19937 m_engine;
	std::vector<bool> m_markings;
	std::vector<std::string> m_lines;
	int m_difficulty;
public:
	Scrambler(const std::string& filePath, int difficulty);

	void loadFromFile(const std::string& filePath);

	void loadFromStream(std::istream& stream);

	void seed();

	void seed(unsigned seed);

	void markLine(std::size_t line, bool value);

	void markLines(std::size_t start, std::size_t end, bool value);

	const std::vector<std::string>& getLines() const;

	std::vector<std::string> getScrambling() const;

private:
	void scramble(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end) const;

};

#endif

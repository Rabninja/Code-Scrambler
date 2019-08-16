#include "Scrambler.h"
#include <fstream>
#include "Utilities.h"


Scrambler::Scrambler(const std::string& filePath, int difficulty)
	: m_difficulty(difficulty)
{
	seed();
	loadFromFile(filePath);
}

void Scrambler::loadFromFile(const std::string& filePath)
{
	std::ifstream stream(filePath, std::ifstream::in);
	loadFromStream(stream);
}

void Scrambler::loadFromStream(std::istream& stream)
{
	std::string line;
	std::size_t lineNumber = 0;
	std::size_t markStart  = std::string::npos;

	m_lines.clear();
	while (std::getline(stream, line)) {
		if (beginsWith(line, "~>")) {
			markStart = lineNumber;
			line = line.substr(2);
		}
		if (endsWith(line, "<~")) {
			markLines(markStart, lineNumber + 1, false);
			line = line.substr(0, line.size() - 2);
			markStart = std::string::npos;
		}

		if (std::find_if_not(line.begin(), line.end(), std::isspace) != line.end()) {
			if (m_difficulty > 0) {
				ltrim(line);
			}
			m_lines.push_back(line);
			++lineNumber;
		}
	}
	m_markings.resize(lineNumber, true);
}

void Scrambler::seed()
{
	seed(std::random_device()());
}

void Scrambler::seed(unsigned seed)
{
	m_engine.seed(seed);
}

void Scrambler::markLine(std::size_t line, bool value)
{
	markLines(line, line + 1, value);
}

void Scrambler::markLines(std::size_t start, std::size_t end, bool value)
{
	if (end > m_markings.size()) {
		m_markings.resize(end, true);
	}

	auto iterator = m_markings.begin();
	std::fill(iterator + start, iterator + end, value);
}

const std::vector<std::string>& Scrambler::getLines() const
{
	return m_lines;
}

std::vector<std::string> Scrambler::getScrambling() const
{
	std::vector<std::string> scrambling{ m_lines };
	std::vector<std::string>::iterator begin{ scrambling.begin() };
	
	const std::size_t size = scrambling.size();
	for (std::size_t idx = 0; idx != size; ++idx) {
		if (!m_markings[idx]) {
			std::vector<std::string>::iterator end = scrambling.begin() + idx;
			scramble(begin, end);
			begin = std::next(end);
		}
	}
	scramble(begin, scrambling.end());
	return scrambling;
}

void Scrambler::scramble(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end) const
{
	std::shuffle(begin, end, m_engine);
}

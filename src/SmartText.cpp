#include "SmartText.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <cmath>

////////////////////////////////////////////////////////////
// Original author: Laurent Gomila
// Taken from SFML(2.4.1) Text.cpp
// Slightly modified VertexArray to be able to work with std::vector<sf::Vertex>
////////////////////////////////////////////////////////////
namespace
{
	// Add an underline or strikethrough line to the vertex array
	void addLine(float xOffset, std::vector<sf::Vertex>& vertices, float lineLength, float lineTop, const sf::Color& color, float offset, float thickness, float outlineThickness = 0)
	{
		const sf::Vector2f texCoords(1.f, 1.f);
		float top = std::roundf(lineTop + offset - (thickness / 2) + 0.5f);
		float bottom = top + std::floor(thickness + 0.5f);

		vertices.emplace_back(sf::Vector2f(-outlineThickness + xOffset, top - outlineThickness), color, texCoords);
		vertices.emplace_back(sf::Vector2f(lineLength + outlineThickness + xOffset, top - outlineThickness), color, texCoords);
		vertices.emplace_back(sf::Vector2f(-outlineThickness + xOffset, bottom + outlineThickness), color, texCoords);
		vertices.emplace_back(sf::Vector2f(-outlineThickness + xOffset, bottom + outlineThickness), color, texCoords);
		vertices.emplace_back(sf::Vector2f(lineLength + outlineThickness + xOffset, top - outlineThickness), color, texCoords);
		vertices.emplace_back(sf::Vector2f(lineLength + outlineThickness + xOffset, bottom + outlineThickness), color, texCoords);
	}

	// Add a glyph quad to the vertex array
	void addGlyphQuad(std::vector<sf::Vertex>& vertices, sf::Vector2f position, const sf::Color& color, const sf::Glyph& glyph, float italic, float outlineThickness = 0)
	{
		float left = glyph.bounds.left;
		float top = glyph.bounds.top;
		float right = glyph.bounds.left + glyph.bounds.width;
		float bottom = glyph.bounds.top + glyph.bounds.height;

		float u1 = static_cast<float>(glyph.textureRect.left);
		float v1 = static_cast<float>(glyph.textureRect.top);
		float u2 = static_cast<float>(glyph.textureRect.left + glyph.textureRect.width);
		float v2 = static_cast<float>(glyph.textureRect.top + glyph.textureRect.height);
		vertices.emplace_back(sf::Vector2f(position.x + left - italic * top - outlineThickness, position.y + top - outlineThickness), color, sf::Vector2f(u1, v1));
		vertices.emplace_back(sf::Vector2f(position.x + right - italic * top - outlineThickness, position.y + top - outlineThickness), color, sf::Vector2f(u2, v1));
		vertices.emplace_back(sf::Vector2f(position.x + left - italic * bottom - outlineThickness, position.y + bottom - outlineThickness), color, sf::Vector2f(u1, v2));
		vertices.emplace_back(sf::Vector2f(position.x + left - italic * bottom - outlineThickness, position.y + bottom - outlineThickness), color, sf::Vector2f(u1, v2));
		vertices.emplace_back(sf::Vector2f(position.x + right - italic * top - outlineThickness, position.y + top - outlineThickness), color, sf::Vector2f(u2, v1));
		vertices.emplace_back(sf::Vector2f(position.x + right - italic * bottom - outlineThickness, position.y + bottom - outlineThickness), color, sf::Vector2f(u2, v2));
	}
}
SmartText::SmartText(const sf::String& text, const sf::Font& font)
	: m_font(&font),
	m_vertices(sf::PrimitiveType::Triangles)
{
	setString(text);
}

SmartText::SmartText()
{

}

SmartText::~SmartText()
{
}

void SmartText::setStyle(sf::Uint32 style)
{
	setStyle(0U, m_string.getSize(), style);
}

void SmartText::setStyle(std::size_t start, sf::Uint32 style)
{
	setStyle(start, m_string.getSize() - start, style);
}

void SmartText::setStyle(std::size_t start, std::size_t length, sf::Uint32 style)
{
	ChunkData chunk(length);
	chunk.stylize(style);

	replaceChunk(start, chunk);
}

void SmartText::setFillColor(sf::Color color)
{
	setFillColor(0U, m_string.getSize(), color);
}

void SmartText::setFillColor(std::size_t start, sf::Color color)
{
	setFillColor(start, m_string.getSize() - start, color);
}

void SmartText::setFillColor(std::size_t start, std::size_t length, sf::Color color)
{
	ChunkData chunk(length);
	chunk.fill(color);

	replaceChunk(start, chunk);
}

void SmartText::setFont(const sf::Font& font)
{
	setFont(0U, m_string.getSize(), font);
}

void SmartText::setFont(std::size_t start, const sf::Font& font)
{
	setFont(start, m_string.getSize() - start, font);
}

void SmartText::setFont(std::size_t start, std::size_t length, const sf::Font& font)
{
	ChunkData chunk(length);
	chunk.fontType(font);

	replaceChunk(start, chunk);
}

void SmartText::setCharacterSize(sf::Uint32 charSize)
{
	setCharacterSize(0U, m_string.getSize(), charSize);
}

void SmartText::setCharacterSize(std::size_t start, sf::Uint32 charSize)
{
	setCharacterSize(start, m_string.getSize() - start, charSize);
}

void SmartText::setCharacterSize(std::size_t start, std::size_t length, sf::Uint32 charSize)
{
	ChunkData chunk(length);
	chunk.charSize(charSize);

	replaceChunk(start, chunk);
}

void SmartText::setProperties(const ChunkData& data)
{
	setProperties(0, m_string.getSize(), data);
}

void SmartText::setProperties(std::size_t start, const ChunkData& data)
{
	setProperties(0, m_string.getSize() - start, data);
}

void SmartText::setProperties(std::size_t start, std::size_t length, const ChunkData& data)
{

	replaceChunk(start, data);
}

void SmartText::setString(const sf::String& text)
{
	m_string = "";
	insert(text, 0);
}

void SmartText::setHighlight(sf::Color color)
{
	setHighlight(0U, m_string.getSize(), color);
}

void SmartText::setHighlight(std::size_t start, sf::Color color)
{
	setHighlight(start, m_string.getSize() - start, color);
}

void SmartText::setHighlight(std::size_t start, std::size_t length, sf::Color color)
{

}

void SmartText::removeHighlights(std::size_t start)
{
	removeHighlights(start, m_string.getSize() - start);
}

void SmartText::removeHighlights(std::size_t start, std::size_t length)
{

}

void SmartText::removeHighlights()
{
	removeHighlights(0U, m_string.getSize());
}

bool SmartText::isHighlighted(std::size_t start) const
{
	return isHighlighted(start, m_string.getSize());
}

bool SmartText::isHighlighted(std::size_t start, std::size_t length) const
{
	for (std::size_t index = getChunkIndex(start), end = getChunkIndex(start + length); index != end; ++index) {
		//if (m_chunks[index].highlight == nullptr) {
			return false;
		//}
	}
	return true;
}

const sf::String& SmartText::getString() const
{
	return m_string;
}

const  SmartText::Chunk& SmartText::getChunk(std::size_t index) const
{
	return m_chunks[index];
}

const std::size_t SmartText::getChunkIndex(std::size_t subIndex) const
{
	const std::size_t chunkSize = m_chunks.size();
	std::size_t chunkIndex = 0;

	for (std::size_t chunk = 0; chunk != chunkSize; ++chunk) {
		if (subIndex < chunkIndex + m_chunks[chunk].length) {
			return chunk;
		}
		else {
			chunkIndex += m_chunks[chunk].length;
		}
	}
	return static_cast<std::size_t>(-1);
}

sf::Vector2f SmartText::findLocalCharacterPos(std::size_t subIndex) const
{
	sf::Vector2f position;

	std::size_t offset = 0U;
	sf::Uint32 previous = 0U;
	for (const auto& chunk : m_chunks) {
		const sf::Font* font = chunk.font;
		const sf::Uint32 characterSize = chunk.characterSize;
		const bool bold = (chunk.style & sf::Text::Style::Bold) != 0;
		const float space = font->getGlyph(L' ', characterSize, bold).advance;
		const float vSpace = font->getLineSpacing(characterSize);

		for (std::size_t index = 0U; index != chunk.length; ++index) {
			const auto& current = m_string[offset + index];
			position.x += font->getKerning(previous, current, characterSize);

			switch (current)
			{
			case ' ':
				position.x = space;
				break;
			case '\t':
				position.x += space * 4.f;
				break;
			case '\n':
				position.y += vSpace;
				break;
			default:
				position.x += font->getGlyph(current, characterSize, bold).advance;
				break;
			}
			previous = current;
		}
		offset += chunk.length;
	}
	return position;
}


sf::FloatRect SmartText::getLocalBounds() const
{
	ensureGeometryUpdate();
	return m_bounds;
}

sf::FloatRect SmartText::getGlobalBounds() const
{
	ensureGeometryUpdate();
	return getTransform().transformRect(m_bounds);
}

sf::Vector2f SmartText::findGlobalCharacterPos(std::size_t subIndex) const
{
	return getTransform().transformPoint(findLocalCharacterPos(subIndex));
}

void SmartText::insert(const sf::String& text, std::size_t index, bool left)
{

}

void SmartText::insert(const sf::String& text, std::size_t index)
{
	//first in last out container
	m_string.insert(index, text);

	insertChunk(index, Chunk(index, text.getSize(), m_font));
	m_needsUpdate = true;
}

void SmartText::erase(std::size_t start, std::size_t length)
{
	m_string.erase(start, length);

	eraseChunk(start, length);
}

void SmartText::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (m_string.isEmpty()) {
		return;
	}
	ensureGeometryUpdate();
	states.transform *= getTransform();

	if (m_vertices.empty()) {
		return;
	}

	const sf::Vertex* vertices = &m_vertices.front();
	const sf::Font* font = m_chunks.front().font;
	std::size_t characterSize = m_chunks.front().characterSize;
	std::size_t previous = 0;
	std::size_t vertexLength = 0;

	for (auto& chunk : m_chunks) {
		if (font != chunk.font || characterSize != chunk.characterSize) {
			states.texture = &font->getTexture(characterSize);
			target.draw(vertices + previous, vertexLength, sf::PrimitiveType::Triangles, states);
			font = chunk.font;
			characterSize = chunk.characterSize;
			previous += vertexLength;
			vertexLength = chunk.verticeLength;
		}
		else {
			vertexLength += chunk.verticeLength;
		}
	}
	states.texture = &font->getTexture(characterSize);
	target.draw(vertices + previous, vertexLength, sf::PrimitiveType::Triangles, states);
}

void SmartText::eraseChunk(std::size_t subIndex, std::size_t length)
{
	if (m_chunks.empty()) {
		return;
	}
	const std::size_t start = getChunkIndex(subIndex);
	const std::size_t end = getChunkIndex(subIndex + length);
	auto chunkIter = m_chunks.begin() + end;

	if (start != end) {
		m_chunks[start].length -= (m_chunks[start].index + m_chunks[start].length) - subIndex;
		m_chunks[end].length -= m_chunks[end].length - ((m_chunks[end].index + m_chunks[start].length) - (subIndex + length));
		m_chunks[end].index = subIndex;
		chunkIter = m_chunks.erase(m_chunks.begin() + start + (m_chunks[start].length != 0), chunkIter);
		if (chunkIter != m_chunks.end()) {
			++chunkIter;
		}
	}

	const auto endChunk = m_chunks.end();
	for (; chunkIter != endChunk; ++chunkIter) {
		chunkIter->index -= length;
	}
	updateChunks(start);
	m_needsUpdate = true;
}

void SmartText::insertChunk(std::size_t subIndex, const Chunk& chunk)
{
	m_needsUpdate = true;

	if (m_chunks.empty()) {
		m_chunks.emplace_back(chunk);
		return;
	}
	const std::size_t start = getChunkIndex(subIndex);
	auto chunkIter = m_chunks.begin() + start;
	if (chunk == m_chunks[start]) {
		m_chunks[start].length += chunk.length;
		++chunkIter;
	}
	else {
		const std::size_t splicedSize = (chunkIter->index + chunkIter->length) - subIndex;
		Chunk splicedChunk = *chunkIter;
		chunkIter->length -= splicedSize;
		splicedChunk.index = chunkIter->index + chunk.length + chunkIter->length;
		splicedChunk.length = splicedSize;

		if (chunkIter->length == 0) {
			*chunkIter = chunk;
			++chunkIter;
			m_chunks.insert(chunkIter++, splicedChunk);
		}
		else {
			++chunkIter;
			m_chunks.insert(chunkIter++, chunk);
			m_chunks.insert(chunkIter++, splicedChunk);
		}
	}
	const auto endChunk = m_chunks.end();
	for (; chunkIter != endChunk; ++chunkIter) {
		chunkIter->index += chunk.length;
	}
}

void SmartText::replaceChunk(std::size_t subIndex, const ChunkData& chunkData)
{
	const std::size_t start = getChunkIndex(subIndex);
	const std::size_t end = getChunkIndex(subIndex + chunkData.length);

	auto chunkIter = m_chunks.begin() + start;
	const auto startChunk = *chunkIter;
	const auto endChunk = end != -1 ? m_chunks[end] : startChunk;
	const auto splicedSize = (chunkIter->index + chunkIter->length) - subIndex;

	std::size_t offset = 0;
	if (splicedSize != startChunk.length) {
		chunkIter->length -= splicedSize;
		chunkIter = m_chunks.insert(chunkIter + 1, startChunk);
		chunkIter->index = subIndex;
		++offset;
	}
	if (chunkData.length < splicedSize) {
		chunkIter->length = chunkData.length;
		chunkIter = m_chunks.insert(chunkIter + 1, startChunk);
		chunkIter->index = subIndex + chunkData.length;
		chunkIter->length = (startChunk.index + startChunk.length) - (subIndex + chunkData.length);
		++offset;
	}
	else {
		chunkIter->length = splicedSize;
	}
	if (end != -1 && start != end && subIndex + chunkData.length != endChunk.index) {
		const std::size_t endSplice = (endChunk.index + endChunk.length) - (subIndex + chunkData.length);
		chunkIter = m_chunks.insert(chunkIter + (end - start), endChunk); //std::prev(m_chunks.begin() + end + offset)
		chunkIter->length -= endSplice;

		auto endIter = std::next(chunkIter);
		endIter->index += chunkIter->length;
		endIter->length = endSplice;
		++offset;
	}

	const bool font = chunkData.font != nullptr;
	for (std::size_t index = start + (splicedSize != startChunk.length), stop = (end == -1 ? m_chunks.size() : end + offset); index < stop; ++index) {
		auto& chunk = m_chunks[index];
		if (font) {
			chunk.font = chunkData.font;
		}
		chunk.outlineThickness = chunkData.outlineThickness.value_or(chunk.outlineThickness);
		chunk.characterSize = chunkData.characterSize.value_or(chunk.characterSize);
		chunk.outlineColor = chunkData.outlineColor.value_or(chunk.outlineColor);
		chunk.fillColor = chunkData.fillColor.value_or(chunk.fillColor);
		chunk.style = chunkData.style.value_or(chunk.style);
	}
	updateChunks(start);
	m_needsUpdate = true;
}

void SmartText::updateChunks(std::size_t start)
{
	auto last = m_chunks.end();
	auto first = m_chunks.begin() + start + 1;
	auto result = first;
	auto previous = std::prev(result);

	while (first != last) {
		if (*previous != *first) {
			if (first != result) {
				*result = std::move(*first);
			}
			previous = result;
			++result;
		}
		else {
			previous->length += first->length;
		}
		++first;
	}

	m_chunks.erase(result, m_chunks.end());
}

SmartText::Chunk::Chunk()
	: Chunk(static_cast<int>(-1), 0)
{
}

SmartText::Chunk::Chunk(std::size_t index_, std::size_t length_, const sf::Font* font_)
	: index(index_),
	length(length_),
	fillColor(sf::Color::White),
	outlineColor(sf::Color::Black),
	style(sf::Text::Style::Regular),
	characterSize(18),
	outlineThickness(0.f),
	font(font_)
{
}

SmartText::Chunk::Chunk(const Chunk& chunk) noexcept
	: index(chunk.index),
	length(chunk.length),
	verticeLength(chunk.verticeLength),
	fillColor(chunk.fillColor),
	outlineColor(chunk.outlineColor),
	lineColor(chunk.lineColor),
	style(chunk.style),
	characterSize(chunk.characterSize),
	font(chunk.font),
	outlineThickness(chunk.outlineThickness)
{
	
}

SmartText::Chunk::Chunk(Chunk&& chunk) noexcept
	: index(std::move(chunk.index)),
	length(std::move(chunk.length)),
	verticeLength(std::move(chunk.verticeLength)),
	fillColor(std::move(chunk.fillColor)),
	outlineColor(std::move(chunk.outlineColor)),
	lineColor(std::move(chunk.lineColor)),
	style(std::move(chunk.style)),
	characterSize((chunk.characterSize)),
	font(std::move(chunk.font)),
	outlineThickness(std::move(chunk.outlineThickness))
{

}

SmartText::Chunk& SmartText::Chunk::operator=(const Chunk& chunk) noexcept
{
	index = chunk.index;
	length = chunk.length;
	verticeLength = chunk.verticeLength;
	fillColor = chunk.fillColor;
	outlineColor = chunk.outlineColor;
	lineColor = chunk.lineColor;
	style = chunk.style;
	characterSize = chunk.characterSize;
	font = chunk.font;
	outlineThickness = chunk.outlineThickness;

	return *this;
}

SmartText::Chunk& SmartText::Chunk::operator=(Chunk&& chunk) noexcept
{
	index = std::move(chunk.index);
	length = std::move(chunk.length);
	verticeLength = std::move(chunk.verticeLength);
	fillColor = std::move(chunk.fillColor);
	outlineColor = std::move(chunk.outlineColor);
	lineColor = std::move(chunk.lineColor);
	style = std::move(chunk.style);
	characterSize = std::move(chunk.characterSize);
	font = std::move(chunk.font);
	outlineThickness = std::move(chunk.outlineThickness);

	return *this;
}

bool SmartText::Chunk::operator==(const Chunk& chunk) const
{
	return fillColor == chunk.fillColor && outlineColor == outlineColor &&
		style == chunk.style && characterSize == chunk.characterSize && font == chunk.font;
}

bool SmartText::Chunk::operator!=(const Chunk& chunk) const
{
	return !(this->operator==(chunk));
}

void SmartText::ensureGeometryUpdate() const
{
	// Do nothing, if geometry has not changed
	if (!m_needsUpdate)
		return;

	// Mark geometry as updated
	m_needsUpdate = false;

	// Clear the previous geometry
	m_vertices.clear();
	m_outlineVertices.clear();
	m_bounds = sf::FloatRect();

	if (m_string.isEmpty()) {
		return;
	}
	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();
	float maxX = 0.f;
	float maxY = 0.f;

	float x = 0.f;
	float y = static_cast<float>(std::max_element(m_chunks.cbegin(), m_chunks.cend(), [](const Chunk& left, const Chunk& right) {
		return left.characterSize < right.characterSize;
		})->characterSize);

	std::size_t offset = 0U;
	sf::Uint32 verticeOffset = 0U;
	sf::Uint32 prevChar = 0U;
	float previousX = 0.f;
	for (auto& chunk : m_chunks) {
		// No font or text: nothing to draw
		if (!chunk.font)
			continue;

		// Compute values related to the text style
		const bool bold = (chunk.style & sf::Text::Style::Bold) != 0;
		const bool underlined = (chunk.style & sf::Text::Style::Underlined) != 0;
		const bool strikeThrough = (chunk.style & sf::Text::Style::StrikeThrough) != 0;
		const float italic = (chunk.style & sf::Text::Style::Italic) ? 0.208f : 0.f; // 12 degrees
		const float underlineOffset = chunk.font->getUnderlinePosition(chunk.characterSize);
		const float underlineThickness = chunk.font->getUnderlineThickness(chunk.characterSize);

		// Compute the location of the strike through dynamically
		// We use the center point of the lowercase 'x' glyph as the reference
		// We reuse the underline thickness as the thickness of the strike through as well
		sf::FloatRect xBounds = chunk.font->getGlyph(L'x', chunk.characterSize, bold).bounds;
		float strikeThroughOffset = xBounds.top + xBounds.height / 2.f;

		// Precompute the variables needed by the algorithm
		float hspace = static_cast<float>(chunk.font->getGlyph(L' ', chunk.characterSize, bold).advance);
		float vspace = static_cast<float>(chunk.font->getLineSpacing(chunk.characterSize));

		// Create one quad for each character
		minX = std::min(minX, static_cast<float>(chunk.characterSize));
		minY = std::min(minY, static_cast<float>(chunk.characterSize));
		for (std::size_t i = 0U; i != chunk.length; ++i)
		{
			sf::Uint32 curChar = m_string[offset + i];

			// Apply the kerning offset
			x += chunk.font->getKerning(prevChar, curChar, chunk.characterSize);
			prevChar = curChar;

			// If we're using the underlined style and there's a new line, draw a line
			if (curChar == L'\n')
			{
				minX = std::min(minX, x);
				minY = std::min(minY, y);

				if (underlined) {
					addLine(previousX, m_vertices, x - previousX, y, chunk.fillColor, underlineOffset, underlineThickness);

					if (chunk.outlineThickness != 0) {
						addLine(previousX, m_outlineVertices, x - previousX, y, chunk.outlineColor, underlineOffset, underlineThickness, chunk.outlineThickness);
					}
				}
				if (strikeThrough)
				{
					addLine(previousX, m_vertices, x - previousX, y, chunk.fillColor, strikeThroughOffset, underlineThickness);

					if (chunk.outlineThickness != 0) {
						addLine(previousX, m_outlineVertices, x - previousX, y, chunk.outlineColor, strikeThroughOffset, underlineThickness, chunk.outlineThickness);
					}
				}
				y += vspace;
				x = 0.f;
				previousX = 0.f;
				maxX = std::max(maxX, x);
				maxY = std::max(maxY, y);

				continue;
			}
			// Handle special characters
			else if ((curChar == ' ') || (curChar == '\t'))
			{
				// Update the current bounds (min coordinates)
				minX = std::min(minX, x);
				minY = std::min(minY, y);

				switch (curChar)
				{
				case ' ':  x += hspace;        break;
				case '\t': x += hspace * 4;    break;
				}

				// Update the current bounds (max coordinates)
				maxX = std::max(maxX, x);
				maxY = std::max(maxY, y);

				// Next glyph, no need to create a quad for whitespace
				continue;
			}

			const sf::Glyph& glyph = chunk.font->getGlyph(curChar, chunk.characterSize, bold, chunk.outlineThickness);
			const float left = glyph.bounds.left;
			const float top = glyph.bounds.top;
			const float right = glyph.bounds.left + glyph.bounds.width;
			const float bottom = glyph.bounds.top + glyph.bounds.height;

			// Apply the outline
			if (chunk.outlineThickness != 0)
			{
				// Add the outline glyph to the vertices
				addGlyphQuad(m_outlineVertices, sf::Vector2f(x, y), chunk.outlineColor, glyph, italic, chunk.outlineThickness);

				// Update the current bounds with the outlined glyph bounds
				minX = std::min(minX, x + left - italic * bottom - chunk.outlineThickness);
				maxX = std::max(maxX, x + right - italic * top - chunk.outlineThickness);
				minY = std::min(minY, y + top - chunk.outlineThickness);
				maxY = std::max(maxY, y + bottom - chunk.outlineThickness);
				addGlyphQuad(m_vertices, sf::Vector2f(x, y), chunk.fillColor, glyph, italic);
			}
			else {
				// Update the current bounds with the non outlined glyph bounds
				addGlyphQuad(m_vertices, sf::Vector2f(x, y), chunk.fillColor, glyph, italic);
				minX = std::min(minX, x + left - italic * bottom);
				maxX = std::max(maxX, x + right - italic * top);
				minY = std::min(minY, y + top);
				maxY = std::max(maxY, y + bottom);
			}

			// Extract the current glyph's description

			// Add the glyph to the vertices
			addGlyphQuad(m_vertices, sf::Vector2f(x, y), chunk.fillColor, glyph, italic);
			// Advance to the next character
			x += glyph.advance;
		}
		offset += chunk.length;
		// If we're using the underlined style, add the last line
		if (underlined && (x > 0))
		{
			addLine(previousX, m_vertices, x - previousX, y, chunk.fillColor, underlineOffset, underlineThickness);

			if (chunk.outlineThickness != 0)
				addLine(previousX, m_outlineVertices, x - previousX, y, chunk.outlineColor, underlineOffset, underlineThickness, chunk.outlineThickness);
		}

		// If we're using the strike through style, add the last line across all characters
		if (strikeThrough && (x > 0))
		{
			addLine(previousX, m_vertices, x - previousX, y, chunk.fillColor, strikeThroughOffset, underlineThickness);

			if (chunk.outlineThickness != 0)
				addLine(previousX, m_outlineVertices, x - previousX, y, chunk.outlineColor, strikeThroughOffset, underlineThickness, chunk.outlineThickness);
		}
		chunk.verticeLength = m_vertices.size() - verticeOffset;
		verticeOffset = m_vertices.size();
		previousX = x;
	}

	// Update the bounding rectangle
	m_bounds.left = minX;
	m_bounds.top = minY;
	m_bounds.width = maxX - minX;
	m_bounds.height = maxY - minY;
}

SmartText::ChunkData::ChunkData(std::size_t length)
	:length(length),
	font(nullptr) {
}

SmartText::ChunkData& SmartText::ChunkData::fill(sf::Color color)
{
	fillColor = color;
	return *this;
}

SmartText::ChunkData& SmartText::ChunkData::outline(sf::Color color, float thickness)
{
	outlineColor = color;
	outlineThickness.emplace(thickness);
	return *this;
}

SmartText::ChunkData& SmartText::ChunkData::stylize(sf::Uint32 style_)
{
	style.emplace(style_);
	return *this;
}

SmartText::ChunkData& SmartText::ChunkData::fontType(const sf::Font& font_)
{
	font = &font_;
	return *this;
}

SmartText::ChunkData& SmartText::ChunkData::charSize(sf::Uint32 size)
{
	characterSize.emplace(size);
	return *this;
}

SmartText::ChunkData& SmartText::ChunkData::highlighter(sf::Color color)
{
	highlight.emplace(color);
	return *this;
}

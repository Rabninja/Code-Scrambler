#pragma once

#ifndef MGUI_SMART_TEXT_H
#define MGUI_SMART_TEXT_H

#include <SFML\Graphics\Text.hpp>
#include <vector>
#include <optional>


class SmartText : public sf::Drawable, public sf::Transformable
{
public:
	struct Chunk {
		std::size_t index;
		std::size_t length;
		std::size_t verticeLength;
		sf::Color fillColor;
		sf::Color outlineColor;
		sf::Color lineColor;
		sf::Uint32 style;
		sf::Uint32 characterSize;
		const sf::Font* font;
		float outlineThickness;

		Chunk();

		Chunk(std::size_t index_, std::size_t length_, const sf::Font* font_ = nullptr);

		Chunk(const Chunk& chunk) noexcept;

		Chunk(Chunk&& chunk) noexcept;

		Chunk& operator=(const Chunk& chunk) noexcept;

		Chunk& operator=(Chunk&& chunk) noexcept;

		bool operator==(const Chunk& chunk) const;

		bool operator!=(const Chunk& chunk) const;
	};

	struct ChunkData {
		std::size_t length;
		const sf::Font* font;
		std::optional<sf::Color> fillColor;
		std::optional<sf::Color> outlineColor;
		std::optional<sf::Color> lineColor;
		std::optional<sf::Uint32> style;
		std::optional<sf::Uint32> characterSize;
		std::optional<float> outlineThickness;
		std::optional<sf::Color> highlight;

		ChunkData(std::size_t length);
		ChunkData() = default;
		ChunkData(ChunkData&&) = default;

		ChunkData& fill(sf::Color color);

		ChunkData& outline(sf::Color color, float thickness = 0.f);

		ChunkData& stylize(sf::Uint32 style_);

		ChunkData& fontType(const sf::Font& font_);

		ChunkData& charSize(sf::Uint32 size);

		ChunkData& highlighter(sf::Color color);
	};
private:
	//Deque for text objects and one whole string
	//Deque for text Data objects to hold information and one whole vertex array
	mutable bool m_needsUpdate;
	const sf::Font* m_font;
	sf::String m_string;
	mutable sf::FloatRect m_bounds;
	mutable std::vector<Chunk> m_chunks;
	mutable std::vector<sf::Vertex> m_vertices;
	mutable std::vector<sf::Vertex> m_outlineVertices;
public:
	SmartText(const sf::String& text, const sf::Font& font);
	SmartText();
	~SmartText();

	void setStyle(sf::Uint32 style);

	void setStyle(std::size_t start, sf::Uint32 style);

	void setStyle(std::size_t start, std::size_t length, sf::Uint32 style);

	void setFillColor(sf::Color color);

	void setFillColor(std::size_t start, sf::Color color);

	void setFillColor(std::size_t start, std::size_t length, sf::Color color);

	void setFont(const sf::Font& font);

	void setFont(std::size_t start, const sf::Font& font);

	void setFont(std::size_t start, std::size_t length, const sf::Font& font);

	void setCharacterSize(sf::Uint32 textSize);

	void setCharacterSize(std::size_t start, sf::Uint32 textSize);

	void setCharacterSize(std::size_t start, std::size_t length, sf::Uint32 textSize);

	void setProperties(const ChunkData& data);

	void setProperties(std::size_t start, const ChunkData& data);

	void setProperties(std::size_t start, std::size_t length, const ChunkData& data);

	void setString(const sf::String& text);

	void setHighlight(sf::Color color);

	void setHighlight(std::size_t start, sf::Color color);

	void setHighlight(std::size_t start, std::size_t length, sf::Color color);

	void removeHighlights(std::size_t start);

	void removeHighlights(std::size_t start, std::size_t length);

	void removeHighlights();

	bool isHighlighted(std::size_t start) const;

	bool isHighlighted(std::size_t start, std::size_t length) const;

	const sf::String& getString() const;

	sf::FloatRect getLocalBounds() const;

	sf::FloatRect getGlobalBounds() const;

	const Chunk& getChunk(std::size_t index) const;

	const std::size_t getChunkIndex(std::size_t subIndex) const;

	sf::Vector2f findLocalCharacterPos(std::size_t subIndex) const;

	sf::Vector2f findGlobalCharacterPos(std::size_t subIndex) const;

	void insert(const sf::String& text, std::size_t index, bool left);

	void insert(const sf::String& text, std::size_t index);

	void erase(std::size_t start, std::size_t length = 1);

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void ensureGeometryUpdate() const;

	void updateChunks(std::size_t start);

	void insertChunk(std::size_t subIndex, const Chunk& chunk);

	void replaceChunk(std::size_t subIndex, const ChunkData& chunk);

	void eraseChunk(std::size_t subIndex, std::size_t length);
};


#endif
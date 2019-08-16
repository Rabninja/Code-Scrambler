#include <SFML/Graphics.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "Scrambler.h"
#include "Highlighter.h"
#include "Utilities.h"
#include "CodeState.h"

struct Settings
{
	std::vector<std::filesystem::path> files;
	std::string fontpath;
	int difficulty;
	int ppi;
	int borders;
	int fit;

	Settings(int argc, const char* argv[])
	{
		pushCodeState("Processing and loading arguments.");
		//std::cout << "[STARTING]: Processing and loading arguments..." << std::endl;
		std::string filepath = getCmdOption(argv, argv + argc, "-file");
		if (filepath.empty()) {
			sf::err() << "ERROR: No filepath passed." << std::endl;
			exitPrompt();
		}
		
		if (!std::filesystem::exists(filepath)) {
			sf::err() << "ERROR: Filepath to code doesn't exist." << std::endl;
			exitPrompt();
		}
		if (std::filesystem::is_directory(filepath)) {
			for (auto& file : std::filesystem::directory_iterator(filepath)) {
				if (file.is_regular_file()) {
					files.push_back(file);
				}
			}
		}
		else {
			files.push_back(filepath);
		}

		std::string argDiff = getCmdOption(argv, argv + argc, "-diff");
		difficulty = parseType<int>(argDiff).value_or(0);
		if (difficulty != 0 && difficulty != 1) {
			sf::err() << "ERROR: Difficulty must have a value of 0(tabs allowed), 1(aligned left)." << std::endl;
			exitPrompt();
		}


		std::string argPPI = getCmdOption(argv, argv + argc, "-ppi");
		ppi = parseType<int>(argPPI).value_or(300);

		fontpath = getCmdOption(argv, argv + argc, "-font");

		std::string argBorders = getCmdOption(argv, argv + argc, "-border");
		borders = parseType<int>(argBorders).value_or(0);
		if (borders > 2 || borders < 0) {
			sf::err() << "ERROR: borders must have a value of 0(no borders), 1(solid borders), 2(stripped borders)." << std::endl;
			exitPrompt();
		}
		std::string argFit = getCmdOption(argv, argv + argc, "-fit");
		fit = parseType<int>(argFit).value_or(1);
		if (fit < 1) {
			sf::err() << "ERROR: fit must be greater than 1." << std::endl;
			exitPrompt();
		}
		popCodeState();
	//	std::cout << "[COMPLETED]: Processing and loading arguments.\n" << std::endl;
	}
};

using ScrambledCode = std::vector<std::string>;

ScrambledCode createScrambling(const std::string& filepath, int difficulty);
void saveScrambling(const ScrambledCode& code, const std::string& file);

sf::Image renderScrambling(const ScrambledCode& code, Settings settings);
void fitImage(sf::Image& image, Settings settings);
void saveImage(const sf::Image& image, const std::string& file);



int main(int argc, const char* argv[])
{
	Settings settings(argc, argv);

	
	for (auto& file : settings.files) {
		system("cls");
		pushCodeState(file.filename().string());
		ScrambledCode scrambling = createScrambling(file.string(), settings.difficulty);
		saveScrambling(scrambling, file.filename().string());
		sf::Image image = renderScrambling(scrambling, settings);
		fitImage(image, settings);
		saveImage(image, file.filename().replace_extension().string());
		popCodeState();
	}
	std::cout << "Press enter to exit...";
	std::cin.get();
	return EXIT_SUCCESS;
}

ScrambledCode createScrambling(const std::string& filepath, int difficulty)
{
	pushCodeState("Scrambling the code.");
	Scrambler scrambler(filepath, difficulty);
	ScrambledCode scrambling = scrambler.getScrambling();
	popCodeState();
	return scrambling;
}

void saveScrambling(const ScrambledCode& code, const std::string& file)
{
	pushCodeState("Saving scramble to file.");
	std::string directory = "..\\code_";
	outputContainer(directory + file, code);
	popCodeState();
}

sf::Image renderScrambling(const ScrambledCode& scrambling, Settings settings)
{
	pushCodeState("Highlighting and rendering the scrambling.");
	sf::Font font;
	if (!font.loadFromFile(settings.fontpath)) {
		sf::err() << "ERROR: Couldn't load the requested font." << std::endl;
		exitPrompt();
	}

	constexpr float PAPER_WIDTH  = 8.50;
	constexpr float PAPER_HEIGHT = 11.0;

	const unsigned width  = PAPER_WIDTH  * settings.ppi;
	const unsigned height = PAPER_HEIGHT * settings.ppi / settings.fit;
	const float borderHeight = std::round(settings.borders ? settings.ppi / 40.f : 0.f);
	
	unsigned characterSize = 4;
	while (std::ceil((font.getLineSpacing(characterSize + 4) * 1.2f) + borderHeight) * scrambling.size() < height) {
		characterSize += 4;
	}
	const float spacing = std::ceil(font.getLineSpacing(characterSize) * 1.2f);

	sf::ContextSettings context;
	context.antialiasingLevel = 4;
	sf::RenderTexture texture;
	texture.create(width, height, context);
	texture.clear(sf::Color::Transparent);

	Highlighter highlighter;
	highlighter.setFont(font);
	highlighter.setCharacterSize(characterSize);

	
	
	float offset = 0.f;
	for (auto& line : scrambling) {
		auto text = highlighter.buildText(line);
		text.setPosition(0.f, offset + spacing / 2.f);
		centerY(text);
		texture.draw(text);
		if (settings.borders == 1) {
			sf::RectangleShape border({ (float)width, borderHeight });
			border.setFillColor(sf::Color::Black);
			border.setPosition(0.f, offset + spacing);
			texture.draw(border);
		}
		if (settings.borders == 2) {
			const float stripWidth = width / 48.f;

			for (std::size_t index = 0; index != 48; ++index) {
				sf::RectangleShape border({ stripWidth * 0.667f, borderHeight });
				border.setFillColor(sf::Color::Black);
				border.setPosition(index*stripWidth, offset + spacing);
				texture.draw(border);
			}
		}
		offset += spacing + borderHeight;

	}
	texture.display();
	popCodeState();
	return texture.getTexture().copyToImage();
}

void fitImage(sf::Image& image, Settings settings)
{

	pushCodeState("Fitting image.");
	constexpr float PAPER_WIDTH = 8.50;
	constexpr float PAPER_HEIGHT = 11.0;

	const unsigned width = PAPER_WIDTH * settings.ppi;
	const unsigned height = PAPER_HEIGHT * settings.ppi;

	sf::Image fitted;
	fitted.create(width, height);
	for (std::size_t index = 0; index != settings.fit; ++index) {
		fitted.copy(image, 0, image.getSize().y * index);
	}
	image = fitted;
	popCodeState();
}

void saveImage(const sf::Image& image, const std::string& file)
{
	pushCodeState("Saving scramble as an image.");
	std::string directory = "..\\image_";
	if (!image.saveToFile(directory + file + ".png")) {
		sf::err() << "ERROR: Couldn't save the image to it's destination." << std::endl;
		exitPrompt();
	}
	popCodeState();
}


#include "pch.h"
#include "Config.h"
#include "FileHandler.h"
#include "Parser.h"
#include "Utilities.h"

void Config::loadConfiguration()
{
	readOrCreateConfigurationFile();
	setVariables();
}

void Config::readOrCreateConfigurationFile()
{
	auto directoriesPath = std::filesystem::path(configFile).parent_path();
	
	//First check if directories exist
	if (!std::filesystem::exists(directoriesPath))
		std::filesystem::create_directories(directoriesPath); //create directories just in case

	if (!std::filesystem::exists(configFile)) //if config file doesn't exist
		return saveConfigToFile(windowSize, aaLevel, fullscreen, vSync); //then just save current config to file.

	readConfigFile();
}

void Config::saveConfigToFile(const sf::Vector2u& resSize, unsigned int antiAliasing, bool fullscreen_, bool vsync_)
{
	using namespace std::string_literals;
	const std::string widthStr  = "width"s        + delimiter + std::to_string(resSize.x);
	const std::string heightStr = "height"s       + delimiter + std::to_string(resSize.y);
	const std::string aaStr     = "antialiasing"s + delimiter + std::to_string(antiAliasing);
	const std::string fullStr   = "fullscreen"s   + delimiter + Utilities::boolToStr(fullscreen_);
	const std::string vsyncStr  = "vsync"s        + delimiter + Utilities::boolToStr(vsync_);

	FileHandler fileHandler;
	fileHandler.createAndSaveToFile(configFile, configWindowCategory, widthStr, heightStr, aaStr, fullStr, vsyncStr);
}

void Config::readConfigFile()
{
	FileHandler fileHandler;
	Parser parser;
	auto data = fileHandler.readFileContent(configFile);
	auto parsedData = parser.parseData(data);

	if (parsedData.empty())
		return;

	//Parsed data should contain only 1 node
	const auto& front = parsedData[0];
	if (front.category == configWindowCategory)
	{
		//parse data of this category.
		for (const auto& [key, val] : front.data)
		{
			if (key == "width")
				windowSize.x = std::stoi(val);
			else if (key == "height")
				windowSize.y = std::stoi(val);
			else if (key == "antialiasing")
				aaLevel = std::stoi(val);
			else if (key == "fullscreen")
				fullscreen = Utilities::strToBool(val);
			else if (key == "vsync")
				vSync = Utilities::strToBool(val);
		}
	}
}

void Config::setVariables()
{
	constexpr float xHPBarMod = 0.27f;
	constexpr float yHPBarMod = 0.037f;
	constexpr float xManaBarMod = 0.2f;
	constexpr float yManaBarMod = 0.03f;

	hpBarPlayerSize = sf::Vector2f{ fWindowSize.x * xHPBarMod, fWindowSize.y * yHPBarMod };
	manaBarSize = sf::Vector2f{ fWindowSize.x * xManaBarMod, fWindowSize.y * yManaBarMod };

	fWindowSize.x = static_cast<float>(windowSize.x);
	fWindowSize.y = static_cast<float>(windowSize.y);
}

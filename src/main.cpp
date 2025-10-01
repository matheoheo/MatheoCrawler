#include "pch.h"
#include "Engine.h"
#include "Config.h"

int main()
{
	Config::loadConfiguration();
	sf::ContextSettings settings;
	settings.antiAliasingLevel = Config::aaLevel;

	Engine engine(settings);
	engine.preloadAssets();
	engine.start();

	return 0;
}


#include "pch.h"
#include "Config.h"

void Config::loadConfiguration()
{
	//load configuration data

	float xAxisMod = 0.27f;
	float yAxisMod = 0.037f;
	hpBarPlayerSize = sf::Vector2f{ fWindowSize.x * xAxisMod, fWindowSize.y * yAxisMod };
	manaBarSize = sf::Vector2f{ fWindowSize.x * 0.20f, fWindowSize.y * 0.03f };
}

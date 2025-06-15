#include "pch.h"
#include "Config.h"

void Config::loadConfiguration()
{
	//load configuration data

	float xAxisMod = 0.27f;
	float yAxisMod = 0.035f;
	hpBarPlayerSize = sf::Vector2f{ fWindowSize.x * xAxisMod, fWindowSize.y * yAxisMod };
}

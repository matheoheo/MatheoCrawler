#include "pch.h"
#include "Engine.h"
#include "Config.h"

int main()
{
	Config::loadConfiguration();
	Engine engine;
	engine.preloadAssets();
	engine.start();
}


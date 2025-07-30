#pragma once
#include <SFML/Graphics.hpp>
#include "AssetManager.h"
#include "EventManager.h"
#include "TextureIdentifiers.h"
#include "FontIdentifiers.h"

struct GameContext
{
	sf::RenderWindow& window;
	AssetManager<TextureIdentifier, sf::Texture>& textures;
	AssetManager<FontIdentifiers, sf::Font>& fonts;
	EventManager& eventManager;

	GameContext(sf::RenderWindow& window, AssetManager<TextureIdentifier, sf::Texture>& textures,
		AssetManager<FontIdentifiers, sf::Font>& fonts, EventManager& eventManager)
		:window(window),
		textures(textures),
		fonts(fonts),
		eventManager(eventManager)
	{

	}
};
#pragma once
#include "GameContext.h"
#include "StateManager.h"

class Engine
{
public:
	Engine(const sf::ContextSettings& settings);
	void start();

	//loads all required textures before even displaying a window.
	void preloadAssets();
private:
	void processEvents();
	void update(const sf::Time& deltaTime);
	void render();


private:
	sf::RenderWindow mWindow;
	AssetManager<TextureIdentifier, sf::Texture> mTextures;
	AssetManager<FontIdentifiers, sf::Font> mFonts;
	EventManager mEventManager;
	GameContext mGameContext;
	StateManager mStateManager;
};


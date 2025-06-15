#include "pch.h"
#include "Engine.h"
#include "Config.h"

Engine::Engine()
	:mWindow(sf::VideoMode(Config::windowSize), "Matheo Crawler", sf::Style::Titlebar | sf::Style::Close,
		Config::fullscreen ? sf::State::Fullscreen : sf::State::Windowed),
	mGameContext(mWindow, mTextures, mFonts, mEventManager)
{
	preloadTextures();
	mStateManager = std::make_unique<GameState>(mGameContext);
}

void Engine::start()
{
	using namespace std::chrono;

	sf::Clock fpsClock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	sf::Time timePerFrame(sf::seconds(1.f / 60.f));

	while (mWindow.isOpen())
	{
		timeSinceLastUpdate += fpsClock.restart();

		auto start = steady_clock::now();

		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents();
			update(timePerFrame);
		}
		render();
		auto end = steady_clock::now();
		auto duration = duration_cast<milliseconds>(end - start);
		auto t = duration.count();
		if(t >= 15)
			std::cout << "Function took " << duration.count() << " ms\n";
	}
}

void Engine::processEvents()
{
	while (const auto event = mWindow.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
			mWindow.close();
		mStateManager->processEvents(event.value());
	}
}

void Engine::update(const sf::Time& deltaTime)
{
	mStateManager->update(deltaTime);
}

void Engine::render()
{
	mWindow.clear();
	mStateManager->render();
	mWindow.display();
}

void Engine::preloadTextures()
{
	mTextures.load(TextureIdentifier::MattLogo, "assets/logo/mattGamesLogo.jpeg");
	mTextures.load(TextureIdentifier::Player, "assets/entities/player/player.png");
	mTextures.load(TextureIdentifier::Skletorus, "assets/entities/skeleton_axe/skeleton.png");
	mTextures.load(TextureIdentifier::Suash, "assets/ui/suash/suashIcon.png");
	mTextures.load(TextureIdentifier::Twarf, "assets/ui/twarf/twarfIcon.png");

	mFonts.load(FontIdentifiers::Default, "assets/font/dungeonFont.ttf");
}

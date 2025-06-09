#include "pch.h"
#include "Engine.h"
#include "Config.h"

Engine::Engine()
	:mWindow(sf::VideoMode(Config::windowSize), "Matheo Crawler"),
	mGameContext(mWindow, mTextures, mFonts, mEventManager)
{
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

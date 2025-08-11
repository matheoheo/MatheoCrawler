#include "pch.h"
#include "LoadingState.h"
#include "Config.h"
#include "Utilities.h"

LoadingState::LoadingState(GameContext& gameContext, LoaderTaskVec taskQueue)
	:IState(gameContext),
	mLoadingSprite(gameContext.textures.get(TextureIdentifier::MattLogo)),
	mLoadingText(gameContext.fonts.get(FontIdentifiers::Default)),
	mStringIndex(0),
	mElaspedFramesCount(0),
	mLoadingFinished(false),
	mTasks(std::move(taskQueue))
{
	defaultView = sf::View(Config::fWindowSize * 0.5f, Config::fWindowSize);
	mGameContext.window.setView(defaultView);
	createLoadingScreen();
}

void LoadingState::onEnter()
{
	mLoadingThread = std::jthread([this]()
		{
			runTasks();
			mLoadingFinished = true;
		});
}

void LoadingState::processEvents(sf::Event event)
{
}

void LoadingState::update(const sf::Time& deltaTime)
{
	updateLoadingScreen();

	if (mLoadingFinished)
	{
		mGameContext.eventManager.notify<PopStateEvent>(PopStateEvent());
		return;
	}

}

void LoadingState::render()
{
	renderLoadingScreen();
}

void LoadingState::createLoadingScreen()
{
	const sf::Vector2f logoSize{ Config::fWindowSize.x * 0.2f, Config::fWindowSize.y * 0.2f };
	Utilities::scaleSprite(mLoadingSprite, Config::fWindowSize);

	constexpr float margin = 5.f;
	//change color of gameLogo so it is half transparent
	mLoadingStrings =
	{
		"Loading in progress",
		"Loading in progress.",
		"Loading in progress..",
		"Loading in progress..."
	};

	auto charSize = Config::getCharacterSize();
	mLoadingText.setCharacterSize(charSize);
	mLoadingText.setString(mLoadingStrings[mStringIndex]);
	mLoadingText.setPosition({ margin, Config::fWindowSize.y - charSize - margin });
}

void LoadingState::updateLoadingScreen()
{
	constexpr size_t frameMod = 20;
	constexpr size_t maxFrames = 1000;
	if (mElaspedFramesCount % frameMod == 0)
	{
		++mStringIndex;
		if (mStringIndex == mLoadingStrings.size())
			mStringIndex = 0;
		mLoadingText.setString(mLoadingStrings[mStringIndex]);
		if (mElaspedFramesCount >= maxFrames)
			mElaspedFramesCount = 0;
	}
	++mElaspedFramesCount;

}

void LoadingState::renderLoadingScreen()
{
	mGameContext.window.draw(mLoadingSprite);
	mGameContext.window.draw(mLoadingText);
}

void LoadingState::runTasks()
{
	for (const auto& task : mTasks)
		task();

	std::cout << "All tasks loaded\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(250));
}

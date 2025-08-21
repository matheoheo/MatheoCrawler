#include "pch.h"
#include "UIManager.h"
#include "Config.h"
#include "PlayerStatusUI.h"
#include "PlayerResourcesUI.h"
#include "MessageLogUI.h"
#include "ActionSelectionUI.h"
#include "PauseMenuUI.h"
#include "GameOverUI.h"

UIManager::UIManager(GameContext& gameContext)
	:mGameContext(gameContext),
	player(nullptr)
{
	registerToEvents();
	mUIView.setSize(Config::fWindowSize);
	mUIView.setCenter({ Config::fWindowSize.x * 0.5f, Config::fWindowSize.y * 0.5f });
}

void UIManager::processEvents(const sf::Event event)
{
	for (const auto& comp : mUIComponents)
	{
		comp->processEvents(event);
	}
	handleShopEvents(event);
}

void UIManager::update(const sf::Time& deltaTime)
{
	const sf::Vector2i mousePos{ sf::Mouse::getPosition(mGameContext.window) };
	const sf::Vector2f fMousePos{
		static_cast<float>(mousePos.x),
		static_cast<float>(mousePos.y)
	};

	for (const auto& comp : mUIComponents)
	{
		comp->update(fMousePos, deltaTime);
	}
	if (mShop)
		mShop->update(fMousePos, deltaTime);
}

void UIManager::render()
{
	mGameContext.window.setView(mUIView);
	for (const auto& comp : mUIComponents)
	{
		comp->render();
	}
	if (mShop)
		mShop->render();
}

void UIManager::setPlayer(Entity* entity)
{
	player = entity;
}

void UIManager::createUI()
{
	mUIComponents.emplace_back(std::make_unique<PlayerStatusUI>(mGameContext, *player));
	mUIComponents.emplace_back(std::make_unique<PlayerResourcesUI>(mGameContext, *player));
	mUIComponents.emplace_back(std::make_unique<MessageLogUI>(mGameContext, *player));
	mUIComponents.emplace_back(std::make_unique<ActionSelectionUI>(mGameContext, *player));
	mUIComponents.emplace_back(std::make_unique<PauseMenuUI>(mGameContext, *player));
	mUIComponents.emplace_back(std::make_unique<GameOverUI>(mGameContext, *player));
}

void UIManager::registerToEvents()
{
	registerToCloseShopEvent();
}

void UIManager::registerToCloseShopEvent()
{
	mGameContext.eventManager.registerEvent<CloseShopEvent>([this](const CloseShopEvent& data)
		{
			mShop = nullptr;
		});
}

void UIManager::handleShopEvents(const sf::Event event)
{
	if (auto data = event.getIf<sf::Event::KeyReleased>())
	{
		if (data->code == sf::Keyboard::Key::M)
		{
			if (mShop)
				mShop = nullptr;
			else
				mShop = std::make_unique<Shop>(mGameContext, *player);
		}
	}
	if (mShop)
		mShop->processEvents(event);
}

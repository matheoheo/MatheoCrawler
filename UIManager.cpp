#include "pch.h"
#include "UIManager.h"
#include "Config.h"
#include "PlayerStatusUI.h"
#include "PlayerResourcesUI.h"
#include "MessageLogUI.h"
#include "ActionSelectionUI.h"

UIManager::UIManager(GameContext& gameContext)
	:mGameContext(gameContext),
	player(nullptr)
{
	mUIView.setSize(Config::fWindowSize);
	mUIView.setCenter({ Config::fWindowSize.x * 0.5f, Config::fWindowSize.y * 0.5f });
}

void UIManager::processEvents(const sf::Event event)
{
	for (const auto& comp : mUIComponents)
	{
		comp->processEvents(event);
	}

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
}

void UIManager::update(const sf::Time& deltaTime)
{
	for (const auto& comp : mUIComponents)
	{
		comp->update({ 0.f, 0.f }, deltaTime);
	}
	if (mShop)
		mShop->update({ 0.f, 0.f }, deltaTime);
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
}

#include "pch.h"
#include "UIManager.h"
#include "Config.h"
#include "PlayerStatusUI.h"
#include "PlayerResourcesUI.h"
#include "MessageLogUI.h"

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
}

void UIManager::update(const sf::Time& deltaTime)
{
	for (const auto& comp : mUIComponents)
	{
		comp->update({ 0.f, 0.f }, deltaTime);
	}
}

void UIManager::render()
{
	mGameContext.window.setView(mUIView);
	for (const auto& comp : mUIComponents)
	{
		comp->render();
	}
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
}

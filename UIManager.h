#pragma once
#include "Entity.h"
#include "EventManager.h"
#include "GameContext.h"
#include "UIComponent.h"
#include "Shop.h"
class UIManager
{
public:
	UIManager(GameContext& gameContext);
	void processEvents(const sf::Event event);
	void update(const sf::Time& deltaTime);
	void render();

	void setPlayer(Entity* entity);
	void createUI();
private:

private:
	GameContext& mGameContext;
	Entity* player;
	sf::View mUIView;
	std::vector<std::unique_ptr<UIComponent>> mUIComponents;
	std::unique_ptr<Shop> mShop;
};


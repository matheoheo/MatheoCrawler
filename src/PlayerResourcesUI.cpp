#include "pch.h"
#include "PlayerResourcesUI.h"
#include "Config.h"
#include "Entity.h"
#include "Utilities.h"
PlayerResourcesUI::PlayerResourcesUI(GameContext& gameContext, Entity& player)
	:UIComponent(gameContext, player),
	mSuash(gameContext.textures.get(TextureIdentifier::Suash)),
	mGoldText(gameContext.fonts.get(FontIdentifiers::Default))
{
	createSuash();
	createGoldText();
	registerToEvents();
}

void PlayerResourcesUI::processEvents(const sf::Event event)
{
}

void PlayerResourcesUI::update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime)
{
}

void PlayerResourcesUI::render()
{
	mGameContext.window.draw(mSuash);
	mGameContext.window.draw(mGoldText);
}

void PlayerResourcesUI::registerToEvents()
{
	mGameContext.eventManager.registerEvent<UpdatePlayerResourcesEvent>([this](const UpdatePlayerResourcesEvent& data)
		{
			updateGoldText();
		});
}

void PlayerResourcesUI::createSuash()
{
	constexpr float yPos = 10.f;
	float xPos = Config::fWindowSize.x * 0.87f;
	mSuash.setPosition({ xPos, yPos });
	float suashDiv = 64.f;
	float suashSize = (Config::fWindowSize.x + Config::fWindowSize.y) / suashDiv;
	Utilities::scaleSprite(mSuash, { suashSize, suashSize });

}

void PlayerResourcesUI::createGoldText()
{
	mGoldText.setCharacterSize(Config::getCharacterSize());
	constexpr float margin = 11.f;
	auto suashPos = mSuash.getPosition();
	auto suashSize = mSuash.getGlobalBounds().size;
	sf::Vector2f textPos{ suashPos.x + suashSize.x + margin, 3.f };
	mGoldText.setPosition(textPos);
	updateGoldText();
}

void PlayerResourcesUI::updateGoldText()
{
	auto& resourcesComp = player.getComponent<PlayerResourcesComponent>();
	mGoldText.setString(std::to_string(resourcesComp.cGold));
}

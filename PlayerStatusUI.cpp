#include "pch.h"
#include "PlayerStatusUI.h"
#include "Config.h"
#include "Utilities.h"

PlayerStatusUI::PlayerStatusUI(GameContext& gameContext, Entity& player)
	:UIComponent(gameContext, player),
	mTwarf(gameContext.textures.get(TextureIdentifier::Twarf)),
	mHpText(gameContext.fonts.get(FontIdentifiers::Default))
{
	createTwarf();
	createHealthBar();
	createHpText();
	registerToEvents();
}

void PlayerStatusUI::processEvents(const sf::Event event)
{
}

void PlayerStatusUI::update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime)
{
}

void PlayerStatusUI::render()
{
	auto& hpBarComp = player.getComponent<HealthBarComponent>();
	mGameContext.window.draw(mTwarfBorder);
	mGameContext.window.draw(mTwarf);
	mGameContext.window.draw(hpBarComp.cBackgroundBar);
	mGameContext.window.draw(hpBarComp.cForegroundBar);
	mGameContext.window.draw(mHpText);
}

void PlayerStatusUI::registerToEvents()
{
	registerToPlayerGotHitEvent();
}

void PlayerStatusUI::registerToPlayerGotHitEvent()
{
	mGameContext.eventManager.registerEvent<PlayerGotHitEvent>([this](const PlayerGotHitEvent& data)
		{
			auto barSize = Utilities::calculateNewBarSize(player, Config::hpBarPlayerSize);
			Utilities::changeHpBarSize(player, barSize);
			updateHpText();
		});
}

void PlayerStatusUI::createTwarf()
{
	sf::Vector2f basePos{ 1.f, 1.f };
	float mod = 0.05f;
	float borderThickness = 2.f;
	sf::Vector2f twarfSize{ Config::fWindowSize.x * mod, Config::fWindowSize.x * mod };
	Utilities::scaleSprite(mTwarf, twarfSize);
	mTwarf.setPosition({ basePos.x + borderThickness, basePos.y + borderThickness });

	mTwarfBorder.setFillColor(sf::Color::Transparent);
	mTwarfBorder.setOutlineThickness(borderThickness);
	mTwarfBorder.setOutlineColor(sf::Color::White);
	mTwarfBorder.setSize({ twarfSize.x + borderThickness, twarfSize.y + borderThickness });
	mTwarfBorder.setPosition(basePos);
}

void PlayerStatusUI::createHealthBar()
{
	auto& hpBarComp = player.getComponent<HealthBarComponent>();
	hpBarComp.cIsVisible = true;
	constexpr sf::Color foregroundColor{ 0, 200, 0 };
	constexpr sf::Color backgroundColor{ 30, 30, 30 };
	constexpr sf::Color outlineColor{ 150, 150, 150 };
	constexpr float outlineThickness = 1.5f;

	hpBarComp.cBackgroundBar.setFillColor(backgroundColor);
	hpBarComp.cForegroundBar.setFillColor(foregroundColor);
	hpBarComp.cBackgroundBar.setOutlineColor(outlineColor); //dark gray (rgb)
	hpBarComp.cBackgroundBar.setOutlineThickness(outlineThickness);
	hpBarComp.cBackgroundBar.setSize(Config::hpBarPlayerSize);
	hpBarComp.cForegroundBar.setSize(Config::hpBarPlayerSize);

	auto twarfBounds = mTwarfBorder.getGlobalBounds();
	float margin = 3.f;
	sf::Vector2f barPos{ twarfBounds.position.x + twarfBounds.size.x + outlineThickness, 2.f};
	hpBarComp.cBackgroundBar.setPosition(barPos);
	hpBarComp.cForegroundBar.setPosition(barPos);
}

void PlayerStatusUI::createHpText()
{
	mHpText.setCharacterSize(Config::getCharacterSize() / 2);
	updateHpText();
	sf::FloatRect bounds{ mHpText.getGlobalBounds() };
	sf::Vector2f origin{ bounds.size.x / 2.f, bounds.size.y / 2.f };
	mHpText.setOrigin(origin);
	positionHpText();
}

void PlayerStatusUI::updateHpText()
{
	auto& statsComp = player.getComponent<CombatStatsComponent>();
	std::string currHp = std::to_string(statsComp.cHealth);
	std::string maxHp = std::to_string(statsComp.cMaxHealth);
	std::string fullString = currHp + "/" + maxHp;
	mHpText.setString(fullString);
}

void PlayerStatusUI::positionHpText()
{
	auto& hpBarComp = player.getComponent<HealthBarComponent>();
	auto barPos = hpBarComp.cBackgroundBar.getPosition();
	auto barCenter = hpBarComp.cBackgroundBar.getGeometricCenter();

	sf::Vector2f textPos{ barPos + barCenter };
	mHpText.setPosition(textPos);
}

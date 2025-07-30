#include "pch.h"
#include "PlayerStatusUI.h"
#include "Config.h"
#include "Utilities.h"

PlayerStatusUI::PlayerStatusUI(GameContext& gameContext, Entity& player)
	:UIComponent(gameContext, player),
	mTwarf(gameContext.textures.get(TextureIdentifier::Twarf)),
	mHpText(gameContext.fonts.get(FontIdentifiers::Default)),
	mManaText(gameContext.fonts.get(FontIdentifiers::Default))
{
	createTwarf();
	createHealthBar();
	createHpText();
	createManaBar();
	createManaText();
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
	auto& manaBarComp = player.getComponent<PlayerManaBarComponent>();
	mGameContext.window.draw(mTwarf);
	mGameContext.window.draw(mTwarfBorder);
	mGameContext.window.draw(hpBarComp.cBackgroundBar);
	mGameContext.window.draw(hpBarComp.cForegroundBar);
	mGameContext.window.draw(mHpText);
	mGameContext.window.draw(manaBarComp.cBackgroundBar);
	mGameContext.window.draw(manaBarComp.cForegroundBar);
	mGameContext.window.draw(mManaText);
}

void PlayerStatusUI::registerToEvents()
{
	registerToPlayerGotHitEvent();
	registerToUpdatePlayerStatusEvent();
}

void PlayerStatusUI::registerToPlayerGotHitEvent()
{
	mGameContext.eventManager.registerEvent<PlayerGotHitEvent>([this](const PlayerGotHitEvent& data)
		{
			updateStatus();
		});
}

void PlayerStatusUI::registerToUpdatePlayerStatusEvent()
{
	mGameContext.eventManager.registerEvent<UpdatePlayerStatusEvent>([this](const UpdatePlayerStatusEvent& data)
		{
			updateStatus();
		});
}

void PlayerStatusUI::createTwarf()
{
	//Twarf is player's icon that is visible in top left corner of screen.
	//I need it to be the same height as height of health & mana bar.
	constexpr float borderThickness = 2.f;
	constexpr sf::Vector2f basePos{ borderThickness, borderThickness };
	constexpr float margin = 1.5f;

	const float desiredHeight = Config::hpBarPlayerSize.y + Config::manaBarSize.y + borderThickness + margin;
	const float ratio = desiredHeight / Config::fWindowSize.x;
	const float size = Config::fWindowSize.x * ratio;

	const sf::Vector2f twarfSize{ size, size };
	Utilities::scaleSprite(mTwarf, twarfSize);
	mTwarf.setPosition(basePos);

	mTwarfBorder.setFillColor(sf::Color::Transparent);
	mTwarfBorder.setOutlineThickness(borderThickness);
	mTwarfBorder.setOutlineColor(sf::Color::White);
	mTwarfBorder.setSize(twarfSize);
	mTwarfBorder.setPosition(basePos);
}

void PlayerStatusUI::createHealthBar()
{
	auto& hpBarComp = player.getComponent<HealthBarComponent>();
	hpBarComp.cIsVisible = true;
	constexpr sf::Color foregroundColor{ Config::hpBarColor };
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
	updateHpText();
	createText(mHpText);
	positionHpText();
}

void PlayerStatusUI::updateHpText()
{
	auto& statsComp = player.getComponent<CombatStatsComponent>();
	updateText(mHpText, statsComp.cHealth, statsComp.cMaxHealth);
}

void PlayerStatusUI::positionHpText()
{
	auto& hpBarComp = player.getComponent<HealthBarComponent>();
	positionText(mHpText, hpBarComp.cBackgroundBar);
}

void PlayerStatusUI::createManaBar()
{
	auto& hpBarComp = player.getComponent<HealthBarComponent>();
	auto& manaBarComp = player.getComponent<PlayerManaBarComponent>();
	auto hpBarPos = hpBarComp.cBackgroundBar.getPosition();
	auto hpBarSize = hpBarComp.cBackgroundBar.getSize();
	constexpr float margin = 3.f;

	sf::Vector2f manaBarPos{ hpBarPos.x, hpBarPos.y + hpBarSize.y + margin };

	manaBarComp.cBackgroundBar.setPosition(manaBarPos);
	manaBarComp.cForegroundBar.setPosition(manaBarPos);
}

void PlayerStatusUI::createManaText()
{
	updateManaText();
	createText(mManaText);
	positionText(mManaText, player.getComponent<PlayerManaBarComponent>().cBackgroundBar);
}

void PlayerStatusUI::updateManaText()
{
	auto& statsComp = player.getComponent<CombatStatsComponent>();
	updateText(mManaText, statsComp.cMana, statsComp.cMaxMana);
}

void PlayerStatusUI::updateStatus()
{
	auto& statsComp = player.getComponent<CombatStatsComponent>();
	auto hpBarSize = Utilities::calculateNewBarSize(Config::hpBarPlayerSize, statsComp.cHealth, statsComp.cMaxHealth);
	auto manaBarSize = Utilities::calculateNewBarSize(Config::manaBarSize, statsComp.cMana, statsComp.cMaxMana);

	auto& hpBarRect = player.getComponent<HealthBarComponent>().cForegroundBar;
	auto& manaBarRect = player.getComponent<PlayerManaBarComponent>().cForegroundBar;
	Utilities::changeBarSize(hpBarRect, hpBarSize);
	Utilities::changeBarSize(manaBarRect, manaBarSize);

	updateHpText();
	updateManaText();
}

void PlayerStatusUI::createText(sf::Text& text)
{
	text.setCharacterSize(Config::getCharacterSize() / 2);
	sf::FloatRect bounds{ text.getGlobalBounds() };
	sf::Vector2f origin{ bounds.size.x / 2.f, bounds.size.y / 2.f };
	text.setOrigin(origin);
}
 
void PlayerStatusUI::updateText(sf::Text& text, int curr, int max)
{
	std::string currStr = std::to_string(curr);
	std::string maxStr = std::to_string(max);
	std::string fullString = currStr + "/" + maxStr;
	text.setString(fullString);
}

void PlayerStatusUI::positionText(sf::Text& text, sf::RectangleShape& rect)
{
	auto barPos = rect.getPosition();
	auto barCenter = rect.getGeometricCenter();

	sf::Vector2f textPos{ barPos + barCenter };
	text.setPosition(textPos);
}

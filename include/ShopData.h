#pragma once
#include "TextButton.h"
#include "AssetManager.h"
#include <string>

enum class TextureIdentifier;
enum class FontIdentifiers;
class sf::Texture;
class sf::Font;

enum class StatType
{
	//Attributes Stat Types
	Health,
	Defence,
	MagicDefence,
	Attack,
	AttackSpeed,

	//BladeArts
	LeftAttack,
	RightAttack,
	ThrustAttack,

	//Spells stat types
	QuickHealSpell,
	MajorHealSpell,
	HealthRegenSpell,
	ManaRegenSpell,
	WaterballSpell,
	PureProjSpell,
	FireballSpell,
	BloodballSpell,

	FrostPillarSpell,
	BladeDanceSpell,
	ToxicCloudSpell,
	ThunderstormSpell
};


struct ShopItem 
{
	int cost = 1;
	std::string itemName;
	StatType statType;
	sf::Sprite itemVisual;
	sf::Sprite currencySprite;
	sf::Text   itemNameText;
	sf::Text   itemCostText;
	TextButton upgradeButton;
	sf::FloatRect interactionBounds;
	std::optional<int> maxUpgradeLevel; ///optional, because not every item/spell is gonna have a limit
	std::optional<int> minLevelToAssign; //minimal level of item that is required before u can assign it.
	bool isAssignable; //can this shop item be assigned to UI Action Slot
	sf::RectangleShape border; //border for visual presentation

	ShopItem(const sf::Font& font, const sf::Texture& itemTexture, const sf::Texture& currencyTexture, StatType type,
		std::optional<int> maxUpgrade = std::nullopt)
		:itemVisual(itemTexture),
		statType(type),
		currencySprite(currencyTexture),
		itemNameText(font),
		itemCostText(font),
		upgradeButton(font, "Upgrade"),
		maxUpgradeLevel(maxUpgrade),
		isAssignable(false)
	{}
};
struct ItemInitData 
{
	TextureIdentifier iconId;
	std::string name;
	StatType type;
	std::optional<int> maxUpgrade = std::nullopt;
};

namespace ShopUtils
{
	using TexturesHolder = AssetManager<TextureIdentifier, sf::Texture>;
	using FontsHolder = AssetManager<FontIdentifiers, sf::Font>;

	float getIconSize();
	unsigned int getItemCharSize();
	unsigned int getItemDescSize();

	ShopItem createItem(const ItemInitData& data, const TexturesHolder& textures, const FontsHolder& fonts);
	void positionItem(ShopItem& item, const sf::Vector2f& iconPos);
	void renderItem(sf::RenderWindow& window, const ShopItem& item);
	sf::Text createItemDescription(const sf::Font& font, const std::string& description, const sf::Vector2f& position,
		float maxDescWidth);

	float getItemWidth(const ShopItem& item);
	float getItemHeight(const ShopItem& item);
}

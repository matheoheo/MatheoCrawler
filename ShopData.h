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
	AttackSpeed
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

	ShopItem(const sf::Font& font, const sf::Texture& itemTexture, const sf::Texture& currencyTexture, StatType type)
		:itemVisual(itemTexture),
		statType(type),
		currencySprite(currencyTexture),
		itemNameText(font),
		itemCostText(font),
		upgradeButton(font, "Upgrade")
	{}
};
struct ItemInitData 
{
	TextureIdentifier iconId;
	std::string name;
	StatType type;
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
}

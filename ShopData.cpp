#include "pch.h"
#include "ShopData.h"
#include "Utilities.h"
#include "AssetManager.h"
#include "FontIdentifiers.h"
#include "TextureIdentifiers.h"
#include "Config.h"

float ShopUtils::getIconSize()
{
	return Config::fWindowSize.x * 0.035f;
}

unsigned int ShopUtils::getItemCharSize()
{
	return Config::getCharacterSize() / 3;
}

unsigned int ShopUtils::getItemDescSize()
{
	return Config::getCharacterSize() / 2;
}

ShopItem ShopUtils::createItem(const ItemInitData& data, const TexturesHolder& textures, const FontsHolder& fonts)
{
	const auto& font = fonts.get(FontIdentifiers::UIFont);
	const auto& iconTexture = textures.get(data.iconId);
	const auto& currencyTexture = textures.get(TextureIdentifier::Suash);
	float iconSize = getIconSize();
	unsigned int charSize = getItemCharSize();

	ShopItem item(font, iconTexture, currencyTexture, data.type);
	item.itemName = data.name;
	Utilities::scaleSprite(item.itemVisual, { iconSize, iconSize });

	item.itemNameText.setCharacterSize(charSize);
	item.itemNameText.setString(item.itemName);
	item.itemNameText.setFillColor(sf::Color{ 230, 230, 230 });

	Utilities::scaleSprite(item.currencySprite, sf::Vector2f(charSize, charSize));
	item.itemCostText.setCharacterSize(charSize);
	item.itemCostText.setString("57893"); //placeholder
	item.itemCostText.setFillColor(sf::Color{ 255, 223, 50 });

	item.upgradeButton.setCharacterSize(charSize);
	item.upgradeButton.setOriginOnCenter();

	return item;
}

void ShopUtils::positionItem(ShopItem& item, const sf::Vector2f& iconPos)
{
	auto iconSize = getIconSize();
	auto charSize = getItemCharSize();

	constexpr float margin = 5.f;
	item.itemVisual.setPosition(iconPos);

	const float sprCenter{ iconPos.x + item.itemVisual.getGlobalBounds().size.x / 2 };

	const sf::Vector2f itemNamePos{ sprCenter, iconPos.y + iconSize + margin };
	item.itemNameText.setPosition(itemNamePos);
	Utilities::setTextOriginOnCenter(item.itemNameText);

	const sf::Vector2f itemCostPos{ sprCenter, itemNamePos.y + charSize + margin };
	item.itemCostText.setPosition(itemCostPos);
	Utilities::setTextOriginOnCenter(item.itemCostText);

	const sf::Vector2f buttonPos{ sprCenter, itemCostPos.y + charSize + margin };
	item.upgradeButton.setPosition(buttonPos);

}

void ShopUtils::renderItem(sf::RenderWindow& window, const ShopItem& item)
{
	window.draw(item.itemVisual);
	window.draw(item.itemNameText);
	window.draw(item.itemCostText);
	window.draw(item.currencySprite);
	item.upgradeButton.render(window);
}

sf::Text ShopUtils::createItemDescription(const sf::Font& font, const std::string& description, const sf::Vector2f& position,
	float maxDescWidth)
{
	auto charSize = getItemDescSize();
	auto wrappedDesc = Utilities::wrapText(description, font, maxDescWidth, charSize);
	
	sf::Text text(font, wrappedDesc, charSize);
	text.setPosition(position);
	text.setFillColor({ 200, 200, 200 });

	return text;
}

float ShopUtils::getItemWidth(const ShopItem& item)
{
	constexpr float margin = 10.f;
	float max = 0.f;

	auto updateBounds = [&](const sf::FloatRect& bounds)
	{
		max = std::max(max, bounds.size.x);
	};

	updateBounds(item.itemNameText.getGlobalBounds());
	updateBounds(item.itemCostText.getGlobalBounds());
	updateBounds(item.itemVisual.getGlobalBounds());

	float res = max + margin;
	return res;
}

float ShopUtils::getItemHeight(const ShopItem& item)
{
	constexpr float margin = 5.f;

	auto sprPos = item.itemVisual.getPosition();
	auto btnPos = item.upgradeButton.getPosition();
	auto btnHeight = item.upgradeButton.getSize().y;

	return ((btnPos.y + btnHeight) - sprPos.y) + margin;
}

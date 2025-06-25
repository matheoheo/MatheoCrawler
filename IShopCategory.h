#pragma once
#include "UIComponent.h"
class IShopCategory : public UIComponent
{
public:
	IShopCategory(GameContext& gameContext, Entity& player);
	virtual ~IShopCategory() = default;

	// Inherited via UIComponent
	virtual void processEvents(const sf::Event event) override;
	virtual void update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime) override;
	virtual void render() override;

	virtual void create(const sf::Vector2f& pos, const sf::Vector2f& categorySize) = 0;
	const std::string& getCategoryName() const;
protected:
	sf::Vector2f mIconSize;
	std::string mName;
};


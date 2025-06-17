#include "pch.h"
#include "ActionSelectionUI.h"
#include "Config.h"
#include "Utilities.h"

ActionSelectionUI::ActionSelectionUI(GameContext& gameContext, Entity& player)
	:UIComponent(gameContext, player),
	mFont(gameContext.fonts.get(FontIdentifiers::Default)),
	mSlotSize(Config::fWindowSize.x * 0.04f),
	mActiveBorderColor(200, 180, 60),
	mInactiveBorderColor(60, 60, 60)
{
	createAttackSlots();
	registerToEvents();
}

void ActionSelectionUI::processEvents(const sf::Event event)
{
}

void ActionSelectionUI::update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime)
{
}

void ActionSelectionUI::render()
{
	for (const auto& slot : mIconSlots)
		renderSlot(slot);
}

void ActionSelectionUI::registerToEvents()
{
	registerToSelectAttackEvent();
}

void ActionSelectionUI::registerToSelectAttackEvent()
{
	mGameContext.eventManager.registerEvent<SelectAttackEvent>([this](const SelectAttackEvent& data)
		{
			if (data.attackId < 0 || data.attackId >= mIconSlots.size())
				return;
			for (auto& icon : mIconSlots)
				icon.border.setOutlineColor(mInactiveBorderColor);
			mIconSlots[data.attackId].border.setOutlineColor(mActiveBorderColor);
		});
}

void ActionSelectionUI::createAttackSlots()
{
	const sf::Vector2f slotSize{ mSlotSize, mSlotSize };
	constexpr float margin = 2.f;
	sf::Vector2f pos{ Config::fWindowSize.x * 0.21f, Config::fWindowSize.y - slotSize.y - margin };
	constexpr size_t slotCount = 3;
	const std::array<SlotData, slotCount> slotDatas =
	{
		SlotData{TextureIdentifier::Devoyer, "1"},
		SlotData{TextureIdentifier::Devoyer, "2"},
		SlotData{TextureIdentifier::Devoyer, "3"}
	};

	for (const auto& data : slotDatas)
	{
		mIconSlots.push_back(createSlot(data.textureId, slotSize, pos, data.keyLabel));
		pos.x += slotSize.x + 2 * margin;
	}
	mIconSlots[0].border.setOutlineColor(mActiveBorderColor);
}

void ActionSelectionUI::renderSlot(const UIIconSlot& slot)
{
	mGameContext.window.draw(slot.icon);
	mGameContext.window.draw(slot.border);
	mGameContext.window.draw(slot.keyText);
}

ActionSelectionUI::UIIconSlot ActionSelectionUI::createSlot(TextureIdentifier textureId, const sf::Vector2f& size,
	const sf::Vector2f& pos, const std::string& keyLabel) const
{
	constexpr float borderThickness = 2.f;
	UIIconSlot slot(mGameContext.textures.get(textureId), mFont);
	slot.border.setSize(size);
	slot.border.setFillColor(sf::Color::Transparent);
	slot.border.setOutlineColor(mInactiveBorderColor);
	slot.border.setOutlineThickness(borderThickness);
	slot.border.setPosition(pos);
	
	Utilities::scaleSprite(slot.icon, size);
	slot.icon.setPosition(pos);

	sf::Vector2f offset{ 1.f, 1.f };
	slot.keyText.setCharacterSize(Config::getCharacterSize() / 2);
	slot.keyText.setString(keyLabel);
	slot.keyText.setPosition(pos + offset);

	return slot;
}

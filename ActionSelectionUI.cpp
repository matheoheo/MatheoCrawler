#include "pch.h"
#include "ActionSelectionUI.h"
#include "Config.h"
#include "Utilities.h"

ActionSelectionUI::ActionSelectionUI(GameContext& gameContext, Entity& player)
	:UIComponent(gameContext, player),
	mFont(gameContext.fonts.get(FontIdentifiers::Default)),
	mSlotSize(Config::fWindowSize.x * 0.04f),
	mActiveBorderColor(200, 180, 60),
	mInactiveBorderColor(60, 60, 60),
	mCooldownFrames(0)
{
	createAttackSlots();
	createSpellSlots();
	registerToEvents();
}

void ActionSelectionUI::processEvents(const sf::Event event)
{
}

void ActionSelectionUI::update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime)
{
	updateCooldowns();
}

void ActionSelectionUI::render()
{
	for (const auto& slot : mIconSlots)
		renderSlot(slot);
	renderCooldowns();
}

void ActionSelectionUI::registerToEvents()
{
	registerToSelectAttackEvent();
	registerToStartSpellCooldownUIEvent();
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

void ActionSelectionUI::registerToStartSpellCooldownUIEvent()
{
	mGameContext.eventManager.registerEvent<StartSpellCooldownUIEvent>([this](const StartSpellCooldownUIEvent& data)
		{
			const UIIconSlot* slot = getSlotBasedOnActivationKey(data.usedKey);
			if (!slot)
				return;

			createCooldown(*slot, &data.spellCd);
		});
}

void ActionSelectionUI::createAttackSlots()
{
	sf::Vector2f pos{ Config::fWindowSize.x * 0.21f, Config::fWindowSize.y - mSlotSize - 2.f };
	constexpr size_t slotCount = 3;
	const std::array<SlotData, slotCount> slotDatas =
	{
		SlotData{TextureIdentifier::Devoyer, "1", Key::Num1},
		SlotData{TextureIdentifier::Devoyer, "2", Key::Num2},
		SlotData{TextureIdentifier::Devoyer, "3", Key::Num3}
	};
	createSlotsList(slotDatas, pos);
	
	mIconSlots[0].border.setOutlineColor(mActiveBorderColor);
}

void ActionSelectionUI::createSpellSlots()
{
	sf::Vector2f pos{ mIconSlots.back().border.getPosition() };
	pos.x += mSlotSize * 10;
	constexpr size_t slotCount = 5;
	const std::array<SlotData, slotCount> slotDatas =
	{
		SlotData{TextureIdentifier::WaterballIcon, "Z", Key::Z},
		SlotData{TextureIdentifier::PureProjIcon, "X", Key::X},
		SlotData{TextureIdentifier::FireballIcon, "C", Key::C},
		SlotData{TextureIdentifier::BloodballIcon, "V", Key::V},
		SlotData{TextureIdentifier::Devoyer, "B", Key::B}
	};
	createSlotsList(slotDatas, pos);
}

void ActionSelectionUI::createSlotsList(std::span< const SlotData> dataSpan, sf::Vector2f pos)
{
	constexpr float margin = 2.f;
	const sf::Vector2f slotSize{ mSlotSize, mSlotSize };

	for (const auto& data : dataSpan)
	{
		mIconSlots.push_back(createSlot(data.textureId, slotSize, pos, data.keyLabel, data.activationKey));
		pos.x += slotSize.x + 2 * margin;
	}
}

void ActionSelectionUI::renderSlot(const UIIconSlot& slot)
{
	mGameContext.window.draw(slot.icon);
	mGameContext.window.draw(slot.border);
	mGameContext.window.draw(slot.keyText);
}

ActionSelectionUI::UIIconSlot ActionSelectionUI::createSlot(TextureIdentifier textureId, const sf::Vector2f& size,
	const sf::Vector2f& pos, const std::string& keyLabel, Key key) const
{
	constexpr float borderThickness = 2.f;
	UIIconSlot slot(mGameContext.textures.get(textureId), mFont, key);
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

const ActionSelectionUI::UIIconSlot* ActionSelectionUI::getSlotBasedOnActivationKey(Key key) const
{
	auto it = std::ranges::find(mIconSlots, key, &UIIconSlot::activationKey);
	if(it == std::ranges::end(mIconSlots))
		return nullptr;

	return &(*it);
}

void ActionSelectionUI::createCooldown(const UIIconSlot& slot, int* cooldown)
{
	if (!cooldown)
		return;

	constexpr sf::Color overlayColor{0, 0, 0, 150};
	unsigned int charSize = Config::getCharacterSize() / 2;
	UICooldown cd(mFont);
	cd.cooldown = cooldown;
	cd.overlay.setFillColor(overlayColor);
	cd.overlay.setSize(slot.border.getSize());
	cd.overlay.setPosition(slot.border.getPosition());

	cd.timerText.setCharacterSize(charSize);
	updateCooldownText(cd);
	sf::Vector2f origin{ cd.timerText.getGlobalBounds().size * 0.5f };
	cd.timerText.setOrigin(origin);
	cd.timerText.setPosition(cd.overlay.getPosition() + cd.overlay.getGeometricCenter());
	mCooldowns.push_back(std::move(cd));
}

void ActionSelectionUI::updateCooldownText(UICooldown& cd)
{
	float fCd = static_cast<float>(*cd.cooldown);
	fCd /= 1000.f; //from milliseconds to seconds.
	std::string time = std::format("{:.1f}", fCd);
	cd.timerText.setString(time);
}

void ActionSelectionUI::removeFinishedCooldowns()
{
	std::erase_if(mCooldowns, [](const UICooldown& cd)
		{
			return *cd.cooldown <= 250;
		});
}

void ActionSelectionUI::updateCooldowns()
{
	++mCooldownFrames;
	if (mCooldownFrames != 15)
		return;

	for (auto& cd : mCooldowns)
	{
		updateCooldownText(cd);
	}
	mCooldownFrames = 0;
	removeFinishedCooldowns();
}

void ActionSelectionUI::renderCooldowns()
{
	for (auto& cd : mCooldowns)
	{
		mGameContext.window.draw(cd.overlay);
		mGameContext.window.draw(cd.timerText);
	}
}

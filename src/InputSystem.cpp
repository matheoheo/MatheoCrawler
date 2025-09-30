#include "pch.h"
#include "InputSystem.h"
#include "Directions.h"
#include "AnimationHolder.h"

InputSystem::InputSystem(SystemContext& systemContext)
	:ISystem(systemContext)
{
	registerToEvents();
	createSpellKeyToIntMap();
}

void InputSystem::processEvents(const sf::Event event)
{
	if (auto data = event.getIf<sf::Event::KeyPressed>())
	{
		handleAttackSelecting(data->code);
		handleCastingSpell(data->code);
	}
}

void InputSystem::update(const sf::Time& deltaTime)
{
	handleMovement();
	handleAttacking();
}

void InputSystem::registerToEvents()
{
	registerToBindSpellEvent();
}

void InputSystem::registerToBindSpellEvent()
{
	mSystemContext.eventManager.registerEvent<BindSpellEvent>([this](const BindSpellEvent& data)
		{
			const auto& player = mSystemContext.entityManager.getPlayer();
			//firstly, we must check if this spell is in player's spellbook
			if (!doesPlayerHaveSpell(data.spellId))
				return;
			
			//now we check if this spell isn't already assigned 
			auto assigned = isSpellAlreadyAssigned(data.spellId);
			if (std::holds_alternative<bool>(assigned)) //the spell isn't assigned already, we can do it.
				assignSpell(data.slotKey, data.spellId);
			else //otherwise we must either swap or just bind to different slot 
			{
				//this is our actual slot that the spell is assigned to
				int assignedSlotId = std::get<int>(assigned);
				auto assignedSlotKey = getKeyBasedOnId(assignedSlotId);
				if (!assignedSlotKey || !mSpellKeyToInt.contains(data.slotKey))
					return; //should never happen

				//now we check, if the slot we want to assign has any spell assigned or not
				auto desiredId = mSpellKeyToInt.at(data.slotKey);
				auto& assignedSpells = player.getComponent<AssignedSpellsComponent>().cAssignedSpells;
				SpellInstance* desiredInstance = assignedSpells.at(desiredId);
				if (!desiredInstance)
				{
					//remove from old spot and assign to new one.
					assignedSpells.at(assignedSlotId) = nullptr;
					assignSpell(data.slotKey, data.spellId, assignedSlotKey.value());
				}
				else
				{
					if (!desiredInstance->data)
						return;
					//just swap
					auto spellIdOfDesiredSlot = desiredInstance->data->spellId;
					assignSpell(data.slotKey, data.spellId, assignedSlotKey.value());
					assignSpell(assignedSlotKey.value(), spellIdOfDesiredSlot);
				}
			}
		});
}

void InputSystem::handleMovement()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		notifyMoveRequest(Direction::Up);
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		notifyMoveRequest(Direction::Bottom);
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		notifyMoveRequest(Direction::Left);
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		notifyMoveRequest(Direction::Right);
}

void InputSystem::handleAttackSelecting(sf::Keyboard::Key pressedKey)
{
	bool num1Pressed = pressedKey == sf::Keyboard::Key::Num1;
	bool num2Pressed = pressedKey == sf::Keyboard::Key::Num2;
	bool num3Pressed = pressedKey == sf::Keyboard::Key::Num3;

	AnimationIdentifier animId;
	int attId;
	if (num1Pressed)
	{
		animId = AnimationIdentifier::Attack1;
		attId = 0;
	}
	else if (num2Pressed)
	{
		animId = AnimationIdentifier::Attack2;
		attId = 1;
	}
	else if (num3Pressed)
	{
		animId = AnimationIdentifier::Attack3;
		attId = 2;
	}

	if (num1Pressed || num2Pressed || num3Pressed)
	{
		selectAttack(animId, attId);
	}
}

void InputSystem::handleAttacking()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
	{
		auto& player = mSystemContext.entityManager.getPlayer();
		auto animId = player.getComponent<AttackSelectionComponent>().cSelectedId;
		notifyAttackRequest(player, animId);
	}
}

void InputSystem::handleCastingSpell(sf::Keyboard::Key pressedKey)
{
	if (auto it = mSpellKeyToInt.find(pressedKey); it != std::end(mSpellKeyToInt))
	{
		auto& player = mSystemContext.entityManager.getPlayer();
		auto& assigned = player.getComponent<AssignedSpellsComponent>();
		auto& spells = assigned.cAssignedSpells;
		if (auto it2 = spells.find(it->second); it2 != std::end(spells))
		{
			if (!it2->second || !it2->second->data)
				return;

			auto spellId = it2->second->data->spellId;
			mSystemContext.eventManager.notify<CastSpellEvent>(CastSpellEvent(player, spellId, pressedKey));
		}
	}
}

void InputSystem::notifyMoveRequest(Direction dir)
{
	auto& player = mSystemContext.entityManager.getPlayer();
	mSystemContext.eventManager.notify<MoveRequestedEvent>(MoveRequestedEvent(player, dir));
}

void InputSystem::notifyAttackRequest(Entity& player, AnimationIdentifier animId)
{
	mSystemContext.eventManager.notify<StartAttackingEvent>(StartAttackingEvent(player, animId));
}

void InputSystem::selectAttack(AnimationIdentifier animId, int id)
{
	auto& attSelectComp = mSystemContext.entityManager.getPlayer().getComponent<AttackSelectionComponent>();
	attSelectComp.cSelectedId = animId;

	mSystemContext.eventManager.notify<SelectAttackEvent>(SelectAttackEvent(id));
}

void InputSystem::createSpellKeyToIntMap()
{
	mSpellKeyToInt[sf::Keyboard::Key::Z] = 0;
	mSpellKeyToInt[sf::Keyboard::Key::X] = 1;
	mSpellKeyToInt[sf::Keyboard::Key::C] = 2;
	mSpellKeyToInt[sf::Keyboard::Key::V] = 3;
	mSpellKeyToInt[sf::Keyboard::Key::B] = 4;

}

bool InputSystem::doesPlayerHaveSpell(SpellIdentifier id) const
{
	const auto& player = mSystemContext.entityManager.getPlayer();
	const auto& spells = player.getComponent<SpellbookComponent>().cSpells;
	auto it = std::ranges::find_if(spells, [id](const auto& pair){
			return pair.second.data && pair.second.data->spellId == id;
		});

	return it != std::ranges::end(spells);
}

std::variant<int, bool> InputSystem::isSpellAlreadyAssigned(SpellIdentifier id) const
{
	const auto& player = mSystemContext.entityManager.getPlayer();
	const auto& assignedComp = player.getComponent<AssignedSpellsComponent>();
	const auto& asSpells = assignedComp.cAssignedSpells; //assignedSpells

	auto it = std::ranges::find_if(asSpells, [id](const auto& pair)	{
			return pair.second && pair.second->data->spellId == id;
		});
	if (it == std::ranges::end(asSpells))
		return false;

	return it->first;
}

void InputSystem::assignSpell(sf::Keyboard::Key slotKey, SpellIdentifier spellId, std::optional<sf::Keyboard::Key> oldSlotKey)
{
	if (mSpellKeyToInt.contains(slotKey))
	{
		const auto& player = mSystemContext.entityManager.getPlayer();
		auto& assignedSpells = player.getComponent<AssignedSpellsComponent>().cAssignedSpells;
		auto& availableSpells = player.getComponent<SpellbookComponent>().cSpells;

		int correspondingId = mSpellKeyToInt.at(slotKey);
		if (correspondingId >= assignedSpells.size())
			return; //this should never happen

		//must find corresponding spell instance
		auto it = std::ranges::find_if(availableSpells, [spellId](const auto& pair) {
			return pair.second.data && pair.second.data->spellId == spellId;
			});
		if (it == std::ranges::end(availableSpells))
			return;

		assignedSpells.at(correspondingId) = &it->second;
		mSystemContext.eventManager.notify<ReBindSpellActionEvent>(ReBindSpellActionEvent(slotKey, spellId, oldSlotKey));
	}
}

std::optional<sf::Keyboard::Key> InputSystem::getKeyBasedOnId(int id) const
{
	auto it = std::ranges::find_if(mSpellKeyToInt, [id](const auto& pair) {
		return pair.second == id;
		});
	if (it == std::ranges::end(mSpellKeyToInt))
		return {};

	return it->first;
}


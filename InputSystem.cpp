#include "pch.h"
#include "InputSystem.h"
#include "Directions.h"
#include "AnimationHolder.h"

InputSystem::InputSystem(SystemContext& systemContext)
	:ISystem(systemContext)
{
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
			auto spellId = it2->second->data->spellId;
			mSystemContext.eventManager.notify<CastSpellEvent>(CastSpellEvent(player, nullptr, spellId, pressedKey));
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


#include "pch.h"
#include "InputSystem.h"
#include "Directions.h"
#include "AnimationHolder.h"

InputSystem::InputSystem(SystemContext& systemContext)
	:ISystem(systemContext)
{
}

void InputSystem::processEvents(const sf::Event event)
{
	if (auto data = event.getIf<sf::Event::KeyPressed>())
	{
		handleAttackSelecting(data->code);
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


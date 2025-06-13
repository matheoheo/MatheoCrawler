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
	/*
	if (auto data = event.getIf<sf::Event::KeyPressed>())
	{
		auto& player = mSystemContext.entityManager.getPlayer();

		switch (data->code)
		{
		case sf::Keyboard::Key::W:
			mSystemContext.eventManager.notify<MoveRequestedEvent>(MoveRequestedEvent(player, Direction::Up));
			break;
		case sf::Keyboard::Key::S:
			mSystemContext.eventManager.notify<MoveRequestedEvent>(MoveRequestedEvent(player, Direction::Bottom));
			break;
		case sf::Keyboard::Key::A:
			mSystemContext.eventManager.notify<MoveRequestedEvent>(MoveRequestedEvent(player, Direction::Left));
			break;
		case sf::Keyboard::Key::D:
			mSystemContext.eventManager.notify<MoveRequestedEvent>(MoveRequestedEvent(player, Direction::Right));
			break;
		default:
			break;
		}
	}*/
}

void InputSystem::update(const sf::Time& deltaTime)
{
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		mSystemContext.eventManager.notify<MoveRequestedEvent>
		(MoveRequestedEvent(mSystemContext.entityManager.getPlayer(), Direction::Up));
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		mSystemContext.eventManager.notify<MoveRequestedEvent>
		(MoveRequestedEvent(mSystemContext.entityManager.getPlayer(), Direction::Bottom));
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		mSystemContext.eventManager.notify<MoveRequestedEvent>
		(MoveRequestedEvent(mSystemContext.entityManager.getPlayer(), Direction::Left));
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		mSystemContext.eventManager.notify<MoveRequestedEvent>
		(MoveRequestedEvent(mSystemContext.entityManager.getPlayer(), Direction::Right));
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
	{
		auto& player = mSystemContext.entityManager.getPlayer();
		mSystemContext.eventManager.notify<StartAttackingEvent>(StartAttackingEvent(player, AnimationIdentifier::Attack3));
	}
}

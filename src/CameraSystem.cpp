#include "pch.h"
#include "CameraSystem.h"
#include "Utilities.h"
#include "Config.h"
CameraSystem::CameraSystem(SystemContext& systemContext, sf::View& gameView)
	:ISystem(systemContext),
	mGameView(gameView)
{
}

void CameraSystem::update(const sf::Time& deltaTime)
{
	//handleZooming();
	followEntity(mSystemContext.entityManager.getPlayer());
}

void CameraSystem::handleViewMovement(const sf::Time& deltaTime)
{
	sf::Vector2f dir;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		dir.y = -1.f;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		dir.y = 1.f;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		dir.x = -1.f;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		dir.x = 1.f;

	mGameView.move(dir * 650.f * deltaTime.asSeconds());
}

void CameraSystem::handleZooming()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::O))
		mGameView.zoom(0.9f);
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::L))
		mGameView.zoom(1.1f);
}

void CameraSystem::followEntity(const Entity& entity)
{
	if (entity.getComponent<EntityStateComponent>().cCurrentState == EntityState::Attacking)
		return;

	auto pos = Utilities::getEntityPos(entity);
	int cameraX = static_cast<int>(pos.x);
	int cameraY = static_cast<int>(pos.y);

	mGameView.setCenter(sf::Vector2f(static_cast<float>(cameraX), static_cast<float>(cameraY)));
}

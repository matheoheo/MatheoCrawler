#pragma once
#include "ISystem.h"
#include <SFML/Graphics.hpp>

class CameraSystem :
    public ISystem
{
public:
    CameraSystem(SystemContext& systemContext, sf::View& gameView);

    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
private:
    void handleViewMovement(const sf::Time& deltaTime);
    void handleZooming();
    void followEntity(const Entity& entity);
private:
    sf::View& mGameView;
};


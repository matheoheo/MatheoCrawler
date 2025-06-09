#pragma once
#include "ISystem.h"

class InputSystem :
    public ISystem
{
public:
    InputSystem(SystemContext& systemContext);
    // Inherited via ISystem

    virtual void processEvents(const sf::Event event) override;
    virtual void update(const sf::Time& deltaTime) override;
};


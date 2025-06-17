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
private:
    void handleMovement();
    void handleAttackSelecting(sf::Keyboard::Key pressedKey);
    void handleAttacking();
    void notifyMoveRequest(Direction dir);
    void notifyAttackRequest(Entity& player, AnimationIdentifier animId);

    void selectAttack(AnimationIdentifier animId, int id);
};


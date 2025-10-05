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
    void registerToEvents();
    void registerToBindSpellEvent();
    void registerToPlayerRunEndedEvent();
    void handleMovement();
    void handleAttackSelecting(sf::Keyboard::Key pressedKey);
    void handleAttacking();
    void handleCastingSpell(sf::Keyboard::Key pressedKey);
    void notifyMoveRequest(Direction dir);
    void notifyAttackRequest(Entity& player, AnimationIdentifier animId);

    void selectAttack(AnimationIdentifier animId, int id);
    void createSpellKeyToIntMap();
    bool doesPlayerHaveSpell(SpellIdentifier id) const;
    //if spell is already assigned, the function returns index from assignedSpellsComponent map
    //otherwise returns false
    std::variant<int, bool> isSpellAlreadyAssigned(SpellIdentifier id) const;
    void assignSpell(sf::Keyboard::Key slotKey, SpellIdentifier spellId, std::optional<sf::Keyboard::Key> oldSlotKey = {});
    std::optional<sf::Keyboard::Key> getKeyBasedOnId(int id) const;
private:
    std::unordered_map<sf::Keyboard::Key, int> mSpellKeyToInt;
    bool mEnabled;
};


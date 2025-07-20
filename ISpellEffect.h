#pragma once

class Entity;
class EventManager;
enum class SpellEffect;
class ISpellEffect
{
public:
	ISpellEffect(int duration);
	virtual void update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime) = 0;
	virtual bool tryMerge(const ISpellEffect& other) = 0;
	
	bool hasEffectFinished() const;
	SpellEffect getSpellEffectId() const;
protected:
	virtual void onEffectFinish() = 0;
	virtual void addTimePassed(const sf::Time& deltaTime);
protected:
	int mDuration;
	int mTimePassed;
	SpellEffect mSpellEffect; //it's identifier
};


#pragma once
class Entity;

class IVisualEffect
{
public:
	IVisualEffect(const Entity& entity, int duration);
	virtual ~IVisualEffect() = default;

	virtual void update(const sf::Time& deltaTime) = 0;
	virtual void render(sf::RenderWindow& window) = 0;
	virtual bool isFinished();
	unsigned int getEntityId() const;
protected:
	virtual void updateEffect(const sf::Time& deltaTime) = 0;
	virtual void onEffectFinish();
	void addTimePassed(const sf::Time& deltaTime);
	bool hasEffectDurationPassed() const;

protected:
	const Entity& mEntity;
	const unsigned int mEntityID; //saving id in case that entity is removed before effect is finished
	const int mDuration;
	int mTimer;
};


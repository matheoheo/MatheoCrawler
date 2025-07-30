#pragma once

//Base class for all tasks.
//Task is just a unit of behavior/action that an AI controlled entity can perform.
//Are designed to be lightweight and focused on a single responsibility.
class Entity;
class EventManager;

class ITask
{
public:
	virtual ~ITask() = default;
	virtual void update(Entity& entity, EventManager& eventManager, const sf::Time& deltaTime) = 0;
	bool isComplete() const;
protected:
	void complete();
protected:
	bool mIsCompleted = false;
};


#include "pch.h"
#include "AnimationSystem.h"
#include "Config.h"
#include "Entity.h"
#include "Utilities.h"

AnimationSystem::AnimationSystem(SystemContext& systemContext, const AnimationHolder& animationHolder)
	:ISystem(systemContext),
	mAnimationHolder(animationHolder)
{
	registerToEvents();
}

void AnimationSystem::update(const sf::Time& deltaTime)
{
	for (const auto& entity : mTrackedEntities)
	{
		auto& animComp = entity->getComponent<AnimationComponent>();
		animComp.cTimer += deltaTime.asMilliseconds();
		if (hasFrameDurationPassed(animComp))
		{
			resetTimer(animComp);
			updateFrame(*entity, animComp);
		}
	}
	removeFinishedEntities();
}

void AnimationSystem::registerToEvents()
{
	registerToPlayGenericWalkEvent();
	registerToFinalizeAnimationEvent();
	registerToPlayAttackAnimationEvent();
	registerToCastSpellAnimationEvent();
}

void AnimationSystem::resetTimer(AnimationComponent& animationComponent) const
{
	animationComponent.cTimer = 0.f;
}

bool AnimationSystem::hasFrameDurationPassed(const AnimationComponent& animationComponent) const
{
	return animationComponent.cTimer >= animationComponent.cFrameDuration;
}

void AnimationSystem::applyCurrentFrame(const Entity& entity, AnimationComponent& animationComponent) const
{
	assert(animationComponent.cFrames);
	assert(animationComponent.cCurrentIndex >= 0 && animationComponent.cCurrentIndex < animationComponent.cFrames->size());
	
	auto& sprite = entity.getComponent<SpriteComponent>().cSprite;
	const auto& frame = (*animationComponent.cFrames)[animationComponent.cCurrentIndex];
	sprite.setTextureRect(frame.frameRect);

	if (animationComponent.cApplyOffset)
	{
		sf::Vector2f mod = { 53.f, 30.f };
		sprite.setPosition(animationComponent.cStartPosition - frame.offset);
	}
}

void AnimationSystem::updateFrame(Entity& entity, AnimationComponent& animationComponent) 
{
	++animationComponent.cCurrentIndex;
	if (animationComponent.cCurrentIndex >= animationComponent.cFrames->size())
		return finalizeAnimation(entity, animationComponent);

	applyCurrentFrame(entity, animationComponent);
}

void AnimationSystem::finalizeAnimation(Entity& entity, AnimationComponent& animationComponent) 
{
	animationComponent.cCurrentIndex = 0;
	animationComponent.cTimer = 0.f;
	animationComponent.cFrames = nullptr;

	moveBackToStartingPosition(entity, animationComponent);
	applyDefaultFrame(entity, animationComponent);
	notifyAnimationFinished(entity, animationComponent);
	setStateToIdle(entity);
	mFinishedEntities.push_back(&entity);
}

void AnimationSystem::applyDefaultFrame(const Entity& entity, AnimationComponent& animationComponent)
{
	auto dir = entity.getComponent<DirectionComponent>().cCurrentDir;
	auto key = GenericAnimationKey{ .id = animationComponent.cDefaultAnimId, .dir = dir };

	const auto& frame = mAnimationHolder.get(key)[0];
	entity.getComponent<SpriteComponent>().cSprite.setTextureRect(frame.frameRect);
}

void AnimationSystem::setStateToIdle(const Entity& entity)
{
	entity.getComponent<EntityStateComponent>().cCurrentState = EntityState::Idle;
}

void AnimationSystem::moveBackToStartingPosition(const Entity& entity, AnimationComponent& animationComponent)
{
	if (animationComponent.cApplyOffset)
		entity.getComponent<SpriteComponent>().cSprite.setPosition(animationComponent.cStartPosition);
}


void AnimationSystem::notifyAnimationFinished(Entity& entity, AnimationComponent& animationComponent)
{
	auto animId = animationComponent.cCurrentId;
	if (Utilities::isAnAttackAnimation(animId))
	{
		auto& attackComp = entity.getComponent<AttackComponent>();
		attackComp.cLastAttackId = attackComp.cNextAttackId;
		mSystemContext.eventManager.notify<AttackAnimationFinishedEvent>
			(AttackAnimationFinishedEvent(entity, attackComp.cLastAttackData));
	}
	else if (Utilities::isSpellCastAnimation(animId))
	{
		auto spellid = entity.getComponent<SpellbookComponent>().cLastSpellId;
		mSystemContext.eventManager.notify<CastSpellFinishedEvent>(CastSpellFinishedEvent(entity, spellid));
	}
}

void AnimationSystem::removeFinishedEntities()
{
	for (const Entity* entity : mFinishedEntities)
		std::erase(mTrackedEntities, entity);
	mFinishedEntities.clear();
}

void AnimationSystem::registerToPlayGenericWalkEvent()
{
	mSystemContext.eventManager.registerEvent<PlayGenericWalkEvent>([this](const PlayGenericWalkEvent& data)
		{
			auto& animComp = data.entity.getComponent<AnimationComponent>();
			const auto& moveComp = data.entity.getComponent<MovementComponent>();
			GenericAnimationKey key{ .id = AnimationIdentifier::GenericWalk,
				.dir = data.entity.getComponent<DirectionComponent>().cCurrentDir };

			animComp.cCurrentIndex = 0;
			animComp.cTimer = 0.f;
			animComp.cCurrentId = key.id;
			animComp.cFrames = &mAnimationHolder.get(key);
			animComp.cStartPosition = data.entity.getComponent<SpriteComponent>().cSprite.getPosition();
			animComp.cFrameDuration = ((Config::getCellSize().x - 3.5f)/ moveComp.cMoveSpeed / animComp.cFrames->size()) * 1000.f;
			animComp.cApplyOffset = false;
			applyCurrentFrame(data.entity, animComp);

			mTrackedEntities.push_back(&data.entity);
		});
}

void AnimationSystem::registerToFinalizeAnimationEvent()
{
	mSystemContext.eventManager.registerEvent<FinalizeAnimationEvent>([this](const FinalizeAnimationEvent& data)
		{
			finalizeAnimation(data.entity, data.entity.getComponent<AnimationComponent>());
		});
}

void AnimationSystem::registerToPlayAttackAnimationEvent()
{
	mSystemContext.eventManager.registerEvent<PlayAttackAnimationEvent>([this](const PlayAttackAnimationEvent& data)
		{
			if (isEntityAlreadyTracked(data.entity) || !Utilities::isAnAttackAnimation(data.animId))
				return;

			auto dir = data.entity.getComponent<DirectionComponent>().cCurrentDir;
			auto entType = data.entity.getComponent<EntityTypeComponent>().cEntityType;
			EntityAnimationKey key{ data.animId, dir, entType };

			auto& animComp = data.entity.getComponent<AnimationComponent>();
			animComp.cApplyOffset = true;
			animComp.cCurrentIndex = 0;
			animComp.cStartPosition = data.entity.getComponent<SpriteComponent>().cSprite.getPosition();
			animComp.cTimer = 0.f;
			animComp.cFrames = &mAnimationHolder.get(key);
			animComp.cCurrentId = data.animId;

			auto& attackComp = data.entity.getComponent<AttackComponent>();
			attackComp.cLastAttackData = &attackComp.cAttackDataMap[data.animId];
			attackComp.cLastAttackId = data.animId;

			auto& combatStatsComp = data.entity.getComponent<CombatStatsComponent>();

			//calculate full animation time
			constexpr float baseAnimSpeed = 1.0f;
			float fullAnimTime = baseAnimSpeed / (combatStatsComp.cAttackSpeed * attackComp.cLastAttackData->speedMultiplier);
			fullAnimTime *= 1000.f; //so we get milliseconds
			attackComp.cAttackCooldownTimer = fullAnimTime;

			//now calculate each frame duration
			auto framesCount = animComp.cFrames->size();
			animComp.cFrameDuration = fullAnimTime / static_cast<float>(framesCount);

			applyCurrentFrame(data.entity, animComp);
			mTrackedEntities.push_back(&data.entity);
		});
}

void AnimationSystem::registerToCastSpellAnimationEvent()
{
	mSystemContext.eventManager.registerEvent<PlayCastSpellAnimation>([this](const PlayCastSpellAnimation& data)
		{
			if (isEntityAlreadyTracked(data.entity))
				return;

			auto dir = data.entity.getComponent<DirectionComponent>().cCurrentDir;
			GenericAnimationKey key{ data.animId, dir };
			
			auto& animComp = data.entity.getComponent<AnimationComponent>();
			animComp.cFrames = &mAnimationHolder.get(key);
			animComp.cApplyOffset = false;
			animComp.cCurrentIndex = 0;
			animComp.cTimer = 0.f;
			animComp.cCurrentId = key.id;
			animComp.cFrameDuration = static_cast<float>(data.castTime) / static_cast<float>(animComp.cFrames->size());

			mTrackedEntities.push_back(&data.entity);
		});
}

#include "pch.h"
#include "PositioningAISystem.h"
#include "Utilities.h"

PositioningAISystem::PositioningAISystem(SystemContext& systemContext, TileMap& tileMap)
	:ISystem(systemContext),
	mTileMap(tileMap)
{
	registerToEvents();
}

void PositioningAISystem::update(const sf::Time& deltaTime)
{
}

void PositioningAISystem::registerToEvents()
{
	registerToRepositionToAttackEvent();
}

void PositioningAISystem::registerToRepositionToAttackEvent()
{
	mSystemContext.eventManager.registerEvent<RepositionToAttackEvent>([this](const RepositionToAttackEvent& data) 
		{
			auto& aiStateComp = data.entity.getComponent<EntityAIStateComponent>();
			if (!isInOptimalPositionToAttack(data.entity, data.target))
			{
				auto& posComp = data.entity.getComponent<PositioningComponent>();
				auto nextPos = getOptimalAttackPosition(data.entity, data.target);
				if (nextPos)
				{
					auto& val = nextPos.value();
					posComp.cTargetCell = val;
					mSystemContext.eventManager.notify<RequestPathEvent>(RequestPathEvent(data.entity, val));
					aiStateComp.cState = EntityAIState::RepositionToAttack;
				}
				else
				{
					aiStateComp.cState = EntityAIState::None;
					posComp.cLastRepositionTryFailed = true;
					posComp.cTargetCell = { 0, 0 };
				}
			}
		});
}

bool PositioningAISystem::isInOptimalPositionToAttack(const Entity& self, const Entity& target) const
{
	auto& positioningComp = self.getComponent<PositioningComponent>();
	auto targetCell = Utilities::getEntityCell(target);
	auto selfCell = Utilities::getEntityCell(self);

	int dx = std::abs(targetCell.x - selfCell.x);
	int dy = std::abs(targetCell.y - selfCell.y);
	if(dx != 0 && dy != 0)
		return false;
	if (dx < positioningComp.cMinRange || dy < positioningComp.cMinRange)
		return false;

	return mTileMap.isLineOfSightClear(selfCell, targetCell);
}

std::optional<sf::Vector2i> PositioningAISystem::getOptimalAttackPosition(Entity& entity, const Entity& target) const
{
	auto& positioningComp = entity.getComponent<PositioningComponent>();
	auto targetCell = Utilities::getEntityCell(target);
	auto myCell = Utilities::getEntityCell(entity);

	//entity can attack only when is standing directly facing target (4 directions only)
	//we must find all cells that are in [cMinRange, cMaxRange] (variables in positioningComponnent)
	//and check if LOS from those cells are clear
	//then we can get one of cells nad move our entity there.

	//we need to define offsets pairs for each direction.
	using DirVecPair = std::pair<Direction, sf::Vector2i>;
	constexpr std::array<DirVecPair, 4> offsets =
	{ {
		{Direction::Up,		{0, -1}},
		{Direction::Left,	{-1, 0}},
		{Direction::Bottom, {0,  1}},
		{Direction::Right,	{1,  0}},
	} };

	//here we gonna keep all cells that are in [cMinRange, cMaxRange] from targetCell
	std::vector<sf::Vector2i> candidates;

	for (const auto& [dir, offset] : offsets)
	{
		for (int i = positioningComp.cMinRange; i <= positioningComp.cMaxRange; ++i)
		{
			sf::Vector2i rangeOffset{ offset.x * i, offset.y * i };
			sf::Vector2i offsetCell = targetCell - rangeOffset;

			//if checked cell is not walkable - we must skip it, as it is no use for us.
			if (!mTileMap.isTileWalkable(offsetCell.x, offsetCell.y))
				continue;

			//if line of sight fails - it is impossible to cast ranged attack.
			//and since we are starting from minRange (closest position to Target) - we can just break from current direction
			//because next LOS are gonna fail also
			if (!mTileMap.isLineOfSightClear(offsetCell, targetCell))
				break;

			candidates.emplace_back(offsetCell);
		}
	}

	//after algorithm finished, and we do not have any candidates - we can't reposition, so just return nullopt
	if (candidates.empty())
		return {};

	auto distFrom = [](const sf::Vector2i& a, const sf::Vector2i& b) -> int
	{
		return Utilities::getDistanceBetweenCells(a, b);
	};
	bool posFocus = positioningComp.cFocusOnDistance;

	//now, we have to sort candidates.
	std::sort(std::begin(candidates), std::end(candidates), [&distFrom, posFocus, &myCell, &targetCell](const auto& a, const auto& b)
		{
			return distFrom(myCell, a) < distFrom(myCell, b);
		});

	//we need to check if path exists to given place, and return only then.
	//for performance reason, we only check first 3 positions - if no path, we fail.
	int i = 0;
	constexpr int maxChecks = 3;
	for (const auto& cand : candidates)
	{
		if (mTileMap.doesPathExist(myCell, cand))
			return cand;
		if (++i > maxChecks)
			break;
	}

	return std::nullopt;
}

#include "pch.h"
#include "BeamSystem.h"
#include "TileMap.h"
#include "Utilities.h"
BeamSystem::BeamSystem(SystemContext& systemContext, const TileMap& tileMap)
	:ISystem(systemContext),
	mTileMap(tileMap)
{
	registerToEvents();
}

void BeamSystem::update(const sf::Time& deltaTime)
{
	for (const auto& beam : mBeamSpells)
		beam->update(deltaTime, mSystemContext.eventManager);

	removeCompletedBeams();
}

void BeamSystem::render(sf::RenderWindow& window)
{
	for (const auto& beam : mBeamSpells)
		beam->render(window);
}

void BeamSystem::registerToEvents()
{
	registerToCastBeamEvent();
}

void BeamSystem::registerToCastBeamEvent()
{
	mSystemContext.eventManager.registerEvent<CastBeamEvent>([this](const CastBeamEvent& data)
		{
			auto spellPair = getSpellPair(data.beamData);
			if (spellPair.first == data.beamData.length)
				mBeamSpells.emplace_back(std::make_unique<BeamSpell>(data.beamData, mTileMap, spellPair.second));
			else
			{
				BeamData verifiedData = data.beamData;
				verifiedData.length = spellPair.first;
				mBeamSpells.emplace_back(std::make_unique<BeamSpell>(verifiedData, mTileMap, spellPair.second));
			}
		});
}

void BeamSystem::removeCompletedBeams()
{
	std::erase_if(mBeamSpells, [](const auto& beam)
		{
			return beam->hasCompleted();
		});
}

BeamSystem::SpellPair BeamSystem::getSpellPair(const BeamData& beamData) const
{
	//This function checks wether the casted beam would hit a solid tile
	//If it would, then the length of the beam must be changed to fit on the map
	SpellPair result{ 0, {} };
	int providedLength = beamData.length;
	int &algLength = result.first;
	Direction dir = beamData.dir;
	sf::Vector2i casterCell = Utilities::getCellIndex(beamData.casterPos);

	for (int i = 1; i <= providedLength; ++i)
	{
		sf::Vector2i cell = casterCell;
		int offset = i;
		if (dir == Direction::Up || dir == Direction::Left)
			offset = i * (-1);

		if (dir == Direction::Up || dir == Direction::Bottom)
			cell.y += offset;
		else if (dir == Direction::Left || dir == Direction::Right)
			cell.x += offset;

		const Tile* tile = mTileMap.getTile(cell.x, cell.y);
		if (tile)
		{
			if (!tile->isWalkableRaw())
				break;
			result.second.push_back(tile);
			++algLength;
		}
	}

	//length of beam cannot be 1
	//because of way it is rendered - from middle point of first tile to middle point to last tile
	//this is a special case, in which we will allow to cast beam on a wall.
	if (algLength == 1)
		algLength = 2;

	return result;
}

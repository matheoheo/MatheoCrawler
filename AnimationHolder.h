#pragma once
#include "AnimationIdentifiers.h"
#include "ConceptDefinitions.h"
#include "Directions.h"
#include "AnimationFrame.h"
#include "EntityTypes.h"
#include "FileHandler.h"
#include "Parser.h"

struct GenericAnimationKey
{
	AnimationIdentifier id;
	Direction dir;

	bool operator==(const GenericAnimationKey& other) const
	{
		return id == other.id && dir == other.dir;
	}

	struct Hash
	{
		size_t operator()(const GenericAnimationKey& key) const
		{
			return std::hash<int>()(static_cast<int>(key.id)) ^
				std::hash<int>()(static_cast<int>(key.dir)) << 1;
		}
	};
};
struct EntityAnimationKey
{
	AnimationIdentifier id;
	Direction dir;
	EntityType type;

	bool operator==(const EntityAnimationKey& other) const
	{
		return id == other.id && dir == other.dir && type == other.type;
	}

	struct Hash
	{
		size_t operator()(const EntityAnimationKey& key) const
		{
			return
				std::hash<int>()(static_cast<int>(key.id)) ^
				std::hash<int>()(static_cast<int>(key.dir)) << 1 ^
				std::hash<int>()(static_cast<int>(key.type)) << 2;
		}
	};
};

class AnimationHolder
{
public:
	using AnimationFrames = std::vector<AnimationFrame>;
	using FrameData = std::unordered_map<std::string, std::string>;
	AnimationHolder();
	void loadAnimations();

	const AnimationFrames& get(const GenericAnimationKey& key) const;
	const AnimationFrames& get(const EntityAnimationKey& key) const;
private:
	void loadGenericAnimations();
	void loadPlayerSpecificAnimations();
	void loadSkletorusSpecificAnimations();
	AnimationFrame makeFrame(const FrameData& data) const;

	int parseInt(const FrameData& data, const std::string& key, int defaultReturn = 0) const;
	float parseFloat(const FrameData& data, const std::string& key, float defaultReturn = 0.f) const;

	template<AnimationKeyType KeyType, typename Container, typename KeyMapper>
	void loadAnimationsFromFile(Container& container, std::string_view filename, KeyMapper mapIdToKey);
private:
	std::unordered_map<GenericAnimationKey, AnimationFrames, GenericAnimationKey::Hash> mGenericAnimations;
	std::unordered_map<EntityAnimationKey, AnimationFrames, EntityAnimationKey::Hash> mEntityAnimations;
};

template<AnimationKeyType KeyType, typename Container, typename KeyMapper>
inline void AnimationHolder::loadAnimationsFromFile(Container& container, std::string_view filename, KeyMapper mapIdToKey)
{
	FileHandler fileHandler;
	Parser parser;
	auto genericData = fileHandler.readFileContent(filename);
	auto parserResult = parser.parseData(genericData);

	for (const auto& animData : parserResult)
	{
		int animId = 0;
		if (animData.category == "spritesheetData")
			animId = parseInt(animData.data, "animationId");

		AnimationFrames frames;
		for (const auto& frameData : animData.childrenNodes)
		{
			if (frameData.category == "frame")
				frames.push_back(makeFrame(frameData.data));
		}

		KeyType key = mapIdToKey(animId);
		container[key] = std::move(frames);
	}
}

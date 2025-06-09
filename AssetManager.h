#pragma once
#include "ConceptDefinitions.h"
#include <unordered_map>
#include <cassert>
template <EnumKey Identifier, SFMLAsset Asset>
class AssetManager
{
public:
	bool load(Identifier id, std::string_view path);
	const Asset& get(Identifier id) const;
private:
	std::unordered_map<Identifier, std::unique_ptr<Asset>> mAssets;
};

template<EnumKey Identifier, SFMLAsset Asset>
inline bool AssetManager<Identifier, Asset>::load(Identifier id, std::string_view path)
{
	auto it = mAssets.try_emplace(id, std::make_unique<Asset>(path));
	return it.second;
}

template<EnumKey Identifier, SFMLAsset Asset>
inline const Asset& AssetManager<Identifier, Asset>::get(Identifier id) const
{
	assert(mAssets.contains(id));
	return std::cref(*mAssets.at(id));
}

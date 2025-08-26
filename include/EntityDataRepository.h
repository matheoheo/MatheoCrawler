#pragma once
#include "EntityTypes.h"
#include "AttackData.h"
#include "Components.h"
struct BaseEntityData;

class EntityDataRepository
{
public:
    using AttackDataMap = std::unordered_map<AnimationIdentifier, AttackData>;

    struct BaseEntityData {
        std::string tag;
        float moveSpeed = 130.f;
        int fovRange = 1;
        CombatStatsComponent combatStats;
        AttackDataMap cAttackDataMap;
    };

    EntityDataRepository();
    const BaseEntityData& get(EntityType entType) const;
private:
    
    void createEntityData();
    void createPlayerEntityData();
    void createSkletorusEntityData();
    void createBonvikEntityData();
    void createMorannaEntityData();
    void createRayEntityData();

    AttackDataMap createSimpleMeleeAttackDataMap() const;
    AttackDataMap createDualStrikeMeleeAttackDataMap() const;
    AttackDataMap createPlayerAttackDataMap() const;
    AttackDataMap createRayAttackDataMap() const;

    std::unordered_map<Direction, AttackData::HitOffsetsVec> getDefaultHitoffsets() const;
private:
    std::unordered_map<EntityType, BaseEntityData> mEntityData;

};


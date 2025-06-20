#include "pch.h"
#include "SpellHolder.h"

SpellHolder::SpellHolder()
{
    initalizeSpells();
}

void SpellHolder::initalizeSpells()
{
    createMajorHeal();
    createQuickHeal();
    createHealthRegen();
    createManaRegen();

    createWaterBall();
}

const SpellData& SpellHolder::get(SpellIdentifier id) const
{
    assert(mSpellsMap.contains(id));

    return mSpellsMap.at(id);
}

void SpellHolder::createQuickHeal()
{
    mSpellsMap[SpellIdentifier::QuickHeal] = SpellData{
        .type = SpellType::Heal,
        .name = "Blank Dream",
        .cost = 19,
        .cooldown = 5 * 1000,
        .castTime = 200,
        .healValue = 0.1f
    };
}

void SpellHolder::createMajorHeal()
{
    mSpellsMap[SpellIdentifier::MajorHeal] = SpellData{
        .type = SpellType::Heal,
        .name = "Ertomus",
        .cost = 41,
        .cooldown = 15 * 1000,
        .castTime = 800,
        .healValue = 0.35f
    };
    
}

void SpellHolder::createHealthRegen()
{
    mSpellsMap[SpellIdentifier::HealthRegen] = SpellData{
        .type = SpellType::Heal,
        .name = "Regenupper",
        .cost = 7,
        .cooldown = 3 * 1000,
        .castTime = 600,
        .healValue = 0.0f,
        .duration = 9 * 1000,
        .bonusHpRegen = 12,
        .bonusManaRegen = 2
    };
}

void SpellHolder::createManaRegen()
{
    mSpellsMap[SpellIdentifier::ManaRegen] = {
        .type = SpellType::Heal,
        .name = "Regenupper",
        .cost = 7,
        .cooldown = 3 * 1000,
        .castTime = 400,
        .healValue = 0.0f,
        .duration = 7 * 1000,
        .bonusHpRegen = 0,
        .bonusManaRegen = 8
    };
}

void SpellHolder::createWaterBall()
{
    SpellData data;
    data.type = SpellType::Projectile;
    data.castTime = 450;
    data.cooldown = 2 * 1000;
    data.cost = 1;
    
    data.projectile = ProjectileSpell{
        .minDmg = 5,
        .maxDmg = 8,
        .shotAmount = 1,
        .range = 6,
        .speed = 300.f,
        .pierce = false,
        .maxTargets = 1
    };

    mSpellsMap.emplace(SpellIdentifier::WaterBall, std::move(data));
}

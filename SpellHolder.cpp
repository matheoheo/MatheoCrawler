#include "pch.h"
#include "SpellHolder.h"

SpellHolder::SpellHolder()
{
    initalizeSpells();
}

SpellHolder& SpellHolder::getInstance()
{
    static SpellHolder holder;
    return holder;
}

SpellData& SpellHolder::get(SpellIdentifier id)
{
    assert(mSpellsMap.contains(id));
    return mSpellsMap.at(id);
}

const SpellData& SpellHolder::get(SpellIdentifier id) const
{
    assert(mSpellsMap.contains(id));
    return mSpellsMap.at(id);
}

const SpellDefinition& SpellHolder::getDefinition(SpellIdentifier id) const
{
    assert(mDefinitions.contains(id));
    return mDefinitions.at(id);
}

void SpellHolder::initalizeSpells()
{
    createMajorHeal();
    createQuickHeal();
    createHealthRegen();
    createManaRegen();

    createWaterBall();
    createPureProjectile();
    createFireball();
    createBloodball();

    createMorannaProjectile();
}

void SpellHolder::createQuickHeal()
{
    HealingSpell hs{
        .healValue = 0.13f
    };

    SpellIdentifier id = SpellIdentifier::QuickHeal;
    mSpellsMap[id] = SpellData{
        .type = SpellType::Heal,
        .spellId = id,
        .cost = 15,
        .cooldown = 9 * 1000,
        .castTime = 300,
        .duration = 0,
        .healing = hs
    };

    mDefinitions[id] = SpellDefinition{
        .data = &mSpellsMap[id],
        .spellInfo{
            .name = "Vita Resurgo",
            .shortDescription = "Quick Heal.",
            .longDescription = "Restores a small amount of health points at a low cost.",
            .textureId = TextureIdentifier::QuickHealIcon   
        }
    };
}

void SpellHolder::createMajorHeal()
{
    HealingSpell hs{
        .healValue = 0.350f
    };
    SpellIdentifier id = SpellIdentifier::MajorHeal;
    mSpellsMap[id] = SpellData{
        .type = SpellType::Heal,
        .spellId = id,
        .cost = 38,
        .cooldown = 18 * 1000,
        .castTime = 700,
        .duration = 0,
        .healing = hs
    };

    mDefinitions[id] = SpellDefinition{
     .data = &mSpellsMap[id],
     .spellInfo{
         .name = "Restorer",
         .shortDescription = "Major Heal.",
         .longDescription = "Heals for a significant part of health points.",
         .textureId = TextureIdentifier::MajorHealIcon //placeholder
     }
    };
}

void SpellHolder::createHealthRegen()
{
    HealingSpell hs{
        .healValue = 0.0f,
        .bonusHpRegen = 11
    };
    SpellIdentifier id{ SpellIdentifier::HealthRegen };

    mSpellsMap[id] = SpellData{
        .type = SpellType::Heal,
        .spellId = id,
        .cost = 21,
        .cooldown = 20 * 1000,
        .castTime = 500,
        .duration = 9 * 1000, //9 seconds
        .healing = hs
    };

    mDefinitions[id] = SpellDefinition{
     .data = &mSpellsMap[id],
     .spellInfo{
         .name = "Bloodrush",
         .shortDescription = "Health Regeneration.",
         .longDescription = "Boosts restoring health points for a few seconds.",
         .textureId = TextureIdentifier::HealthRegenIcon //placeholder
     }
    };
}

void SpellHolder::createManaRegen()
{
    HealingSpell hs{
        .healValue = 0.0f,
        .bonusHpRegen = 0,
        .bonusManaRegen = 7
    };

    SpellIdentifier id{ SpellIdentifier::ManaRegen };
    mSpellsMap[id] = {
        .type = SpellType::Heal,
        .spellId = id,
        .cost = 5,
        .cooldown = 16 * 1000,
        .castTime = 400,
        .duration = 7 * 1000, //7 seconds
        .healing = hs
    };

    mDefinitions[id] = SpellDefinition{
     .data = &mSpellsMap[id],
     .spellInfo{
         .name = "Harmony",
         .shortDescription = "Mana Regeneration.",
         .longDescription = "Brings calmness to your soul, granting increased mana regeneration.",
         .textureId = TextureIdentifier::ManaRegenIcon //placeholder
     }
    };
}

void SpellHolder::createWaterBall()
{
    SpellIdentifier id{ SpellIdentifier::WaterBall };

    SpellData data;
    data.type = SpellType::Projectile;
    data.spellId = id;
    data.castTime = 450;
    data.cooldown = 5 * 1000;
    data.cost = 8;
    
    data.projectile = ProjectileSpell{
        .minDmg = 10,
        .maxDmg = 16,
        .shotAmount = 1,
        .range = 5,
        .speed = 400.f,
        .pierce = false,
        .maxTargets = 1,
        .effect = SpellEffect::MovementSpeedSlow
    };

    mSpellsMap.emplace(id, std::move(data));

    mDefinitions[id] = SpellDefinition{
     .data = &mSpellsMap[id],
     .spellInfo{
         .name = "Aqua Balloon",
         .shortDescription = "Water Ball Projectile.",
         .longDescription = "Launches a concentrated orb of water in a straight line that travels quickly.",
         .textureId = TextureIdentifier::WaterballIcon //placeholder
     }
    };
}

void SpellHolder::createPureProjectile()
{
    SpellIdentifier id{ SpellIdentifier::PureProjectile };
    SpellData data;
    data.type = SpellType::Projectile;
    data.spellId = id;
    data.castTime = 500;
    data.cooldown = 10 * 1000;
    data.cost = 15;

    data.projectile = ProjectileSpell{
        .minDmg = 20,
        .maxDmg = 26,
        .shotAmount = 1,
        .range = 6,
        .speed = 350.f,
        .pierce = false,
        .maxTargets = 1
    };

    mSpellsMap.emplace(id, std::move(data));

    mDefinitions[id] = SpellDefinition{
     .data = &mSpellsMap[id],
     .spellInfo{
         .name = "Salver",
         .shortDescription = "Triple Shot.",
         .longDescription = "Fires 3 bullets in looking direction, allowing you to hit several targets.",
         .textureId = TextureIdentifier::PureProjIcon //placeholder
     }
    };
}

void SpellHolder::createFireball()
{
    SpellIdentifier id{ SpellIdentifier::Fireball };
    SpellData data;
    data.type = SpellType::Projectile;
    data.spellId = id;
    data.castTime = 400;
    data.cooldown = 14 * 1000;
    data.cost = 18;

    data.projectile = ProjectileSpell{
        .minDmg = 17,
        .maxDmg = 25,
        .shotAmount = 1,
        .range = 3,
        .speed = 500.f,
        .pierce = false,
        .maxTargets = 1,
        .effect = SpellEffect::FireBurn
    };

    mSpellsMap.emplace(id, std::move(data));

    mDefinitions[id] = SpellDefinition{
     .data = &mSpellsMap[id],
     .spellInfo{
         .name = "Fireball",
         .shortDescription = "The blaze of the fire.",
         .longDescription = "Cast a powerful spell that realeses blazing fireballs outward in all four directions.",
         .textureId = TextureIdentifier::FireballIcon //placeholder
     }
    };
}

void SpellHolder::createBloodball()
{
    SpellIdentifier id{ SpellIdentifier::Bloodball };
    SpellData data;
    data.type = SpellType::Projectile;
    data.spellId = id;
    data.castTime = 550;
    data.cooldown = 10 * 1000;
    data.cost = 19;

    data.projectile = ProjectileSpell{
        .minDmg = 19,
        .maxDmg = 29,
        .shotAmount = 1,
        .range = 6,
        .speed = 500.f,
        .pierce = false,
        .maxTargets = 1
    };

    mSpellsMap.emplace(SpellIdentifier::Bloodball, std::move(data));

    mDefinitions[id] = SpellDefinition{
     .data = &mSpellsMap[id],
     .spellInfo{
         .name = "Bloodball",
         .shortDescription = "Cursed blood energy.",
         .longDescription = "A sphere charget with energy of spilled blood, designed to hit hard and leave its mark.",
         .textureId = TextureIdentifier::BloodballIcon //placeholder
     }
    };
}

void SpellHolder::createMorannaProjectile()
{
    SpellIdentifier id{ SpellIdentifier::MorannaProjectile };
    SpellData data;
    data.type = SpellType::Projectile;
    data.spellId = id;
    data.castTime = 0; 
    data.cooldown = 0;
    data.cost = 0;

    data.projectile = ProjectileSpell{
        .minDmg = 1,
        .maxDmg = 2,
        .shotAmount = 1,
        .range = 4,
        .speed = 600.f,
        .pierce = false,
        .maxTargets = 1
    };

    mSpellsMap.emplace(id, std::move(data));
}

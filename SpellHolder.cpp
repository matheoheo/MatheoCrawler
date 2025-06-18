#include "pch.h"
#include "SpellHolder.h"

SpellHolder::SpellHolder()
{
    initalizeSpells();
}

void SpellHolder::initalizeSpells()
{
    createBasicHeal();
}

const SpellData& SpellHolder::get(SpellIdentifier id) const
{
    assert(mSpellsMap.contains(id));

    return mSpellsMap.at(id);
}

void SpellHolder::createBasicHeal()
{
    mSpellsMap[SpellIdentifier::BasicHeal] = SpellData{
        .type = SpellType::Heal,
        .name = "Ertomus",
        .cost = 11,
        .cooldown = 2 * 1000,
        .castTime = 500,
        .healValue = 0.3f
    };
    
}

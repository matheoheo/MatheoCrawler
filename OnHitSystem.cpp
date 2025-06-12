#include "pch.h"
#include "OnHitSystem.h"
#include "Random.h"

OnHitSystem::OnHitSystem(SystemContext& systemContext)
    :ISystem(systemContext)
{
    registerToEvents();
}

void OnHitSystem::update(const sf::Time& deltaTime)
{
}

void OnHitSystem::registerToEvents()
{
    registerToHitByAttackEvent();
}

void OnHitSystem::registerToHitByAttackEvent()
{
    mSystemContext.eventManager.registerEvent<HitByAttackEvent>([this](const HitByAttackEvent& data)
        {
            for (Entity* ent : data.hitEntities)
            {
                int damage = calculateDamage(data.attacker, *ent);
                auto damagePair = takeDamage(*ent, damage);
                if (damagePair.second)
                {
                    if(!ent->hasComponent<PlayerComponent>())
                        notifyEntityDied(*ent);
                }
                mSystemContext.eventManager.notify<HealthBarUpdateEvent>(HealthBarUpdateEvent(*ent, damagePair.first));
            }
        });
}

int OnHitSystem::calculateDamage(const Entity& attacker, const Entity& target) const
{
    auto& attackerStats = attacker.getComponent<CombatStatsComponent>();
    auto& attackComp = attacker.getComponent<AttackComponent>();
    auto& targetStats = attacker.getComponent<CombatStatsComponent>();

    //in order to not have always flat damage, we add some randomness to calculations
    int baseDmg = attackerStats.cAttackDamage;
    int minDmg = static_cast<int>(baseDmg * 0.8f);
    int maxDmg = static_cast<int>(baseDmg * 1.2f);
    int thisDmg = Random::get(minDmg, maxDmg);
    thisDmg -= targetStats.cDefence / 2;

    if (attackComp.cLastAttackData)
        thisDmg *= attackComp.cLastAttackData->damageMultiplier;

    return thisDmg;
}

std::pair<int, bool> OnHitSystem::takeDamage(const Entity& target, int damage)
{
    auto& stats = target.getComponent<CombatStatsComponent>();
    if (damage > stats.cHealth)
        damage = stats.cHealth;

    stats.cHealth -= damage;
    return std::make_pair(damage, stats.cHealth <= 0);
}

void OnHitSystem::notifyEntityDied(Entity& entity)
{
    mSystemContext.eventManager.notify<EntityDiedEvent>(EntityDiedEvent(entity));
}

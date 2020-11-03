#include "global.h"
#include "dynamax.h"
#include "battle.h"
#include "battle_evolution.h"
#include "battle_interface.h"

static bool32 IsPartnerMonFromSameTrainer(u8 battlerId)
{
    if (GET_BATTLER_SIDE2(battlerId) == B_SIDE_OPPONENT && gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
        return FALSE;
    else if (GET_BATTLER_SIDE2(battlerId) == B_SIDE_PLAYER && gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
        return FALSE;
    else if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
        return FALSE;
    else
        return TRUE;
}

bool32 CanBattlerEvo(u8 battlerId)
{
    u8 battlerPosition = GET_BATTLER_POSITION(battlerId);
    u8 partnerPosition = GET_BATTLER_POSITION(BATTLE_PARTNER(battlerId));
    struct BattleEvolutionData* mega = &gBattleStruct->mega;

    // Check if trainer already mega evolved a pokemon.
    if (mega->alreadyEvolved[battlerPosition])
        return FALSE;
    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE) {
        if (IsPartnerMonFromSameTrainer(battlerId)
            && (mega->alreadyEvolved[partnerPosition] || (mega->toEvolve & (1 << BATTLE_PARTNER(battlerId)))))
            return FALSE;
    }
    return CheckEvolutionType(mega, battlerId);
}


void AfterEvolutionChangeStates(u32 battlerId, struct Pokemon* pokemon)
{

}
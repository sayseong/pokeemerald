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

bool32 IsEvolutionHappened(u32 battlerId)
{
    return GetEvolutionTypeForBattler(battlerId) >= EvolutionMegaHappend;
}

void CreateTrigger(u8 battlerId, u8 palId)
{
    GetBattleEvolutionFunc(battlerId)->CreateOrShowTrigger(battlerId, palId);
}

void HideTriggerSprite(void)
{
    GetBattleEvolutionFunc(gActiveBattler)->HideTriggerSprite();
}

u8 CreateIndicator(u32 battlerId)
{
    return GetBattleEvolutionFunc(battlerId)->CreateIndicator(battlerId);
}

void DoEvolution(u32 battlerId)
{
    GetBattleEvolutionFunc(battlerId)->DoEvolution(battlerId);
}

void UndoEvolution(u32 monId)
{
    GetBattleEvolutionFuncByPos(monId, 0)->UndoEvolution(monId);
}

void ChangeTriggerSprite(u8 battler, u8 state)
{
    GetBattleEvolutionFunc(battler)->ChangeTriggerSprite(state);
}

u8 GetIndicatorSpriteId(u32 healthboxSpriteId)
{
    return GetMegaIndicatorSpriteId(healthboxSpriteId);
}

static const u8 gDynamaxWeatherEffectCmd[] = { 0x7D, 0xBB, 0x95, 0xC8 };

void HandleTerrainMove(u32 moveEffect);

static const u8 gDynamaxStatChangeScript[] = { 0, 0x15, 0x3c };//donothing seteffectwithchance return
extern const u8 BattleScript_PrintWeatherInfo[];

void HandleDynamaxMoveEffect()
{
    u8 arg;
    arg = gBattleMoves[gCurrentMove].argument;
    if (arg <= DYNAMAX_SET_SANDSTORM)
    {
        gBattleScriptingCommandsTable[gDynamaxWeatherEffectCmd[arg]]();
        if (gMoveResultFlags & MOVE_RESULT_MISSED) return;
        gActiveBattler = gBattlerAttacker;
        BtlController_EmitBattleAnimation(0, 0xA + arg, 0);
        MarkBattlerForControllerExec(gActiveBattler);
        BattleScriptPush(gBattlescriptCurrInstr + 1);
        gBattlescriptCurrInstr = BattleScript_PrintWeatherInfo;
        return;
    }
    if (arg <= DYNAMAX_SET_PSYCHIC_TERRAIN)
    {
        HandleTerrainMove(268 + arg - DYNAMAX_SET_MISTY_TERRAIN);
        return;
    }
    if (arg >= MOVE_EFFECT_ATK_PLUS_1 && arg <= MOVE_EFFECT_SP_DEF_MINUS_1)
    {
        //stat change twice
        if (gBattleScripting.savedMoveEffect == MOVE_EFFECT_DYNAMAX)
        {
            gBattleScripting.moveEffect = arg;
            BattleScriptPush(gBattlescriptCurrInstr + 1);
            gBattlescriptCurrInstr = gDynamaxStatChangeScript;
            gBattleScriptingCommandsTable[0x15]();
            gBattleScripting.savedMoveEffect = 0;
        }
        else
        {
            gBattleScripting.moveEffect = arg;
            gBattleScriptingCommandsTable[0x15]();
            gEffectBattler = BATTLE_PARTNER(gEffectBattler);
        }
        return;
    }

}
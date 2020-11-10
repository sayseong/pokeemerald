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

static const u8 gDynamaxRepeatMoveEffectScript[] = { 0, 0x15, 0x3c };//donothing seteffectwithchance return
extern const u8 BattleScript_PrintWeatherInfo[];
extern const u8 BattleScript_DynamaxPrintTerrain[];

void RepeatCommand(u8 arg, u8 cmd) {

}

void SetEffectTargetAll(u8 arg) {
    if (gBattleScripting.savedMoveEffect == MOVE_EFFECT_DYNAMAX)
    {
        gBattleScripting.moveEffect = arg;
        BattleScriptPush(gBattlescriptCurrInstr + 1);
        gBattlescriptCurrInstr = gDynamaxRepeatMoveEffectScript;
        gBattleScriptingCommandsTable[0x15]();
        gBattleScripting.savedMoveEffect = 0;
        gBattleScripting.moveEffect = MOVE_EFFECT_DYNAMAX;
    }
    else
    {
        if (IsBattlerAlive(BATTLE_PARTNER(gBattlerTarget)))
        {
            gBattlerTarget = BATTLE_PARTNER(gBattlerTarget);//临时更改为队友
            gBattleScripting.moveEffect = arg;
            gBattleScriptingCommandsTable[0x15]();
        }
        else
        {
            gBattlescriptCurrInstr++;
        }
    }
}

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
        BattleScriptPush(gBattlescriptCurrInstr + 1);
        gBattlescriptCurrInstr = BattleScript_DynamaxPrintTerrain;
        HandleTerrainMove(268 + arg - DYNAMAX_SET_MISTY_TERRAIN);
        return;
    }
    if (arg >= MOVE_EFFECT_ATK_PLUS_1 && arg <= MOVE_EFFECT_SP_DEF_MINUS_1)
    {
        //stat change twice
        SetEffectTargetAll(arg);
        return;
    }

}
#include "constants/moves.h"
#include "random.h"

extern u8 BattleScript_DynamaxPrintAuroraVeil[];
extern u8 BattleScript_DynamaxTryinfatuating[];
void HandleGiaMaxMoveEffect()
{
    u8 arg;
    arg = gBattleMoves[gCurrentMove].argument;
    switch (arg + G_MAX_WILDFIRE)
    {
    case G_MAX_WILDFIRE:
        gBattleStruct->mega.gMaxFieldCounter = 4;
        gBattleStruct->mega.gMaxFieldType = TYPE_FIRE;
        gBattlescriptCurrInstr++;
        break;
    case G_MAX_BEFUDDLE://随机异常
        SetEffectTargetAll(MOVE_EFFECT_SLEEP +(Random() & 3));
        break;
    case G_MAX_VOLT_CRASH:
        SetEffectTargetAll(MOVE_EFFECT_PARALYSIS);
        break;
    case G_MAX_GOLD_RUSH:
        gPaydayMoney += (gBattleMons[gBattlerAttacker].level * 200);
        SetEffectTargetAll(MOVE_EFFECT_CONFUSION);
        break;
    case G_MAX_CHI_STRIKE://FLAG_HIGH_CRIT
        break;
    case G_MAX_TERROR:
        SetEffectTargetAll(MOVE_EFFECT_PREVENT_ESCAPE);
        break;
    case G_MAX_RESONANCE:
        BattleScriptPush(gBattlescriptCurrInstr + 1);
        gBattlescriptCurrInstr = BattleScript_DynamaxPrintAuroraVeil;
        break;
    case G_MAX_CUDDLE:

        break;
    }
}
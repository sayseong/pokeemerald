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

void SetEffectTargetAll(u8 arg) {

    if (gBattleScripting.savedMoveEffect == MOVE_EFFECT_DYNAMAX)
    {
        gBattleScripting.moveEffect = arg;
        gBattlescriptCurrInstr--;
        SetMoveEffect(FALSE, MOVE_EFFECT_CERTAIN);
        gBattleScripting.savedMoveEffect = 0;
        gBattleScripting.moveEffect = MOVE_EFFECT_DYNAMAX;
    }
    else
    {
        if (IsBattlerAlive(BATTLE_PARTNER(gBattlerTarget)))
        {
            gBattlerTarget = BATTLE_PARTNER(gBattlerTarget);//临时更改为队友
            gBattleScripting.moveEffect = arg;
            SetMoveEffect(FALSE, MOVE_EFFECT_CERTAIN);
        }
        else
        {
            gBattlescriptCurrInstr++;
        }
        gBattleScripting.moveEffect = 0;
    }
}
extern u8 BattleScript_SpikesFree[];
extern u8 BattleScript_ToxicSpikesFree[];
extern u8 BattleScript_StickyWebFree[];
extern u8 BattleScript_StealthRockFree[];
extern u8 BattleScript_DynamaxClearFieldEffects[];

void HandleClearRocks() {
    u8 atkSide = GET_BATTLER_SIDE2(gBattlerAttacker);

    if (gSideStatuses[atkSide] & SIDE_STATUS_SPIKES)
    {
        gSideStatuses[atkSide] &= ~(SIDE_STATUS_SPIKES);
        gSideTimers[atkSide].spikesAmount = 0;
        BattleScriptPushCursor();
        gBattlescriptCurrInstr = BattleScript_SpikesFree;
    }
    else if (gSideStatuses[atkSide] & SIDE_STATUS_TOXIC_SPIKES)
    {
        gSideStatuses[atkSide] &= ~(SIDE_STATUS_TOXIC_SPIKES);
        gSideTimers[atkSide].toxicSpikesAmount = 0;
        BattleScriptPushCursor();
        gBattlescriptCurrInstr = BattleScript_ToxicSpikesFree;
    }
    else if (gSideStatuses[atkSide] & SIDE_STATUS_STICKY_WEB)
    {
        gSideStatuses[atkSide] &= ~(SIDE_STATUS_STICKY_WEB);
        gSideTimers[atkSide].stickyWebAmount = 0;
        BattleScriptPushCursor();
        gBattlescriptCurrInstr = BattleScript_StickyWebFree;
    }
    else if (gSideStatuses[atkSide] & SIDE_STATUS_STEALTH_ROCK)
    {
        gSideStatuses[atkSide] &= ~(SIDE_STATUS_STEALTH_ROCK);
        gSideTimers[atkSide].stealthRockAmount = 0;
        BattleScriptPushCursor();
        gBattlescriptCurrInstr = BattleScript_StealthRockFree;
    }
    else if (gFieldStatuses > 0)
    {
        gFieldStatuses = 0;
        gFieldTimers.grassyTerrainTimer = 0;
        gFieldTimers.mistyTerrainTimer = 0;
        gFieldTimers.electricTerrainTimer = 0;
        gFieldTimers.psychicTerrainTimer = 0;
        gBattleCommunication[MULTISTRING_CHOOSER] = 0;
        BattleScriptPushCursor();
        gBattlescriptCurrInstr = BattleScript_DynamaxClearFieldEffects;
    }
    else if (gSideTimers[BATTLE_OPPOSITE(atkSide)].reflectTimer)
    {
        gSideStatuses[BATTLE_OPPOSITE(atkSide)] &= ~(SIDE_STATUS_REFLECT);
        gSideTimers[BATTLE_OPPOSITE(atkSide)].reflectTimer = 0;
        gBattleCommunication[MULTISTRING_CHOOSER] = 1;
        BattleScriptPushCursor();
        gBattlescriptCurrInstr = BattleScript_DynamaxClearFieldEffects;
    }
    else if (gSideTimers[BATTLE_OPPOSITE(atkSide)].lightscreenTimer)
    {
        gSideStatuses[BATTLE_OPPOSITE(atkSide)] &= ~(SIDE_STATUS_LIGHTSCREEN);
        gSideTimers[BATTLE_OPPOSITE(atkSide)].lightscreenTimer = 0;
        gBattleCommunication[MULTISTRING_CHOOSER] = 2;
        BattleScriptPushCursor();
        gBattlescriptCurrInstr = BattleScript_DynamaxClearFieldEffects;
    }
    else
    {
        gBattlescriptCurrInstr++;
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
    }
    else if (arg <= DYNAMAX_SET_PSYCHIC_TERRAIN)
    {
        BattleScriptPush(gBattlescriptCurrInstr + 1);
        gBattlescriptCurrInstr = BattleScript_DynamaxPrintTerrain;
        HandleTerrainMove(268 + arg - DYNAMAX_SET_MISTY_TERRAIN);
    }
    else if (arg >= MOVE_EFFECT_ATK_PLUS_1 && arg <= MOVE_EFFECT_SP_DEF_MINUS_1)
    {
        //stat change twice
        SetEffectTargetAll(arg);
        return;
    }
    gBattleScripting.moveEffect = 0;
}
#include "constants/moves.h"
#include "random.h"

extern u8 BattleScript_DynamaxPrintAuroraVeil[];
extern u8 BattleScript_DyamaxTryinfatuating[];
extern u8 BattleScript_DynamaxRecycleItem[];
extern u8 BattleScript_DynamaxSetTealthrock[];
extern u8 BattleScript_DynamaxHealSelfAll[];
extern u8 BattleScript_DyanamaxTryppreduce[];

void HandleGiaMaxMoveEffect()
{
    u8 arg;
    arg = gBattleMoves[gCurrentMove].argument;
#define bs_push(co, ptr) BattleScriptPush(gBattlescriptCurrInstr+co);gBattlescriptCurrInstr = BattleScript_DynamaxSetTealthrock;
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
    case G_MAX_REPLENISH:
        BattleScriptPush(gBattlescriptCurrInstr + 1);
        gBattlescriptCurrInstr = BattleScript_DynamaxRecycleItem;
        break;
    case G_MAX_MALODOR:
        SetEffectTargetAll(MOVE_EFFECT_POISON);
        break;
    case G_MAX_STONESURGE:
        bs_push(1, BattleScript_DynamaxSetTealthrock)
        break;
    case G_MAX_WIND_RAGE:
        HandleClearRocks();
        break;
    case G_MAX_STUN_SHOCK:
        SetEffectTargetAll(Random() & 1 ? MOVE_EFFECT_POISON : MOVE_EFFECT_PARALYSIS);
        break;
    case G_MAX_FINALE:
        if (gBattleScripting.savedMoveEffect == MOVE_EFFECT_DYNAMAX)
        {
            gBattleScripting.savedMoveEffect = 0;
            if (gBattleMons[gBattlerAttacker].maxHP != gBattleMons[gBattlerAttacker].hp)
            {
                gEffectBattler = gBattlerAttacker;
                gBattleMoveDamage = gBattleMons[gEffectBattler].hp - gBattleMons[gEffectBattler].maxHP;
                bs_push(0, BattleScript_DynamaxHealSelfAll)
            }
        }
        else
        {
            gBattlescriptCurrInstr++;
            if (gBattleMons[BATTLE_PARTNER(gBattlerAttacker)].maxHP != gBattleMons[BATTLE_PARTNER(gBattlerAttacker)].hp)
            {
                gEffectBattler = BATTLE_PARTNER(gBattlerAttacker);
                gBattleMoveDamage = gBattleMons[gEffectBattler].hp - gBattleMons[gEffectBattler].maxHP;
                bs_push(0, BattleScript_DynamaxHealSelfAll)
            }
        }
        break;
    case G_MAX_DEPLETION:
        bs_push(1, BattleScript_DynamaxTryppreduce);
        break;
    case G_MAX_GRAVITAS:
        break;
    case G_MAX_VOLCALITH:
        break;
    case G_MAX_SANDBLAST:
        break;
    }
}
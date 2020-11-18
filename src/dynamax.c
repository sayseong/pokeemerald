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
    return CalEvolutionType(battlerId) != EvolutionNone;
}

bool32 IsEvolutionHappened(u32 battlerId)
{
    return gBattleStruct->mega.evolutionType[GET_BATTLER_SIDE2(battlerId)][gBattlerPartyIndexes[battlerId]] > EvolutionNone;
}

void CreateTrigger(u8 battlerId, u8 palId)
{
    TypeCreateOrShowTrigger func = GetBattleEvolutionFunc(battlerId)->CreateOrShowTrigger;
    func(battlerId, palId);//for debugger can step into
}

void HideTriggerSprite(void)
{
    TypeHideTriggerSprite func = GetBattleEvolutionFunc(gActiveBattler)->HideTriggerSprite;
    func();
    gBattleStruct->mega.playerSelect = FALSE;
}

u8 CreateIndicator(u32 battlerId)
{
     TypeCreateIndicator func = GetBattleEvolutionFunc(battlerId)->CreateIndicator;
     return func(battlerId);
}

void DoEvolution(u32 battlerId)
{
    TypeDoEvolution func = GetBattleEvolutionFunc(battlerId)->DoEvolution;
    func(battlerId);
}

void UndoEvolution(u32 monId)
{
    TypeUndoEvolution func = GetBattlerFuncByEvolutionType(monId, 0)->UndoEvolution;
    func(monId);
}

void ChangeTriggerSprite(u8 battler, u8 state)
{
    TypeChangeTriggerSprite func = GetBattleEvolutionFunc(battler)->ChangeTriggerSprite;
    func(state);
}

u8 GetIndicatorSpriteId(u32 healthboxSpriteId)
{
    return GetMegaIndicatorSpriteId(healthboxSpriteId);
}

static const u8 gDynamaxWeatherEffectCmd[] = { 0x7D, 0xBB, 0x95, 0xC8 };

void HandleTerrainMove(u32 moveEffect);

extern u8 gDynamaxRepeatMoveEffectScript[];//donothing seteffectwithchance return
extern const u8 BattleScript_PrintWeatherInfo[];
extern const u8 BattleScript_DynamaxPrintTerrain[];

void SetEffectTargetAll(u8 arg) {

    if (gBattleScripting.savedMoveEffect == MOVE_EFFECT_DYNAMAX)
    {
        gBattleScripting.moveEffect = arg;
        BattleScriptPush(gBattlescriptCurrInstr + 1);
        gBattlescriptCurrInstr = gDynamaxRepeatMoveEffectScript;
        SetMoveEffect(FALSE, MOVE_EFFECT_CERTAIN);
        gBattleScripting.savedMoveEffect = 0;
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
    }
}
extern u8 BattleScript_SpikesFree[];
extern u8 BattleScript_ToxicSpikesFree[];
extern u8 BattleScript_StickyWebFree[];
extern u8 BattleScript_StealthRockFree[];
extern u8 BattleScript_DynamaxClearFieldEffects[];

void* HandleClearRocks() {
    u8 atkSide = GET_BATTLER_SIDE2(gBattlerAttacker);

    if (gSideStatuses[atkSide] & SIDE_STATUS_SPIKES)
    {
        gSideStatuses[atkSide] &= ~(SIDE_STATUS_SPIKES);
        gSideTimers[atkSide].spikesAmount = 0;
        return BattleScript_SpikesFree;
    }
    else if (gSideStatuses[atkSide] & SIDE_STATUS_TOXIC_SPIKES)
    {
        gSideStatuses[atkSide] &= ~(SIDE_STATUS_TOXIC_SPIKES);
        gSideTimers[atkSide].toxicSpikesAmount = 0;
        return BattleScript_ToxicSpikesFree;
    }
    else if (gSideStatuses[atkSide] & SIDE_STATUS_STICKY_WEB)
    {
        gSideStatuses[atkSide] &= ~(SIDE_STATUS_STICKY_WEB);
        gSideTimers[atkSide].stickyWebAmount = 0;
        return BattleScript_StickyWebFree;
    }
    else if (gSideStatuses[atkSide] & SIDE_STATUS_STEALTH_ROCK)
    {
        gSideStatuses[atkSide] &= ~(SIDE_STATUS_STEALTH_ROCK);
        gSideTimers[atkSide].stealthRockAmount = 0;
        return BattleScript_StealthRockFree;
    }
    else if (gFieldStatuses > 0)
    {
        gFieldStatuses = 0;
        gFieldTimers.grassyTerrainTimer = 0;
        gFieldTimers.mistyTerrainTimer = 0;
        gFieldTimers.electricTerrainTimer = 0;
        gFieldTimers.psychicTerrainTimer = 0;
        gBattleCommunication[MULTISTRING_CHOOSER] = 0;
        return BattleScript_DynamaxClearFieldEffects;
    }
    else if (gSideTimers[BATTLE_OPPOSITE(atkSide)].reflectTimer)
    {
        gSideStatuses[BATTLE_OPPOSITE(atkSide)] &= ~(SIDE_STATUS_REFLECT);
        gSideTimers[BATTLE_OPPOSITE(atkSide)].reflectTimer = 0;
        gBattleCommunication[MULTISTRING_CHOOSER] = 1;
        return BattleScript_DynamaxClearFieldEffects;
    }
    else if (gSideTimers[BATTLE_OPPOSITE(atkSide)].lightscreenTimer)
    {
        gSideStatuses[BATTLE_OPPOSITE(atkSide)] &= ~(SIDE_STATUS_LIGHTSCREEN);
        gSideTimers[BATTLE_OPPOSITE(atkSide)].lightscreenTimer = 0;
        gBattleCommunication[MULTISTRING_CHOOSER] = 2;
        return BattleScript_DynamaxClearFieldEffects;
    }
    else
    {
        return NULL;
    }
}

void SetGMaxFieldData(u8 type)
{
    if (gBattleStruct->mega.gMaxFieldType != type || gBattleStruct->mega.gMaxFieldCounter == 0)
    {
        gBattleStruct->mega.gMaxFieldCounter = 4;
        gBattleStruct->mega.gMaxFieldType = type;
    }
}
void HandleGiaMaxMoveEffect();
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
    }
    else if (arg == DYNAMAX_GMAX_1)
    {
        HandleGiaMaxMoveEffect();
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
extern u8 BattleScript_DynamaxTryppreduce[];
extern u8 BattleScript_DynamaxHealPartyStatus[];
extern u8 BattleScript_DynamaxSetTorment[];

inline void BattleScriptPushCurrent(void* newPtr)
{
    BattleScriptPushCursor();
    gBattlescriptCurrInstr = newPtr;
}

void HandleGiaMaxMoveEffect()
{
#define bs_push(co, ptr) BattleScriptPush(gBattlescriptCurrInstr+co);gBattlescriptCurrInstr = ptr;
    switch (gCurrentMove)
    {
    case MOVE_G_MAX_WILDFIRE:
        SetGMaxFieldData(TYPE_FIRE);
        gBattlescriptCurrInstr++;
        break;
    case MOVE_G_MAX_BEFUDDLE://随机异常
        SetEffectTargetAll(MOVE_EFFECT_SLEEP +(Random() & 3));
        break;
    case MOVE_G_MAX_VOLT_CRASH:
        SetEffectTargetAll(MOVE_EFFECT_PARALYSIS);
        break;
    case MOVE_G_MAX_GOLD_RUSH:
        gPaydayMoney += (gBattleMons[gBattlerAttacker].level * 200);
        SetEffectTargetAll(MOVE_EFFECT_CONFUSION);
        break;
    case MOVE_G_MAX_CHI_STRIKE://FLAG_HIGH_CRIT
        break;
    case MOVE_G_MAX_TERROR:
        SetEffectTargetAll(MOVE_EFFECT_PREVENT_ESCAPE);
        break;
    case MOVE_G_MAX_RESONANCE:
        BattleScriptPush(gBattlescriptCurrInstr + 1);
        gBattlescriptCurrInstr = BattleScript_DynamaxPrintAuroraVeil;
        break;
    case MOVE_G_MAX_CUDDLE:

        break;
    case MOVE_G_MAX_REPLENISH:
        BattleScriptPush(gBattlescriptCurrInstr + 1);
        gBattlescriptCurrInstr = BattleScript_DynamaxRecycleItem;
        break;
    case MOVE_G_MAX_MALODOR:
        SetEffectTargetAll(MOVE_EFFECT_POISON);
        break;
    case MOVE_G_MAX_STONESURGE:
        bs_push(1, BattleScript_DynamaxSetTealthrock)
        break;
    case MOVE_G_MAX_WIND_RAGE:
    {
        void* ptr = HandleClearRocks();
        if (ptr) BattleScriptPushCurrent(ptr);
        else gBattlescriptCurrInstr++;
    }
        break;
    case MOVE_G_MAX_STUN_SHOCK:
        SetEffectTargetAll(Random() & 1 ? MOVE_EFFECT_POISON : MOVE_EFFECT_PARALYSIS);
        break;
    case MOVE_G_MAX_FINALE:
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
    case MOVE_G_MAX_DEPLETION:
        bs_push(1, BattleScript_DynamaxTryppreduce);
        break;
    case MOVE_G_MAX_GRAVITAS:
        break;
    case MOVE_G_MAX_VOLCALITH:
        break;
    case MOVE_G_MAX_SANDBLAST://MOVE_EFFECT_WRAP
        break;
    case MOVE_G_MAX_SNOOZE://sleep
        break;
    case MOVE_G_MAX_TARTNESS://闪避率
        break;
    case MOVE_G_MAX_SWEETNESS:
        bs_push(1, BattleScript_DynamaxHealPartyStatus)
        break;
    case MOVE_G_MAX_SMITE://BattleScript_EffectConfuseHit
        break;
    case MOVE_G_MAX_STEELSURGE:
        break;
    case MOVE_G_MAX_MELTDOWN:
        gBattlescriptCurrInstr = BattleScript_DynamaxSetTorment;
        break;
    case MOVE_G_MAX_FOAM_BURST:
        SetEffectTargetAll(MOVE_EFFECT_SPD_MINUS_2);
        break;
    case MOVE_G_MAX_CENTIFERNO:
        break;
    case MOVE_G_MAX_DRUM_SOLO://FLAG_TARGET_ABILITY_IGNORED
    case MOVE_G_MAX_FIREBALL:
    case MOVE_G_MAX_HYDROSNIPE:
        break;
    case MOVE_G_MAX_VINE_LASH:
        SetGMaxFieldData(TYPE_GRASS);
        gBattlescriptCurrInstr++;
        break;
    case MOVE_G_MAX_CANNONADE:
        SetGMaxFieldData(TYPE_WATER);
        gBattlescriptCurrInstr++;
        break;
    }
#undef bs_push
}

extern u8 BattleScript_SlowStartEnds[];

bool32 HandleDynamaxEndTurnEffect()
{
    bool32 ret;
    struct BattleEvolutionData* mega;
    mega = &gBattleStruct->mega;
    ret = FALSE;
    switch (mega->gMaxEndTurnTracer)
    {
    case 0://revert
        if (mega->timer[GET_BATTLER_SIDE2(gActiveBattler)]
            && gBattleStruct->mega.evolutionType[GET_BATTLER_SIDE2(gActiveBattler)][gBattlerPartyIndexes[gActiveBattler]] == EvolutionDynamax
            && --mega->timer[GET_BATTLER_SIDE2(gActiveBattler)] == 0)
        {
            BattleScriptExecute(BattleScript_SlowStartEnds);
            ret = TRUE;
        }
        gBattleStruct->mega.gMaxEndTurnTracer++;
        break;
    case 1://field
        gBattleStruct->mega.gMaxEndTurnTracer = 0;
        gBattleStruct->turnEffectsBattlerId++;
        break;
    }

    return ret;
}
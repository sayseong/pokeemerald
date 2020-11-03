extern "C" {
#define GUARD_SPRITE_H
#define _STDDEF_H
#include <stdint.h>
#include "constants/global.h"
typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t    s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef volatile u8   vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;
typedef volatile s8   vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

typedef float  f32;
typedef double f64;

typedef u8  bool8;
typedef u16 bool16;
typedef u32 bool32;
typedef vu8  vbool8;
typedef vu16 vbool16;
typedef vu32 vbool32;
#include "constants/battle.h"
#include "global.berry.h"

#define PARTY_SIZE 6
struct BattleTv_Side
{
    u32 spikesMonId:3;
    u32 reflectMonId:3;
    u32 lightScreenMonId:3;
    u32 safeguardMonId:3;
    u32 mistMonId:3;
    u32 futureSightMonId:3;
    u32 doomDesireMonId:3;
    u32 perishSongMonId:3;
    u32 wishMonId:3;
    u32 grudgeMonId:3;
    u32 usedMoveSlot:2;
    u32 spikesMoveSlot:2;
    u32 reflectMoveSlot:2;
    u32 lightScreenMoveSlot:2;
    u32 safeguardMoveSlot:2;
    u32 mistMoveSlot:2;
    u32 futureSightMoveSlot:2;
    u32 doomDesireMoveSlot:2;
    u32 perishSongMoveSlot:2;
    u32 wishMoveSlot:2;
    u32 grudgeMoveSlot:2;
    u32 destinyBondMonId:3;
    u32 destinyBondMoveSlot:2;
    u32 faintCause:4;
    u32 faintCauseMonId:3;
    u32 explosion:1;
    u32 explosionMoveSlot:2;
    u32 explosionMonId:3;
    u32 perishSong:1;
};

struct BattleTv_Position
{
    u32 curseMonId:3;
    u32 leechSeedMonId:3;
    u32 nightmareMonId:3;
    u32 wrapMonId:3;
    u32 attractMonId:3;
    u32 confusionMonId:3;
    u32 curseMoveSlot:2;
    u32 leechSeedMoveSlot:2;
    u32 nightmareMoveSlot:2;
    u32 wrapMoveSlot:2;
    u32 attractMoveSlot:2;
    u32 confusionMoveSlot:2;
    u32 waterSportMoveSlot:2;
    u32 waterSportMonId:3;
    u32 mudSportMonId:3;
    u32 mudSportMoveSlot:2;
    u32 ingrainMonId:3;
    u32 ingrainMoveSlot:2;
    u32 attackedByMonId:3;
    u32 attackedByMoveSlot:2;
};

struct BattleTv_Mon
{
    u32 psnMonId:3;
    u32 badPsnMonId:3;
    u32 brnMonId:3;
    u32 prlzMonId:3;
    u32 slpMonId:3;
    u32 frzMonId:3;
    u32 psnMoveSlot:2;
    u32 badPsnMoveSlot:2;
    u32 brnMoveSlot:2;
    u32 prlzMoveSlot:2;
    u32 slpMoveSlot:2;
    u32 frzMoveSlot:2;
};

struct BattleTv
{
    struct BattleTv_Mon mon[2][PARTY_SIZE]; // [side][partyId]
    struct BattleTv_Position pos[2][2]; // [side][flank]
    struct BattleTv_Side side[2]; // [side]
};

struct BattleTvMovePoints
{
    s16 points[2][PARTY_SIZE * 4];
};

struct MegaEvolutionData
{
    u8 toEvolve; // As flags using gBitTable.
    u8 evolvedPartyIds[2]; // As flags using gBitTable;
    bool8 alreadyEvolved[4]; // Array id is used for mon position.
    u16 evolvedSpecies[MAX_BATTLERS_COUNT];
    u16 playerEvolvedSpecies;
    u8 battlerId;
    bool8 playerSelect;
    u8 triggerSpriteId;
    struct DynamaxData
    {
        bool8 toBeUsed[MAX_BATTLERS_COUNT];
        bool8 used[MAX_BATTLERS_COUNT];
        s8 timer[MAX_BATTLERS_COUNT]; //Negative number means permanent
        u8 partyIndex[2];
        u8 shieldSpriteIds[5]; //Shields for raid battles
        u8 shieldCount;					//The amount of shields created in a Raid Battle
        u8 shieldsDestroyed;			//The amount of shields destroyed in a Raid Battle
        u8 stormLevel;					//The number of Pokemon the raid boss has KO'd.
        u8 repeatedAttacks;				//The amount of times the raid boss took another attack
        bool8 active : 1;
        bool8 viewing : 1;
        bool8 raidShieldsUp : 1;
        bool8 attackAgain : 1;
        bool8 nullifiedStats : 1;
        u8 backupMoveSelectionCursorPos;
        u16 turnStartHP;
        u16 backupRaidMonItem;
    } dynamaxData;
};

struct Illusion
{
    u8 on;
    u8 set;
    u8 broken;
    u8 partyId;
    struct Pokemon *mon;
};

struct BattleEvolutionData {
    u8 evolutionType[2][6];
    u8 alreadyEvolved[4];
    u8 partyEvolvedType[6];
    u8 playerSelect;
    u8 triggerSpriteId;
    u8 dynamaxTriggerId;
    u8 battlerId;
    u8 toEvolve;
    u16 evolvedSpecies[4];
    u16 playerEvolvedSpecies;
};

struct BattleStruct
{
    u8 turnEffectsTracker;
    u8 turnEffectsBattlerId;
    u8 turnCountersTracker;
    u16 wrappedMove[MAX_BATTLERS_COUNT];
    u8 moveTarget[MAX_BATTLERS_COUNT];
    u8 expGetterMonId;
    u8 wildVictorySong;
    u8 dynamicMoveType;
    u8 wrappedBy[MAX_BATTLERS_COUNT];
    u16 assistPossibleMoves[PARTY_SIZE * 4]; // Each of mons can know max 4 moves.
    u8 focusPunchBattlerId;
    u8 battlerPreventingSwitchout;
    u8 moneyMultiplier;
    u8 savedTurnActionNumber;
    u8 switchInAbilitiesCounter;
    u8 faintedActionsState;
    u8 faintedActionsBattlerId;
    u16 expValue;
    u8 field_52;
    u8 sentInPokes;
    bool8 selectionScriptFinished[MAX_BATTLERS_COUNT];
    u8 field_58[4];
    u8 monToSwitchIntoId[MAX_BATTLERS_COUNT];
    u8 field_60[4][3];
    u8 runTries;
    u8 caughtMonNick[11 + 1];
    u8 safariGoNearCounter;
    u8 safariPkblThrowCounter;
    u8 safariEscapeFactor;
    u8 safariCatchFactor;
    u8 linkBattleVsSpriteId_V; // The letter "V"
    u8 linkBattleVsSpriteId_S; // The letter "S"
    u8 formToChangeInto;
    u8 chosenMovePositions[MAX_BATTLERS_COUNT];
    u8 stateIdAfterSelScript[MAX_BATTLERS_COUNT];
    u8 field_8B; // related to player's pokemon switching
    u8 stringMoveType;
    u8 expGetterBattlerId;
    u8 field_91; // related to gAbsentBattlerFlags, possibly absent flags turn ago?
    u8 palaceFlags; // First 4 bits are "is < 50% HP and not asleep" for each battler, last 4 bits are selected moves to pass to AI
    u8 field_93; // related to choosing pokemon?
    u8 wallyBattleState;
    u8 wallyMovesState;
    u8 wallyWaitFrames;
    u8 wallyMoveFrames;
    u16 lastTakenMove[MAX_BATTLERS_COUNT]; // Last move that a battler was hit with.
    u16 hpOnSwitchout[2];
    u32 savedBattleTypeFlags;
    u8 abilityPreventingSwitchout;
    u8 hpScale;
    u16 synchronizeMoveEffect;
    bool8 anyMonHasTransformed;
    void (*savedCallback)(void);
    u16 usedHeldItems[MAX_BATTLERS_COUNT];
    u16 chosenItem[MAX_BATTLERS_COUNT];
    u8 AI_itemType[2];
    u8 AI_itemFlags[2];
    u16 choicedMove[MAX_BATTLERS_COUNT];
    u16 changedItems[MAX_BATTLERS_COUNT];
    u8 intimidateBattler;
    u8 switchInItemsCounter;
    u8 arenaTurnCounter;
    u8 turnSideTracker;
    u8 givenExpMons; // Bits for enemy party's pokemon that gave exp to player's party.
    u16 lastTakenMoveFrom[MAX_BATTLERS_COUNT][MAX_BATTLERS_COUNT]; // a 2-D array [target][attacker]
    u16 castformPalette[MAX_BATTLERS_COUNT][16];
    u8 field_180; // weird field, used in battle_main.c, once accessed as an array of u32 overwriting the field below
    u8 field_181;
    u8 field_182;
    u8 field_183;
    BattleEnigmaBerry battleEnigmaBerry;
    u8 wishPerishSongState;
    u8 wishPerishSongBattlerId;
    bool8 overworldWeatherDone;
    u8 atkCancellerTracker;
    struct BattleTvMovePoints tvMovePoints;
    struct BattleTv tv;
    u8 AI_monToSwitchIntoId[MAX_BATTLERS_COUNT];
    s8 arenaMindPoints[2];
    s8 arenaSkillPoints[2];
    u16 arenaStartHp[2];
    u8 arenaLostPlayerMons; // Bits for party member, lost as in referee's decision, not by fainting.
    u8 arenaLostOpponentMons;
    u8 alreadyStatusedMoveAttempt; // As bits for battlers; For example when using Thunder Wave on an already paralyzed pokemon.
    u8 debugBattler;
    u8 magnitudeBasePower;
    u8 presentBasePower;
    u8 roostTypes[MAX_BATTLERS_COUNT][3];
    u8 savedBattlerTarget;
    bool8 ateBoost[MAX_BATTLERS_COUNT];
    u8 activeAbilityPopUps; // as bits for each battler
    bool8 throwingPokeBall;
    //struct MegaEvolutionData mega;
    struct BattleEvolutionData mega;
    const u8 *trainerSlideMsg;
    bool8 trainerSlideLowHpMsgDone;
    u8 introState;
    u8 ateBerry[2]; // array id determined by side, each party pokemon as bit
    u8 stolenStats[8]; // hp byte is used for which stats to raise, other inform about by how many stages
    u8 lastMoveFailed; // as bits for each battler, for the sake of Stomping Tantrum
    u8 lastMoveTarget[MAX_BATTLERS_COUNT]; // The last target on which each mon used a move, for the sake of Instruct
    u8 debugHoldEffects[MAX_BATTLERS_COUNT]; // These override actual items' hold effects.
    u8 tracedAbility[MAX_BATTLERS_COUNT];
    u16 hpBefore[MAX_BATTLERS_COUNT]; // Hp of battlers before using a move. For Berserk
    bool8 spriteIgnore0Hp;
    struct Illusion illusion[MAX_BATTLERS_COUNT];
    s8 aiFinalScore[MAX_BATTLERS_COUNT][MAX_BATTLERS_COUNT][MAX_MON_MOVES]; // AI, target, moves to make debugging easier
    u8 soulheartBattlerId;
    u8 friskedBattler; // Frisk needs to identify 2 battlers in double battles.
    bool8 friskedAbility; // If identifies two mons, show the ability pop-up only once.
    u8 sameMoveTurns[MAX_BATTLERS_COUNT]; // For Metronome, number of times the same moves has been SUCCESFULLY used.
    u16 moveEffect2; // For Knock Off
    u16 changedSpecies[PARTY_SIZE]; // For Zygarde or future forms when multiple mons can change into the same pokemon.
};

typedef unsigned long int size_t;
#include "battle_evocpp.h"
#include "pokemon.h"
#include "item.h"
#include "battle_util.h"
#include "battle_interface.h"
#include "constants/hold_effects.h"
extern struct BattlePokemon gBattleMons[4];
extern struct BattleStruct* gBattleStruct;


}
extern u8 gActiveBattler;
extern u8 gLastUsedItem;
class BattleEvoCpp
{
public:
    virtual bool32 CanPokemonEvolution (struct Pokemon* poke) const {return 0;}
    virtual void CreateOrShowTrigger(u8 battlerId, u8 palId) const{}
    virtual void DistoryTrigger(u32 battlerId) const {};
    virtual void DoEvolution(u32 battlerId) const {}
    virtual u32 CreateIndicator(u32 battlerId) const {return 0;}
    virtual void UndoEvolution (u32 monId) const{}
    virtual void HideTriggerSprite() const {}
    virtual void ChangeTriggerSprite(u8 state) const {}
};

extern const u8 BattleScript_MegaEvolution[];

class BattleMegaEvo : public BattleEvoCpp
{
public:
    bool32 CanPokemonEvolution(struct Pokemon *poke) const override {
       /* u16 itemId;

        itemId = GetMonData(poke, MON_DATA_HELD_ITEM);
#if !USE_BATTLE_DEBUG
        if (ItemId_GetHoldEffect(itemId) != HOLD_EFFECT_MEGA_STONE){
        return false;
    }
#endif
        if (GetMegaEvolutionSpecies(GetMonData(poke, MON_DATA_SPECIES), itemId) == 0)
            return false;*/

        // All checks passed, the mon CAN mega evolve.
        return true;
    }
     void DistoryTrigger(u32 battlerId) const override {}
    void CreateOrShowTrigger(u8 battlerId, u8 palId) const override {
         CreateMegaTriggerSprite( battlerId,  palId);
    }

    u32 CreateIndicator(u32 battlerId) const override{
        return CreateMegaIndicatorSprite(battlerId);
    }

    void DoEvolution(u32 battlerId) const override {
        gLastUsedItem = gBattleMons[battlerId].item;
        BattleScriptExecute(BattleScript_MegaEvolution);
    }

    void UndoEvolution(u32 monId) const override {
        UndoMegaEvolution(monId);
    }

    void HideTriggerSprite() const override{
        HideMegaTriggerSprite();
    }

    void ChangeTriggerSprite(u8 state) const override{
        ChangeMegaTriggerSprite(gBattleStruct->mega.triggerSpriteId, state);
    }
};
static const BattleEvoCpp  A1 = BattleEvoCpp();
static const BattleMegaEvo A2 = BattleMegaEvo();
static const BattleEvoCpp* const BattleEvoCpps[] = {
    &A1, &A2,
};

static const BattleEvoCpp* GetBattleEvolutionFunc(u32 battlerId) {
    auto arg = GetEvolutionTypeForBattler(battlerId);
    return BattleEvoCpps[arg];
}

static void InitBattleEvolutionForParty(u8* array, struct Pokemon* poke) {
    int i;
    int j;
    for (i = 0; i < 6; ++i) {
        for (j = EvolutionMega; j < 2; ++j) {
            if (BattleEvoCpps[j]->CanPokemonEvolution(&poke[i])) {
                array[i] = j;
                break;
            }
        }
    }
}

extern "C" {

void InitBattleStruct()
{
    InitBattleEvolutionForParty(gBattleStruct->mega.evolutionType[0], gPlayerParty);
    InitBattleEvolutionForParty(gBattleStruct->mega.evolutionType[1], gEnemyParty);
}

void CreateTrigger(u8 battlerId, u8 palId)
{
    GetBattleEvolutionFunc(battlerId)->CreateOrShowTrigger(battlerId, palId);
}

void DistoryTrigger(u32 battlerId)
{
    GetBattleEvolutionFunc(battlerId)->DistoryTrigger(battlerId);
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
    BattleEvoCpps[gBattleStruct->mega.evolutionType[0][monId]]->UndoEvolution(monId);
}

void ChangeTriggerSprite(u8 battler, u8 state)
{
    GetBattleEvolutionFunc(battler)->ChangeTriggerSprite(state);
}

u8 GetIndicatorSpriteId(u32 healthboxSpriteId)
{
    return GetMegaIndicatorSpriteId(healthboxSpriteId);
}
}

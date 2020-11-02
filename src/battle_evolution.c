#include "global.h"
#include "battle.h"
#include "battle_evolution.h"
#include "decompress.h"
#include "item.h"
#include "constants/hold_effects.h"
#include "battle_interface.h"
#include "battle_scripts.h"
extern void HandleMoveSwitching(void);

extern void HandleInputChooseMove(void);

#define gPlttBufferFaded2 (&gPlttBufferFaded[0x100])
enum MegaGraphicsTags
{
    GFX_TAG_MEGA_INDICATOR = 0xFDF0,
    GFX_TAG_ALPHA_INDICATOR,
    GFX_TAG_OMEGA_INDICATOR,
    GFX_TAG_ULTRA_INDICATOR,
    GFX_TAG_MEGA_TRIGGER,
    GFX_TAG_ULTRA_TRIGGER,
    GFX_TAG_Z_TRIGGER,
    GFX_TAG_DYNAMAX_INDICATOR,
    GFX_TAG_DYNAMAX_TRIGGER,
    GFX_TAG_RAID_SHIELD,
};

enum
{
    MegaTriggerNothing,
    MegaTriggerLightUp,
    MegaTriggerNormalColour,
    MegaTriggerGrayscale,
};

void SpriteCb_MegaTrigger(struct Sprite* self);
void SpriteCB_MegaIndicator(struct Sprite* self);
static void DestroyDynamaxTrigger();

/*extern const u32 Dynamax_IndicatorTiles[];
//extern const u32 Dynamax_TriggerTiles[]; //For some reason this doesn't work
extern const u32 Dynamax_Trigger_WorkingTiles[]; //This is used as the image until the bug is fixed
extern const u16 Dynamax_TriggerPal[];
static const struct CompressedSpriteSheet sDynamaxIndicatorSpriteSheet =
    {
        Dynamax_IndicatorTiles, (8 * 8) / 2, GFX_TAG_DYNAMAX_INDICATOR
    };
static const struct CompressedSpriteSheet sDynamaxTriggerSpriteSheet =
    {
        Dynamax_Trigger_WorkingTiles, (32 * 32) / 2, GFX_TAG_DYNAMAX_TRIGGER
    };
static const struct SpritePalette sDynamaxTriggerPalette =
    {
        Dynamax_TriggerPal, GFX_TAG_DYNAMAX_TRIGGER
    };
static const struct OamData sIndicatorOam =
    {
        .affineMode = ST_OAM_AFFINE_OFF,
        .objMode = ST_OAM_OBJ_NORMAL,
        .shape = SPRITE_SHAPE(8x8),
        .size = SPRITE_SIZE(8x8),
        .priority = 0, //Above sprites
    };
static const struct OamData sTriggerOam =
    {
        .affineMode = ST_OAM_AFFINE_OFF,
        .objMode = ST_OAM_OBJ_NORMAL,
        .shape = SPRITE_SHAPE(32x32),
        .size = SPRITE_SIZE(32x32),
        .priority = 2, //Same level as healthbox
    };
static const struct SpriteTemplate sDynamaxIndicatorSpriteTemplate =
    {
        .tileTag = GFX_TAG_DYNAMAX_INDICATOR,
        .paletteTag = GFX_TAG_MEGA_INDICATOR,
        .oam = &sIndicatorOam,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_MegaIndicator,
    };

static const struct SpriteTemplate sDynamaxTriggerSpriteTemplate =
    {
        .tileTag = GFX_TAG_DYNAMAX_TRIGGER,
        .paletteTag = GFX_TAG_DYNAMAX_TRIGGER,
        .oam = &sTriggerOam,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCb_MegaTrigger,
    };*/
#define RGB(r, g, b) ((r) | ((g) << 5) | ((b) << 10))
static const u16 sIgnoredTriggerColours[] =
    {
        RGB(7, 10, 8),
        RGB(7, 6, 6),
        RGB(15, 18, 16),
        RGB(10, 13, 12),
        RGB(13, 13, 11),
        RGB(31, 31, 31),
        RGB(4, 7, 0),
        RGB(4, 4, 0),
        RGB(0, 0, 0),
    };

#define TRIGGER_BANK self->data[4]
#define PALETTE_STATE self->data[1]
#define TAG self->template->tileTag
#define PAL_TAG self->template->paletteTag


static bool8 IsIgnoredTriggerColour(u16 colour)
{
    u32 i;
    for (i = 0; i < ARRAY_COUNT(sIgnoredTriggerColours); ++i) {
        if (sIgnoredTriggerColours[i] == colour)
            return TRUE;
    }

    return FALSE;
}

static u16 LightUpTriggerSymbol(u16 clra)
{

    u32 parta;
    u32 partb;
    u32 part;
    u16 clr;

    const u16 clrb = 0x7FFF;
    const u32 currentAlpha = 20;
    const u32 gmask = 0x1F << 5;
    const u32 rbmask = ((0x1F) | (0x1F << 10));
    const u32 rbhalf = 0x4010;
    const u32 ghalf = 0x0200;

    // Red and blue
    parta = clra & rbmask;
    partb = clrb & rbmask;
    part = (partb - parta) * (32 - currentAlpha) + parta * 32 + rbhalf;
    clr = (part >> 5) & rbmask;

    // Green
    parta = clra & gmask;
    partb = clrb & gmask;
    part = (partb - parta) * (32 - currentAlpha) + parta * 32 + ghalf;
    clr |= (part >> 5) & gmask;

    return clr;
}

/*void TryLoadDynamaxTrigger(void)
{
    u8 spriteId;

    if (gBattleTypeFlags & (BATTLE_TYPE_SAFARI))
        return;

    if (!(gBattleTypeFlags & BATTLE_TYPE_DYNAMAX))
        return;

    LoadSpritePalette(&sDynamaxTriggerPalette);
    LoadCompressedSpriteSheetUsingHeap(&sDynamaxTriggerSpriteSheet);

    spriteId = CreateSprite(&sDynamaxTriggerSpriteTemplate, 130, 90, 1);
    gSprites[spriteId].data[3] = 24;
    gSprites[spriteId].pos1.x = -32;
    gSprites[spriteId].data[4] = gActiveBattler;
}*/

static void DestroyDynamaxTrigger(void)
{
    int i;
    FreeSpritePaletteByTag(GFX_TAG_DYNAMAX_TRIGGER);
    FreeSpriteTilesByTag(GFX_TAG_DYNAMAX_TRIGGER);

    for (i = 0; i < MAX_SPRITES; ++i) {
        if (gSprites[i].template->tileTag == GFX_TAG_DYNAMAX_TRIGGER)
            DestroySprite(&gSprites[i]);
    }
}

enum BattleEvolutionType GetEvolutionType(struct BattleEvolutionData* evolutionData, u8 battlerId)
{
    return evolutionData->evolutionType[battlerId & 1][GET_BATTLER_POSITION(battlerId)];
}
enum BattleEvolutionType GetEvolutionTypeForBattler(u8 battlerId) {
    return GetEvolutionType(&gBattleStruct->mega, battlerId);
}

void SetEvolutionType(struct BattleStruct* battleStruct, u8 battlerId, enum BattleEvolutionType value)
{
    battleStruct->mega.evolutionType[battlerId & 1][GET_BATTLER_POSITION(battlerId)] = value;
}


bool32 CheckEvolutionType(struct BattleEvolutionData* evolutionData, u8 battlerId)
{
    enum BattleEvolutionType type =  GetEvolutionType(evolutionData, battlerId);
    return type > EvolutionNone && type < EvolutionMegaHappend;
}

bool32 CanPokemonMega(struct Pokemon* mon) {
    u16 itemId;

    itemId = GetMonData(mon, MON_DATA_HELD_ITEM);
#if !USE_BATTLE_DEBUG
    if (ItemId_GetHoldEffect(itemId) != HOLD_EFFECT_MEGA_STONE){
        return FALSE;
    }
#endif
    if (GetMegaEvolutionSpecies(GetMonData(mon, MON_DATA_SPECIES), itemId) == 0)
        return FALSE;

    // All checks passed, the mon CAN mega evolve.
    return TRUE;
}

void DoMegaEvolution(u32 battlerId) {
    gLastUsedItem = gBattleMons[battlerId].item;
    BattleScriptExecute(BattleScript_MegaEvolution);
}

static void ChangeMegaTrigger(u8 state) {
    ChangeMegaTriggerSprite(gBattleStruct->mega.triggerSpriteId, state);
}

static const void* const sDummyBattleEvolutionFunc[sizeof(struct BattleEvolutionFunc) / 4] = {
    [0 ... sizeof(struct BattleEvolutionFunc) / 4 - 1] = &DummyBattleInterfaceFunc,
};

static const struct BattleEvolutionFunc sBattleEvolutionMega = {
    .CanPokemonEvolution = CanPokemonMega,
    .IsEvolutionHappened = NULL,
    .CreateOrShowTrigger = CreateMegaTriggerSprite,
    .CreateIndicator = CreateMegaIndicatorSprite,
    .PrepareEvolution = NULL,
    .DoEvolution = DoMegaEvolution,
    .UndoEvolution = UndoMegaEvolution,
    .HideTriggerSprite = HideMegaTriggerSprite,
    .ChangeTriggerSprite = ChangeMegaTrigger,
};

static const struct BattleEvolutionFunc *const sBattleEvolutionFuncs[] =
    {
        [EvolutionNone] = (const struct BattleEvolutionFunc*)sDummyBattleEvolutionFunc,
        [EvolutionMega] = &sBattleEvolutionMega,
        [EvolutionDynamax] = (const struct BattleEvolutionFunc*)sDummyBattleEvolutionFunc,
        [EvolutionMegaHappend] = (const struct BattleEvolutionFunc*)sDummyBattleEvolutionFunc,
        [EvolutionDynamaxHappend] = (const struct BattleEvolutionFunc*)sDummyBattleEvolutionFunc,
    };

static void InitBattleEvolutionForParty(u8* array, struct Pokemon* poke) {
    int i;
    int j;
    for (i = 0; i < 6; ++i) {
        for (j = 0; j < ARRAY_COUNT(sBattleEvolutionFuncs); ++j) {
            if (sBattleEvolutionFuncs[j] && sBattleEvolutionFuncs[j]->CanPokemonEvolution(&poke[i])) {
                array[i] = j;
            }
        }
    }
}


void InitBattleStruct()
{
    InitBattleEvolutionForParty(gBattleStruct->mega.evolutionType[0], gPlayerParty);
    InitBattleEvolutionForParty(gBattleStruct->mega.evolutionType[1], gEnemyParty);
}

const struct BattleEvolutionFunc* GetBattleEvolutionFunc(u8 battlerId) {
    return sBattleEvolutionFuncs[GetEvolutionType(&gBattleStruct->mega, battlerId)];
}

const struct BattleEvolutionFunc* GetBattleEvolutionFuncByPos(u32 monId, u8 side){
    return sBattleEvolutionFuncs[gBattleStruct->mega.evolutionType[side][monId]];
}
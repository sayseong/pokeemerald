#include "global.h"
#include "battle.h"
#include "battle_evolution.h"
#include "decompress.h"
#include "item.h"
#include "constants/hold_effects.h"
#include "battle_interface.h"
#include "battle_scripts.h"
#include "palette.h"
extern void HandleMoveSwitching(void);

extern void HandleInputChooseMove(void);

#define gPlttBufferFaded2 (&gPlttBufferFaded[0x100])
enum MegaGraphicsTags
{
    GFX_TAG_MEGA_TRIGGER = TAG_MEGA_TRIGGER_TILE,
    GFX_TAG_MEGA_INDICATOR,
    GFX_TAG_DYNAMAX_INDICATOR,
    GFX_TAG_DYNAMAX_TRIGGER,
};


extern void SpriteCb_MegaTrigger(struct Sprite* self);
extern void SpriteCB_MegaIndicator(struct Sprite* self);
static void DestroyDynamaxTrigger();

static const u32 Dynamax_IndicatorTiles[] = INCBIN_U32("graphics/battle_interface/Dynamax_Indicator.4bpp");
static const u32 Dynamax_IndicatorPal[] = INCBIN_U32("graphics/battle_interface/Dynamax_Indicator.gbapal");
//extern const u32 Dynamax_TriggerTiles[]; //For some reason this doesn't work
static const u32 Dynamax_Trigger_WorkingTiles[] = INCBIN_U32("graphics/battle_interface/Dynamax_Trigger_Working.4bpp.lz"); //This is used as the image until the bug is fixed
static const u16 Dynamax_TriggerPal[] = INCBIN_U32("graphics/battle_interface/Dynamax_Trigger_Working.gbapal");

const struct SpriteSheet sDynamaxIndicatorSpriteSheet =
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
const struct SpritePalette sDynamaxIndicatorPalette =
    {
        Dynamax_TriggerPal, GFX_TAG_DYNAMAX_INDICATOR
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
    };

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


static bool8 IsIgnoredTriggerColour(u16 colour)
{
    u32 i;
    for (i = 0; i < ARRAY_COUNT(sIgnoredTriggerColours); ++i) {
        if (sIgnoredTriggerColours[i] == colour)
            return TRUE;
    }

    return FALSE;
}

u32 CreateDynamaxIndicator(u32 battlerId) {
    return CreateMegaIndicatorSprite(battlerId, &sDynamaxIndicatorPalette, &sDynamaxIndicatorSpriteSheet, &sDynamaxIndicatorSpriteTemplate);
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

void ChangeDynamaxTriggerSprite(u8 state) {
    u16* pal = &gPlttBufferFaded2[IndexOfSpritePaletteTag(GFX_TAG_DYNAMAX_TRIGGER) * 16];
    u8 i;

    for(i = 1; i < 16; i++)
    {
        if (IsIgnoredTriggerColour(Dynamax_TriggerPal[i])) continue;

        switch(state) {
        case 0:
            pal[i] = LightUpTriggerSymbol(Dynamax_TriggerPal[i]);
            break;
        case 1:
            pal[i] = Dynamax_TriggerPal[i];
            break;
        }
    }
}

static void DestroyDynamaxTrigger(void)
{
    int i;
    FreeSpritePaletteByTag(GFX_TAG_DYNAMAX_TRIGGER);
    FreeSpriteTilesByTag(GFX_TAG_DYNAMAX_TRIGGER);

    for (i = 0; i < MAX_SPRITES; ++i) {
        if (gSprites[i].template->tileTag == GFX_TAG_DYNAMAX_TRIGGER)
            DestroySprite(&gSprites[i]);
    }
    gBattleStruct->mega.dynamaxTriggerId = 0xFF;
}


static void SpriteCB_DynamaxTrigger(struct Sprite* sprite){
    s16 xshift, yshift;
    struct Sprite* healthbox;
    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
    {
        xshift = -6;
        yshift = -2;

        if (IndexOfSpritePaletteTag(TYPE_ICON_TAG) != 0xFF) //Type icons are shown
            xshift -= 8;
    }
    else
    {
        xshift = -5;
        yshift = 1;
    }
    healthbox = &gSprites[gHealthboxSpriteIds[sprite->data[0]]];
    u8 y = healthbox->oam.y;

    if (y)
    {
        // Copy the healthbox's position (it has various animations)
        //self->y = healthbox->y + 20;
        sprite->pos1.x = (healthbox->oam.x) + xshift + sprite->data[3];
        if (sprite->pos1.x == healthbox->pos1.x - 15)
            DestroyDynamaxTrigger();
        sprite->pos1.y = healthbox->pos1.y + yshift + healthbox->pos2.y;
    }
    else
    {
        // The box is offscreen, so hide this one as well
        sprite->pos1.x = -32;
    }
    if (sprite->data[1])
    {
        if (sprite->data[3] > 0)
            sprite->data[3] -= 2;
        else
            sprite->data[3] = 0;
    }
    else
    {
        if (sprite->data[3] < 24)
            sprite->data[3] += 2;
        else
            sprite->data[3] = 24;
    }
}

void TryLoadDynamaxTrigger(u8 battlerId, u8 UNUSED palId)
{

    if (gBattleTypeFlags & (BATTLE_TYPE_SAFARI))
        return;

    if (!(gBattleTypeFlags & BATTLE_TYPE_DYNAMAX))
        return;
    if (gBattleStruct->mega.dynamaxTriggerId == 0xFF)
    {
        LoadSpritePalette(&sDynamaxTriggerPalette);
        LoadCompressedSpriteSheetUsingHeap(&sDynamaxTriggerSpriteSheet);
        gBattleStruct->mega.dynamaxTriggerId = CreateSprite(&sDynamaxTriggerSpriteTemplate, 130, 90, 1);
    }
    gSprites[gBattleStruct->mega.dynamaxTriggerId].data[3] = 24;
    gSprites[gBattleStruct->mega.dynamaxTriggerId].pos1.x = -32;
    gSprites[gBattleStruct->mega.dynamaxTriggerId].data[0] = battlerId;
    gSprites[gBattleStruct->mega.dynamaxTriggerId].data[1] = 0;
}

void HideDynamaxTriggerSprite(void)
{
    ChangeDynamaxTriggerSprite(0);
    gSprites[gBattleStruct->mega.dynamaxTriggerId].data[1] = TRUE;
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

static bool32 CanPokemonMega(struct Pokemon* mon) {
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

static void DoMegaEvolution(u32 battlerId) {
    gLastUsedItem = gBattleMons[battlerId].item;
    BattleScriptExecute(BattleScript_MegaEvolution);
}

static void ChangeMegaTrigger(u8 state) {
    ChangeMegaTriggerSprite(gBattleStruct->mega.triggerSpriteId, state);
}

static const bool32 DummyBattleEvolutionFunc(void) {
    return FALSE;
}

static const void* const sDummyBattleEvolutionFunc[sizeof(struct BattleEvolutionFunc) / 4] = {
    [0 ... sizeof(struct BattleEvolutionFunc) / 4 - 1] = &DummyBattleEvolutionFunc,
};

static const struct BattleEvolutionFunc sBattleEvolutionMega = {
    .CanPokemonEvolution = CanPokemonMega,
    .IsEvolutionHappened = NULL,
    .CreateOrShowTrigger = CreateMegaTriggerSprite,
    .CreateIndicator = CreateMegaIndicator,
    .PrepareEvolution = NULL,
    .DoEvolution = DoMegaEvolution,
    .UndoEvolution = UndoMegaEvolution,
    .HideTriggerSprite = HideMegaTriggerSprite,
    .ChangeTriggerSprite = ChangeMegaTrigger,
};

static const struct BattleEvolutionFunc sBattleEvolutionDynamax = {
    .CreateOrShowTrigger = TryLoadDynamaxTrigger,
    .CreateIndicator = CreateDynamaxIndicator,
    .ChangeTriggerSprite = ChangeDynamaxTriggerSprite,
    .HideTriggerSprite = HideDynamaxTriggerSprite
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
            if (sBattleEvolutionFuncs[j]->CanPokemonEvolution(&poke[i])) {
                array[i] = j;
                break;
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
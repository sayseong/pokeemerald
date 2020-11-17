#include "global.h"
#include "battle.h"
#include "battle_evolution.h"
#include "decompress.h"
#include "item.h"
#include "constants/hold_effects.h"
#include "battle_interface.h"
#include "battle_scripts.h"
#include "palette.h"
#include "task.h"
#include "battle_anim.h"

#define tBattler    data[0]
#define tHide       data[1]

#define SINGLES_MEGA_TRIGGER_POS_X_OPTIMAL (30)
#define SINGLES_MEGA_TRIGGER_POS_X_PRIORITY (31)
#define SINGLES_MEGA_TRIGGER_POS_X_SLIDE (15)
#define SINGLES_MEGA_TRIGGER_POS_Y_DIFF (-11)

#define DOUBLES_MEGA_TRIGGER_POS_X_OPTIMAL (30)
#define DOUBLES_MEGA_TRIGGER_POS_X_PRIORITY (31)
#define DOUBLES_MEGA_TRIGGER_POS_X_SLIDE (15)
#define DOUBLES_MEGA_TRIGGER_POS_Y_DIFF (-4)

void GetBattlerHealthboxCoords(u8 battler, s16 *x, s16 *y);
extern u8 BattleScript_Dynamax[];

enum MegaGraphicsTags
{
	GFX_TAG_MEGA_TRIGGER = TAG_MEGA_TRIGGER_TILE,
	GFX_TAG_MEGA_INDICATOR,
	GFX_TAG_DYNAMAX_INDICATOR,
	GFX_TAG_DYNAMAX_TRIGGER,
};

static void SpriteCB_DynamaxTrigger(struct Sprite* self);
void SpriteCb_MegaIndicator(struct Sprite* sprite);
static void DestroyDynamaxTrigger();
static const u32 Dynamax_IndicatorTiles[] = INCBIN_U32("graphics/battle_interface/Dynamax_Indicator.4bpp");
static const u16 Dynamax_IndicatorPal[] = INCBIN_U16("graphics/battle_interface/Dynamax_Indicator.gbapal");
//extern const u32 Dynamax_TriggerTiles[]; //For some reason this doesn't work
static const u32 Dynamax_Trigger_WorkingTiles[] = INCBIN_U32("graphics/battle_interface/Dynamax_Trigger_Working.4bpp.lz"); //This is used as the image until the bug is fixed
static const u16 Dynamax_TriggerPal[] = INCBIN_U16("graphics/battle_interface/Dynamax_Trigger_Working.gbapal");

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
	Dynamax_IndicatorPal, GFX_TAG_DYNAMAX_INDICATOR
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
	.paletteTag = GFX_TAG_DYNAMAX_INDICATOR,
	.oam = &sIndicatorOam,
	.anims = gDummySpriteAnimTable,
	.images = NULL,
	.affineAnims = gDummySpriteAffineAnimTable,
	.callback = SpriteCb_MegaIndicator,
};

static const struct SpriteTemplate sDynamaxTriggerSpriteTemplate =
{
	.tileTag = GFX_TAG_DYNAMAX_TRIGGER,
	.paletteTag = GFX_TAG_DYNAMAX_TRIGGER,
	.oam = &sTriggerOam,
	.anims = gDummySpriteAnimTable,
	.images = NULL,
	.affineAnims = gDummySpriteAffineAnimTable,
	.callback = SpriteCB_DynamaxTrigger,
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
	for (i = 0; i < ARRAY_COUNT(sIgnoredTriggerColours); ++i)
	{
		if (sIgnoredTriggerColours[i] == colour)
			return TRUE;
	}

	return FALSE;
}

#define hOther_IndicatorSpriteId    data[6] // For Mega Evo

static const s8 sIndicatorPosDoubles[][2] =
{
	[B_POSITION_PLAYER_LEFT] = {53, -5},
	[B_POSITION_OPPONENT_LEFT] = {45, -5},
	[B_POSITION_PLAYER_RIGHT] = {53, -5},
	[B_POSITION_OPPONENT_RIGHT] = {45, -5},
};
static const s8 sIndicatorPosSingles[][2] =
{
	[B_POSITION_PLAYER_LEFT] = {53, -5},
	[B_POSITION_OPPONENT_LEFT] = {45, -5},
};
u32 CreateDynamaxIndicator(u32 battlerId)
{
	u32 spriteId, position;
	s16 x, y;

	LoadSpritePalette(&sDynamaxIndicatorPalette);
	LoadSpriteSheet(&sDynamaxIndicatorSpriteSheet);

	position = GetBattlerPosition(battlerId);
	GetBattlerHealthboxCoords(battlerId, &x, &y);
	if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
	{
		x += sIndicatorPosDoubles[position][0];
		y += sIndicatorPosDoubles[position][1];
	}
	else
	{
		x += sIndicatorPosSingles[position][0];
		y += sIndicatorPosSingles[position][1];
	}
	spriteId = CreateSpriteAtEnd(&sDynamaxIndicatorSpriteTemplate, x, y, 0);
	gSprites[gSprites[gHealthboxSpriteIds[battlerId]].oam.affineParam].hOther_IndicatorSpriteId = spriteId;

	gSprites[spriteId].tBattler = battlerId;
	return spriteId;
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

void ChangeDynamaxTriggerSprite(u8 state)
{
	u16* pal;
	u8 i;
	pal = &gPlttBufferFaded[IndexOfSpritePaletteTag(GFX_TAG_DYNAMAX_TRIGGER) * 16 + 0x100];

	for (i = 1; i < 16; i++)
	{
		if (IsIgnoredTriggerColour(Dynamax_TriggerPal[i])) continue;

		switch (state)
		{
		case 1:
			pal[i] = LightUpTriggerSymbol(Dynamax_TriggerPal[i]);
			break;
		case 0:
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

	for (i = 0; i < MAX_SPRITES; ++i)
	{
		if (gSprites[i].template->tileTag == GFX_TAG_DYNAMAX_TRIGGER)
			DestroySprite(&gSprites[i]);
	}
	gBattleStruct->mega.dynamaxTriggerId = 0;
}


static void SpriteCB_DynamaxTrigger(struct Sprite* sprite)
{
	s32 xSlide, xPriority, xOptimal;

	if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
	{
		xSlide = DOUBLES_MEGA_TRIGGER_POS_X_SLIDE;
		xPriority = DOUBLES_MEGA_TRIGGER_POS_X_PRIORITY + 9;
		xOptimal = DOUBLES_MEGA_TRIGGER_POS_X_OPTIMAL + 9;
	}
	else
	{
		xSlide = SINGLES_MEGA_TRIGGER_POS_X_SLIDE;
		xPriority = SINGLES_MEGA_TRIGGER_POS_X_PRIORITY + 9;
		xOptimal = SINGLES_MEGA_TRIGGER_POS_X_OPTIMAL + 9;
	}

	if (sprite->tHide)
	{
		if (sprite->pos1.x != gSprites[gHealthboxSpriteIds[sprite->tBattler]].pos1.x - xSlide)
			sprite->pos1.x += 2;
		if (sprite->pos1.x >= gSprites[gHealthboxSpriteIds[sprite->tBattler]].pos1.x - xPriority)
			sprite->oam.priority = 2;
		else
			sprite->oam.priority = 1;
		if (sprite->pos1.x >= gSprites[gHealthboxSpriteIds[sprite->tBattler]].pos1.x - xSlide)
			DestroyDynamaxTrigger();
	}
	else
	{
		if (sprite->pos1.x != gSprites[gHealthboxSpriteIds[sprite->tBattler]].pos1.x - xOptimal)
			sprite->pos1.x--;
		if (sprite->pos1.x >= gSprites[gHealthboxSpriteIds[sprite->tBattler]].pos1.x - xPriority)
			sprite->oam.priority = 2;
		else
			sprite->oam.priority = 1;
	}
}

void TryLoadDynamaxTrigger(u8 battlerId, u8 UNUSED palId)
{
	s16 x, y;
	if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
	{
		x = 20;
		y = 2;
	}
	else
	{
		x = 20;
		y = 2;
	}
	/*if (gBattleTypeFlags & (BATTLE_TYPE_SAFARI))
		return;

	if (!(gBattleTypeFlags & BATTLE_TYPE_DYNAMAX))
		return;*/
	x = gSprites[gHealthboxSpriteIds[battlerId]].pos1.x - x;
	y = gSprites[gHealthboxSpriteIds[battlerId]].pos1.y - y;
	if (gBattleStruct->mega.dynamaxTriggerId == 0)
	{
		LoadSpritePalette(&sDynamaxTriggerPalette);
		LoadCompressedSpriteSheetUsingHeap(&sDynamaxTriggerSpriteSheet);
		gBattleStruct->mega.dynamaxTriggerId = CreateSprite(&sDynamaxTriggerSpriteTemplate, x, y, 2);
	}
	gSprites[gBattleStruct->mega.dynamaxTriggerId].tBattler = battlerId;
	gSprites[gBattleStruct->mega.dynamaxTriggerId].tHide = 0;
    gSprites[gBattleStruct->mega.dynamaxTriggerId].pos1.y = y;
    gSprites[gBattleStruct->mega.dynamaxTriggerId].pos1.x = x;
}

void HideDynamaxTriggerSprite(void)
{
	ChangeDynamaxTriggerSprite(0);
	gSprites[gBattleStruct->mega.dynamaxTriggerId].tHide = TRUE;
}


void SetEvolutionType(struct BattleStruct* battleStruct, u8 battlerId, enum BattleEvolutionType value)
{
	battleStruct->mega.evolutionType[GET_BATTLER_SIDE2(battlerId)][gBattlerPartyIndexes[battlerId]] = value;
}



static bool32 CanPokemonMega(struct Pokemon* mon)
{
	u16 itemId;

	itemId = GetMonData(mon, MON_DATA_HELD_ITEM);

	if (ItemId_GetHoldEffect(itemId) != HOLD_EFFECT_MEGA_STONE){
		return FALSE;
	}

	if (GetMegaEvolutionSpecies(GetMonData(mon, MON_DATA_SPECIES), itemId) == 0)
		return FALSE;

	// All checks passed, the mon CAN mega evolve.
	return TRUE;
}
#define ITEM_DYNAMAX_BRAND 0
static bool32 CanPokemonDynamax(struct Pokemon* mon)
{
	/*if (gBattleTypeFlags & BATTLE_TYPE_DYNAMAX) {
		if (!CanPokemonMega(mon) && CheckBagHasItem(ITEM_DYNAMAX_BRAND, 1))
			return 1;
	}
	return 0;*/
	return !CanPokemonMega(mon);
}

const union AffineAnimCmd sDynamaxGrowthAffineAnimCmds[] =
	{
		AFFINEANIMCMD_FRAME(-2, -2, 0, 64), //Double in size
		AFFINEANIMCMD_FRAME(0, 0, 0, 64),
		AFFINEANIMCMD_FRAME(16, 16, 0, 8),
		AFFINEANIMCMD_END,
	};

const union AffineAnimCmd sDynamaxGrowthAttackAnimationAffineAnimCmds[] =
	{
		AFFINEANIMCMD_FRAME(-4, -4, 0, 32), //Double in size quicker
		AFFINEANIMCMD_FRAME(0, 0, 0, 32), //Pause for less
		AFFINEANIMCMD_FRAME(16, 16, 0, 8),
		AFFINEANIMCMD_END,
	};

static void AnimTask_DynamaxGrowthStep(u8 taskId)
{
	if (!RunAffineAnimFromTaskData(&gTasks[taskId]))
		DestroyAnimVisualTask(taskId);
}
void AnimTask_GrowAndShrink_Step(u8);
//Arg 0: Animation for attack
void AnimTask_GrowthAffine(u8 taskId)
{
	struct Task* task;
	task = &gTasks[taskId];
	PrepareAffineAnimInTaskData(task, GetAnimBattlerSpriteId(ANIM_ATTACKER),
		LoadPointerFromVars(gBattleAnimArgs[0], gBattleAnimArgs[1]));
	task->func = AnimTask_DynamaxGrowthStep;
}

static void DoMegaEvolution(u32 battlerId)
{
    SetEvolutionType(gBattleStruct, battlerId, EvolutionMega);
	gLastUsedItem = gBattleMons[battlerId].item;
	BattleScriptExecute(BattleScript_MegaEvolution);
}
#include "constants/moves.h"
#include "constants/species.h"

struct GMaxSpeciesInfo
{
    u16 species;
    u16 move;
    u16 targetSpecies;
};

struct GMaxInfo
{
    const struct GMaxSpeciesInfo* speciesInfo;
    u8 type;
};
static const struct GMaxSpeciesInfo gDynamaxSpecies[] =
{
    {SPECIES_CHARIZARD, MOVE_G_MAX_WILDFIRE, SPECIES_CHARIZARD_GIGA},
    {SPECIES_BUTTERFREE, MOVE_G_MAX_BEFUDDLE, SPECIES_BUTTERFREE_GIGA},
    {SPECIES_PIKACHU, MOVE_G_MAX_VOLT_CRASH, SPECIES_PIKACHU_GIGA},
    {SPECIES_MEOWTH, MOVE_G_MAX_GOLD_RUSH, SPECIES_MEOWTH_GIGA},
    {SPECIES_MACHAMP, MOVE_G_MAX_CHI_STRIKE, SPECIES_MACHAMP_GIGA},
    {SPECIES_GENGAR, MOVE_G_MAX_TERROR, SPECIES_GENGAR_GIGA},
    {SPECIES_KINGLER, MOVE_G_MAX_FOAM_BURST, SPECIES_KINGLER_GIGA},
    {SPECIES_LAPRAS, MOVE_G_MAX_RESONANCE, SPECIES_LAPRAS_GIGA},
    {SPECIES_EEVEE, MOVE_G_MAX_CUDDLE, SPECIES_EEVEE_GIGA},
    {SPECIES_SNORLAX, MOVE_G_MAX_REPLENISH, SPECIES_SNORLAX_GIGA},
    {SPECIES_GARBODOR, MOVE_G_MAX_MALODOR, SPECIES_GARBODOR_GIGA},
    {SPECIES_MELMETAL, MOVE_G_MAX_MELTDOWN, SPECIES_MELMETAL_GIGA},
    {SPECIES_DREDNAW, MOVE_G_MAX_STONESURGE, SPECIES_DREDNAW_GIGA},
    {SPECIES_CORVIKNIGHT, MOVE_G_MAX_WIND_RAGE, SPECIES_CORVIKNIGHT_GIGA},
    {SPECIES_TOXTRICITY, MOVE_G_MAX_STUN_SHOCK, SPECIES_TOXTRICITY_GIGA},
    {SPECIES_ALCREMIE, MOVE_G_MAX_FINALE, SPECIES_ALCREMIE_GIGA},
    {SPECIES_DURALUDON, MOVE_G_MAX_DEPLETION, SPECIES_DURALUDON_GIGA},
    {SPECIES_ORBEETLE, MOVE_G_MAX_GRAVITAS, SPECIES_ORBEETLE_GIGA},
    {SPECIES_COALOSSAL, MOVE_G_MAX_VOLCALITH, SPECIES_COALOSSAL_GIGA},
    {SPECIES_SANDACONDA, MOVE_G_MAX_SANDBLAST, SPECIES_SANDACONDA_GIGA},
    {SPECIES_GRIMMSNARL, MOVE_G_MAX_SNOOZE, SPECIES_GRIMMSNARL_GIGA},
    {SPECIES_FLAPPLE, MOVE_G_MAX_TARTNESS, SPECIES_FLAPPLE_GIGA},
    {SPECIES_APPLETUN, MOVE_G_MAX_SWEETNESS, SPECIES_APPLETUN_GIGA},
    {SPECIES_HATTERENE, MOVE_G_MAX_SMITE, SPECIES_HATTERENE_GIGA},
    {SPECIES_COPPERAJAH, MOVE_G_MAX_STEELSURGE, SPECIES_COPPERAJAH_GIGA},
    {SPECIES_CENTISKORCH, MOVE_G_MAX_CENTIFERNO, SPECIES_CENTISKORCH_GIGA},
    {SPECIES_RILLABOOM, MOVE_G_MAX_DRUM_SOLO,SPECIES_RILLABOOM_GIGA},
    {SPECIES_CINDERACE, MOVE_G_MAX_FIREBALL, SPECIES_CINDERACE_GIGA},
    {SPECIES_INTELEON, MOVE_G_MAX_HYDROSNIPE, SPECIES_INTELEON_GIGA},
    {SPECIES_VENUSAUR, MOVE_G_MAX_VINE_LASH, SPECIES_VENUSAUR_GIGA},
    {SPECIES_BLASTOISE, MOVE_G_MAX_CANNONADE, SPECIES_BLASTOISE_GIGA},
    {SPECIES_URSHIFU, MOVE_G_MAX_ONE_BLOW, SPECIES_URSHIFU_RAPID_GIGA},
    {SPECIES_URSHIFU, MOVE_G_MAX_RAPID_FLOW, SPECIES_URSHIFU_SINGLE_GIGA},
    {0xFFFF, 0xFFFF, 0}
};

struct GMaxInfo GetGMaxSpeciesInfo(u16 species)
{
    u16 i;
    struct GMaxInfo ret;
    for(i = 0;i < ARRAY_COUNT(gDynamaxSpecies); i++)
    {
        if (species == gDynamaxSpecies[i].species)
        {
            break;
        }
    }
    ret.speciesInfo = &gDynamaxSpecies[i];
    if (ret.speciesInfo->targetSpecies == 0) ret.speciesInfo = &gDynamaxSpecies[ARRAY_COUNT(gDynamaxSpecies) - 1];
    if (i < ARRAY_COUNT(gDynamaxSpecies)) ret.type = gBattleMoves[gDynamaxSpecies[i].move].type;
    else ret.type = 0xff;
    return ret;
}

void GetMaxMove(struct BattlePokemon* mon)
{
    u16* moveResult = mon->moves;
    int i,j,pokeMoveType;
    struct GMaxInfo gMaxSpeciesInfo = GetGMaxSpeciesInfo(mon->species);
    for (i = 0; i < 4; ++i)
    {
        if (mon->moves[i] == 0) break;
        pokeMoveType = gBattleMoves[mon->moves[i]].type;
        if (pokeMoveType == gMaxSpeciesInfo.type)
        {
            *moveResult++ = gMaxSpeciesInfo.speciesInfo->move;
        }
        else if (gBattleMoves[mon->moves[i]].split == SPLIT_STATUS)
        {
            *moveResult++ = MOVE_MAX_GUARD;
        }
        else
        {
            for(j = MOVE_MAX_FLARE;j <= MOVE_MAX_STEELSPIKE;j++)
            {
                if (pokeMoveType == gBattleMoves[j].type)
                {
                    *moveResult++ = j;
                    break;
                }
            }
        }
    }
}

static void DoDynamaxEvolution(u32 battlerId)
{
	BattleScriptExecute(BattleScript_Dynamax);
	gBattleMons[battlerId].maxHP *= 2;
	gBattleMoveDamage = -gBattleMons[battlerId].hp;
	gBattleMons[battlerId].hp *= 2;
	gBattleStruct->mega.timer[GET_BATTLER_SIDE(battlerId)] = 3;
    SetEvolutionType(gBattleStruct, battlerId, EvolutionDynamax);
	GetMaxMove(&gBattleMons[battlerId]);
    gChosenMoveByBattler[gActiveBattler] = gBattleMons[battlerId].moves[gBattleStruct->chosenMovePositions[gActiveBattler]];
	BtlController_EmitSetMonData(0, REQUEST_MAX_HP_BATTLE, 0, 2, &gBattleMons[gActiveBattler].maxHP);
	MarkBattlerForControllerExec(battlerId);
}
static void UndoDynamaxEvolution(u32 monId) {
	/*gPlayerParty[monId].hp /= 2;
	gPlayerParty[monId].maxHP /= 2;*/
}

static void ChangeMegaTrigger(u8 state)
{
	ChangeMegaTriggerSprite(gBattleStruct->mega.triggerSpriteId, state);
}

static bool32 DummyBattleEvolutionFunc(void)
{
	return FALSE;
}

static const void* const sDummyBattleEvolutionFunc[sizeof(struct BattleEvolutionFunc) / 4] = {
	[0 ... sizeof(struct BattleEvolutionFunc) / 4 - 1] = &DummyBattleEvolutionFunc,
};

static const struct BattleEvolutionFunc sBattleEvolutionMega = {
	.CanPokemonEvolution = CanPokemonMega,
	.CreateOrShowTrigger = CreateMegaTriggerSprite,
	.CreateIndicator = CreateMegaIndicatorSprite,
	.DoEvolution = DoMegaEvolution,
	.UndoEvolution = UndoMegaEvolution,
	.HideTriggerSprite = HideMegaTriggerSprite,
	.ChangeTriggerSprite = ChangeMegaTrigger,
};

static const struct BattleEvolutionFunc sBattleEvolutionDynamax = {
	.CreateOrShowTrigger = TryLoadDynamaxTrigger,
	.CreateIndicator = CreateDynamaxIndicator,
	.ChangeTriggerSprite = ChangeDynamaxTriggerSprite,
	.HideTriggerSprite = HideDynamaxTriggerSprite,
	.CanPokemonEvolution = CanPokemonDynamax,
	.DoEvolution = DoDynamaxEvolution,
	.UndoEvolution = UndoDynamaxEvolution,
};

static const struct BattleEvolutionFunc* const sBattleEvolutionFuncs[] =
	{
		[EvolutionNone] = (const struct BattleEvolutionFunc*)sDummyBattleEvolutionFunc,
		[EvolutionMega] = &sBattleEvolutionMega,
		[EvolutionDynamax] = &sBattleEvolutionDynamax,
		[EvolutionEnd] = (const struct BattleEvolutionFunc*)sDummyBattleEvolutionFunc
	};

struct Pokemon* GetBankPartyData(u8 bank)
{
    u8 index = gBattlerPartyIndexes[bank];
    return (GET_BATTLER_SIDE2(bank) == B_SIDE_OPPONENT) ? &gEnemyParty[index] : &gPlayerParty[index];
}

enum BattleEvolutionType CalEvolutionType(u8 battlerId)
{
    u32 i, result;
    struct Pokemon* pokemon;
    TypeCanPokemonEvolution func;
    pokemon = GetBankPartyData(battlerId);
    for (i = 1; i < ARRAY_COUNT(sBattleEvolutionFuncs); ++i)
    {
        func = sBattleEvolutionFuncs[i]->CanPokemonEvolution;
        result = func(pokemon);
        if (result) return i;
    }
    return EvolutionNone;
}

enum BattleEvolutionType GetEvolutionType(u8 battlerId)
{
    return gBattleStruct->mega.evolutionType[GET_BATTLER_SIDE2(battlerId)][gBattlerPartyIndexes[battlerId]];
}

const struct BattleEvolutionFunc* GetBattleEvolutionFunc(u8 battlerId)
{
    u8 type = GetEvolutionType(battlerId);
    if (!type) type = CalEvolutionType(battlerId);
	return sBattleEvolutionFuncs[type];
}

const struct BattleEvolutionFunc* GetBattlerFuncByEvolutionType(u32 monId, u8 side)// for evolved
{
	return sBattleEvolutionFuncs[gBattleStruct->mega.evolutionType[side][monId]];
}

const u32 gMonFrontPic_Dynamax_1[] = INCBIN_U32("graphics/pokemon/dynamax/1_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_1[] = INCBIN_U32("graphics/pokemon/dynamax/1_back.4bpp.lz");
const u32 gMonPalette_Dynamax_1[] = INCBIN_U32("graphics/pokemon/dynamax/1_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_1[] = INCBIN_U32("graphics/pokemon/dynamax/1_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_2[] = INCBIN_U32("graphics/pokemon/dynamax/2_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_2[] = INCBIN_U32("graphics/pokemon/dynamax/2_back.4bpp.lz");
const u32 gMonPalette_Dynamax_2[] = INCBIN_U32("graphics/pokemon/dynamax/2_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_2[] = INCBIN_U32("graphics/pokemon/dynamax/2_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_3[] = INCBIN_U32("graphics/pokemon/dynamax/3_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_3[] = INCBIN_U32("graphics/pokemon/dynamax/3_back.4bpp.lz");
const u32 gMonPalette_Dynamax_3[] = INCBIN_U32("graphics/pokemon/dynamax/3_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_3[] = INCBIN_U32("graphics/pokemon/dynamax/3_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_4[] = INCBIN_U32("graphics/pokemon/dynamax/4_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_4[] = INCBIN_U32("graphics/pokemon/dynamax/4_back.4bpp.lz");
const u32 gMonPalette_Dynamax_4[] = INCBIN_U32("graphics/pokemon/dynamax/4_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_4[] = INCBIN_U32("graphics/pokemon/dynamax/4_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_5[] = INCBIN_U32("graphics/pokemon/dynamax/5_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_5[] = INCBIN_U32("graphics/pokemon/dynamax/5_back.4bpp.lz");
const u32 gMonPalette_Dynamax_5[] = INCBIN_U32("graphics/pokemon/dynamax/5_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_5[] = INCBIN_U32("graphics/pokemon/dynamax/5_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_6[] = INCBIN_U32("graphics/pokemon/dynamax/6_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_6[] = INCBIN_U32("graphics/pokemon/dynamax/6_back.4bpp.lz");
const u32 gMonPalette_Dynamax_6[] = INCBIN_U32("graphics/pokemon/dynamax/6_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_6[] = INCBIN_U32("graphics/pokemon/dynamax/6_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_7[] = INCBIN_U32("graphics/pokemon/dynamax/7_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_7[] = INCBIN_U32("graphics/pokemon/dynamax/7_back.4bpp.lz");
const u32 gMonPalette_Dynamax_7[] = INCBIN_U32("graphics/pokemon/dynamax/7_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_7[] = INCBIN_U32("graphics/pokemon/dynamax/7_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_8[] = INCBIN_U32("graphics/pokemon/dynamax/8_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_8[] = INCBIN_U32("graphics/pokemon/dynamax/8_back.4bpp.lz");
const u32 gMonPalette_Dynamax_8[] = INCBIN_U32("graphics/pokemon/dynamax/8_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_8[] = INCBIN_U32("graphics/pokemon/dynamax/8_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_9[] = INCBIN_U32("graphics/pokemon/dynamax/9_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_9[] = INCBIN_U32("graphics/pokemon/dynamax/9_back.4bpp.lz");
const u32 gMonPalette_Dynamax_9[] = INCBIN_U32("graphics/pokemon/dynamax/9_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_9[] = INCBIN_U32("graphics/pokemon/dynamax/9_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_10[] = INCBIN_U32("graphics/pokemon/dynamax/10_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_10[] = INCBIN_U32("graphics/pokemon/dynamax/10_back.4bpp.lz");
const u32 gMonPalette_Dynamax_10[] = INCBIN_U32("graphics/pokemon/dynamax/10_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_10[] = INCBIN_U32("graphics/pokemon/dynamax/10_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_11[] = INCBIN_U32("graphics/pokemon/dynamax/11_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_11[] = INCBIN_U32("graphics/pokemon/dynamax/11_back.4bpp.lz");
const u32 gMonPalette_Dynamax_11[] = INCBIN_U32("graphics/pokemon/dynamax/11_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_11[] = INCBIN_U32("graphics/pokemon/dynamax/11_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_12[] = INCBIN_U32("graphics/pokemon/dynamax/12_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_12[] = INCBIN_U32("graphics/pokemon/dynamax/12_back.4bpp.lz");
const u32 gMonPalette_Dynamax_12[] = INCBIN_U32("graphics/pokemon/dynamax/12_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_12[] = INCBIN_U32("graphics/pokemon/dynamax/12_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_13[] = INCBIN_U32("graphics/pokemon/dynamax/13_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_13[] = INCBIN_U32("graphics/pokemon/dynamax/13_back.4bpp.lz");
const u32 gMonPalette_Dynamax_13[] = INCBIN_U32("graphics/pokemon/dynamax/13_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_13[] = INCBIN_U32("graphics/pokemon/dynamax/13_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_14[] = INCBIN_U32("graphics/pokemon/dynamax/14_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_14[] = INCBIN_U32("graphics/pokemon/dynamax/14_back.4bpp.lz");
const u32 gMonPalette_Dynamax_14[] = INCBIN_U32("graphics/pokemon/dynamax/14_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_14[] = INCBIN_U32("graphics/pokemon/dynamax/14_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_15[] = INCBIN_U32("graphics/pokemon/dynamax/15_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_15[] = INCBIN_U32("graphics/pokemon/dynamax/15_back.4bpp.lz");
const u32 gMonPalette_Dynamax_15[] = INCBIN_U32("graphics/pokemon/dynamax/15_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_15[] = INCBIN_U32("graphics/pokemon/dynamax/15_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_16[] = INCBIN_U32("graphics/pokemon/dynamax/16_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_16[] = INCBIN_U32("graphics/pokemon/dynamax/16_back.4bpp.lz");
const u32 gMonPalette_Dynamax_16[] = INCBIN_U32("graphics/pokemon/dynamax/16_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_16[] = INCBIN_U32("graphics/pokemon/dynamax/16_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_17[] = INCBIN_U32("graphics/pokemon/dynamax/17_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_17[] = INCBIN_U32("graphics/pokemon/dynamax/17_back.4bpp.lz");
const u32 gMonPalette_Dynamax_17[] = INCBIN_U32("graphics/pokemon/dynamax/17_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_17[] = INCBIN_U32("graphics/pokemon/dynamax/17_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_18[] = INCBIN_U32("graphics/pokemon/dynamax/18_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_18[] = INCBIN_U32("graphics/pokemon/dynamax/18_back.4bpp.lz");
const u32 gMonPalette_Dynamax_18[] = INCBIN_U32("graphics/pokemon/dynamax/18_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_18[] = INCBIN_U32("graphics/pokemon/dynamax/18_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_19[] = INCBIN_U32("graphics/pokemon/dynamax/19_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_19[] = INCBIN_U32("graphics/pokemon/dynamax/19_back.4bpp.lz");
const u32 gMonPalette_Dynamax_19[] = INCBIN_U32("graphics/pokemon/dynamax/19_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_19[] = INCBIN_U32("graphics/pokemon/dynamax/19_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_20[] = INCBIN_U32("graphics/pokemon/dynamax/20_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_20[] = INCBIN_U32("graphics/pokemon/dynamax/20_back.4bpp.lz");
const u32 gMonPalette_Dynamax_20[] = INCBIN_U32("graphics/pokemon/dynamax/20_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_20[] = INCBIN_U32("graphics/pokemon/dynamax/20_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_21[] = INCBIN_U32("graphics/pokemon/dynamax/21_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_21[] = INCBIN_U32("graphics/pokemon/dynamax/21_back.4bpp.lz");
const u32 gMonPalette_Dynamax_21[] = INCBIN_U32("graphics/pokemon/dynamax/21_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_21[] = INCBIN_U32("graphics/pokemon/dynamax/21_back.gbapal.lz");
const u32 gMonFrontPic_Dynamax_22[] = INCBIN_U32("graphics/pokemon/dynamax/22_front.4bpp.lz");
const u32 gMonBackPic_Dynamax_22[] = INCBIN_U32("graphics/pokemon/dynamax/22_back.4bpp.lz");
const u32 gMonPalette_Dynamax_22[] = INCBIN_U32("graphics/pokemon/dynamax/22_front.gbapal.lz");
const u32 gMonShinyPalette_Dynamax_22[] = INCBIN_U32("graphics/pokemon/dynamax/22_back.gbapal.lz");

void AnimTaskSwapDynamaxSprite(u8 taskId)
{
    if (GetGMaxSpeciesInfo(gBattleMons[gActiveBattler].species).type != 0xFF)
    {
        BattleLoadSubstituteOrMonSpriteGfx(gActiveBattler, TRUE);
    }
    DestroyAnimVisualTask(taskId);
}
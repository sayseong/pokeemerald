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
static enum BattleEvolutionType GetEvolutionType(struct BattleEvolutionData* evolutionData, u8 battlerId);
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
			sprite->pos1.x++;
		if (sprite->pos1.x >= gSprites[gHealthboxSpriteIds[sprite->tBattler]].pos1.x - xPriority)
			sprite->oam.priority = 2;
		else
			sprite->oam.priority = 1;
		if (sprite->pos1.x == gSprites[gHealthboxSpriteIds[sprite->tBattler]].pos1.x - xSlide)
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
	gSprites[gBattleStruct->mega.dynamaxTriggerId].data[0] = battlerId;
	gSprites[gBattleStruct->mega.dynamaxTriggerId].data[1] = 0;
}

void HideDynamaxTriggerSprite(void)
{
	ChangeDynamaxTriggerSprite(0);
	gSprites[gBattleStruct->mega.dynamaxTriggerId].tHide = TRUE;
}


enum BattleEvolutionType GetEvolutionTypeForBattler(u8 battlerId)
{
	return GetEvolutionType(&gBattleStruct->mega, battlerId);
}

void SetEvolutionType(struct BattleStruct* battleStruct, u8 battlerId, enum BattleEvolutionType value)
{
	battleStruct->mega.evolutionType[battlerId & 1][GET_BATTLER_POSITION(battlerId)] = value;
}

bool32 CheckEvolutionType(struct BattleEvolutionData* evolutionData, u8 battlerId)
{
	enum BattleEvolutionType type = GetEvolutionType(evolutionData, battlerId);
	return type == EvolutionNone;
}

static bool32 CanPokemonMega(struct Pokemon* mon)
{
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
static void DoDynamaxEvolution(u32 battlerId)
{
	int i, j;
	BattleScriptExecute(BattleScript_Dynamax);
	gBattleMons[battlerId].maxHP *= 2;
	gBattleMoveDamage = -gBattleMons[battlerId].hp;
	gBattleMons[battlerId].hp *= 2;
	gBattleStruct->mega.timer[GET_BATTLER_SIDE(battlerId)] = 3;
    SetEvolutionType(gBattleStruct, battlerId, EvolutionDynamax);
	for (i = 0; i < 4; ++i)
	{
		for(j = MOVE_MAX_GUARD;j <= MOVE_MAX_STEELSPIKE;j++){
			if (gBattleMons[battlerId].moves[i]>0 && gBattleMoves[gBattleMons[battlerId].moves[i]].type == gBattleMoves[j].type){
				gBattleMons[battlerId].moves[i] = j;
			}
		}
	}
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
		[EvolutionDynamax] = &sBattleEvolutionDynamax
	};

static void InitBattleEvolutionForParty(u8* array, struct Pokemon* poke)
{
	int i;
	int j;
	for (i = 0; i < 6; ++i)
	{
		for (j = 0; j < ARRAY_COUNT(sBattleEvolutionFuncs); ++j)
		{
			if (sBattleEvolutionFuncs[j]->CanPokemonEvolution(&poke[i]))
			{
				array[i] = j;
				break;
			}
		}
	}
}

struct Pokemon* GetBankPartyData(u8 bank)
{
    u8 index = gBattlerPartyIndexes[bank];
    return (GET_BATTLER_SIDE2(bank) == B_SIDE_OPPONENT) ? &gEnemyParty[index] : &gPlayerParty[index];
}

enum BattleEvolutionType GetEvolutionType(struct BattleEvolutionData* evolutionData, u8 battlerId)
{
    int i;
    struct Pokemon* pokemon = GetBankPartyData(battlerId);
    for (i = 1; i < ARRAY_COUNT(sBattleEvolutionFuncs); ++i)
    {
        if (sBattleEvolutionFuncs[i]->CanPokemonEvolution(pokemon)) {
            return i;
        }
    }
    return 0;
}

const struct BattleEvolutionFunc* GetBattleEvolutionFunc(u8 battlerId)
{
	return sBattleEvolutionFuncs[GetEvolutionType(&gBattleStruct->mega, battlerId)];
}

const struct BattleEvolutionFunc* GetBattlerFuncByEvolutionType(u32 monId, u8 side)// for evolved
{
	return sBattleEvolutionFuncs[gBattleStruct->mega.evolutionType[side][monId]];
}
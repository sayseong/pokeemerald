#include "global.h"
#include "main.h"
#include "battle.h"
#include "battle_interface.h"
#include "battle_anim.h"
#include "frontier_util.h"
#include "battle_message.h"
#include "battle_tent.h"
#include "battle_factory.h"
#include "bg.h"
#include "contest.h"
#include "contest_effect.h"
#include "data.h"
#include "daycare.h"
#include "decompress.h"
#include "dynamic_placeholder_text_util.h"
#include "event_data.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "international_string_util.h"
#include "item.h"
#include "link.h"
#include "m4a.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "mon_markings.h"
#include "party_menu.h"
#include "palette.h"
#include "pokeball.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokemon_storage_system.h"
#include "pokemon_summary_screen.h"
#include "region_map.h"
#include "scanline_effect.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "tv.h"
#include "window.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/party_menu.h"
#include "constants/region_map_sections.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "constants/species.h"

// Screen titles (upper left)
#define PSS_LABEL_WINDOW_POKEMON_INFO_TITLE 1
#define PSS_LABEL_WINDOW_POKEMON_SKILLS_TITLE 2
#define PSS_LABEL_WINDOW_BATTLE_MOVES_TITLE 3
#define PSS_LABEL_WINDOW_IV_EV_TITLE 4

// Button control text (upper right)
#define PSS_LABEL_WINDOW_PROMPT_CANCEL 5
#define PSS_LABEL_WINDOW_PROMPT_INFO 6
#define PSS_LABEL_WINDOW_PROMPT_SWITCH 7

// Above the pokemon's portrait
#define PSS_LABEL_WINDOW_PORTRAIT_LVL 9
#define PSS_LABEL_WINDOW_PORTRAIT_NICKNAME 10
#define PSS_LABEL_WINDOW_END 11

// Dynamic fields for the Pokemon Info page
#define PSS_DATA_WINDOW_INFO 0
#define PSS_DATA_WINDOW_INFO_TRAINER_MEMO 1

// Dynamic fields for the Pokemon Skills page
#define PSS_DATA_WINDOW_SKILLS_HP 0
#define PSS_DATA_WINDOW_SKILLS_STATS 1
#define PSS_DATA_WINDOW_SKILLS_EXP 2
#define PSS_DATA_WINDOW_SKILLS_ABILITY 3
#define PSS_DATA_WINDOW_SKILLS_DESCRIPTION 4 // Ability description

// Dynamic fields for the Battle Moves and Contest Moves pages.
#define PSS_DATA_WINDOW_MOVE_NAMES 0
#define PSS_DATA_WINDOW_MOVE_DESCRIPTION 1

// Dynamic fields for the Iv/Ev page
#define PSS_EV_WINDOW_STATS 0
#define PSS_IV_WINDOW_STATS 1

#define MOVE_SELECTOR_SPRITES_COUNT 2
// for the spriteIds field in PokemonSummaryScreenData
enum
{
    SPRITE_ARR_ID_MON,
    SPRITE_ARR_ID_BALL,
    SPRITE_ARR_ID_STATUS,
    SPRITE_ARR_ID_HP_BAR1,
    SPRITE_ARR_ID_HP_BAR2,
    SPRITE_ARR_ID_MON_ICON,
    SPRITE_ARR_ID_TYPE_MON1,
    SPRITE_ARR_ID_TYPE_MON2,
    SPRITE_ARR_ID_SPLIT,
    SPRITE_ARR_ID_NATURE_PLUS,
    SPRITE_ARR_ID_NATURE_MINUS,
    SPRITE_ARR_ID_TYPE_MOVE,
    SPRITE_ARR_ID_MOVE_SELECTOR1 = SPRITE_ARR_ID_TYPE_MOVE + 5, // 10 sprites that make up the selector
    SPRITE_ARR_ID_MOVE_SELECTOR2 = SPRITE_ARR_ID_MOVE_SELECTOR1 + MOVE_SELECTOR_SPRITES_COUNT,
    SPRITE_ARR_ID_COUNT = SPRITE_ARR_ID_MOVE_SELECTOR2 + MOVE_SELECTOR_SPRITES_COUNT
};

// for the icon sprite ids
enum
{
    SUMMARY_ICON_HP,
    SUMMARY_ICON_ATTACK,
    SUMMARY_ICON_DEFENSE,
    SUMMARY_ICON_SPATK,
    SUMMARY_ICON_SPDEF,
    SUMMARY_ICON_SPEED,
    SUMMARY_ICON_ABILITY,
    SUMMARY_ICON_EXP,
    SUMMARY_ICON_TRAINER_MEMO,
    SUMMARY_ICON_ID_NO,
    SUMMARY_ICON_ITEM,
    SUMMARY_ICON_NATURE,
    SUMMARY_ICON_NO,
    SUMMARY_ICON_NAME,
    SUMMARY_ICON_TYPE,
    SUMMARY_ICON_OT,
    SUMMARY_ICON_TOTAL_EVS,
    SUMMARY_ICON_IVS,
    SUMMARY_ICON_POWER,
    SUMMARY_ICON_ACCURACY,
    SUMMARY_ICON_EFFECT,
    SUMMARY_ICON_APPEAL,
    SUMMARY_ICON_JAM,
    SUMMARY_ICON_COUNT,
};

static EWRAM_DATA struct PokemonSummaryScreenData
{
    /*0x00*/ union {
        struct Pokemon *mons;
        struct BoxPokemon *boxMons;
    } monList;
    /*0x04*/ MainCallback callback;
    /*0x0C*/ struct Pokemon currentMon;
    /*0x70*/ struct PokeSummary
    {
        u16 species; // 0x0
        u16 species2; // 0x2
        u8 isEgg; // 0x4
        u8 level; // 0x5
        u8 ribbonCount; // 0x6
        u8 ailment; // 0x7
        u8 abilityNum; // 0x8
        u8 metLocation; // 0x9
        u8 metLevel; // 0xA
        u8 metGame; // 0xB
        u32 pid; // 0xC
        u32 exp; // 0x10
        u16 moves[MAX_MON_MOVES]; // 0x14
        u8 pp[MAX_MON_MOVES]; // 0x1C
        u16 currentHP; // 0x20
        u16 maxHP; // 0x22
        u16 stats[NUM_STATS - 1];
        u16 item; // 0x2E
        u16 friendship; // 0x30
        u8 OTGender; // 0x32
        u8 nature; // 0x33
        u8 ppBonuses; // 0x34
        u8 sanity; // 0x35
        u8 OTName[17]; // 0x36
        u8 nickname[POKEMON_NAME_LENGTH + 1];
        u8 evs[NUM_STATS + 1];
        u8 ivs[NUM_STATS];
        u32 OTID; // 0x48
    } summary;
    u16 bgTilemapBuffers[PSS_PAGE_COUNT][2][0x400];
    u8 mode;
    bool8 isBoxMon;
    u8 curMonIndex;
    u8 maxMonIndex;
    u8 currPageIndex;
    u8 minPageIndex;
    u8 maxPageIndex;
    bool8 lockMonFlag; // This is used to prevent the player from changing pokemon in the move deleter select, etc, but it is not needed because the input is handled differently there
    u16 newMove;
    u8 firstMoveIndex;
    u8 secondMoveIndex;
    bool8 lockMovesFlag; // This is used to prevent the player from changing position of moves in a battle or when trading.
    u8 bgDisplayOrder; // Determines the order page backgrounds are loaded while scrolling between them
    u8 windowIds[8];
    u8 laserSpriteIds[5];
    u8 spriteIds[SPRITE_ARR_ID_COUNT];
    u8 iconSpriteIds[SUMMARY_ICON_COUNT][2][2]; // Two sets allowing you to create 2 of every icon.
    bool8 unk40EF;
    s16 switchCounter; // Used for various switch statement cases that decompress/load graphics or pokemon data
    u8 prevEvs[NUM_STATS + 1]; // To avoid lag when switching mons.
    u8 prevIvs[NUM_STATS];
    bool8 textScrollMon; // When displaying text for the specific page
    bool8 moveSelectionMode;
    bool8 contestMode;
} *sMonSummaryScreen = NULL;

EWRAM_DATA u8 gLastViewedMonIndex = 0;
static EWRAM_DATA u8 sMoveSlotToReplace = 0;
static EWRAM_DATA u8 sUnknownTaskId = 0;

// forward declarations
static bool8 LoadGraphics(void);
static void CB2_InitSummaryScreen(void);
static void InitBGs(void);
static bool8 DecompressGraphics(void);
static void CopyMonToSummaryStruct(struct Pokemon* a);
static bool8 ExtractMonDataToSummaryStruct(struct Pokemon *mon, bool32 atOnce);
static void CloseSummaryScreen(u8 taskId);
static void Task_HandleInput(u8 taskId);
static void ChangeSummaryPokemon(u8 taskId, s8 a);
static void Task_ChangeSummaryMon(u8 taskId);
static s8 AdvanceMonIndex(s8 delta);
static s8 AdvanceMultiBattleMonIndex(s8 delta);
static bool8 IsValidToViewInMulti(struct Pokemon* mon);
static void ChangePage(u8 taskId, s8 a);
static void PssScrollRight(u8 taskId);
static void PssScrollRightEnd(u8 taskId);
static void PssScrollLeft(u8 taskId);
static void PssScrollLeftEnd(u8 taskId);
static void TryDrawBars(void);
static void SwitchToFromContestView(u32 moveIndex);
static void SwitchToMoveSelection(u8 taskId, TaskFunc func);
static void Task_HandleInput_MoveSelect(u8 taskId);
static bool8 HasMoreThanOneMove(void);
static void ChangeSelectedMove(s16 *taskData, s8 direction, u8 *moveIndexPtr);
static void CloseMoveSelectMode(u8 taskId);
static void SwitchToMovePositionSwitchMode(u8 a);
static void Task_HandleInput_MovePositionSwitch(u8 taskId);
static void ExitMovePositionSwitchMode(u8 taskId, bool8 swapMoves);
static void SwapMonMoves(struct Pokemon *mon, u8 moveIndex1, u8 moveIndex2);
static void SwapBoxMonMoves(struct BoxPokemon *mon, u8 moveIndex1, u8 moveIndex2);
static void Task_SetHandleReplaceMoveInput(u8 taskId);
static void Task_HandleReplaceMoveInput(u8 taskId);
static bool8 CanReplaceMove(void);
static void DrawPagination(void);
static void DetailedMovesTilemapDisplay(u16 *dst, u16 palette, bool8 remove);
static void DrawPokerusCuredSymbol(struct Pokemon* mon);
static void DrawExperienceProgressBar(void);
static void DrawHpBar(void);
static void LimitEggSummaryPageDisplay(void);
static void ResetWindows(void);
static void PrintTextOnWindow(u8 windowId, const u8 *string, u8 x, u8 y, u8 lineSpacing, u8 colorId);
static void PrintPageNamesAndStats(void);
static void PutPageWindowsAndSprites(u8 a);
static void ClearPageWindowsAndSprites(u8 a);
static void RemoveWindowByIndex(u8 a);
static void SetSummaryIconInvisibility(u32 iconId, u32 setId, bool32 invisible);
static void PrintPageSpecificText(u8 pageIndex, bool32 scrollMon);
static void CreateTextPrinterTask(u8 a);
static void PrintInfoPageText(void);
static void PrintIvEvs(void);
static void Task_PrintIvEvs(u8 taskId);
static void Task_PrintInfoPage(u8 taskId);
static bool8 DoesMonOTMatchOwner(void);
static bool8 DidMonComeFromGBAGames(void);
static bool8 IsInGamePartnerMon(void);
static void Task_PrintSkillsPage(u8 taskId);
static void PrintBattleMoves(void);
static void Task_PrintBattleMoves(u8 taskId);
static void PrintMoveNameAndPP(u32 windowId, u32 moveIndex);
static void PrintMoveDetails(u16 a);
static void PrintMoveCancel(bool32 remove);
static void SwapMovesNamesPP(u8 moveIndex1, u8 moveIndex2);
static void PrintHMMovesCantBeForgotten(void);
static void ResetSpriteIds(void);
static void DestroySpriteInArray(u8 spriteArrayId);
static void SetSpriteCoords(u8 spriteArrayId, s16 x, s16 y);
static void SetSpriteAnim(u8 spriteArrayId, u8 animId);
static void SetSpriteInvisibility(u8 spriteArrayId, bool8 invisible);
static void HidePageSpecificSprites(void);
static void SetTypeIcons(void);
static void CreateMoveTypeIcons(void);
static void CreateNewInterfaceIcons(void);
static void SetMonTypeIcons(void);
static void SetMoveTypeIcons(void);
static void SetContestMoveTypeIcons(void);
static void SetNewMoveTypeIcon(void);
static void SwapMovesTypeSprites(u8 moveIndex1, u8 moveIndex2);
static u8 LoadMonGfxAndSprite(struct Pokemon *a, s16 *b);
static u8 CreateMonSprite(struct Pokemon *unused);
static void SpriteCB_Pokemon(struct Sprite *);
static void StopPokemonAnimations(void);
static void CreateCaughtBallSprite(struct Pokemon *mon);
static void CreateSetStatusSprite(void);
static void CreateMoveSelectorSprites(u8 idArrayStart);
static void SpriteCb_MoveSelector(struct Sprite *sprite);
static void DestroyMoveSelectorSprites(u8 firstArrayId);
static void SwapMoveSelectors(void);
static void MakeMoveSelectorVisible(u8 a);
static void PrintMonInfo(void);
static void PrintSkillsPageText(void);

// const rom data
#include "data/text/move_descriptions.h"
#include "data/text/nature_names.h"

static const struct BgTemplate sBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0,
    },
    {
        .bg = 1,
        .charBaseIndex = 2,
        .mapBaseIndex = 27,
        .screenSize = 1,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0,
    },
    {
        .bg = 2,
        .charBaseIndex = 2,
        .mapBaseIndex = 25,
        .screenSize = 1,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0,
    },
    {
        .bg = 3,
        .charBaseIndex = 2,
        .mapBaseIndex = 29,
        .screenSize = 1,
        .paletteMode = 0,
        .priority = 3,
        .baseTile = 0,
    },
};

static const s8 sMultiBattleOrder[] = {0, 2, 3, 1, 4, 5};
static const struct WindowTemplate sSummaryTemplate[] =
{
    [PSS_LABEL_WINDOW_POKEMON_INFO_TITLE] = {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 11,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 1,
    },
    [PSS_LABEL_WINDOW_POKEMON_SKILLS_TITLE] = {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 11,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 23,
    },
    [PSS_LABEL_WINDOW_BATTLE_MOVES_TITLE] = {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 11,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 45,
    },
    [PSS_LABEL_WINDOW_IV_EV_TITLE] = {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 11,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 67,
    },
    [PSS_LABEL_WINDOW_PROMPT_CANCEL] = {
        .bg = 0,
        .tilemapLeft = 22,
        .tilemapTop = 0,
        .width = 8,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 89,
    },
    [PSS_LABEL_WINDOW_PROMPT_INFO] = {
        .bg = 0,
        .tilemapLeft = 22,
        .tilemapTop = 0,
        .width = 8,
        .height = 2,
        .paletteNum = 7,
        .baseBlock = 105,
    },
    [PSS_LABEL_WINDOW_PROMPT_SWITCH] = {
        .bg = 0,
        .tilemapLeft = 22,
        .tilemapTop = 0,
        .width = 8,
        .height = 2,
        .paletteNum = 7,
        .baseBlock = 121,
    },
    [PSS_LABEL_WINDOW_PORTRAIT_LVL] = {
        .bg = 0,
        .tilemapLeft = 16,
        .tilemapTop = 2,
        .width = 4,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 181,
    },
    [PSS_LABEL_WINDOW_PORTRAIT_NICKNAME] = {
        .bg = 0,
        .tilemapLeft = 20,
        .tilemapTop = 2,
        .width = 10,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 225,
    },
    [PSS_LABEL_WINDOW_END] = DUMMY_WIN_TEMPLATE
};
static const struct WindowTemplate sPageInfoTemplate[] =
{
    [PSS_DATA_WINDOW_INFO] = {
        .bg = 0,
        .tilemapLeft = 6,
        .tilemapTop = 2,
        .width = 10,
        .height = 14,
        .paletteNum = 1,
        .baseBlock = 245,
    },
    [PSS_DATA_WINDOW_INFO_TRAINER_MEMO] = {
        .bg = 0,
        .tilemapLeft = 3,
        .tilemapTop = 17,
        .width = 25,
        .height = 4,
        .paletteNum = 1,
        .baseBlock = 500,
    },
};
static const struct WindowTemplate sPageSkillsTemplate[] =
{
    [PSS_DATA_WINDOW_SKILLS_HP] = {
        .bg = 0,
        .tilemapLeft = 8,
        .tilemapTop = 2,
        .width = 8,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 245,
    },
    [PSS_DATA_WINDOW_SKILLS_STATS] = {
        .bg = 0,
        .tilemapLeft = 8,
        .tilemapTop = 4,
        .width = 6,
        .height = 8,
        .paletteNum = 1,
        .baseBlock = 270,
    },
    [PSS_DATA_WINDOW_SKILLS_EXP] = {
        .bg = 0,
        .tilemapLeft = 8,
        .tilemapTop = 12,
        .width = 16,
        .height = 5,
        .paletteNum = 1,
        .baseBlock = 370,
    },
    [PSS_DATA_WINDOW_SKILLS_ABILITY] = {
        .bg = 0,
        .tilemapLeft = 7,
        .tilemapTop = 15,
        .width = 10,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 470,
    },
    [PSS_DATA_WINDOW_SKILLS_DESCRIPTION] = {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 17,
        .width = 18,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 490,
    },
};
static const struct WindowTemplate sPageMovesTemplate[] = // This is used for both battle and contest moves
{
    [PSS_DATA_WINDOW_MOVE_NAMES] = {
        .bg = 0,
        .tilemapLeft = 4,
        .tilemapTop = 2,
        .width = 12,
        .height = 18,
        .paletteNum = 1,
        .baseBlock = 250,
    },
    [PSS_DATA_WINDOW_MOVE_DESCRIPTION] = {
        .bg = 0,
        .tilemapLeft = 16,
        .tilemapTop = 7,
        .width = 16,
        .height = 20,
        .paletteNum = 1,
        .baseBlock = 540,
    },
};
static const struct WindowTemplate sPageIvEvTemplate[] =
{
    [PSS_EV_WINDOW_STATS] = {
        .bg = 0,
        .tilemapLeft = 8,
        .tilemapTop = 2,
        .width = 6,
        .height = 12,
        .paletteNum = 1,
        .baseBlock = 250,
    },
    [PSS_IV_WINDOW_STATS] = {
        .bg = 0,
        .tilemapLeft = 5,
        .tilemapTop = 14,
        .width = 17,
        .height = 7,
        .paletteNum = 1,
        .baseBlock = 450,
    },
};

static const u8 sTextColors[][3] =
{
    {0, 1, 2}, //0
    {0, 3, 4}, //1
    {0, 5, 6}, //2
    {0, 7, 8}, //3
    {0, 9, 10},  //4
    {0, 11, 12}, //5
    {0, 13, 14}, //6
    {0, 7, 8}, //7
    {13, 15, 14}, //8
    {0, 12, 11}, //9
    {0, 3, 4}, //10
    {0, 13, 15}, //11
    {0, 7, 8}, //12
    {0, 4, 2} //13
};

static const u8 sSummaryAButtonBitmap[] = INCBIN_U8("graphics/interface/summary_a_button.4bpp");
static const u8 sSummaryBButtonBitmap[] = INCBIN_U8("graphics/interface/summary_b_button.4bpp");

static void (*const sTextPrinterFunctions[])(void) =
{
    [PSS_PAGE_INFO] = PrintInfoPageText,
    [PSS_PAGE_SKILLS] = PrintSkillsPageText,
    [PSS_PAGE_BATTLE_MOVES] = PrintBattleMoves,
    [PSS_PAGE_IV_EVS] = PrintIvEvs,
};

static void (*const sTextPrinterTasks[])(u8 taskId) =
{
    [PSS_PAGE_INFO] = Task_PrintInfoPage,
    [PSS_PAGE_SKILLS] = Task_PrintSkillsPage,
    [PSS_PAGE_BATTLE_MOVES] = Task_PrintBattleMoves,
    [PSS_PAGE_IV_EVS] = Task_PrintIvEvs,
};

#define TAG_MON_STATUS 30001
#define TAG_MOVE_TYPES 30002
#define TAG_MON_MARKINGS 30003
#define TAG_SUMMARY_ICONS 30004
#define TAG_HP_BAR 30005 // two tags
#define TAG_NATURE_ICONS 30007
#define TAG_SPLIT_ICON 30008
#define TAG_MOVE_SELECTOR_RED 30009
#define TAG_MOVE_SELECTOR_BLUE 30010
#define TAG_LASER_GRID 30015 // 5 tags

static const struct OamData sOamData_SplitIcons =
{
    .shape = SPRITE_SHAPE(16x16),
    .size = SPRITE_SIZE(16x16),
    .priority = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_SplitIcons =
{
    .data = gSplitIcons_Gfx,
    .size = 0x180,
    .tag = TAG_SPLIT_ICON,
};

static const struct SpritePalette sSpritePalette_SplitIcons =
{
    .data = gSplitIcons_Pal,
    .tag = TAG_SPLIT_ICON
};

static const union AnimCmd sSpriteAnim_SplitIcon0[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SplitIcon1[] =
{
    ANIMCMD_FRAME(4, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SplitIcon2[] =
{
    ANIMCMD_FRAME(8, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_SplitIcons[] =
{
    sSpriteAnim_SplitIcon0,
    sSpriteAnim_SplitIcon1,
    sSpriteAnim_SplitIcon2,
};

static const struct SpriteTemplate sSpriteTemplate_SplitIcons =
{
    .tileTag = TAG_SPLIT_ICON,
    .paletteTag = TAG_SPLIT_ICON,
    .oam = &sOamData_SplitIcons,
    .anims = sSpriteAnimTable_SplitIcons,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct OamData sOamData_NatureIcon =
{
    .shape = SPRITE_SHAPE(16x8),
    .size = SPRITE_SIZE(16x8),
    .priority = 1,
};

static const union AnimCmd sSpriteAnim_NatureMinus[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_NaturePlus[] =
{
    ANIMCMD_FRAME(2, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_Natures[] =
{
    sSpriteAnim_NatureMinus,
    sSpriteAnim_NaturePlus
};

static const struct SpriteTemplate sSpriteTemplate_NatureIcon =
{
    .tileTag = TAG_NATURE_ICONS,
    .paletteTag = TAG_SUMMARY_ICONS,
    .oam = &sOamData_NatureIcon,
    .anims = sSpriteAnimTable_Natures,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const struct CompressedSpriteSheet sSpriteSheet_NatureIcons =
{
    .data = gSummaryNatureIcons_Gfx,
    .size = 0x80,
    .tag = TAG_NATURE_ICONS
};

static const struct OamData sOamData_HpBar[] =
{
    {
        .shape = SPRITE_SHAPE(32x8),
        .size = SPRITE_SIZE(32x8),
        .priority = 1,
    },
    {
        .shape = SPRITE_SHAPE(16x8),
        .size = SPRITE_SIZE(16x8),
        .priority = 1,
    }
};

static const struct SpriteTemplate sSpriteTemplate_HpBar[] =
{
    {
        .tileTag = TAG_HP_BAR,
        .paletteTag = TAG_SUMMARY_ICONS,
        .oam = &sOamData_HpBar[0],
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy
    },
    {
        .tileTag = TAG_HP_BAR + 1,
        .paletteTag = TAG_SUMMARY_ICONS,
        .oam = &sOamData_HpBar[1],
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy
    },
};

static const struct OamData sOamData_LaserGrid =
{
    .shape = SPRITE_SHAPE(64x64),
    .size = SPRITE_SIZE(64x64),
    .priority = 1,
};

static const struct OamData sOamData_LaserGrid2 =
{
    .shape = SPRITE_SHAPE(32x32),
    .size = SPRITE_SIZE(32x32),
    .priority = 1,
};

static const struct OamData sOamData_LaserGrid3 =
{
    .shape = SPRITE_SHAPE(16x16),
    .size = SPRITE_SIZE(16x16),
    .priority = 1,
};

static const struct CompressedSpriteSheet gSpriteSheet_LaserGrid1 =
{
    .data = gLaserGrid1_Gfx,
    .size = 0x080,
    .tag = TAG_LASER_GRID + 1
};
static const struct SpritePalette gSpritePalette_LaserGrid =
{
    .data = gLaserGrid_Pal,
    .tag = TAG_LASER_GRID,
};
static const struct SpriteTemplate sSpriteTemplate_LaserGrid1 =
{
    .tileTag = TAG_LASER_GRID + 1,
    .paletteTag = TAG_LASER_GRID,
    .oam = &sOamData_LaserGrid3,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct CompressedSpriteSheet gSpriteSheet_LaserGrid2 =
{
    .data = gLaserGrid2_Gfx,
    .size = 0x800,
    .tag = TAG_LASER_GRID + 2
};

static const struct SpriteTemplate sSpriteTemplate_LaserGrid2 =
{
    .tileTag = TAG_LASER_GRID + 2,
    .paletteTag = TAG_LASER_GRID,
    .oam = &sOamData_LaserGrid,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct CompressedSpriteSheet gSpriteSheet_LaserGrid3 =
{
    .data = gLaserGrid3_Gfx,
    .size = 0x200,
    .tag = TAG_LASER_GRID + 3
};

static const struct SpriteTemplate sSpriteTemplate_LaserGrid3 =
{
    .tileTag = TAG_LASER_GRID + 3,
    .paletteTag = TAG_LASER_GRID,
    .oam = &sOamData_LaserGrid2,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct CompressedSpriteSheet gSpriteSheet_LaserGrid4 =
{
    .data = gLaserGrid4_Gfx,
    .size = 0x800,
    .tag = TAG_LASER_GRID + 4
};

static const struct SpriteTemplate sSpriteTemplate_LaserGrid4 =
{
    .tileTag = TAG_LASER_GRID + 4,
    .paletteTag = TAG_LASER_GRID,
    .oam = &sOamData_LaserGrid,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct OamData sOamData_SummaryIcons =
{
    .shape = SPRITE_SHAPE(32x16),
    .size = SPRITE_SIZE(32x16),
    .priority = 1,
};

static const struct CompressedSpriteSheet sSpriteSheet_SummaryIcons =
{
    gSummaryIcons_Gfx, 0x2E00, TAG_SUMMARY_ICONS
};
static const struct SpritePalette sSpritePalette_SummaryIcons =
{
    gSummaryIcons_Pal, TAG_SUMMARY_ICONS
};

#define FRAME(picId, frameId) ANIMCMD_FRAME((((picId * 2) + frameId) * 8), 0)

static const union AnimCmd sSpriteAnim_SummaryHp1[] =
{
    FRAME(SUMMARY_ICON_HP, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryHp2[] =
{
    FRAME(SUMMARY_ICON_HP, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryAttack1[] =
{
    FRAME(SUMMARY_ICON_ATTACK, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryAttack2[] =
{
    FRAME(SUMMARY_ICON_ATTACK, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryDefense1[] =
{
    FRAME(SUMMARY_ICON_DEFENSE, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryDefense2[] =
{
    FRAME(SUMMARY_ICON_DEFENSE, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummarySpatk1[] =
{
    FRAME(SUMMARY_ICON_SPATK, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummarySpatk2[] =
{
    FRAME(SUMMARY_ICON_SPATK, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummarySpdef1[] =
{
    FRAME(SUMMARY_ICON_SPDEF, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummarySpdef2[] =
{
    FRAME(SUMMARY_ICON_SPDEF, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummarySpeed1[] =
{
    FRAME(SUMMARY_ICON_SPEED, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummarySpeed2[] =
{
    FRAME(SUMMARY_ICON_SPEED, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryAbility1[] =
{
    FRAME(SUMMARY_ICON_ABILITY, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryAbility2[] =
{
    FRAME(SUMMARY_ICON_ABILITY, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryExp1[] =
{
    FRAME(SUMMARY_ICON_EXP, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryExp2[] =
{
    FRAME(SUMMARY_ICON_EXP, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryTrainerMemo1[] =
{
    FRAME(SUMMARY_ICON_TRAINER_MEMO, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryTrainerMemo2[] =
{
    FRAME(SUMMARY_ICON_TRAINER_MEMO, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryIdNo1[] =
{
    FRAME(SUMMARY_ICON_ID_NO, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryIdNo2[] =
{
    FRAME(SUMMARY_ICON_ID_NO, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryItem1[] =
{
    FRAME(SUMMARY_ICON_ITEM, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryItem2[] =
{
    FRAME(SUMMARY_ICON_ITEM, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryNature1[] =
{
    FRAME(SUMMARY_ICON_NATURE, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryNature2[] =
{
    FRAME(SUMMARY_ICON_NATURE, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryNo1[] =
{
    FRAME(SUMMARY_ICON_NO, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryNo2[] =
{
    FRAME(SUMMARY_ICON_NO, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryName1[] =
{
    FRAME(SUMMARY_ICON_NAME, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryName2[] =
{
    FRAME(SUMMARY_ICON_NAME, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryType1[] =
{
    FRAME(SUMMARY_ICON_TYPE, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryType2[] =
{
    FRAME(SUMMARY_ICON_TYPE, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryOt1[] =
{
    FRAME(SUMMARY_ICON_OT, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryOt2[] =
{
    FRAME(SUMMARY_ICON_OT, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryTotalEvs1[] =
{
    FRAME(SUMMARY_ICON_TOTAL_EVS, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryTotalEvs2[] =
{
    FRAME(SUMMARY_ICON_TOTAL_EVS, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryIvs1[] =
{
    FRAME(SUMMARY_ICON_IVS, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryIvs2[] =
{
    FRAME(SUMMARY_ICON_IVS, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryPower1[] =
{
    FRAME(SUMMARY_ICON_POWER, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryPower2[] =
{
    FRAME(SUMMARY_ICON_POWER, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryAccuracy1[] =
{
    FRAME(SUMMARY_ICON_ACCURACY, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryAccuracy2[] =
{
    FRAME(SUMMARY_ICON_ACCURACY, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryEffect1[] =
{
    FRAME(SUMMARY_ICON_EFFECT, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryEffect2[] =
{
    FRAME(SUMMARY_ICON_EFFECT, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryAppeal1[] =
{
    FRAME(SUMMARY_ICON_APPEAL, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryAppeal2[] =
{
    FRAME(SUMMARY_ICON_APPEAL, 1),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryJam1[] =
{
    FRAME(SUMMARY_ICON_JAM, 0),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_SummaryJam2[] =
{
    FRAME(SUMMARY_ICON_JAM, 1),
    ANIMCMD_END
};
static const union AnimCmd *const sSpriteAnimTable_SummaryIcons[] =
{
    sSpriteAnim_SummaryHp1,
    sSpriteAnim_SummaryHp2,
    sSpriteAnim_SummaryAttack1,
    sSpriteAnim_SummaryAttack2,
    sSpriteAnim_SummaryDefense1,
    sSpriteAnim_SummaryDefense2,
    sSpriteAnim_SummarySpatk1,
    sSpriteAnim_SummarySpatk2,
    sSpriteAnim_SummarySpdef1,
    sSpriteAnim_SummarySpdef2,
    sSpriteAnim_SummarySpeed1,
    sSpriteAnim_SummarySpeed2,
    sSpriteAnim_SummaryAbility1,
    sSpriteAnim_SummaryAbility2,
    sSpriteAnim_SummaryExp1,
    sSpriteAnim_SummaryExp2,
    sSpriteAnim_SummaryTrainerMemo1,
    sSpriteAnim_SummaryTrainerMemo2,
    sSpriteAnim_SummaryIdNo1,
    sSpriteAnim_SummaryIdNo2,
    sSpriteAnim_SummaryItem1,
    sSpriteAnim_SummaryItem2,
    sSpriteAnim_SummaryNature1,
    sSpriteAnim_SummaryNature2,
    sSpriteAnim_SummaryNo1,
    sSpriteAnim_SummaryNo2,
    sSpriteAnim_SummaryName1,
    sSpriteAnim_SummaryName2,
    sSpriteAnim_SummaryType1,
    sSpriteAnim_SummaryType2,
    sSpriteAnim_SummaryOt1,
    sSpriteAnim_SummaryOt2,
    sSpriteAnim_SummaryTotalEvs1,
    sSpriteAnim_SummaryTotalEvs2,
    sSpriteAnim_SummaryIvs1,
    sSpriteAnim_SummaryIvs2,
    sSpriteAnim_SummaryPower1,
    sSpriteAnim_SummaryPower2,
    sSpriteAnim_SummaryAccuracy1,
    sSpriteAnim_SummaryAccuracy2,
    sSpriteAnim_SummaryEffect1,
    sSpriteAnim_SummaryEffect2,
    sSpriteAnim_SummaryAppeal1,
    sSpriteAnim_SummaryAppeal2,
    sSpriteAnim_SummaryJam1,
    sSpriteAnim_SummaryJam2,
};

static const struct SpriteTemplate sSpriteTemplate_SummaryIcons =
{
    .tileTag = TAG_SUMMARY_ICONS,
    .paletteTag = TAG_SUMMARY_ICONS,
    .oam = &sOamData_SummaryIcons,
    .anims = sSpriteAnimTable_SummaryIcons,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

#undef FRAME

// Coordinates for the summary icon sprites for all pages.
struct CoordsId
{
    s32 id;
    s16 x;
    s16 y;
};

static const struct CoordsId sPageInfoIcons[] =
{
    {SUMMARY_ICON_NO, 9, 22},
    {SUMMARY_ICON_NAME, 9, 39},
    {SUMMARY_ICON_TYPE, 9, 54},
    {SUMMARY_ICON_OT, 9, 70},
    {SUMMARY_ICON_ID_NO, 9, 79},
    {SUMMARY_ICON_ITEM, 9, 95},
    {SUMMARY_ICON_NATURE, 9, 110},
    {SUMMARY_ICON_TRAINER_MEMO, 20, 129},
    {-1}
};

static const struct CoordsId sPageSkillsIcons[] =
{
    {SUMMARY_ICON_HP, 8, 22},
    {SUMMARY_ICON_ATTACK, 8, 43},
    {SUMMARY_ICON_DEFENSE, 8, 54},
    {SUMMARY_ICON_SPATK, 8, 67},
    {SUMMARY_ICON_SPDEF, 8, 79},
    {SUMMARY_ICON_SPEED, 8, 91},
    {SUMMARY_ICON_EXP, 8, 103},
    {SUMMARY_ICON_ABILITY, 15, 128},
    {-1}
};

static const struct CoordsId sPageIvEvsIcons[] =
{
    {SUMMARY_ICON_HP, 8, 21},
    {SUMMARY_ICON_ATTACK, 8, 35},
    {SUMMARY_ICON_DEFENSE, 8, 46},
    {SUMMARY_ICON_SPATK, 8, 58},
    {SUMMARY_ICON_SPDEF, 8, 71},
    {SUMMARY_ICON_SPEED, 8, 83},
    {SUMMARY_ICON_TOTAL_EVS, 16, 97},
    {SUMMARY_ICON_IVS, 0, 114},
    {SUMMARY_ICON_HP, 7, 128},
    {SUMMARY_ICON_ATTACK, 20, 142},
    {SUMMARY_ICON_DEFENSE, 33, 154},
    {SUMMARY_ICON_SPATK, 83, 129},
    {SUMMARY_ICON_SPDEF, 96, 142},
    {SUMMARY_ICON_SPEED, 109, 155},
    {-1}
};

static const struct CoordsId sPageBattleMoves[] =
{
    {SUMMARY_ICON_POWER, 130, 63},
    {SUMMARY_ICON_ACCURACY, 130, 79},
    {SUMMARY_ICON_EFFECT, 130, 95},
    {-1},
};

static const struct CoordsId* const sSummaryIconCoords[5] =
{
    [PSS_PAGE_INFO] = sPageInfoIcons,
    [PSS_PAGE_SKILLS] = sPageSkillsIcons,
    [PSS_PAGE_BATTLE_MOVES] = sPageBattleMoves,
    [PSS_PAGE_IV_EVS] = sPageIvEvsIcons,
};

static const struct OamData sOamData_MoveTypes =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = 0,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x16),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};
static const union AnimCmd sSpriteAnim_TypeNormal[] = {
    ANIMCMD_FRAME(TYPE_NORMAL * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFighting[] = {
    ANIMCMD_FRAME(TYPE_FIGHTING * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFlying[] = {
    ANIMCMD_FRAME(TYPE_FLYING * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypePoison[] = {
    ANIMCMD_FRAME(TYPE_POISON * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeGround[] = {
    ANIMCMD_FRAME(TYPE_GROUND * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeRock[] = {
    ANIMCMD_FRAME(TYPE_ROCK * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeBug[] = {
    ANIMCMD_FRAME(TYPE_BUG * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeGhost[] = {
    ANIMCMD_FRAME(TYPE_GHOST * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeSteel[] = {
    ANIMCMD_FRAME(TYPE_STEEL * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeMystery[] = {
    ANIMCMD_FRAME(TYPE_MYSTERY * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFire[] = {
    ANIMCMD_FRAME(TYPE_FIRE * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeWater[] = {
    ANIMCMD_FRAME(TYPE_WATER * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeGrass[] = {
    ANIMCMD_FRAME(TYPE_GRASS * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeElectric[] = {
    ANIMCMD_FRAME(TYPE_ELECTRIC * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypePsychic[] = {
    ANIMCMD_FRAME(TYPE_PSYCHIC * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeIce[] = {
    ANIMCMD_FRAME(TYPE_ICE * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeDragon[] = {
    ANIMCMD_FRAME(TYPE_DRAGON * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeDark[] = {
    ANIMCMD_FRAME(TYPE_DARK * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFairy[] = {
    ANIMCMD_FRAME(TYPE_FAIRY * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategoryCool[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_COOL + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategoryBeauty[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_BEAUTY + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategoryCute[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_CUTE + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategorySmart[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_SMART + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategoryTough[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_TOUGH + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd *const sSpriteAnimTable_MoveTypes[NUMBER_OF_MON_TYPES + CONTEST_CATEGORIES_COUNT] = {
    sSpriteAnim_TypeNormal,
    sSpriteAnim_TypeFighting,
    sSpriteAnim_TypeFlying,
    sSpriteAnim_TypePoison,
    sSpriteAnim_TypeGround,
    sSpriteAnim_TypeRock,
    sSpriteAnim_TypeBug,
    sSpriteAnim_TypeGhost,
    sSpriteAnim_TypeSteel,
    sSpriteAnim_TypeMystery,
    sSpriteAnim_TypeFire,
    sSpriteAnim_TypeWater,
    sSpriteAnim_TypeGrass,
    sSpriteAnim_TypeElectric,
    sSpriteAnim_TypePsychic,
    sSpriteAnim_TypeIce,
    sSpriteAnim_TypeDragon,
    sSpriteAnim_TypeDark,
    sSpriteAnim_TypeFairy,
    sSpriteAnim_CategoryCool,
    sSpriteAnim_CategoryBeauty,
    sSpriteAnim_CategoryCute,
    sSpriteAnim_CategorySmart,
    sSpriteAnim_CategoryTough,
};

static const struct CompressedSpriteSheet sSpriteSheet_MoveTypes =
{
    .data = gMoveTypes_Gfx,
    .size = (NUMBER_OF_MON_TYPES + CONTEST_CATEGORIES_COUNT) * 0x100,
    .tag = TAG_MOVE_TYPES
};
static const struct SpriteTemplate sSpriteTemplate_MoveTypes =
{
    .tileTag = TAG_MOVE_TYPES,
    .paletteTag = TAG_MOVE_TYPES,
    .oam = &sOamData_MoveTypes,
    .anims = sSpriteAnimTable_MoveTypes,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};
static const u8 sMoveTypeToOamPaletteNum[NUMBER_OF_MON_TYPES + CONTEST_CATEGORIES_COUNT] =
{
    [TYPE_NORMAL] = 13,
    [TYPE_FIGHTING] = 13,
    [TYPE_FLYING] = 14,
    [TYPE_POISON] = 14,
    [TYPE_GROUND] = 13,
    [TYPE_ROCK] = 13,
    [TYPE_BUG] = 15,
    [TYPE_GHOST] = 14,
    [TYPE_STEEL] = 13,
    [TYPE_MYSTERY] = 15,
    [TYPE_FIRE] = 13,
    [TYPE_WATER] = 14,
    [TYPE_GRASS] = 15,
    [TYPE_ELECTRIC] = 13,
    [TYPE_PSYCHIC] = 14,
    [TYPE_ICE] = 14,
    [TYPE_DRAGON] = 15,
    [TYPE_DARK] = 13,
    [TYPE_FAIRY] = 14,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_COOL] = 13,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_BEAUTY] = 14,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_CUTE] = 14,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_SMART] = 15,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_TOUGH] = 13,
};

static const struct OamData sOamData_MoveSelector =
{
    .shape = SPRITE_SHAPE(64x32),
    .size = SPRITE_SIZE(64x32),
    .priority = 1,
};
static const union AnimCmd sSpriteAnim_MoveSelector0[] = {
    ANIMCMD_FRAME(0, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_MoveSelector1[] = {
    ANIMCMD_FRAME(0, 0, TRUE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd *const sSpriteAnimTable_MoveSelector[] = {
    sSpriteAnim_MoveSelector0,
    sSpriteAnim_MoveSelector1,
};
static const struct CompressedSpriteSheet sSpriteSheet_MoveSelectorBlue =
{
    .data = gSummaryMoveSelectBlue_Gfx,
    .size = 64*32,
    .tag = TAG_MOVE_SELECTOR_BLUE
};
static const struct CompressedSpriteSheet sSpriteSheet_MoveSelectorRed =
{
    .data = gSummaryMoveSelectRed_Gfx,
    .size = 64*32,
    .tag = TAG_MOVE_SELECTOR_RED
};
static const struct SpriteTemplate sSpriteTemplate_MoveSelectorRed =
{
    .tileTag = TAG_MOVE_SELECTOR_RED,
    .paletteTag = TAG_SUMMARY_ICONS,
    .oam = &sOamData_MoveSelector,
    .anims = sSpriteAnimTable_MoveSelector,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCb_MoveSelector
};
static const struct SpriteTemplate sSpriteTemplate_MoveSelectorBlue =
{
    .tileTag = TAG_MOVE_SELECTOR_BLUE,
    .paletteTag = TAG_SUMMARY_ICONS,
    .oam = &sOamData_MoveSelector,
    .anims = sSpriteAnimTable_MoveSelector,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCb_MoveSelector
};
static const struct OamData sOamData_StatusCondition =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = 0,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x8),
    .tileNum = 0,
    .priority = 3,
    .paletteNum = 0,
    .affineParam = 0,
};
static const union AnimCmd sSpriteAnim_StatusPoison[] = {
    ANIMCMD_FRAME(0, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusParalyzed[] = {
    ANIMCMD_FRAME(4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusSleep[] = {
    ANIMCMD_FRAME(8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusFrozen[] = {
    ANIMCMD_FRAME(12, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusBurn[] = {
    ANIMCMD_FRAME(16, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusPokerus[] = {
    ANIMCMD_FRAME(20, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusFaint[] = {
    ANIMCMD_FRAME(24, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd *const sSpriteAnimTable_StatusCondition[] = {
    sSpriteAnim_StatusPoison,
    sSpriteAnim_StatusParalyzed,
    sSpriteAnim_StatusSleep,
    sSpriteAnim_StatusFrozen,
    sSpriteAnim_StatusBurn,
    sSpriteAnim_StatusPokerus,
    sSpriteAnim_StatusFaint,
};
static const struct CompressedSpriteSheet sStatusIconsSpriteSheet =
{
    .data = gStatusGfx_Icons,
    .size = 0x380,
    .tag = TAG_MON_STATUS
};
static const struct CompressedSpritePalette sStatusIconsSpritePalette =
{
    .data = gStatusPal_Icons,
    .tag = TAG_MON_STATUS
};
static const struct SpriteTemplate sSpriteTemplate_StatusCondition =
{
    .tileTag = TAG_MON_STATUS,
    .paletteTag = TAG_MON_STATUS,
    .oam = &sOamData_StatusCondition,
    .anims = sSpriteAnimTable_StatusCondition,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

// code
void ShowPokemonSummaryScreen(u8 mode, void *mons, u8 monIndex, u8 maxMonIndex, void (*callback)(void))
{
    sMonSummaryScreen = AllocZeroed(sizeof(*sMonSummaryScreen));
    sMonSummaryScreen->mode = mode;
    sMonSummaryScreen->monList.mons = mons;
    sMonSummaryScreen->curMonIndex = monIndex;
    sMonSummaryScreen->maxMonIndex = maxMonIndex;
    sMonSummaryScreen->callback = callback;

    if (mode == PSS_MODE_BOX)
        sMonSummaryScreen->isBoxMon = TRUE;
    else
        sMonSummaryScreen->isBoxMon = FALSE;

    switch (mode)
    {
    case PSS_MODE_NORMAL:
    case PSS_MODE_BOX:
        sMonSummaryScreen->minPageIndex = 0;
        sMonSummaryScreen->maxPageIndex = PSS_PAGE_COUNT - 1;
        break;
    case PSS_MODE_LOCK_MOVES:
        sMonSummaryScreen->minPageIndex = 0;
        sMonSummaryScreen->maxPageIndex = PSS_PAGE_COUNT - 1;
        sMonSummaryScreen->lockMovesFlag = TRUE;
        break;
    case PSS_MODE_SELECT_MOVE:
        sMonSummaryScreen->minPageIndex = PSS_PAGE_BATTLE_MOVES;
        sMonSummaryScreen->maxPageIndex = PSS_PAGE_BATTLE_MOVES;
        sMonSummaryScreen->lockMonFlag = TRUE;
        break;
    }

    memset(sMonSummaryScreen->iconSpriteIds, 0xFF, sizeof(sMonSummaryScreen->iconSpriteIds));
    sMonSummaryScreen->currPageIndex = sMonSummaryScreen->minPageIndex;
    SummaryScreen_SetUnknownTaskId(-1);

    if (gMonSpritesGfxPtr == NULL)
        sub_806F2AC(0, 0);

    SetMainCallback2(CB2_InitSummaryScreen);
}

void ShowSelectMovePokemonSummaryScreen(struct Pokemon *mons, u8 monIndex, u8 maxMonIndex, void (*callback)(void), u16 newMove)
{
    ShowPokemonSummaryScreen(PSS_MODE_SELECT_MOVE, mons, monIndex, maxMonIndex, callback);
    sMonSummaryScreen->newMove = newMove;
}

void ShowPokemonSummaryScreenSet40EF(u8 mode, struct BoxPokemon *mons, u8 monIndex, u8 maxMonIndex, void (*callback)(void))
{
    ShowPokemonSummaryScreen(mode, mons, monIndex, maxMonIndex, callback);
    sMonSummaryScreen->unk40EF = TRUE;
}

static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    do_scheduled_bg_tilemap_copies_to_vram();
    UpdatePaletteFade();
}

static void VBlank(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void CB2_InitSummaryScreen(void)
{
    while (sub_81221EC() != TRUE && LoadGraphics() != TRUE && sub_81221AC() != TRUE);
}

static bool8 LoadGraphics(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankHBlankCallbacksToNull();
        ResetVramOamAndBgCntRegs();
        clear_scheduled_bg_copies_to_vram();
        gMain.state++;
        break;
    case 1:
        ScanlineEffect_Stop();
        gMain.state++;
        break;
    case 2:
        ResetPaletteFade();
        gPaletteFade.bufferTransferDisabled = 1;
        gMain.state++;
        break;
    case 3:
        ResetSpriteData();
        gMain.state++;
        break;
    case 4:
        FreeAllSpritePalettes();
        gMain.state++;
        break;
    case 5:
        InitBGs();
        sMonSummaryScreen->switchCounter = 0;
        gMain.state++;
        break;
    case 6:
        if (DecompressGraphics() != FALSE)
            gMain.state++;
        break;
    case 7:
        ResetWindows();
        gMain.state++;
        break;
    case 8:
        DrawPagination();
        gMain.state++;
        break;
    case 9:
        CopyMonToSummaryStruct(&sMonSummaryScreen->currentMon);
        sMonSummaryScreen->switchCounter = 0;
        gMain.state++;
        break;
    case 10:
        if (ExtractMonDataToSummaryStruct(&sMonSummaryScreen->currentMon, FALSE) != 0)
            gMain.state++;
        break;
    case 11:
        PrintMonInfo();
        gMain.state++;
        break;
    case 12:
        PrintPageNamesAndStats();
        gMain.state++;
        break;
    case 13:
        PrintPageSpecificText(sMonSummaryScreen->currPageIndex, FALSE);
        gMain.state++;
        break;
    case 14:
        if (sMonSummaryScreen->currPageIndex == PSS_PAGE_BATTLE_MOVES)
        {
            SetBgTilemapBuffer(2, sMonSummaryScreen->bgTilemapBuffers[PSS_PAGE_BATTLE_MOVES][0]);
            ChangeBgX(2, 0x10000, 1);
        }
        LimitEggSummaryPageDisplay();
        DrawPokerusCuredSymbol(&sMonSummaryScreen->currentMon);
        gMain.state++;
        break;
    case 15:
        PutPageWindowsAndSprites(sMonSummaryScreen->currPageIndex);
        gMain.state++;
        break;
    case 16:
        ResetSpriteIds();
        CreateMoveTypeIcons();
        CreateNewInterfaceIcons();
        sMonSummaryScreen->switchCounter = 0;
        gMain.state++;
        break;
    case 17:
        sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON] = LoadMonGfxAndSprite(&sMonSummaryScreen->currentMon, &sMonSummaryScreen->switchCounter);
        if (sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON] != 0xFF)
        {
            sMonSummaryScreen->switchCounter = 0;
            gMain.state++;
        }
        break;
    case 18:
        gMain.state++;
        break;
    case 19:
        CreateCaughtBallSprite(&sMonSummaryScreen->currentMon);
        gMain.state++;
        break;
    case 20:
        CreateSetStatusSprite();
        gMain.state++;
        break;
    case 21:
        SetTypeIcons();
        gMain.state++;
        break;
    case 22:
        if (sMonSummaryScreen->mode != PSS_MODE_SELECT_MOVE)
            CreateTask(Task_HandleInput, 0);
        else
            CreateTask(Task_SetHandleReplaceMoveInput, 0);
        gMain.state++;
        break;
    case 23:
        BlendPalettes(0xFFFFFFFF, 16, 0);
        gMain.state++;
        break;
    case 24:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB_BLACK);
        gPaletteFade.bufferTransferDisabled = 0;
        gMain.state++;
        break;
    default:
        SetVBlankCallback(VBlank);
        SetMainCallback2(MainCB2);
        return TRUE;
    }
    return FALSE;
}

static void InitBGs(void)
{
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sBgTemplates, ARRAY_COUNT(sBgTemplates));
    SetBgTilemapBuffer(1, sMonSummaryScreen->bgTilemapBuffers[PSS_PAGE_BATTLE_MOVES][0]);
    SetBgTilemapBuffer(2, sMonSummaryScreen->bgTilemapBuffers[PSS_PAGE_SKILLS][0]);
    SetBgTilemapBuffer(3, sMonSummaryScreen->bgTilemapBuffers[PSS_PAGE_INFO][0]);
    ResetAllBgsCoordinates();
    schedule_bg_copy_tilemap_to_vram(1);
    schedule_bg_copy_tilemap_to_vram(2);
    schedule_bg_copy_tilemap_to_vram(3);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    ShowBg(0);
    ShowBg(1);
    ShowBg(2);
    ShowBg(3);
}

static bool8 DecompressGraphics(void)
{
    switch (sMonSummaryScreen->switchCounter)
    {
    case 0:
        reset_temp_tile_data_buffers();
        decompress_and_copy_tile_data_to_vram(1, &gStatusScreenBitmap, 0, 0, 0);
        sMonSummaryScreen->switchCounter++;
        break;
    case 1:
        if (free_temp_tile_data_buffers_if_possible() != 1)
        {
            LZDecompressWram(gPageInfoTilemap, sMonSummaryScreen->bgTilemapBuffers[PSS_PAGE_INFO][0]);
            sMonSummaryScreen->switchCounter++;
        }
        break;
    case 2:
        LZDecompressWram(gSummaryScreen_Info_Page, sMonSummaryScreen->bgTilemapBuffers[PSS_PAGE_INFO][1]);
        sMonSummaryScreen->switchCounter++;
        break;
    case 3:
        LZDecompressWram(gPageSkillsTilemap, sMonSummaryScreen->bgTilemapBuffers[PSS_PAGE_SKILLS][1]);
        sMonSummaryScreen->switchCounter++;
        break;
    case 4:
        LZDecompressWram(gPageBattleMovesTilemap, sMonSummaryScreen->bgTilemapBuffers[PSS_PAGE_BATTLE_MOVES][1]);
        sMonSummaryScreen->switchCounter++;
        break;
    case 5:
        LZDecompressWram(gPageIvEvTilemap, sMonSummaryScreen->bgTilemapBuffers[PSS_PAGE_IV_EVS][1]);
        sMonSummaryScreen->switchCounter++;
        break;
    case 6:
        LoadCompressedPalette(gStatusScreenPalette, 0, 0x100);
        LoadPalette(&gUnknown_08D85620, 0x81, 0x1E);
        sMonSummaryScreen->switchCounter++;
        break;
    case 7:
        LoadCompressedPalette(gMoveTypes_Pal, 0x1D0, 0x60);
        LoadCompressedSpritePalette(&sStatusIconsSpritePalette);
        LoadSpritePalette(&sSpritePalette_SummaryIcons);
        LoadSpritePalette(&gSpritePalette_LaserGrid);
        LoadSpritePalette(&sSpritePalette_SplitIcons);
        LoadCompressedSpriteSheet(&sSpriteSheet_MoveTypes);
        LoadCompressedSpriteSheet(&sStatusIconsSpriteSheet);
        LoadCompressedSpriteSheet(&gSpriteSheet_LaserGrid1);
        LoadCompressedSpriteSheet(&gSpriteSheet_LaserGrid2);
        LoadCompressedSpriteSheet(&gSpriteSheet_LaserGrid3);
        LoadCompressedSpriteSheet(&gSpriteSheet_LaserGrid4);
        LoadCompressedSpriteSheet(&sSpriteSheet_SummaryIcons);
        LoadCompressedSpriteSheet(&sSpriteSheet_NatureIcons);
        LoadCompressedSpriteSheet(&sSpriteSheet_SplitIcons);
        LoadCompressedSpriteSheet(&sSpriteSheet_MoveSelectorBlue);
        LoadCompressedSpriteSheet(&sSpriteSheet_MoveSelectorRed);
        sMonSummaryScreen->switchCounter = 0;
        return TRUE;
    }
    return FALSE;
}

static void CopyMonToSummaryStruct(struct Pokemon *mon)
{
    if (!sMonSummaryScreen->isBoxMon)
    {
        struct Pokemon *partyMon = sMonSummaryScreen->monList.mons;
        *mon = partyMon[sMonSummaryScreen->curMonIndex];
    }
    else
    {
        struct BoxPokemon *boxMon = sMonSummaryScreen->monList.boxMons;
        BoxMonToMon(&boxMon[sMonSummaryScreen->curMonIndex], mon);
    }
}

static bool8 ExtractMonDataToSummaryStruct(struct Pokemon *mon, bool32 atOnce)
{
    u32 i, total;
    struct PokeSummary *sum = &sMonSummaryScreen->summary;
    // Spread the data extraction over multiple frames.
    switch (sMonSummaryScreen->switchCounter)
    {
    case 0:
        sum->species = GetMonData(mon, MON_DATA_SPECIES);
        sum->species2 = GetMonData(mon, MON_DATA_SPECIES2);
        sum->exp = GetMonData(mon, MON_DATA_EXP);
        sum->level = GetMonData(mon, MON_DATA_LEVEL);
        sum->abilityNum = GetMonData(mon, MON_DATA_ABILITY_NUM);
        sum->item = GetMonData(mon, MON_DATA_HELD_ITEM);
        sum->pid = GetMonData(mon, MON_DATA_PERSONALITY);
        sum->sanity = GetMonData(mon, MON_DATA_SANITY_IS_BAD_EGG);

        if (sum->sanity)
            sum->isEgg = TRUE;
        else
            sum->isEgg = GetMonData(mon, MON_DATA_IS_EGG);

        if (!atOnce)
            break;
    case 1:
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            sum->moves[i] = GetMonData(mon, MON_DATA_MOVE1+i);
            sum->pp[i] = GetMonData(mon, MON_DATA_PP1+i);
        }
        for (i = 0, total = 0; i < NUM_STATS; i++)
        {
            sum->evs[i] = GetMonData(mon, MON_DATA_HP_EV + i);
            total += sum->evs[i];
            sum->ivs[i] = GetMonData(mon, MON_DATA_HP_IV + i);
        }
        sum->evs[i] = total;
        sum->ppBonuses = GetMonData(mon, MON_DATA_PP_BONUSES);
        if (!atOnce)
            break;
    case 2:
        sum->nature = GetNature(mon);
        sum->currentHP = GetMonData(mon, MON_DATA_HP);
        sum->maxHP = GetMonData(mon, MON_DATA_MAX_HP);
        if (sMonSummaryScreen->monList.mons == gPlayerParty || sMonSummaryScreen->mode == PSS_MODE_BOX || sMonSummaryScreen->unk40EF == TRUE)
        {
            for (i = 0; i < NUM_STATS - 1; i++)
                sum->stats[i] = GetMonData(mon, MON_DATA_ATK + i);
        }
        else
        {
            for (i = 0; i < NUM_STATS - 1; i++)
                sum->stats[i] = GetMonData(mon, MON_DATA_ATK2 + i);
        }
        if (!atOnce)
            break;
    case 3:
        GetMonData(mon, MON_DATA_OT_NAME, sum->OTName);
        ConvertInternationalString(sum->OTName, GetMonData(mon, MON_DATA_LANGUAGE));
        GetMonData(mon, MON_DATA_NICKNAME, sum->nickname);
        sum->ailment = GetMonAilment(mon);
        sum->OTGender = GetMonData(mon, MON_DATA_OT_GENDER);
        sum->OTID = GetMonData(mon, MON_DATA_OT_ID);
        sum->metLocation = GetMonData(mon, MON_DATA_MET_LOCATION);
        sum->metLevel = GetMonData(mon, MON_DATA_MET_LEVEL);
        sum->metGame = GetMonData(mon, MON_DATA_MET_GAME);
        sum->friendship = GetMonData(mon, MON_DATA_FRIENDSHIP);
        if (!atOnce)
            break;
    default:
        sum->ribbonCount = GetMonData(mon, MON_DATA_RIBBON_COUNT);
        return TRUE;
    }
    sMonSummaryScreen->switchCounter++;
    return FALSE;
}

static void FreeSummaryScreen(void)
{
    FreeAllWindowBuffers();
    Free(sMonSummaryScreen);
}

static void BeginCloseSummaryScreen(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
    gTasks[taskId].func = CloseSummaryScreen;
}

static void CloseSummaryScreen(u8 taskId)
{
    if (sub_81221EC() != TRUE && !gPaletteFade.active)
    {
        SetMainCallback2(sMonSummaryScreen->callback);
        gLastViewedMonIndex = sMonSummaryScreen->curMonIndex;
        SummaryScreen_DestroyUnknownTask();
        ResetSpriteData();
        FreeAllSpritePalettes();
        StopCryAndClearCrySongs();
        m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 0x100);
        if (gMonSpritesGfxPtr == NULL)
            sub_806F47C(0);
        FreeSummaryScreen();
        DestroyTask(taskId);
    }
}

static void Task_HandleInput(u8 taskId)
{
    if (sub_81221EC() != TRUE && !gPaletteFade.active)
    {
        if (gMain.newKeys & DPAD_UP)
        {
            ChangeSummaryPokemon(taskId, -1);
        }
        else if (gMain.newKeys & DPAD_DOWN)
        {
            ChangeSummaryPokemon(taskId, 1);
        }
        else if ((gMain.newKeys & DPAD_LEFT) || GetLRKeysPressed() == MENU_L_PRESSED)
        {
            ChangePage(taskId, -1);
        }
        else if ((gMain.newKeys & DPAD_RIGHT) || GetLRKeysPressed() == MENU_R_PRESSED)
        {
            ChangePage(taskId, 1);
        }
        else if (gMain.newKeys & A_BUTTON)
        {
            switch (sMonSummaryScreen->currPageIndex)
            {
            case PSS_PAGE_INFO:
                StopPokemonAnimations();
                PlaySE(SE_SELECT);
                BeginCloseSummaryScreen(taskId);
                break;
            case PSS_PAGE_BATTLE_MOVES:
                PlaySE(SE_SELECT);
                SwitchToMoveSelection(taskId, Task_HandleInput_MoveSelect);
                break;
            }
        }
        else if (gMain.newKeys & SELECT_BUTTON && sMonSummaryScreen->currPageIndex == PSS_PAGE_BATTLE_MOVES)
        {
            SwitchToFromContestView(0);
        }
        else if (gMain.newKeys & B_BUTTON)
        {
            StopPokemonAnimations();
            PlaySE(SE_SELECT);
            BeginCloseSummaryScreen(taskId);
        }
    }
}

static void ChangeSummaryPokemon(u8 taskId, s8 delta)
{
    s8 monId;

    if (!sMonSummaryScreen->lockMonFlag)
    {
        if (sMonSummaryScreen->isBoxMon == TRUE)
        {
            if (sMonSummaryScreen->currPageIndex != PSS_PAGE_INFO)
            {
                if (delta == 1)
                    delta = 0;
                else
                    delta = 2;
            }
            else
            {
                if (delta == 1)
                    delta = 1;
                else
                    delta = 3;
            }
            monId = sub_80D214C(sMonSummaryScreen->monList.boxMons, sMonSummaryScreen->curMonIndex, sMonSummaryScreen->maxMonIndex, delta);
        }
        else if (IsMultiBattle() == TRUE)
        {
            monId = AdvanceMultiBattleMonIndex(delta);
        }
        else
        {
            monId = AdvanceMonIndex(delta);
        }

        if (monId != -1)
        {
            PlaySE(SE_SELECT);
            if (sMonSummaryScreen->summary.ailment != AILMENT_NONE)
            {
                SetSpriteInvisibility(SPRITE_ARR_ID_STATUS, TRUE);
            }
            sMonSummaryScreen->curMonIndex = monId;
            gTasks[taskId].data[0] = 0;
            gTasks[taskId].func = Task_ChangeSummaryMon;
        }
    }
}

static void Task_ChangeSummaryMon(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (data[0])
    {
    case 0:
        StopCryAndClearCrySongs();
        break;
    case 1:
        SummaryScreen_DestroyUnknownTask();
        DestroySpriteAndFreeResources(&gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]]);
        break;
    case 2:
        CopyMonToSummaryStruct(&sMonSummaryScreen->currentMon);
        sMonSummaryScreen->switchCounter = 0;
        ExtractMonDataToSummaryStruct(&sMonSummaryScreen->currentMon, TRUE);
        // Avoid ball display lag.
        if (ItemIdToBallId(GetMonData(&sMonSummaryScreen->currentMon, MON_DATA_POKEBALL)) !=
            gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_BALL]].data[0])
        {
            DestroySpriteAndFreeResources(&gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_BALL]]);
            CreateCaughtBallSprite(&sMonSummaryScreen->currentMon);
        }
        break;
    case 3:
        DrawPokerusCuredSymbol(&sMonSummaryScreen->currentMon);
        data[1] = 0;
        break;
    case 4:
        sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON] = LoadMonGfxAndSprite(&sMonSummaryScreen->currentMon, &data[1]);
        if (sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON] == 0xFF)
            return;
        gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].data[2] = 1;
        TryDrawBars();
        data[1] = 0;
        break;
    case 5:
        SetTypeIcons();
        PrintMonInfo();
        break;
    case 6:
        PrintPageSpecificText(sMonSummaryScreen->currPageIndex, TRUE);
        LimitEggSummaryPageDisplay();
        break;
    case 7:
        gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].data[2] = 0;
        break;
    default:
        if (sub_81221EC() == 0)
        {
            data[0] = 0;
            gTasks[taskId].func = Task_HandleInput;
        }
        return;
    }
    data[0]++;
}

static s8 AdvanceMonIndex(s8 delta)
{
    struct Pokemon *mon = sMonSummaryScreen->monList.mons;

    if (sMonSummaryScreen->currPageIndex == PSS_PAGE_INFO)
    {
        if (delta == -1 && sMonSummaryScreen->curMonIndex == 0)
            return -1;
        else if (delta == 1 && sMonSummaryScreen->curMonIndex >= sMonSummaryScreen->maxMonIndex)
            return -1;
        else
            return sMonSummaryScreen->curMonIndex + delta;
    }
    else
    {
        s8 index = sMonSummaryScreen->curMonIndex;

        do
        {
            index += delta;
            if (index < 0 || index > sMonSummaryScreen->maxMonIndex)
                return -1;
        } while (GetMonData(&mon[index], MON_DATA_IS_EGG));
        return index;
    }
}

static s8 AdvanceMultiBattleMonIndex(s8 delta)
{
    struct Pokemon *mons = sMonSummaryScreen->monList.mons;
    s8 index, arrId = 0;
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (sMultiBattleOrder[i] == sMonSummaryScreen->curMonIndex)
        {
            arrId = i;
            break;
        }
    }

    while (TRUE)
    {
        const s8 *order = sMultiBattleOrder;

        arrId += delta;
        if (arrId < 0 || arrId >= PARTY_SIZE)
            return -1;
        index = order[arrId];
        if (IsValidToViewInMulti(&mons[index]) == TRUE)
            return index;
    }
}

static bool8 IsValidToViewInMulti(struct Pokemon* mon)
{
    if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_NONE)
        return FALSE;
    else if (sMonSummaryScreen->curMonIndex != 0 || !GetMonData(mon, MON_DATA_IS_EGG))
        return TRUE;
    else
        return FALSE;
}

static void ChangePage(u8 taskId, s8 delta)
{
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    s16 *data = gTasks[taskId].data;

    if (summary->isEgg)
        return;
    else if (delta == -1 && sMonSummaryScreen->currPageIndex == sMonSummaryScreen->minPageIndex)
        return;
    else if (delta == 1 && sMonSummaryScreen->currPageIndex == sMonSummaryScreen->maxPageIndex)
        return;

    PlaySE(SE_SELECT);
    ClearPageWindowsAndSprites(sMonSummaryScreen->currPageIndex);
    sMonSummaryScreen->currPageIndex += delta;
    data[0] = 0;
    if (delta == 1)
        SetTaskFuncWithFollowupFunc(taskId, PssScrollRight, gTasks[taskId].func);
    else
        SetTaskFuncWithFollowupFunc(taskId, PssScrollLeft, gTasks[taskId].func);
    CreateTextPrinterTask(sMonSummaryScreen->currPageIndex);
    HidePageSpecificSprites();
}

static void PssScrollRight(u8 taskId) // Scroll right
{
    s16 *data = gTasks[taskId].data;
    if (data[0] == 0)
    {
        if (sMonSummaryScreen->bgDisplayOrder == 0)
        {
            data[1] = 1;
            SetBgAttribute(1, BG_ATTR_PRIORITY, 1);
            SetBgAttribute(2, BG_ATTR_PRIORITY, 2);
            schedule_bg_copy_tilemap_to_vram(1);
        }
        else
        {
            data[1] = 2;
            SetBgAttribute(2, BG_ATTR_PRIORITY, 1);
            SetBgAttribute(1, BG_ATTR_PRIORITY, 2);
            schedule_bg_copy_tilemap_to_vram(2);
        }
        ChangeBgX(data[1], 0, 0);
        SetBgTilemapBuffer(data[1], sMonSummaryScreen->bgTilemapBuffers[sMonSummaryScreen->currPageIndex][0]);
        ShowBg(1);
        ShowBg(2);
        data[0]++;
    }
    else
    {
        ChangeBgX(data[1], 0x10000, 0);
        gTasks[taskId].func = PssScrollRightEnd;
    }
}

static void PssScrollRightEnd(u8 taskId) // display right
{
    s16 *data = gTasks[taskId].data;
    sMonSummaryScreen->bgDisplayOrder ^= 1;
    data[1] = 0;
    data[0] = 0;
    DrawPagination();
    PutPageWindowsAndSprites(sMonSummaryScreen->currPageIndex);
    SetTypeIcons();
    TryDrawBars();
    SwitchTaskToFollowupFunc(taskId);
}

static void PssScrollLeft(u8 taskId) // Scroll left
{
    s16 *data = gTasks[taskId].data;
    if (data[0] == 0)
    {
        if (sMonSummaryScreen->bgDisplayOrder == 0)
            data[1] = 2;
        else
            data[1] = 1;
        ChangeBgX(data[1], 0x10000, 0);
        data[0]++;
    }
    else
    {
        ChangeBgX(data[1], 0, 0);
        gTasks[taskId].func = PssScrollLeftEnd;
    }
}

static void PssScrollLeftEnd(u8 taskId) // display left
{
    s16 *data = gTasks[taskId].data;
    if (sMonSummaryScreen->bgDisplayOrder == 0)
    {
        SetBgAttribute(1, BG_ATTR_PRIORITY, 1);
        SetBgAttribute(2, BG_ATTR_PRIORITY, 2);
        schedule_bg_copy_tilemap_to_vram(2);
    }
    else
    {
        SetBgAttribute(2, BG_ATTR_PRIORITY, 1);
        SetBgAttribute(1, BG_ATTR_PRIORITY, 2);
        schedule_bg_copy_tilemap_to_vram(1);
    }
    if (sMonSummaryScreen->currPageIndex > 1)
    {
        SetBgTilemapBuffer(data[1], sMonSummaryScreen->bgTilemapBuffers[sMonSummaryScreen->currPageIndex - 1][0]);
        ChangeBgX(data[1], 0x10000, 0);
    }
    ShowBg(1);
    ShowBg(2);
    sMonSummaryScreen->bgDisplayOrder ^= 1;
    data[1] = 0;
    data[0] = 0;
    DrawPagination();
    PutPageWindowsAndSprites(sMonSummaryScreen->currPageIndex);
    SetTypeIcons();
    TryDrawBars();
    SwitchTaskToFollowupFunc(taskId);
}

static void TryDrawBars(void)
{
    if (sMonSummaryScreen->currPageIndex == PSS_PAGE_SKILLS)
    {
        DrawExperienceProgressBar();
        DrawHpBar();
    }
}

static void SwitchToFromContestView(u32 moveIndex)
{
    sMonSummaryScreen->contestMode ^= 1;
    FillWindowPixelBuffer(PSS_LABEL_WINDOW_BATTLE_MOVES_TITLE, PIXEL_FILL(0));
    PrintTextOnWindow(PSS_LABEL_WINDOW_BATTLE_MOVES_TITLE, sMonSummaryScreen->contestMode ? gText_ContestMoves : gText_BattleMoves, 2, 0, 0, 3);
    if (sMonSummaryScreen->moveSelectionMode)
    {
        PrintMoveDetails((moveIndex >= MAX_MON_MOVES) ? 0 : sMonSummaryScreen->summary.moves[moveIndex]);
    }
    if (sMonSummaryScreen->contestMode)
        SetContestMoveTypeIcons();
    else
        SetMoveTypeIcons();
}

static void SwitchToMoveSelection(u8 taskId, TaskFunc func)
{
    u32 move, i;

    sMonSummaryScreen->firstMoveIndex = 0;
    sMonSummaryScreen->moveSelectionMode = TRUE;
    move = sMonSummaryScreen->summary.moves[sMonSummaryScreen->firstMoveIndex];
    if (!sMonSummaryScreen->lockMovesFlag)
    {
        ClearWindowTilemap(PSS_LABEL_WINDOW_PROMPT_INFO);
        if (sMonSummaryScreen->mode != PSS_MODE_SELECT_MOVE)
            PutWindowTilemap(PSS_LABEL_WINDOW_PROMPT_SWITCH);
    }

    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].invisible = TRUE;
    for (i = 2; i < 5; i++)
        gSprites[sMonSummaryScreen->laserSpriteIds[i]].invisible = TRUE;
    SetSummaryIconInvisibility(SUMMARY_ICON_ACCURACY, 0, FALSE);
    SetSummaryIconInvisibility(SUMMARY_ICON_POWER, 0, FALSE);
    SetSummaryIconInvisibility(SUMMARY_ICON_EFFECT, 0, FALSE);
    ClearWindowTilemap(PSS_LABEL_WINDOW_PORTRAIT_LVL);

    DetailedMovesTilemapDisplay(sMonSummaryScreen->bgTilemapBuffers[PSS_PAGE_BATTLE_MOVES][0], 3, FALSE);
    PrintMoveDetails(move);
    if (sMonSummaryScreen->mode != PSS_MODE_SELECT_MOVE)
        PrintMoveCancel(FALSE);
    SetNewMoveTypeIcon();
    SetMonTypeIcons();

    LoadMonIconPalette(sMonSummaryScreen->summary.species);
    sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON_ICON] = CreateMonIcon(sMonSummaryScreen->summary.species,
                                                                         SpriteCallbackDummy, 148, 34, 0,
                                                                         sMonSummaryScreen->summary.pid, FALSE);
    schedule_bg_copy_tilemap_to_vram(0);
    schedule_bg_copy_tilemap_to_vram(1);
    schedule_bg_copy_tilemap_to_vram(2);
    CreateMoveSelectorSprites(SPRITE_ARR_ID_MOVE_SELECTOR1);
    gTasks[taskId].func = func;
}

static void Task_HandleInput_MoveSelect(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (sub_81221EC() != 1)
    {
        if (gMain.newKeys & DPAD_UP)
        {
            data[0] = 4;
            ChangeSelectedMove(data, -1, &sMonSummaryScreen->firstMoveIndex);
        }
        else if (gMain.newKeys & DPAD_DOWN)
        {
            data[0] = 4;
            ChangeSelectedMove(data, 1, &sMonSummaryScreen->firstMoveIndex);
        }
        else if (gMain.newKeys & A_BUTTON)
        {
            if (sMonSummaryScreen->lockMovesFlag == TRUE
             || (sMonSummaryScreen->newMove == MOVE_NONE && sMonSummaryScreen->firstMoveIndex == MAX_MON_MOVES))
            {
                PlaySE(SE_SELECT);
                CloseMoveSelectMode(taskId);
            }
            else if (HasMoreThanOneMove() == TRUE)
            {
                PlaySE(SE_SELECT);
                SwitchToMovePositionSwitchMode(taskId);
            }
            else
            {
                PlaySE(SE_HAZURE);
            }
        }
        else if (gMain.newKeys & B_BUTTON)
        {
            PlaySE(SE_SELECT);
            CloseMoveSelectMode(taskId);
        }
        else if (gMain.newKeys & SELECT_BUTTON)
        {
            SwitchToFromContestView(sMonSummaryScreen->firstMoveIndex);
        }
    }
}

static bool8 HasMoreThanOneMove(void)
{
    u8 i;
    for (i = 1; i < MAX_MON_MOVES; i++)
    {
        if (sMonSummaryScreen->summary.moves[i] != 0)
            return TRUE;
    }
    return FALSE;
}

static void ChangeSelectedMove(s16 *taskData, s8 direction, u8 *moveIndexPtr)
{
    s8 i, newMoveIndex;
    u16 move;

    PlaySE(SE_SELECT);
    newMoveIndex = *moveIndexPtr;
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        newMoveIndex += direction;
        if (newMoveIndex > taskData[0])
            newMoveIndex = 0;
        else if (newMoveIndex < 0)
            newMoveIndex = taskData[0];

        if (newMoveIndex == MAX_MON_MOVES)
        {
            move = sMonSummaryScreen->newMove;
            break;
        }
        move = sMonSummaryScreen->summary.moves[newMoveIndex];
        if (move != 0)
            break;
    }
    schedule_bg_copy_tilemap_to_vram(1);
    schedule_bg_copy_tilemap_to_vram(2);
    PrintMoveDetails(move);

    *moveIndexPtr = newMoveIndex;
    // Not sure what the purpose of this function is, seems to have no effect whatsoever.
    if (moveIndexPtr == &sMonSummaryScreen->firstMoveIndex)
        MakeMoveSelectorVisible(SPRITE_ARR_ID_MOVE_SELECTOR1);
    else
        MakeMoveSelectorVisible(SPRITE_ARR_ID_MOVE_SELECTOR2);
}

static void CloseMoveSelectMode(u8 taskId)
{
    u32 i;

    sMonSummaryScreen->moveSelectionMode = FALSE;
    DestroyMoveSelectorSprites(SPRITE_ARR_ID_MOVE_SELECTOR1);
    ClearWindowTilemap(PSS_LABEL_WINDOW_PROMPT_SWITCH);
    PutWindowTilemap(PSS_LABEL_WINDOW_PROMPT_INFO);
    PrintMoveDetails(0);
    PrintMoveCancel(TRUE);
    DetailedMovesTilemapDisplay(sMonSummaryScreen->bgTilemapBuffers[PSS_PAGE_BATTLE_MOVES][0], 3, TRUE);

    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].invisible = FALSE;
    SetSummaryIconInvisibility(SUMMARY_ICON_ACCURACY, 0, TRUE);
    SetSummaryIconInvisibility(SUMMARY_ICON_POWER, 0, TRUE);
    SetSummaryIconInvisibility(SUMMARY_ICON_EFFECT, 0, TRUE);
    for (i = 2; i < 5; i++)
        gSprites[sMonSummaryScreen->laserSpriteIds[i]].invisible = FALSE;
    FreeMonIconPalette(sMonSummaryScreen->summary.species);
    FreeAndDestroyMonIconSprite(&gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON_ICON]]);
    SetSpriteInvisibility(SPRITE_ARR_ID_TYPE_MON1, TRUE);
    SetSpriteInvisibility(SPRITE_ARR_ID_TYPE_MON2, TRUE);

    PutWindowTilemap(PSS_LABEL_WINDOW_PORTRAIT_LVL);
    schedule_bg_copy_tilemap_to_vram(0);
    schedule_bg_copy_tilemap_to_vram(1);
    schedule_bg_copy_tilemap_to_vram(2);
    gTasks[taskId].func = Task_HandleInput;
}

static void SwitchToMovePositionSwitchMode(u8 taskId)
{
    sMonSummaryScreen->secondMoveIndex = sMonSummaryScreen->firstMoveIndex;
    SwapMoveSelectors();
    CreateMoveSelectorSprites(SPRITE_ARR_ID_MOVE_SELECTOR2);
    gTasks[taskId].func = Task_HandleInput_MovePositionSwitch;
}

static void Task_HandleInput_MovePositionSwitch(u8 taskId)
{
    s16* data = gTasks[taskId].data;

    if (sub_81221EC() != TRUE)
    {
        if (gMain.newKeys & DPAD_UP)
        {
            data[0] = 3;
            ChangeSelectedMove(&data[0], -1, &sMonSummaryScreen->secondMoveIndex);
        }
        else if (gMain.newKeys & DPAD_DOWN)
        {
            data[0] = 3;
            ChangeSelectedMove(&data[0], 1, &sMonSummaryScreen->secondMoveIndex);
        }
        else if (gMain.newKeys & A_BUTTON)
        {
            if (sMonSummaryScreen->firstMoveIndex == sMonSummaryScreen->secondMoveIndex)
                ExitMovePositionSwitchMode(taskId, FALSE);
            else
                ExitMovePositionSwitchMode(taskId, TRUE);
        }
        else if (gMain.newKeys & B_BUTTON)
        {
            ExitMovePositionSwitchMode(taskId, FALSE);
        }
        else if (gMain.newKeys & SELECT_BUTTON)
        {
            SwitchToFromContestView(sMonSummaryScreen->secondMoveIndex);
        }
    }
}

static void ExitMovePositionSwitchMode(u8 taskId, bool8 swapMoves)
{
    PlaySE(SE_SELECT);
    SwapMoveSelectors();
    DestroyMoveSelectorSprites(SPRITE_ARR_ID_MOVE_SELECTOR1);

    if (swapMoves == TRUE && sMonSummaryScreen->firstMoveIndex != sMonSummaryScreen->secondMoveIndex)
    {
        if (!sMonSummaryScreen->isBoxMon)
        {
            struct Pokemon *mon = sMonSummaryScreen->monList.mons;
            SwapMonMoves(&mon[sMonSummaryScreen->curMonIndex], sMonSummaryScreen->firstMoveIndex, sMonSummaryScreen->secondMoveIndex);
        }
        else
        {
            struct BoxPokemon *boxMon = sMonSummaryScreen->monList.boxMons;
            SwapBoxMonMoves(&boxMon[sMonSummaryScreen->curMonIndex], sMonSummaryScreen->firstMoveIndex, sMonSummaryScreen->secondMoveIndex);
        }
        CopyMonToSummaryStruct(&sMonSummaryScreen->currentMon);
        SwapMovesNamesPP(sMonSummaryScreen->firstMoveIndex, sMonSummaryScreen->secondMoveIndex);
        SwapMovesTypeSprites(sMonSummaryScreen->firstMoveIndex, sMonSummaryScreen->secondMoveIndex);
        sMonSummaryScreen->firstMoveIndex = sMonSummaryScreen->secondMoveIndex;
        PrintMoveDetails(sMonSummaryScreen->summary.moves[sMonSummaryScreen->firstMoveIndex]);
    }

    schedule_bg_copy_tilemap_to_vram(1);
    schedule_bg_copy_tilemap_to_vram(2);
    gTasks[taskId].func = Task_HandleInput_MoveSelect;
}

static void SwapMonMoves(struct Pokemon *mon, u8 moveIndex1, u8 moveIndex2)
{
    struct PokeSummary* summary = &sMonSummaryScreen->summary;

    u16 move1 = summary->moves[moveIndex1];
    u16 move2 = summary->moves[moveIndex2];
    u8 move1pp = summary->pp[moveIndex1];
    u8 move2pp = summary->pp[moveIndex2];
    u8 ppBonuses = summary->ppBonuses;

    // Calculate PP bonuses
    u8 ppUpMask1 = gPPUpGetMask[moveIndex1];
    u8 ppBonusMove1 = (ppBonuses & ppUpMask1) >> (moveIndex1 * 2);
    u8 ppUpMask2 = gPPUpGetMask[moveIndex2];
    u8 ppBonusMove2 = (ppBonuses & ppUpMask2) >> (moveIndex2 * 2);
    ppBonuses &= ~ppUpMask1;
    ppBonuses &= ~ppUpMask2;
    ppBonuses |= (ppBonusMove1 << (moveIndex2 * 2)) + (ppBonusMove2 << (moveIndex1 * 2));

    // Swap the moves
    SetMonData(mon, MON_DATA_MOVE1 + moveIndex1, &move2);
    SetMonData(mon, MON_DATA_MOVE1 + moveIndex2, &move1);
    SetMonData(mon, MON_DATA_PP1 + moveIndex1, &move2pp);
    SetMonData(mon, MON_DATA_PP1 + moveIndex2, &move1pp);
    SetMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);

    summary->moves[moveIndex1] = move2;
    summary->moves[moveIndex2] = move1;

    summary->pp[moveIndex1] = move2pp;
    summary->pp[moveIndex2] = move1pp;

    summary->ppBonuses = ppBonuses;
}

static void SwapBoxMonMoves(struct BoxPokemon *mon, u8 moveIndex1, u8 moveIndex2)
{
    struct PokeSummary* summary = &sMonSummaryScreen->summary;

    u16 move1 = summary->moves[moveIndex1];
    u16 move2 = summary->moves[moveIndex2];
    u8 move1pp = summary->pp[moveIndex1];
    u8 move2pp = summary->pp[moveIndex2];
    u8 ppBonuses = summary->ppBonuses;

    // Calculate PP bonuses
    u8 ppUpMask1 = gPPUpGetMask[moveIndex1];
    u8 ppBonusMove1 = (ppBonuses & ppUpMask1) >> (moveIndex1 * 2);
    u8 ppUpMask2 = gPPUpGetMask[moveIndex2];
    u8 ppBonusMove2 = (ppBonuses & ppUpMask2) >> (moveIndex2 * 2);
    ppBonuses &= ~ppUpMask1;
    ppBonuses &= ~ppUpMask2;
    ppBonuses |= (ppBonusMove1 << (moveIndex2 * 2)) + (ppBonusMove2 << (moveIndex1 * 2));

    // Swap the moves
    SetBoxMonData(mon, MON_DATA_MOVE1 + moveIndex1, &move2);
    SetBoxMonData(mon, MON_DATA_MOVE1 + moveIndex2, &move1);
    SetBoxMonData(mon, MON_DATA_PP1 + moveIndex1, &move2pp);
    SetBoxMonData(mon, MON_DATA_PP1 + moveIndex2, &move1pp);
    SetBoxMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);

    summary->moves[moveIndex1] = move2;
    summary->moves[moveIndex2] = move1;

    summary->pp[moveIndex1] = move2pp;
    summary->pp[moveIndex2] = move1pp;

    summary->ppBonuses = ppBonuses;
}

static void Task_SetHandleReplaceMoveInput(u8 taskId)
{
    SetNewMoveTypeIcon();
    CreateMoveSelectorSprites(SPRITE_ARR_ID_MOVE_SELECTOR1);
    PutPageWindowsAndSprites(PSS_PAGE_BATTLE_MOVES);
    SwitchToMoveSelection(taskId, Task_HandleReplaceMoveInput);
}

static void Task_HandleReplaceMoveInput(u8 taskId)
{
    s16* data = gTasks[taskId].data;

    if (sub_81221EC() != TRUE)
    {
        if (gPaletteFade.active != TRUE)
        {
            if (gMain.newKeys & DPAD_UP)
            {
                data[0] = 4;
                ChangeSelectedMove(data, -1, &sMonSummaryScreen->firstMoveIndex);
            }
            else if (gMain.newKeys & DPAD_DOWN)
            {
                data[0] = 4;
                ChangeSelectedMove(data, 1, &sMonSummaryScreen->firstMoveIndex);
            }
            else if (gMain.newKeys & DPAD_LEFT || GetLRKeysPressed() == MENU_L_PRESSED)
            {
                ChangePage(taskId, -1);
            }
            else if (gMain.newKeys & DPAD_RIGHT || GetLRKeysPressed() == MENU_R_PRESSED)
            {
                ChangePage(taskId, 1);
            }
            else if (gMain.newKeys & A_BUTTON)
            {
                if (CanReplaceMove() == TRUE)
                {
                    StopPokemonAnimations();
                    PlaySE(SE_SELECT);
                    sMoveSlotToReplace = sMonSummaryScreen->firstMoveIndex;
                    gSpecialVar_0x8005 = sMoveSlotToReplace;
                    BeginCloseSummaryScreen(taskId);
                }
                else
                {
                    PlaySE(SE_HAZURE);
                }
            }
            else if (gMain.newKeys & B_BUTTON)
            {
                StopPokemonAnimations();
                PlaySE(SE_SELECT);
                sMoveSlotToReplace = MAX_MON_MOVES;
                gSpecialVar_0x8005 = MAX_MON_MOVES;
                BeginCloseSummaryScreen(taskId);
            }
            else if (gMain.newKeys & SELECT_BUTTON)
            {
                SwitchToFromContestView(sMonSummaryScreen->firstMoveIndex);
            }
        }
    }
}

static bool8 CanReplaceMove(void)
{
    if (sMonSummaryScreen->firstMoveIndex == MAX_MON_MOVES
        || sMonSummaryScreen->newMove == MOVE_NONE)
        return TRUE;
    else
        return FALSE;
}

u8 GetMoveSlotToReplace(void)
{
    return sMoveSlotToReplace;
}

static void DrawPagination(void) // Updates the pagination dots at the top of the summary screen
{
    u16 *alloced = Alloc(32);
    u8 i;

    for (i = 0; i < 4; i++)
    {
        u8 j = i * 2;

        if (i < sMonSummaryScreen->minPageIndex)
        {
            alloced[j + 0] = 0x40;
            alloced[j + 1] = 0x40;
            alloced[j + 8] = 0x50;
            alloced[j + 9] = 0x50;
        }
        else if (i > sMonSummaryScreen->maxPageIndex)
        {
            alloced[j + 0] = 0x47;
            alloced[j + 1] = 0x47;
            alloced[j + 8] = 0x57;
            alloced[j + 9] = 0x57;
        }
        else if (i < sMonSummaryScreen->currPageIndex)
        {
            alloced[j + 0] = 0x42;
            alloced[j + 1] = 0x40;
            alloced[j + 8] = 0x52;
            alloced[j + 9] = 0x50;
        }
        else if (i == sMonSummaryScreen->currPageIndex)
        {
            if (i != sMonSummaryScreen->maxPageIndex)
            {
                alloced[j + 0] = 0x41;
                alloced[j + 1] = 0x43;
                alloced[j + 8] = 0x51;
                alloced[j + 9] = 0x53;
            }
            else
            {
                alloced[j + 0] = 0x41;
                alloced[j + 1] = 0x48;
                alloced[j + 8] = 0x51;
                alloced[j + 9] = 0x58;
            }
        }
        else if (i != sMonSummaryScreen->maxPageIndex)
        {
            alloced[j + 0] = 0x44;
            alloced[j + 1] = 0x45;
            alloced[j + 8] = 0x54;
            alloced[j + 9] = 0x55;
        }
        else
        {
            alloced[j + 0] = 0x44;
            alloced[j + 1] = 0x46;
            alloced[j + 8] = 0x54;
            alloced[j + 9] = 0x56;
        }
    }
    CopyToBgTilemapBufferRect_ChangePalette(3, alloced, 11, 0, 8, 2, 16);
    schedule_bg_copy_tilemap_to_vram(3);
    Free(alloced);
}

static void DetailedMovesTilemapDisplay(u16 *dst, u16 palette, bool8 remove)
{
    u16 i, id, id2;

    palette *= 0x1000;
    id = 0x44F, id2 = 0x601;
    if (!remove)
    {
        for (i = 0; i < 15; i++)
        {
            dst[id + i] = gSummaryScreenWindow_Tilemap[i] + palette;
            dst[id + i + 0x20] = gSummaryScreenWindow_Tilemap[i + 15] + palette;
            dst[id + i + 0x40] = gSummaryScreenWindow_Tilemap[i + 30] + palette;
            dst[id + i + 0x60] = gSummaryScreenWindow_Tilemap[i + 45] + palette;
            dst[id + i + 0x80] = gSummaryScreenWindow_Tilemap[i + 60] + palette;
            dst[id + i + 0xA0] = gSummaryScreenWindow_Tilemap[i + 75] + palette;
            dst[id + i + 0xC0] = gSummaryScreenWindow_Tilemap[i + 90] + palette;
            dst[id + i + 0xE0] = gSummaryScreenWindow_Tilemap[i + 105] + palette;
            dst[id + i + 0x100] = gSummaryScreenWindow_Tilemap[i + 120] + palette;
            dst[id + i + 0x120] = gSummaryScreenWindow_Tilemap[i + 135] + palette;
            dst[id + i + 0x140] = gSummaryScreenWindow_Tilemap[i + 150] + palette;
            dst[id + i + 0x160] = gSummaryScreenWindow_Tilemap[i + 165] + palette;
            dst[id + i + 0x180] = gSummaryScreenWindow_Tilemap[i + 180] + palette;
            dst[id + i + 0x1A0] = gSummaryScreenWindow_Tilemap[i + 195] + palette;
            dst[id + i + 0x1C0] = gSummaryScreenWindow_Tilemap[i + 210] + palette;
            dst[id + i + 0x1E0] = gSummaryScreenWindow_Tilemap[i + 225] + palette;
            dst[id + i + 0x200] = gSummaryScreenWindow_Tilemap[i + 240] + palette;
            dst[id + i + 0x220] = gSummaryScreenWindow_Tilemap[i + 255] + palette;
        }
        for (i = 0; i < 13; i++)
        {
            dst[id2 + i] = gSummaryScreenWindow_Tilemap_NewMove[i];
            dst[id2 + i + 0x20] = gSummaryScreenWindow_Tilemap_NewMove[i + 13];
            dst[id2 + i + 0x40] = gSummaryScreenWindow_Tilemap_NewMove[i + 26];
            dst[id2 + i + 0x60] = gSummaryScreenWindow_Tilemap_NewMove[i + 39];
        }
    }
    else
    {
        for (i = 0; i < 15; i++)
        {
            dst[id + i] = gSummaryScreenWindow_Tilemap[i + 270];
            dst[id + i + 0x20] = gSummaryScreenWindow_Tilemap[i + 285];
            dst[id + i + 0x40] = gSummaryScreenWindow_Tilemap[i + 300];
            dst[id + i + 0x60] = gSummaryScreenWindow_Tilemap[i + 315];
            dst[id + i + 0x80] = gSummaryScreenWindow_Tilemap[i + 330];
            dst[id + i + 0xA0] = gSummaryScreenWindow_Tilemap[i + 345];
            dst[id + i + 0xC0] = gSummaryScreenWindow_Tilemap[i + 360];
            dst[id + i + 0xE0] = gSummaryScreenWindow_Tilemap[i + 375];
            dst[id + i + 0x100] = gSummaryScreenWindow_Tilemap[i + 390];
            dst[id + i + 0x120] = gSummaryScreenWindow_Tilemap[i + 405];
            dst[id + i + 0x140] = gSummaryScreenWindow_Tilemap[i + 420];
            dst[id + i + 0x160] = gSummaryScreenWindow_Tilemap[i + 435];
            dst[id + i + 0x180] = gSummaryScreenWindow_Tilemap[i + 450];
            dst[id + i + 0x1A0] = gSummaryScreenWindow_Tilemap[i + 465];
            dst[id + i + 0x1C0] = gSummaryScreenWindow_Tilemap[i + 480];
            dst[id + i + 0x1E0] = gSummaryScreenWindow_Tilemap[i + 495];
            dst[id + i + 0x200] = gSummaryScreenWindow_Tilemap[i + 510];
            dst[id + i + 0x220] = gSummaryScreenWindow_Tilemap[i + 525];
        }
        for (i = 0; i < 13; i++)
        {
            dst[id2 + i] = gSummaryScreenWindow_Tilemap_NewMove[i + 52];
            dst[id2 + i + 0x20] = gSummaryScreenWindow_Tilemap_NewMove[i + 65];
            dst[id2 + i + 0x40] = gSummaryScreenWindow_Tilemap_NewMove[i + 78];
            dst[id2 + i + 0x60] = gSummaryScreenWindow_Tilemap_NewMove[i + 91];
        }
    }
}

static void DrawPokerusCuredSymbol(struct Pokemon *mon) // This checks if the mon has been cured of pokerus
{
    if (!CheckPartyPokerus(mon, 0) && CheckPartyHasHadPokerus(mon, 0)) // If yes it draws the cured symbol
    {
        sMonSummaryScreen->bgTilemapBuffers[PSS_PAGE_INFO][0][0x223] = 0x03;
        sMonSummaryScreen->bgTilemapBuffers[PSS_PAGE_INFO][1][0x223] = 0x03;
    }
    else
    {
        sMonSummaryScreen->bgTilemapBuffers[PSS_PAGE_INFO][0][0x223] = 0x03;
        sMonSummaryScreen->bgTilemapBuffers[PSS_PAGE_INFO][1][0x223] = 0x03;
    }
    schedule_bg_copy_tilemap_to_vram(3);
}

static void DrawExperienceProgressBar(void)
{
    s64 barTicks;
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    u16 *dst;
    u32 i;

    if (summary->level < MAX_LEVEL)
    {
        u32 expBetweenLevels = gExperienceTables[gBaseStats[summary->species].growthRate][summary->level + 1] - gExperienceTables[gBaseStats[summary->species].growthRate][summary->level];
        u32 expSinceLastLevel = summary->exp - gExperienceTables[gBaseStats[summary->species].growthRate][summary->level];

        // Calculate the number of 1-pixel "ticks" to illuminate in the experience progress bar.
        // There are 8 tiles that make up the bar, and each tile has 8 "ticks". Hence, the numerator
        // is multiplied by 64.
        barTicks = expSinceLastLevel * 64 / expBetweenLevels;
        if (barTicks == 0 && expSinceLastLevel != 0)
            barTicks = 1;
    }
    else
    {
        barTicks = 0;
    }

    dst = &sMonSummaryScreen->bgTilemapBuffers[PSS_PAGE_SKILLS][1][0x1C3];
    for (i = 0; i < 8; i++)
    {
        if (barTicks > 7)
            dst[i] = 0x20A8;
        else
            dst[i] = 0x20A0 + (barTicks % 8);
        barTicks -= 8;
        if (barTicks < 0)
            barTicks = 0;
    }

    if (GetBgTilemapBuffer(1) == sMonSummaryScreen->bgTilemapBuffers[PSS_PAGE_SKILLS][0])
        schedule_bg_copy_tilemap_to_vram(1);
    else
        schedule_bg_copy_tilemap_to_vram(2);
}

#define PIXEL_COORDS_TO_OFFSET(x, y)(			\
/*Add tiles by X*/								\
((y / 8) * 32 * 8)								\
/*Add tiles by X*/								\
+ ((x / 8) * 32)								\
/*Add pixels by Y*/								\
+ ((((y) - ((y / 8) * 8))) * 4)				    \
/*Add pixels by X*/								\
+ ((((x) - ((x / 8) * 8)) / 2)))

static inline void WritePixel(u8 *dst, u32 x, u32 y, u32 value)
{
    if (x & 1)
    {
        dst[PIXEL_COORDS_TO_OFFSET(x, y)] &= ~0xF0;
        dst[PIXEL_COORDS_TO_OFFSET(x, y)] |= (value << 4);
    }
    else
    {
        dst[PIXEL_COORDS_TO_OFFSET(x, y)] &= ~0xF;
        dst[PIXEL_COORDS_TO_OFFSET(x, y)] |= (value);
    }
}

static void DestroyHpBarSprite(void)
{
    FreeSpriteTilesByTag(TAG_HP_BAR);
    FreeSpriteTilesByTag(TAG_HP_BAR + 1);
    DestroySpriteInArray(SPRITE_ARR_ID_HP_BAR1);
    DestroySpriteInArray(SPRITE_ARR_ID_HP_BAR2);
}

static void DrawHpBar(void)
{
    u32 i, colorId, hpPixels;
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    // New mon, destroy sprite
    if (sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_HP_BAR1] != 0xFF)
        DestroyHpBarSprite();

    if (summary->currentHP != 0)
    {
        u8 *gfx = AllocZeroed(32 * 8);
        u8 *gfx2 = AllocZeroed(16 * 8);
        struct SpriteSheet sheets[] = {{gfx, 32 * 8, TAG_HP_BAR}, {gfx2, 16 * 8, TAG_HP_BAR + 1}, {}};

        switch (GetHPBarLevel(summary->currentHP, summary->maxHP)) // percent of health
        {
        case HP_BAR_FULL:
        case HP_BAR_GREEN:
            colorId = 3; // green
            break;
        case HP_BAR_YELLOW:
            colorId = 5; // yellow
            break;
        case HP_BAR_RED:
        default:
            colorId = 7; // red
            break;
        }
        hpPixels = (summary->currentHP * 48) / summary->maxHP; // 48 is bar pixel width
        if (hpPixels == 0)
            hpPixels = 1;
        for (i = 0; i < hpPixels; i++)
        {
            if (i < 32)
            {
                WritePixel(gfx, i, 3, colorId);
                WritePixel(gfx, i, 4, colorId + 1);
                WritePixel(gfx, i, 5, colorId + 1);
            }
            else
            {
                WritePixel(gfx2, i - 32, 3, colorId);
                WritePixel(gfx2, i - 32, 4, colorId + 1);
                WritePixel(gfx2, i - 32, 5, colorId + 1);
            }
        }

        LoadSpriteSheets(sheets);

        sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_HP_BAR1] = CreateSprite(&sSpriteTemplate_HpBar[0], 85, 32, 1);
        sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_HP_BAR2] = CreateSprite(&sSpriteTemplate_HpBar[1], 85 + 24, 32, 1);

        Free(gfx);
        Free(gfx2);
    }
}

static void LimitEggSummaryPageDisplay(void) // If the pokemon is an egg, limit the number of pages displayed to 1
{
    if (sMonSummaryScreen->summary.isEgg)
        ChangeBgX(3, 0x10000, 0);
    else
        ChangeBgX(3, 0, 0);
}

static void ResetWindows(void)
{
    u8 i;

    InitWindows(sSummaryTemplate);
    DeactivateAllTextPrinters();
    for (i = 0; i < PSS_LABEL_WINDOW_END; i++)
    {
        FillWindowPixelBuffer(i, PIXEL_FILL(0));
    }
    for (i = 0; i < ARRAY_COUNT(sMonSummaryScreen->windowIds); i++)
    {
        sMonSummaryScreen->windowIds[i] = 0xFF;
    }
}

static void PrintTextOnWindow(u8 windowId, const u8 *string, u8 x, u8 y, u8 lineSpacing, u8 colorId)
{
    AddTextPrinterParameterized4(windowId, 2, x, y, 0, lineSpacing, sTextColors[colorId], 0, string);
}

static void PrintMonInfo(void)
{
    u8 text[16], *txtPtr, gender;
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    struct Pokemon *mon = &sMonSummaryScreen->currentMon;

    FillWindowPixelBuffer(PSS_LABEL_WINDOW_PORTRAIT_LVL, PIXEL_FILL(0));
    FillWindowPixelBuffer(PSS_LABEL_WINDOW_PORTRAIT_NICKNAME, PIXEL_FILL(0));
    if (sMonSummaryScreen->summary.isEgg)
    {

    }
    else
    {
        txtPtr = StringCopy(text, gText_LevelSymbol);
        ConvertIntToDecimalStringN(txtPtr, summary->level, STR_CONV_MODE_LEFT_ALIGN, 3);
        PrintTextOnWindow(PSS_LABEL_WINDOW_PORTRAIT_LVL, text, 2, 0, 0, 2);
        PutWindowTilemap(PSS_LABEL_WINDOW_PORTRAIT_LVL);

        GetMonNickname(mon, text);
        PrintTextOnWindow(PSS_LABEL_WINDOW_PORTRAIT_NICKNAME, text, 0, 0, 0, 2);
        if (summary->species2 != SPECIES_NIDORAN_F && summary->species2 != SPECIES_NIDORAN_M)
        {
            gender = GetMonGender(mon);
            if (gender == MON_MALE)
                PrintTextOnWindow(PSS_LABEL_WINDOW_PORTRAIT_NICKNAME, gText_MaleSymbol, 70, 0, 0, 1);
            else
                PrintTextOnWindow(PSS_LABEL_WINDOW_PORTRAIT_NICKNAME, gText_FemaleSymbol, 70, 0, 0, 4);
        }
        PutWindowTilemap(PSS_LABEL_WINDOW_PORTRAIT_NICKNAME);
    }
}

static void PrintAOrBButtonIcon(u8 windowId, bool8 bButton, u32 x)
{
    // sSummaryBButtonBitmap - 0x80 = sSummaryAButtonBitmap
    BlitBitmapToWindow(windowId, (bButton) ? sSummaryBButtonBitmap : sSummaryBButtonBitmap - 0x80, x, 0, 16, 16);
}

static void PrintPageNamesAndStats(void)
{
    int stringXPos, iconXPos, statsXPos;
    static const u8 evText[] = _("Pokémon EV & IVs");

    PrintTextOnWindow(PSS_LABEL_WINDOW_POKEMON_INFO_TITLE, gText_PkmnInfo, 2, 0, 0, 3);
    PrintTextOnWindow(PSS_LABEL_WINDOW_POKEMON_SKILLS_TITLE, gText_PkmnSkills, 2, 0, 0, 3);
    PrintTextOnWindow(PSS_LABEL_WINDOW_BATTLE_MOVES_TITLE, sMonSummaryScreen->contestMode ? gText_ContestMoves : gText_BattleMoves, 2, 0, 0, 3);
    PrintTextOnWindow(PSS_LABEL_WINDOW_IV_EV_TITLE, evText, 2, 0, 0, 3);

    stringXPos = GetStringRightAlignXOffset(1, gText_Cancel2, 62);
    iconXPos = stringXPos - 16;
    if (iconXPos < 0)
        iconXPos = 0;
    PrintAOrBButtonIcon(PSS_LABEL_WINDOW_PROMPT_CANCEL, FALSE, iconXPos);
    PrintTextOnWindow(PSS_LABEL_WINDOW_PROMPT_CANCEL, gText_Cancel2, stringXPos, 1, 0, 0);

    stringXPos = GetStringRightAlignXOffset(1, gText_Info, 62);
    iconXPos = stringXPos - 16;
    if (iconXPos < 0)
        iconXPos = 0;
    PrintAOrBButtonIcon(PSS_LABEL_WINDOW_PROMPT_INFO, FALSE, iconXPos);
    PrintTextOnWindow(PSS_LABEL_WINDOW_PROMPT_INFO, gText_Info, stringXPos, 1, 0, 0);

    stringXPos = GetStringRightAlignXOffset(1, gText_Switch, 62);
    iconXPos = stringXPos - 16;
    if (iconXPos < 0)
        iconXPos = 0;
    PrintAOrBButtonIcon(PSS_LABEL_WINDOW_PROMPT_SWITCH, FALSE, iconXPos);
    PrintTextOnWindow(PSS_LABEL_WINDOW_PROMPT_SWITCH, gText_Switch, stringXPos, 0, 0, 0);
}

static void DestroySummaryIcons(u32 page)
{
    s32 i;
    const struct CoordsId *coords = sSummaryIconCoords[page];
    for (i = 0; coords[i].id != -1; i++)
    {
        u8 *ptr = sMonSummaryScreen->iconSpriteIds[coords[i].id][1];
        if (ptr[1] == 0xFF || ptr[0] == 0xFF)
            ptr = sMonSummaryScreen->iconSpriteIds[coords[i].id][0];
        DestroySprite(&gSprites[ptr[0]]);
        ptr[0] = 0xFF;
        DestroySprite(&gSprites[ptr[1]]);
        ptr[1] = 0xFF;
    }
}

static void SetSummaryIconInvisibility(u32 iconId, u32 setId, bool32 invisible)
{
    u8 *ptr = sMonSummaryScreen->iconSpriteIds[iconId][setId];

    if (ptr[0] != 0xFF)
        gSprites[ptr[0]].invisible = invisible;
    if (ptr[1] != 0xFF)
        gSprites[ptr[1]].invisible = invisible;
}

static void CreateSummaryIcons(u32 page)
{
    s32 i;
    const struct CoordsId *coords = sSummaryIconCoords[page];
    for (i = 0; coords[i].id != -1; i++)
    {
        u8 *ptr = sMonSummaryScreen->iconSpriteIds[coords[i].id][0];
        if (ptr[0] != 0xFF || ptr[1] != 0xFF)
            ptr = sMonSummaryScreen->iconSpriteIds[coords[i].id][1];
        ptr[0] = CreateSprite(&sSpriteTemplate_SummaryIcons, coords[i].x, coords[i].y, 0);
        StartSpriteAnim(&gSprites[ptr[0]], coords[i].id * 2);

        ptr[1] = CreateSprite(&sSpriteTemplate_SummaryIcons, coords[i].x + 32, coords[i].y, 0);
        StartSpriteAnim(&gSprites[ptr[1]], coords[i].id * 2 + 1);
    }
}

static void PutPageWindowsAndSprites(u8 page)
{
    u32 i;

    ClearWindowTilemap(PSS_LABEL_WINDOW_POKEMON_INFO_TITLE);
    ClearWindowTilemap(PSS_LABEL_WINDOW_POKEMON_SKILLS_TITLE);
    ClearWindowTilemap(PSS_LABEL_WINDOW_BATTLE_MOVES_TITLE);
    ClearWindowTilemap(PSS_LABEL_WINDOW_IV_EV_TITLE);

    CreateSummaryIcons(page);
    switch (page)
    {
    case PSS_PAGE_INFO:
        PutWindowTilemap(PSS_LABEL_WINDOW_POKEMON_INFO_TITLE);
        PutWindowTilemap(PSS_DATA_WINDOW_INFO);
        PutWindowTilemap(PSS_DATA_WINDOW_INFO_TRAINER_MEMO);
        PutWindowTilemap(PSS_LABEL_WINDOW_PROMPT_CANCEL);
        break;
    case PSS_PAGE_SKILLS:
        PutWindowTilemap(PSS_LABEL_WINDOW_POKEMON_SKILLS_TITLE);
        gSprites[sMonSummaryScreen->laserSpriteIds[0]].invisible = FALSE;
        break;
    case PSS_PAGE_BATTLE_MOVES:
        PutWindowTilemap(PSS_LABEL_WINDOW_BATTLE_MOVES_TITLE);
        if (sMonSummaryScreen->mode != PSS_MODE_SELECT_MOVE)
            PutWindowTilemap(PSS_LABEL_WINDOW_PROMPT_INFO);
        gSprites[sMonSummaryScreen->laserSpriteIds[0]].invisible = TRUE;
        SetSummaryIconInvisibility(SUMMARY_ICON_ACCURACY, 0, TRUE);
        SetSummaryIconInvisibility(SUMMARY_ICON_POWER, 0, TRUE);
        SetSummaryIconInvisibility(SUMMARY_ICON_EFFECT, 0, TRUE);
        break;
    case PSS_PAGE_IV_EVS:
        PutWindowTilemap(PSS_LABEL_WINDOW_IV_EV_TITLE);
        gSprites[sMonSummaryScreen->laserSpriteIds[0]].invisible = FALSE;
        break;
    }

    for (i = 0; i < ARRAY_COUNT(sMonSummaryScreen->windowIds); i++)
        PutWindowTilemap(sMonSummaryScreen->windowIds[i]);

    schedule_bg_copy_tilemap_to_vram(0);
}

static void ClearPageWindowsAndSprites(u8 page)
{
    u32 i;

    DestroySummaryIcons(page);
    switch (page)
    {
    case PSS_PAGE_INFO:
        ClearWindowTilemap(PSS_LABEL_WINDOW_PROMPT_CANCEL);
        break;
    case PSS_PAGE_SKILLS:
        DestroyHpBarSprite();
        break;
    case PSS_PAGE_BATTLE_MOVES:
        if (sMonSummaryScreen->mode != PSS_MODE_SELECT_MOVE)
            ClearWindowTilemap(PSS_LABEL_WINDOW_PROMPT_INFO);
        break;
    }

    for (i = 0; i < ARRAY_COUNT(sMonSummaryScreen->windowIds); i++)
        RemoveWindowByIndex(i);

    schedule_bg_copy_tilemap_to_vram(0);
}

static u8 AddWindowFromTemplateList(const struct WindowTemplate *template, u8 templateId)
{
    u8 *windowIdPtr = &sMonSummaryScreen->windowIds[templateId];
    if (*windowIdPtr == 0xFF)
    {
        *windowIdPtr = AddWindow(&template[templateId]);
        FillWindowPixelBuffer(*windowIdPtr, PIXEL_FILL(0));
    }
    return *windowIdPtr;
}

static void RemoveWindowByIndex(u8 windowIndex)
{
    u8 *windowIdPtr = &sMonSummaryScreen->windowIds[windowIndex];
    if (*windowIdPtr != 0xFF)
    {
        ClearWindowTilemap(*windowIdPtr);
        RemoveWindow(*windowIdPtr);
        *windowIdPtr = 0xFF;
    }
}

static void PrintPageSpecificText(u8 pageIndex, bool32 scrollMon)
{
    u32 i;
    for (i = 0; i < ARRAY_COUNT(sMonSummaryScreen->windowIds); i++)
    {
        if (sMonSummaryScreen->windowIds[i] != 0xFF
            && !(scrollMon && pageIndex == PSS_PAGE_IV_EVS)) // Handled in the function itself to avoid lag.
                FillWindowPixelBuffer(sMonSummaryScreen->windowIds[i], PIXEL_FILL(0));
    }
    sMonSummaryScreen->textScrollMon = scrollMon;
    sTextPrinterFunctions[pageIndex]();
}

static void CreateTextPrinterTask(u8 pageIndex)
{
    sMonSummaryScreen->textScrollMon = FALSE;
    CreateTask(sTextPrinterTasks[pageIndex], 16);
}

static void GetMetLevelString(u8 *dst)
{
    u8 level = sMonSummaryScreen->summary.metLevel;
    if (level == 0)
        level = EGG_HATCH_LEVEL;
    ConvertIntToDecimalStringN(dst, level, STR_CONV_MODE_LEFT_ALIGN, 3);
    DynamicPlaceholderTextUtil_SetPlaceholderPtr(3, dst);
}

static void BufferTrainerMemo(struct PokeSummary *summary, u8 *dst)
{
    const u8 *txtPtr;

    DynamicPlaceholderTextUtil_Reset();
    if (InBattleFactory() || InSlateportBattleTent() || IsInGamePartnerMon())
    {
        DynamicPlaceholderTextUtil_ExpandPlaceholders(dst, gText_XNature);
    }
    else
    {
        u8 *metLevelString = Alloc(32);
        u8 *metLocationString = Alloc(32);

        GetMetLevelString(metLevelString);
        if (summary->metLocation < MAPSEC_NONE)
        {
            GetMapNameHandleAquaHideout(metLocationString, summary->metLocation);
            DynamicPlaceholderTextUtil_SetPlaceholderPtr(4, metLocationString);
        }

        if (DoesMonOTMatchOwner())
        {
            if (summary->metLevel == 0)
                txtPtr = (summary->metLocation >= MAPSEC_NONE) ? gText_XNatureHatchedSomewhereAt : gText_XNatureHatchedAtYZ;
            else
                txtPtr = (summary->metLocation >= MAPSEC_NONE) ? gText_XNatureMetSomewhereAt : gText_XNatureMetAtYZ;
        }
        else
        {
            if (summary->metLocation == METLOC_FATEFUL_ENCOUNTER)
                txtPtr = gText_XNatureFatefulEncounter;
            else if (summary->metLocation != METLOC_IN_GAME_TRADE && DidMonComeFromGBAGames())
                txtPtr = (summary->metLocation >= MAPSEC_NONE) ? gText_XNatureObtainedInTrade : gText_XNatureProbablyMetAt;
            else
                txtPtr = gText_XNatureObtainedInTrade;
        }

        DynamicPlaceholderTextUtil_ExpandPlaceholders(dst, txtPtr);
        Free(metLevelString);
        Free(metLocationString);
    }
}

static void PrintInfoPageText(void)
{
    u32 windowId, dexNum;
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    u8 *text = Alloc(100);

    if (sMonSummaryScreen->summary.isEgg)
    {

    }
    else
    {
        // Number
        windowId = AddWindowFromTemplateList(sPageInfoTemplate, PSS_DATA_WINDOW_INFO);
        dexNum = SpeciesToPokedexNum(summary->species);
        if (dexNum != 0xFFFF)
            ConvertIntToDecimalStringN(text, dexNum, STR_CONV_MODE_LEADING_ZEROS, 3);
        else
            StringCopy(text, gText_Number2);

        PrintTextOnWindow(windowId, text, 3, 0, 0, 0);

        // Name
        PrintTextOnWindow(windowId, summary->nickname, 3, 16, 0, 0);

        // OT
        PrintTextOnWindow(windowId, summary->OTName, 3, 44, 0, 0);

        // ID no
        ConvertIntToDecimalStringN(text, summary->OTID & 0xFFFF, STR_CONV_MODE_LEADING_ZEROS, 5);
        PrintTextOnWindow(windowId, text, 3, 59, 0, 0);

        // Item
        if (summary->item != 0)
            CopyItemName(summary->item, text);
        else
            StringCopy(text, gText_None);
        PrintTextOnWindow(windowId, text, 3, 72, 0, 0);

        // Nature
        PrintTextOnWindow(windowId, gNatureNamePointers[summary->nature], 3, 87, 0, 0);

        // Trainer Memo
        windowId = AddWindowFromTemplateList(sPageInfoTemplate, PSS_DATA_WINDOW_INFO_TRAINER_MEMO);
        BufferTrainerMemo(summary, text);
        PrintTextOnWindow(windowId, text, 3, 0, 0, 0);
    }

    Free(text);
}

static void Task_PrintInfoPage(u8 taskId)
{
    if (gTasks[taskId].data[0]++ >= 1)
    {
        PrintInfoPageText();
        DestroyTask(taskId);
    }
}

static bool8 DoesMonOTMatchOwner(void)
{
    struct PokeSummary *sum = &sMonSummaryScreen->summary;
    u32 trainerId;
    u8 gender;

    if (sMonSummaryScreen->monList.mons == gEnemyParty)
    {
        u8 multiID = GetMultiplayerId() ^ 1;
        trainerId = gLinkPlayers[multiID].trainerId & 0xFFFF;
        gender = gLinkPlayers[multiID].gender;
        StringCopy(gStringVar1, gLinkPlayers[multiID].name);
    }
    else
    {
        trainerId = GetPlayerIDAsU32() & 0xFFFF;
        gender = gSaveBlock2Ptr->playerGender;
        StringCopy(gStringVar1, gSaveBlock2Ptr->playerName);
    }

    if (gender != sum->OTGender || trainerId != (sum->OTID & 0xFFFF) || StringCompareWithoutExtCtrlCodes(gStringVar1, sum->OTName))
        return FALSE;
    else
        return TRUE;
}

static bool8 DidMonComeFromGBAGames(void)
{
    struct PokeSummary *sum = &sMonSummaryScreen->summary;
    if (sum->metGame > 0 && sum->metGame <= VERSION_LEAF_GREEN)
        return TRUE;
    return FALSE;
}

bool8 DidMonComeFromRSE(void)
{
    struct PokeSummary *sum = &sMonSummaryScreen->summary;
    if (sum->metGame > 0 && sum->metGame <= VERSION_EMERALD)
        return TRUE;
    return FALSE;
}

static bool8 IsInGamePartnerMon(void)
{
    if ((gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER) && gMain.inBattle)
    {
        if (sMonSummaryScreen->curMonIndex == 1 || sMonSummaryScreen->curMonIndex == 4 || sMonSummaryScreen->curMonIndex == 5)
            return TRUE;
    }
    return FALSE;
}

static void PrintSkillsPageText(void)
{
    u8 text[20], *txtPtr, windowId, colorId;
    u32 i, ability, exp = 0;
    static const u8 order[] = {STAT_ATK, STAT_DEF, STAT_SPATK, STAT_SPDEF, STAT_SPEED};
    struct PokeSummary *sum = &sMonSummaryScreen->summary;
    const s8 *natureStats = gNatureStatTable[sum->nature];

    // HP
    windowId = AddWindowFromTemplateList(sPageSkillsTemplate, PSS_DATA_WINDOW_SKILLS_HP);
    txtPtr = ConvertIntToDecimalStringN(text, sum->currentHP, STR_CONV_MODE_LEFT_ALIGN, 3);
    *(txtPtr++) = CHAR_SLASH;
    ConvertIntToDecimalStringN(txtPtr, sum->maxHP, STR_CONV_MODE_LEFT_ALIGN, 3);
    PrintTextOnWindow(windowId, text, 3, 0, 0, 0);

    // Stats atk-speed
    windowId = AddWindowFromTemplateList(sPageSkillsTemplate, PSS_DATA_WINDOW_SKILLS_STATS);
    for (i = 0; i < NUM_STATS - 1; i++)
    {
        u32 statId = order[i] - 1;
        ConvertIntToDecimalStringN(text, sum->stats[statId], STR_CONV_MODE_LEFT_ALIGN, 3);
        if (natureStats[statId] > 0)
        {
            SetSpriteInvisibility(SPRITE_ARR_ID_NATURE_PLUS, FALSE);
            SetSpriteCoords(SPRITE_ARR_ID_NATURE_PLUS, 0, 12 * i);
            colorId = 11;
        }
        else if (natureStats[statId] < 0)
        {
            SetSpriteInvisibility(SPRITE_ARR_ID_NATURE_MINUS, FALSE);
            SetSpriteCoords(SPRITE_ARR_ID_NATURE_MINUS, 0, 12 * i);
            colorId = 13;
        }
        else
        {
            colorId = 0;
        }
        PrintTextOnWindow(windowId, text, 3, 12 * i + 4, 0, colorId);
    }

    // Exp
    windowId = AddWindowFromTemplateList(sPageSkillsTemplate, PSS_DATA_WINDOW_SKILLS_EXP);
    ConvertIntToDecimalStringN(text, sum->exp, STR_CONV_MODE_LEFT_ALIGN, 8);
    PrintTextOnWindow(windowId, text, 3, 0, 0, 0);

    txtPtr = StringCopy(text, gText_NextLv);
    if (sum->level < MAX_LEVEL)
        exp = gExperienceTables[gBaseStats[sum->species].growthRate][sum->level + 1] - sum->exp;
    ConvertIntToDecimalStringN(txtPtr, exp, STR_CONV_MODE_LEFT_ALIGN, 7);
    PrintTextOnWindow(windowId, text, 29, 12, 0, 0);

    // Ability
    windowId = AddWindowFromTemplateList(sPageSkillsTemplate, PSS_DATA_WINDOW_SKILLS_ABILITY);
    ability = GetAbilityBySpecies(sum->species2, sum->abilityNum);
    PrintTextOnWindow(windowId, gAbilityNames[ability], 4, 0, 0, 0);

    // Ability description
    windowId = AddWindowFromTemplateList(sPageSkillsTemplate, PSS_DATA_WINDOW_SKILLS_DESCRIPTION);
    PrintTextOnWindow(windowId, gAbilityDescriptionPointers[ability], 0, 0, 0, 0);
}

static void Task_PrintSkillsPage(u8 taskId)
{
    if (gTasks[taskId].data[0]++ >= 1)
    {
        PrintSkillsPageText();
        DestroyTask(taskId);
    }
}

static void PrintEvStats(void)
{
    u8 string[20], *txtPtr, windowId, i, y;
    static const u8 coords[] =
    {
        [STAT_HP] = 0,
        [STAT_ATK] = 12,
        [STAT_DEF] = 24,
        [STAT_SPATK] = 36,
        [STAT_SPDEF] = 48,
        [STAT_SPEED] = 60,
        [NUM_STATS] = 74,
    };
    static const char evsTotalString[][5] = {_("/252"), _("/512")};

    windowId = AddWindowFromTemplateList(sPageIvEvTemplate, PSS_EV_WINDOW_STATS);
    for (i = 0; i < NUM_STATS + 1; i++)
    {
        y = coords[i];
        if (sMonSummaryScreen->textScrollMon)
        {
            if (sMonSummaryScreen->prevEvs[i] == sMonSummaryScreen->summary.evs[i])
                continue;
            FillWindowPixelRect(windowId, PIXEL_FILL(0), 0, y, 21, 14);
        }
        sMonSummaryScreen->prevEvs[i] = sMonSummaryScreen->summary.evs[i];
        txtPtr = ConvertIntToDecimalStringN(string, sMonSummaryScreen->summary.evs[i], STR_CONV_MODE_RIGHT_ALIGN, 3);
        StringCopy(txtPtr, evsTotalString[(i == NUM_STATS) ? 1 : 0]);
        PrintTextOnWindow(windowId, string, 2, y, 0, 0);
    }
}

static void PrintIvStats(void)
{
    static const u8 coords[NUM_STATS][2] =
    {
        [STAT_HP] = {14, 10},
        [STAT_ATK] = {24, 23},
        [STAT_DEF] = {39, 36},
        [STAT_SPATK] = {98, 10},
        [STAT_SPDEF] = {111, 23},
        [STAT_SPEED] = {125, 36},
    };
    u8 string[10], windowId, i;

    windowId = AddWindowFromTemplateList(sPageIvEvTemplate, PSS_IV_WINDOW_STATS);
    for (i = 0; i < NUM_STATS; i++)
    {
        if (sMonSummaryScreen->textScrollMon)
        {
            if (sMonSummaryScreen->prevIvs[i] == sMonSummaryScreen->summary.ivs[i])
                continue;
            FillWindowPixelRect(windowId, PIXEL_FILL(0), coords[i][0], coords[i][1], 15, 12);
        }
        sMonSummaryScreen->prevIvs[i] = sMonSummaryScreen->summary.ivs[i];
        ConvertIntToDecimalStringN(string, sMonSummaryScreen->summary.ivs[i], STR_CONV_MODE_RIGHT_ALIGN, 2);
        PrintTextOnWindow(windowId, string, coords[i][0], coords[i][1], 0, 0);
    }
}

static void PrintIvEvs(void)
{
    PrintEvStats();
    PrintIvStats();
}

static void Task_PrintIvEvs(u8 taskId)
{
    if (gTasks[taskId].data[0]++ >= 1)
    {
        PrintIvEvs();
        DestroyTask(taskId);
    }
}

static void PrintBattleMoves(void)
{
    u8 windowId = AddWindowFromTemplateList(sPageMovesTemplate, PSS_DATA_WINDOW_MOVE_NAMES);

    PrintMoveNameAndPP(windowId, 0);
    PrintMoveNameAndPP(windowId, 1);
    PrintMoveNameAndPP(windowId, 2);
    PrintMoveNameAndPP(windowId, 3);
    if (sMonSummaryScreen->mode == PSS_MODE_SELECT_MOVE)
        PrintMoveNameAndPP(windowId, 4);
    PrintMoveDetails(0);
}

static void Task_PrintBattleMoves(u8 taskId)
{
    if (gTasks[taskId].data[0]++ >= 1)
    {
        PrintBattleMoves();
        DestroyTask(taskId);
    }
}

static void PrintMoveNameAndPP(u32 windowId, u32 moveIndex)
{
    u8 text[12], *txtPtr, pp;
    u8 y = 3 + moveIndex * 29;
    u32 move = (moveIndex >= 4) ? sMonSummaryScreen->newMove : sMonSummaryScreen->summary.moves[moveIndex];

    if (move)
    {
        PrintTextOnWindow(windowId, gMoveNames[move], 10, y, 0, 0);

        text[0] = CHAR_P;
        text[1] = CHAR_P;
        text[2] = CHAR_SPACE;
        pp = (moveIndex >= 4) ? gBattleMoves[move].pp : sMonSummaryScreen->summary.pp[moveIndex];
        txtPtr = ConvertIntToDecimalStringN(&text[3], pp, STR_CONV_MODE_RIGHT_ALIGN, 2);
        *(txtPtr++) = CHAR_SLASH;
        if (moveIndex < 4)
            pp = CalculatePPWithBonus(move, sMonSummaryScreen->summary.ppBonuses, moveIndex);
        ConvertIntToDecimalStringN(txtPtr, pp, STR_CONV_MODE_LEFT_ALIGN, 2);
        AddTextPrinterParameterized3(windowId, 0, 29, y + 11, sTextColors[0], 0, text);
    }
}

static void PrintMoveCancel(bool32 remove)
{
    u8 windowId = AddWindowFromTemplateList(sPageMovesTemplate, PSS_DATA_WINDOW_MOVE_NAMES);

    if (!remove)
    {
        PrintTextOnWindow(windowId, gText_Cancel, 10, 8 + 4 * 29, 0, 0);
    }
    else
    {
        FillWindowPixelRect(windowId, PIXEL_FILL(0), 0, 120, 68, 17);
        PrintTextOnWindow(windowId, gText_EmptyString3, 10, 3 + 4 * 29, 0, 0);
    }
}

static void PrintPowAcc(u8 windowId, u8 *text, u16 move, u8 x, u8 y, u32 which)
{
    if (which == 0)
    {
        if (sMonSummaryScreen->contestMode)
            ConvertIntToDecimalStringN(text, gContestEffects[gContestMoves[move].effect].appeal / 10, STR_CONV_MODE_LEFT_ALIGN, 1);
        else if (gBattleMoves[move].power < 2)
            StringCopy(text, gText_ThreeDashes);
        else
            ConvertIntToDecimalStringN(text, gBattleMoves[move].power, STR_CONV_MODE_LEFT_ALIGN, gBattleMoves[move].power >= 100 ? 3 : 2);
    }
    else
    {
        if (sMonSummaryScreen->contestMode)
            ConvertIntToDecimalStringN(text, gContestEffects[gContestMoves[move].effect].jam / 10, STR_CONV_MODE_LEFT_ALIGN, 1);
        else if (gBattleMoves[move].accuracy == 0)
            StringCopy(text, gText_ThreeDashes);
        else
            ConvertIntToDecimalStringN(text, gBattleMoves[move].accuracy, STR_CONV_MODE_LEFT_ALIGN, gBattleMoves[move].accuracy >= 100 ? 3 : 2);
    }
    PrintTextOnWindow(windowId, text, GetStringCenterAlignXOffset(2, text, x), y, 0, 0);
}

static void PrintMoveDetails(u16 move)
{
    u8 text[24];
    const u8 *txtPtr;
    u8 windowId = AddWindowFromTemplateList(sPageMovesTemplate, PSS_DATA_WINDOW_MOVE_DESCRIPTION);

    FillWindowPixelBuffer(windowId, PIXEL_FILL(0));
    if (move == 0)
    {
        PrintTextOnWindow(windowId, gText_EmptyString3, 0, 0, 0, 0);
        SetSpriteInvisibility(SPRITE_ARR_ID_SPLIT, TRUE);
    }
    else
    {
        PrintPowAcc(windowId, text, move, 121, 3, 0);
        PrintPowAcc(windowId, text, move, 121, 17, 1);
        if (!sMonSummaryScreen->contestMode)
        {
            SetSpriteInvisibility(SPRITE_ARR_ID_SPLIT, FALSE);
            SetSpriteAnim(SPRITE_ARR_ID_SPLIT, gBattleMoves[move].split);
        }

        if (sMonSummaryScreen->contestMode)
            txtPtr = gContestEffectDescriptionPointers[gContestMoves[move].effect];
        else
            txtPtr = gMoveDescriptionPointers[move - 1];
        PrintTextOnWindow(windowId, txtPtr, 4, 42, 0, 0);
    }
}

static void SwapMovesNamesPP(u8 moveIndex1, u8 moveIndex2)
{
    u8 windowId1 = AddWindowFromTemplateList(sPageMovesTemplate, PSS_DATA_WINDOW_MOVE_NAMES);
    u8 windowId2 = AddWindowFromTemplateList(sPageMovesTemplate, PSS_DATA_WINDOW_MOVE_DESCRIPTION);

    FillWindowPixelRect(windowId1, PIXEL_FILL(0), 0, 3 + moveIndex1 * 29, 78, 24);
    FillWindowPixelRect(windowId1, PIXEL_FILL(0), 0, 3 + moveIndex2 * 29, 78, 24);

    FillWindowPixelRect(windowId2, PIXEL_FILL(0), 0, moveIndex1 * 16, 48, 16);
    FillWindowPixelRect(windowId2, PIXEL_FILL(0), 0, moveIndex2 * 16, 48, 16);

    PrintMoveNameAndPP(windowId1, moveIndex1);
    PrintMoveNameAndPP(windowId1, moveIndex2);
}

static void PrintHMMovesCantBeForgotten(void)
{
    u8 windowId = AddWindowFromTemplateList(sPageMovesTemplate, PSS_DATA_WINDOW_MOVE_DESCRIPTION);
    FillWindowPixelBuffer(windowId, PIXEL_FILL(0));
    PrintTextOnWindow(windowId, gText_HMMovesCantBeForgotten2, 6, 1, 0, 0);
}

static void ResetSpriteIds(void)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sMonSummaryScreen->spriteIds); i++)
        sMonSummaryScreen->spriteIds[i] = 0xFF;
}

static void DestroySpriteInArray(u8 spriteArrayId)
{
    if (sMonSummaryScreen->spriteIds[spriteArrayId] != 0xFF)
    {
        DestroySprite(&gSprites[sMonSummaryScreen->spriteIds[spriteArrayId]]);
        sMonSummaryScreen->spriteIds[spriteArrayId] = 0xFF;
    }
}

static void SetSpriteCoords(u8 spriteArrayId, s16 x, s16 y)
{
    gSprites[sMonSummaryScreen->spriteIds[spriteArrayId]].pos2.x = x;
    gSprites[sMonSummaryScreen->spriteIds[spriteArrayId]].pos2.y = y;
}

static void SetSpriteAnim(u8 spriteArrayId, u8 animId)
{
    StartSpriteAnim(&gSprites[sMonSummaryScreen->spriteIds[spriteArrayId]], animId);
}

static void SetSpriteInvisibility(u8 spriteArrayId, bool8 invisible)
{
    gSprites[sMonSummaryScreen->spriteIds[spriteArrayId]].invisible = invisible;
}

static void HidePageSpecificSprites(void)
{
    // Keeps Pokémon, caught ball and status sprites visible.
    u32 i;

    for (i = SPRITE_ARR_ID_TYPE_MON1; i < ARRAY_COUNT(sMonSummaryScreen->spriteIds); i++)
    {
        if (sMonSummaryScreen->spriteIds[i] != 0xFF)
            SetSpriteInvisibility(i, TRUE);
    }
}

static void SetTypeIcons(void)
{
    switch (sMonSummaryScreen->currPageIndex)
    {
    case PSS_PAGE_INFO:
        SetMonTypeIcons();
        break;
    case PSS_PAGE_BATTLE_MOVES:
        if (sMonSummaryScreen->contestMode)
            SetContestMoveTypeIcons();
        else
            SetMoveTypeIcons();
        SetNewMoveTypeIcon();
        break;
    }
}

static void CreateMoveTypeIcons(void)
{
    u32 i;

    for (i = SPRITE_ARR_ID_TYPE_MON1; i < SPRITE_ARR_ID_TYPE_MOVE + 5; i++)
    {
        if (sMonSummaryScreen->spriteIds[i] == 0xFF)
            sMonSummaryScreen->spriteIds[i] = CreateSprite(&sSpriteTemplate_MoveTypes, 0, 0, 2);

        SetSpriteInvisibility(i, TRUE);
    }
}

static void CreateNewInterfaceIcons(void)
{
    sMonSummaryScreen->laserSpriteIds[0] = CreateSprite(&sSpriteTemplate_LaserGrid3, 16, 144, 2);
    sMonSummaryScreen->laserSpriteIds[1] = CreateSprite(&sSpriteTemplate_LaserGrid1, 120, 24, 2);
    sMonSummaryScreen->laserSpriteIds[2] = CreateSprite(&sSpriteTemplate_LaserGrid2, 152, 56, 2);
    sMonSummaryScreen->laserSpriteIds[3] = CreateSprite(&sSpriteTemplate_LaserGrid4, 192, 120, 2);
    sMonSummaryScreen->laserSpriteIds[4] = CreateSprite(&sSpriteTemplate_LaserGrid4, 240, 168, 2);

    sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_NATURE_MINUS] = CreateSprite(&sSpriteTemplate_NatureIcon, 54, 43, 2);
    SetSpriteInvisibility(SPRITE_ARR_ID_NATURE_MINUS, TRUE);

    sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_NATURE_PLUS] = CreateSprite(&sSpriteTemplate_NatureIcon, 54, 43, 2);
    SetSpriteAnim(SPRITE_ARR_ID_NATURE_PLUS, 1);
    SetSpriteInvisibility(SPRITE_ARR_ID_NATURE_PLUS, TRUE);

    sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_SPLIT] = CreateSprite(&sSpriteTemplate_SplitIcons, 216, 65, 0);
    SetSpriteInvisibility(SPRITE_ARR_ID_SPLIT, TRUE);
}

static void SetTypeSpritePosAndPal(u8 typeId, u8 x, u8 y, u8 spriteArrayId)
{
    struct Sprite *sprite = &gSprites[sMonSummaryScreen->spriteIds[spriteArrayId]];
    StartSpriteAnim(sprite, typeId);
    sprite->oam.paletteNum = sMoveTypeToOamPaletteNum[typeId];
    sprite->pos1.x = x;
    sprite->pos1.y = y;
    SetSpriteInvisibility(spriteArrayId, FALSE);
}

static void SetMonTypeIcons(void)
{
    s16 x1, x2, y;
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    if (summary->isEgg)
    {
        SetTypeSpritePosAndPal(TYPE_MYSTERY, 66, 54, SPRITE_ARR_ID_TYPE_MON1);
        SetSpriteInvisibility(SPRITE_ARR_ID_TYPE_MON2, TRUE);
    }
    else
    {
        if (sMonSummaryScreen->moveSelectionMode)
            x1 = 184, x2 = 220, y = 42;
        else
            x1 =  66, x2 = 102, y = 54;
        SetTypeSpritePosAndPal(gBaseStats[summary->species].type1, x1, y, SPRITE_ARR_ID_TYPE_MON1);
        if (gBaseStats[summary->species].type1 != gBaseStats[summary->species].type2)
        {
            SetTypeSpritePosAndPal(gBaseStats[summary->species].type2, x2, y, SPRITE_ARR_ID_TYPE_MON2);
            SetSpriteInvisibility(SPRITE_ARR_ID_TYPE_MON2, FALSE);
        }
        else
        {
            SetSpriteInvisibility(SPRITE_ARR_ID_TYPE_MON2, TRUE);
        }
    }
}

static void SetMoveTypeIcons(void)
{
    u8 i;
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (summary->moves[i] != MOVE_NONE)
            SetTypeSpritePosAndPal(gBattleMoves[summary->moves[i]].type, 18, 25 + (i * 29), i + SPRITE_ARR_ID_TYPE_MOVE);
        else
            SetSpriteInvisibility(i + SPRITE_ARR_ID_TYPE_MOVE, TRUE);
    }
}

static void SetContestMoveTypeIcons(void)
{
    u8 i;
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (summary->moves[i] != MOVE_NONE)
            SetTypeSpritePosAndPal(NUMBER_OF_MON_TYPES + gContestMoves[summary->moves[i]].contestCategory, 18, 25 + (i * 29), i + SPRITE_ARR_ID_TYPE_MOVE);
        else
            SetSpriteInvisibility(i + SPRITE_ARR_ID_TYPE_MOVE, TRUE);
    }
}

static void SetNewMoveTypeIcon(void)
{
    if (sMonSummaryScreen->newMove == MOVE_NONE)
    {
        SetSpriteInvisibility(SPRITE_ARR_ID_TYPE_MOVE + 4, TRUE);
    }
    else
    {
        if (!sMonSummaryScreen->contestMode)
            SetTypeSpritePosAndPal(gBattleMoves[sMonSummaryScreen->newMove].type, 18, 25 + (4 * 29), SPRITE_ARR_ID_TYPE_MOVE + 4);
        else
            SetTypeSpritePosAndPal(NUMBER_OF_MON_TYPES + gContestMoves[sMonSummaryScreen->newMove].contestCategory, 18, 25 + (4 * 29), SPRITE_ARR_ID_TYPE_MOVE + 4);
    }
}

static void SwapMovesTypeSprites(u8 moveIndex1, u8 moveIndex2)
{
    struct Sprite *sprite1 = &gSprites[sMonSummaryScreen->spriteIds[moveIndex1 + SPRITE_ARR_ID_TYPE_MOVE]];
    struct Sprite *sprite2 = &gSprites[sMonSummaryScreen->spriteIds[moveIndex2 + SPRITE_ARR_ID_TYPE_MOVE]];

    u8 temp = sprite1->animNum;
    sprite1->animNum = sprite2->animNum;
    sprite2->animNum = temp;

    temp = sprite1->oam.paletteNum;
    sprite1->oam.paletteNum = sprite2->oam.paletteNum;
    sprite2->oam.paletteNum = temp;

    sprite1->animBeginning = TRUE;
    sprite1->animEnded = FALSE;
    sprite2->animBeginning = TRUE;
    sprite2->animEnded = FALSE;
}

static u8 LoadMonGfxAndSprite(struct Pokemon *mon, s16 *state)
{
    const struct CompressedSpritePalette *pal;
    struct PokeSummary *summary = &sMonSummaryScreen->summary;

    switch (*state)
    {
    default:
        return CreateMonSprite(mon);
    case 0:
        if (gMain.inBattle)
        {
            if (sub_80688F8(3, sMonSummaryScreen->curMonIndex))
                HandleLoadSpecialPokePic_DontHandleBonded_Alakazam(&gMonFrontPicTable[summary->species2], gMonSpritesGfxPtr->sprites[1], summary->species2, summary->pid);
            else
                HandleLoadSpecialPokePic_2(&gMonFrontPicTable[summary->species2], gMonSpritesGfxPtr->sprites[1], summary->species2, summary->pid);
        }
        else
        {
            if (gMonSpritesGfxPtr != NULL)
            {
                if (sMonSummaryScreen->monList.mons == gPlayerParty || sMonSummaryScreen->mode == PSS_MODE_BOX || sMonSummaryScreen->unk40EF == TRUE)
                    HandleLoadSpecialPokePic_2(&gMonFrontPicTable[summary->species2], gMonSpritesGfxPtr->sprites[1], summary->species2, summary->pid);
                else
                    HandleLoadSpecialPokePic_DontHandleBonded_Alakazam(&gMonFrontPicTable[summary->species2], gMonSpritesGfxPtr->sprites[1], summary->species2, summary->pid);
            }
            else
            {
                if (sMonSummaryScreen->monList.mons == gPlayerParty || sMonSummaryScreen->mode == PSS_MODE_BOX || sMonSummaryScreen->unk40EF == TRUE)
                    HandleLoadSpecialPokePic_2(&gMonFrontPicTable[summary->species2], sub_806F4F8(0, 1), summary->species2, summary->pid);
                else
                    HandleLoadSpecialPokePic_DontHandleBonded_Alakazam(&gMonFrontPicTable[summary->species2], sub_806F4F8(0, 1), summary->species2, summary->pid);
            }
        }
        (*state)++;
        return 0xFF;
    case 1:
        pal = GetMonSpritePalStructFromOtIdPersonality(summary->species2, summary->OTID, summary->pid);
        LoadCompressedSpritePalette(pal);
        SetMultiuseSpriteTemplateToPokemon(pal->tag, 1);
        (*state)++;
        return 0xFF;
    }
}

static void PlayMonCry(void)
{
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    if (!summary->isEgg)
    {
        if (ShouldPlayNormalMonCry(&sMonSummaryScreen->currentMon) == TRUE)
            PlayCry3(summary->species2, 0, 0);
        else
            PlayCry3(summary->species2, 0, 11);
    }
}

static u8 CreateMonSprite(struct Pokemon *unused)
{
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    u8 spriteId = CreateSprite(&gMultiuseSpriteTemplate, 204, 68, 5);

    FreeSpriteOamMatrix(&gSprites[spriteId]);
    gSprites[spriteId].data[0] = summary->species2;
    gSprites[spriteId].data[1] = IsMonSpriteNotFlipped(summary->species2);
    gSprites[spriteId].data[2] = 0;
    gSprites[spriteId].callback = SpriteCB_Pokemon;
    gSprites[spriteId].oam.priority = 0;

    if (!gSprites[spriteId].data[1])
        gSprites[spriteId].hFlip = TRUE;
    else
        gSprites[spriteId].hFlip = FALSE;

    return spriteId;
}

static void SpriteCB_Pokemon(struct Sprite *sprite)
{
    struct PokeSummary *summary = &sMonSummaryScreen->summary;

    if (!gPaletteFade.active && sprite->data[2] != 1)
    {
        PlayMonCry();
        PokemonSummaryDoMonAnimation(sprite, sprite->data[0], summary->isEgg);
    }
}

void SummaryScreen_SetUnknownTaskId(u8 a0)
{
    sUnknownTaskId = a0;
}

void SummaryScreen_DestroyUnknownTask(void)
{
    if (sUnknownTaskId != 0xFF)
    {
        DestroyTask(sUnknownTaskId);
        sUnknownTaskId = 0xFF;
    }
}

static void StopPokemonAnimations(void)  // A subtle effect, this function stops pokemon animations when leaving the PSS
{
    u16 i, paletteIndex;

    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].animPaused = TRUE;
    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].callback = SpriteCallbackDummy;
    StopPokemonAnimationDelayTask();

    paletteIndex = (gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].oam.paletteNum * 16) | 0x100;

    for (i = 0; i < 16; i++)
    {
        u16 id = i + paletteIndex;
        gPlttBufferUnfaded[id] = gPlttBufferFaded[id];
    }
}

static void SpriteCb_Ball(struct Sprite *sprite)
{
    if (sMonSummaryScreen->currPageIndex == PSS_PAGE_BATTLE_MOVES)
        sprite->invisible = sMonSummaryScreen->moveSelectionMode;
}

static void CreateCaughtBallSprite(struct Pokemon *mon)
{
    u8 ball = ItemIdToBallId(GetMonData(mon, MON_DATA_POKEBALL));

    LoadBallGfx(ball);
    sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_BALL] = CreateSprite(&gBallSpriteTemplates[ball], 136, 40, 3);
    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_BALL]].callback = SpriteCb_Ball;
    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_BALL]].oam.priority = 1;
    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_BALL]].data[0] = ball;
}

static void CreateSetStatusSprite(void)
{
    u8 *spriteId = &sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_STATUS];
    u8 statusAnim;

    if (*spriteId == 0xFF)
        *spriteId = CreateSprite(&sSpriteTemplate_StatusCondition, 156, 40, 0);

    statusAnim = GetMonAilment(&sMonSummaryScreen->currentMon);
    if (statusAnim != 0)
    {
        StartSpriteAnim(&gSprites[*spriteId], statusAnim - 1);
        SetSpriteInvisibility(SPRITE_ARR_ID_STATUS, FALSE);
    }
    else
    {
        SetSpriteInvisibility(SPRITE_ARR_ID_STATUS, TRUE);
    }
}

static void CreateMoveSelectorSprites(u8 idArrayStart)
{
    u32 i, subpriority;
    const struct SpriteTemplate *sprTemplate;
    u8 *spriteIds = &sMonSummaryScreen->spriteIds[idArrayStart];

    if (sMonSummaryScreen->currPageIndex >= PSS_PAGE_BATTLE_MOVES)
    {
        if (idArrayStart == SPRITE_ARR_ID_MOVE_SELECTOR1)
            subpriority = 4, sprTemplate = &sSpriteTemplate_MoveSelectorBlue;
        else
            subpriority = 3, sprTemplate = &sSpriteTemplate_MoveSelectorRed;

        for (i = 0; i < MOVE_SELECTOR_SPRITES_COUNT; i++)
        {
            spriteIds[i] = CreateSprite(sprTemplate, i * 48 + 35, 32, subpriority);
            if (i == 1)
                SetSpriteAnim(idArrayStart + 1, 1);
            gSprites[spriteIds[i]].data[0] = idArrayStart;
            gSprites[spriteIds[i]].data[2] = idArrayStart + i;
        }
    }
}

static void SpriteCb_MoveSelector(struct Sprite *sprite)
{
    if (!sMonSummaryScreen->moveSelectionMode)
    {
        sMonSummaryScreen->spriteIds[sprite->data[2]] = 0xFF;
        DestroySprite(sprite);
    }
    else
    {
        if (sprite->data[0] == SPRITE_ARR_ID_MOVE_SELECTOR2
        || sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MOVE_SELECTOR2] == 0xFF)
        {
            sprite->data[1] = (sprite->data[1] + 1) & 0x1F;
            if (sprite->data[1] > 25)
                sprite->invisible = TRUE;
            else
                sprite->invisible = FALSE;
        }

        if (sprite->data[0] == SPRITE_ARR_ID_MOVE_SELECTOR1)
            sprite->pos2.y = sMonSummaryScreen->firstMoveIndex * 29;
        else
            sprite->pos2.y = sMonSummaryScreen->secondMoveIndex * 29;
    }
}

static void DestroyMoveSelectorSprites(u8 firstArrayId)
{
    u32 i;
    for (i = 0; i < MOVE_SELECTOR_SPRITES_COUNT; i++)
        DestroySpriteInArray(firstArrayId + i);
}

static void SwapMoveSelectors(void)
{
    u32 i, temp;
    u8 *spriteIds = sMonSummaryScreen->spriteIds;

    for (i = 0; i < MOVE_SELECTOR_SPRITES_COUNT; i++)
        SWAP(spriteIds[SPRITE_ARR_ID_MOVE_SELECTOR1 + i], spriteIds[SPRITE_ARR_ID_MOVE_SELECTOR2 + i], temp);
}

static void MakeMoveSelectorVisible(u8 firstSpriteId)
{
    u32 i;
    u8 *spriteIds = &sMonSummaryScreen->spriteIds[firstSpriteId];

    for (i = 0; i < MOVE_SELECTOR_SPRITES_COUNT; i++)
    {
        gSprites[spriteIds[i]].data[1] = 0;
        gSprites[spriteIds[i]].invisible = FALSE;
    }
}

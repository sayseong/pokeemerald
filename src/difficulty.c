#include "global.h"
#include "event_data.h"
#include "random.h"
#include "data.h"
#include "item.h"
#include "battle_util.h"
#include "constants/trainers.h"
#include "constants/items.h"
#include "constants/abilities.h"
#include "constants/berry.h"
#include "constants/battle_move_effects.h"

static EWRAM_DATA bool8 sHasMegaStone = FALSE;

static const u8 sBaseStatOffsets[] =
{
    [STAT_HP] = offsetof(struct BaseStats, baseHP),
    [STAT_ATK] = offsetof(struct BaseStats, baseAttack),
    [STAT_DEF] = offsetof(struct BaseStats, baseDefense),
    [STAT_SPEED] = offsetof(struct BaseStats, baseSpeed),
    [STAT_SPATK] = offsetof(struct BaseStats, baseSpAttack),
    [STAT_SPDEF] = offsetof(struct BaseStats, baseSpDefense),
};

static const u8 sLevelRanges[DIFFICULTY_COUNT - 1][9][2] = // difficulty, badge count
{
    [DIFFICULTY_MID - 1] =
    {
       {1, 2}, // no badges
       {2, 3}, // 1 badge
       {3, 4}, // 2 badges
       {3, 4}, // 3 badges
       {3, 4}, // 4 badges
       {4, 5}, // 5 badges
       {4, 5}, // 6 badges
       {5, 5}, // 7 badges
       {6, 8}, // 8 badges
    },
    [DIFFICULTY_HARD - 1] =
    {
       {2, 3}, // no badges
       {4, 6}, // 1 badge
       {5, 7}, // 2 badges
       {6, 8}, // 3 badges
       {7, 9}, // 4 badges
       {9, 12}, // 5 badges
       {10, 14}, // 6 badges
       {12, 15}, // 7 badges
       {15, 17}, // 8 badges
    },
    [DIFFICULTY_INSANE - 1] =
    {
       {5, 7}, // no badges
       {7, 10}, // 1 badge
       {10, 12}, // 2 badges
       {14, 17}, // 3 badges
       {19, 23}, // 4 badges
       {24, 28}, // 5 badges
       {30, 35}, // 6 badges
       {36, 40}, // 7 badges
       {45, 50}, // 8 badges
    },
};

static const u8 sEvsRanges[DIFFICULTY_COUNT - 1][9][2] = // difficulty, badge count
{
    [DIFFICULTY_MID - 1] =
    {
       {4, 20}, // no badges
       {4, 40}, // 1 badge
       {4, 60}, // 2 badges
       {4, 90}, // 3 badges
       {20, 125}, // 4 badges
       {40, 150}, // 5 badges
       {60, 200}, // 6 badges
       {100, 225}, // 7 badges
       {120, 250}, // 8 badges
    },
    [DIFFICULTY_HARD - 1] =
    {
       {8, 40}, // no badges
       {30, 60}, // 1 badge
       {40, 100}, // 2 badges
       {60, 150}, // 3 badges
       {80, 170}, // 4 badges
       {100, 200}, // 5 badges
       {150, 220}, // 6 badges
       {200, 240}, // 7 badges
       {252, 252}, // 8 badges
    },
    [DIFFICULTY_INSANE - 1] =
    {
       {25, 50}, // no badges
       {100, 160}, // 1 badge
       {120, 180}, // 2 badges
       {144, 200}, // 3 badges
       {200, 252}, // 4 badges
       {230, 252}, // 5 badges
       {252, 252}, // 6 badges
       {252, 252}, // 7 badges
       {252, 252}, // 8 badges
    },
};

static const u8 sIvsRanges[DIFFICULTY_COUNT - 1][9][2] = // difficulty, badge count
{
    [DIFFICULTY_MID - 1] =
    {
       {0, 0}, // no badges
       {0, 5}, // 1 badge
       {0, 10}, // 2 badges
       {0, 4}, // 3 badges
       {0, 5}, // 4 badges
       {1, 10}, // 5 badges
       {2, 15}, // 6 badges
       {3, 20}, // 7 badges
       {4, 25}, // 8 badges
    },
    [DIFFICULTY_HARD - 1] =
    {
       {0, 10}, // no badges
       {0, 15}, // 1 badge
       {0, 20}, // 2 badges
       {5, 30}, // 3 badges
       {10, 31}, // 4 badges
       {15, 31}, // 5 badges
       {20, 31}, // 6 badges
       {25, 31}, // 7 badges
       {30, 31}, // 8 badges
    },
    [DIFFICULTY_INSANE - 1] =
    {
       {0, 31}, // no badges
       {10, 31}, // 1 badge
       {20, 31}, // 2 badges
       {25, 31}, // 3 badges
       {30, 31}, // 4 badges
       {31, 31}, // 5 badges
       {31, 31}, // 6 badges
       {31, 31}, // 7 badges
       {31, 31}, // 8 badges
    },
};

static const u8 sIvsOdds[DIFFICULTY_COUNT - 1][NUM_STATS] = // Odds of getting max ivs range depending on base stat's rank
{
    [DIFFICULTY_MID - 1] = {10, 8, 6, 4, 2, 1},
    [DIFFICULTY_HARD - 1] = {30, 15, 8, 5, 3, 1},
    [DIFFICULTY_INSANE - 1] = {50, 35, 20, 15, 13, 10},
};

static const u8 sNatureOdds[DIFFICULTY_COUNT - 1] = {5, 25, 52};

// code
static int CountBadges(u32 trainerId)
{
    int i, badgeCount = 0;

    for (i = 0; i < NUM_BADGES; i++)
        badgeCount += FlagGet(i + FLAG_BADGE01_GET);
    // If we're battling a leader, count it as the badge was already owned
    if (gTrainers[trainerId].trainerClass == TRAINER_CLASS_LEADER && badgeCount < 8)
        badgeCount++;
    return badgeCount;
}

static int GetRandomRange(int min, int max)
{
    return min + (Random() % (abs(max - min) + 1));
}

static bool32 PercentChance(int odds)
{
    return (Random() % 100 < odds);
}

static u32 GetTrainerMonLevel(u32 trainerId, u32 level)
{
    int badgeCount;
    u32 difficulty = gSaveBlock2Ptr->optionsDifficulty;

    if (difficulty != DIFFICULTY_EASY)
    {
        badgeCount = CountBadges(trainerId);
        level += GetRandomRange(sLevelRanges[difficulty - 1][badgeCount][0], sLevelRanges[difficulty - 1][badgeCount][1]);
    }
    if (level > MAX_LEVEL)
        level = MAX_LEVEL;
    return level;
}

struct IdVal
{
    u16 id;
    u16 value;
};

static void RankBaseStats(struct IdVal *stats, u32 species)
{
    int i, j;
    struct IdVal temp;

    for (i = 0; i < NUM_STATS; i++)
    {
        u8 *baseStats = (u8*) &gBaseStats[species];
        stats[i].id = i;
        stats[i].value = baseStats[sBaseStatOffsets[i]];
    }

    // Sort
    for (i = 0; i < NUM_STATS; i++)
    {
        for (j = i + 1; j < NUM_STATS; j++)
        {
            if (stats[i].value < stats[j].value)
                SWAP(stats[i], stats[j], temp);
        }
    }
}

static bool32 HasMoveWithEffect(const u16 *moves, u32 effect)
{
    return (gBattleMoves[moves[0]].effect == effect
            || gBattleMoves[moves[1]].effect == effect
            || gBattleMoves[moves[2]].effect == effect
            || gBattleMoves[moves[3]].effect == effect);
}

static bool32 HasAttackingMove(const u16 *moves)
{
    return (gBattleMoves[moves[0]].power != 0
            || gBattleMoves[moves[1]].power != 0
            || gBattleMoves[moves[2]].power != 0
            || gBattleMoves[moves[3]].power != 0);
}

static bool32 HasAttackingTypeMove(const u16 *moves, u32 type)
{
    return ((gBattleMoves[moves[0]].power != 0 && gBattleMoves[moves[0]].type == type)
            || (gBattleMoves[moves[1]].power != 0 && gBattleMoves[moves[0]].type == type)
            || (gBattleMoves[moves[2]].power != 0 && gBattleMoves[moves[0]].type == type)
            || (gBattleMoves[moves[3]].power != 0 && gBattleMoves[moves[0]].type == type));
}

static bool32 HasMoveWithSplit(const u16 *moves, u32 split)
{
    return (gBattleMoves[moves[0]].split == split
            || (moves[1] && gBattleMoves[moves[1]].split == split)
            || (moves[2] && gBattleMoves[moves[2]].split == split)
            || (moves[3] && gBattleMoves[moves[3]].split == split));
}

static bool32 HasMoveWithMissChance(const u16 *moves)
{
    return ((gBattleMoves[moves[0]].accuracy > 1 && gBattleMoves[moves[0]].accuracy < 100)
            || (moves[1] && gBattleMoves[moves[1]].accuracy > 1 && gBattleMoves[moves[1]].accuracy < 100)
            || (moves[2] && gBattleMoves[moves[2]].accuracy > 1 && gBattleMoves[moves[2]].accuracy < 100)
            || (moves[3] && gBattleMoves[moves[3]].accuracy > 1 && gBattleMoves[moves[3]].accuracy < 100));
}

static bool32 IsSuperEffective(u32 species, u32 type)
{
    u16 modifier = UQ_4_12(1.0);

    MulModifier(&modifier, GetTypeModifier(type, gBaseStats[species].type1));
    if (gBaseStats[species].type2 != gBaseStats[species].type1)
        MulModifier(&modifier, GetTypeModifier(type, gBaseStats[species].type2));

    return (modifier == UQ_4_12(2.0) || modifier == UQ_4_12(4.0));
}

static bool32 IsType(u32 species, u32 type)
{
    return (gBaseStats[species].type1 == type || gBaseStats[species].type2 == type);
}

static u32 ChooseHeldItem(struct Pokemon *mon, u32 species, u32 trainerId, u32 difficulty, u32 badgeCount) // difficulty minus 1
{
    static const u8 megaAllowed[] = {8, 6, 4};
    static const u8 megaOdds[][DIFFICULTY_COUNT - 1] = {{5, 15, 20}, {20, 70, 100}}; // higher odds are for the boss trainer
    static const u8 battleItemsAllowed[] = {6, 4, 2};
    static const u8 battleItemOdds[][DIFFICULTY_COUNT - 1] = {{6, 20, 30}, {30, 85, 100}};
    u16 moves[4];
    u32 i, j, holdItem, effect, isBossTrainer;

    switch (gTrainers[trainerId].trainerClass)
    {
    case TRAINER_CLASS_AQUA_ADMIN:
    case TRAINER_CLASS_MAGMA_ADMIN:
    case TRAINER_CLASS_MAGMA_LEADER:
    case TRAINER_CLASS_AQUA_LEADER:
    case TRAINER_CLASS_LEADER:
    case TRAINER_CLASS_ELITE_FOUR:
    case TRAINER_CLASS_CHAMPION:
    case TRAINER_CLASS_PKMN_TRAINER_1:
    case TRAINER_CLASS_PKMN_TRAINER_2:
    case TRAINER_CLASS_PKMN_TRAINER_3:
        isBossTrainer = TRUE;
        break;
    default:
        isBossTrainer = FALSE;
        break;
    }

    // Try mega stone
    if (!sHasMegaStone && badgeCount >= megaAllowed[difficulty] && PercentChance(megaOdds[isBossTrainer][difficulty]))
    {
        for (i = 0; i < EVOS_PER_MON; i++)
        {
            if (gEvolutionTable[species][i].method == EVO_MEGA_EVOLUTION)
            {
                sHasMegaStone = TRUE;
                // See if it has two possible mega evos
                if (i + 1 < EVOS_PER_MON && gEvolutionTable[species][i + 1].method == EVO_MEGA_EVOLUTION && Random() & 1)
                    return gEvolutionTable[species][i + 1].method;
                else
                    return gEvolutionTable[species][i].method;
            }
        }
    }

    if (badgeCount >= battleItemsAllowed[difficulty] && PercentChance(battleItemOdds[isBossTrainer][difficulty]))
    {
        u32 ability = GetMonAbility(mon);
        // Gather moves data for battle items
        for (i = 0; i < MAX_MON_MOVES; i++)
            moves[i] = GetMonData(mon, MON_DATA_MOVE1 + i, NULL);
        // 10 random tries
        for (i = 0; i < 10; i++)
        {
            // try a berry
            if ((i == 0 && Random() % 1) || i % 3 == 0)
            {
                holdItem = GetRandomRange(FIRST_BERRY_INDEX, LAST_BERRY_INDEX);
                switch (holdItem)
                {
                // Useless except for Natural Gift
                case ITEM_RAZZ_BERRY:
                case ITEM_BLUK_BERRY:
                case ITEM_NANAB_BERRY:
                case ITEM_WEPEAR_BERRY:
                case ITEM_PINAP_BERRY:
                case ITEM_MAGOST_BERRY:
                case ITEM_RABUTA_BERRY:
                case ITEM_NOMEL_BERRY:
                case ITEM_SPELON_BERRY:
                case ITEM_PAMTRE_BERRY:
                case ITEM_WATMEL_BERRY:
                case ITEM_DURIN_BERRY:
                case ITEM_BELUE_BERRY:
                case ITEM_GREPA_BERRY:
                case ITEM_HONDEW_BERRY:
                case ITEM_KELPSY_BERRY:
                case ITEM_POMEG_BERRY:
                case ITEM_QUALOT_BERRY:
                case ITEM_TAMATO_BERRY:
                    if (!HasMoveWithEffect(moves, EFFECT_NATURAL_GIFT))
                        holdItem = 0;
                    break;
                case ITEM_BABIRI_BERRY:
                case ITEM_CHARTI_BERRY:
                case ITEM_CHOPLE_BERRY:
                case ITEM_COBA_BERRY:
                case ITEM_COLBUR_BERRY:
                case ITEM_HABAN_BERRY:
                case ITEM_KASIB_BERRY:
                case ITEM_KEBIA_BERRY:
                case ITEM_OCCA_BERRY:
                case ITEM_PASSHO_BERRY:
                case ITEM_PAYAPA_BERRY:
                case ITEM_RINDO_BERRY:
                case ITEM_SHUCA_BERRY:
                case ITEM_TANGA_BERRY:
                case ITEM_WACAN_BERRY:
                case ITEM_YACHE_BERRY:
                    if (!IsSuperEffective(species, ItemId_GetHoldEffectParam(holdItem)))
                        holdItem = 0;
                    break;
                case ITEM_CHERI_BERRY:
                    if (IsType(species, TYPE_ELECTRIC) || IsType(species, TYPE_GROUND) || ability == ABILITY_LIMBER || ability == ABILITY_COMATOSE)
                        holdItem = 0;
                    break;
                case ITEM_PECHA_BERRY:
                    if (IsType(species, TYPE_POISON) || IsType(species, TYPE_STEEL) || ability == ABILITY_IMMUNITY || ability == ABILITY_COMATOSE)
                        holdItem = 0;
                    break;
                case ITEM_RAWST_BERRY:
                    if (IsType(species, TYPE_FIRE) || ability == ABILITY_WATER_VEIL || ability == ABILITY_COMATOSE)
                        holdItem = 0;
                    break;
                case ITEM_CHESTO_BERRY:
                    if (ability == ABILITY_COMATOSE || ability == ABILITY_VITAL_SPIRIT || ability == ABILITY_INSOMNIA)
                        holdItem = 0;
                    break;
                case ITEM_ASPEAR_BERRY:
                    if (ability == ABILITY_COMATOSE || ability == ABILITY_MAGMA_ARMOR)
                        holdItem = 0;
                    break;
                case ITEM_PERSIM_BERRY:
                    if (ability == ABILITY_COMATOSE || ability == ABILITY_OWN_TEMPO)
                        holdItem = 0;
                    break;
                case ITEM_ORAN_BERRY:
                    if (badgeCount >= 5)
                        holdItem = 0;
                    break;
                case ITEM_LUM_BERRY:
                    if (ability == ABILITY_COMATOSE)
                        holdItem = 0;
                    break;
                case ITEM_FIGY_BERRY:
                    if (GetFlavorRelationByPersonality(GetMonData(mon, MON_DATA_PERSONALITY, NULL), FLAVOR_SPICY) < 0)
                        holdItem = 0;
                    break;
                case ITEM_WIKI_BERRY:
                    if (GetFlavorRelationByPersonality(GetMonData(mon, MON_DATA_PERSONALITY, NULL), FLAVOR_DRY) < 0)
                        holdItem = 0;
                    break;
                case ITEM_MAGO_BERRY:
                    if (GetFlavorRelationByPersonality(GetMonData(mon, MON_DATA_PERSONALITY, NULL), FLAVOR_SWEET) < 0)
                        holdItem = 0;
                    break;
                case ITEM_AGUAV_BERRY:
                    if (GetFlavorRelationByPersonality(GetMonData(mon, MON_DATA_PERSONALITY, NULL), FLAVOR_BITTER) < 0)
                        holdItem = 0;
                    break;
                case ITEM_IAPAPA_BERRY:
                    if (GetFlavorRelationByPersonality(GetMonData(mon, MON_DATA_PERSONALITY, NULL), FLAVOR_SOUR) < 0)
                        holdItem = 0;
                    break;
                case ITEM_LANSAT_BERRY: // Critical up
                    if (!HasAttackingMove(moves))
                        holdItem = 0;
                    break;
                case ITEM_LIECHI_BERRY:
                    if (!HasMoveWithSplit(moves, SPLIT_PHYSICAL))
                        holdItem = 0;
                    break;
                case ITEM_PETAYA_BERRY:
                    if (!HasMoveWithSplit(moves, SPLIT_SPECIAL))
                        holdItem = 0;
                    break;
                }
            }
            else // try a battle item
            {
                holdItem = GetRandomRange(ITEM_BRIGHT_POWDER, ITEM_GRASSY_SEED);
                switch (holdItem)
                {
                // not allowed
                case ITEM_MACHO_BRACE:
                case ITEM_EXP_SHARE:
                case ITEM_AMULET_COIN:
                case ITEM_CLEANSE_TAG:
                case ITEM_SMOKE_BALL:
                case ITEM_OVAL_STONE:
                case ITEM_PROTECTOR:
                case ITEM_ELECTIRIZER:
                case ITEM_MAGMARIZER:
                case ITEM_DUBIOUS_DISC:
                case ITEM_REAPER_CLOTH:
                case ITEM_PRISM_SCALE:
                case ITEM_WHIPPED_DREAM:
                case ITEM_SACHET:
                case ITEM_POWER_BRACER:
                case ITEM_POWER_BELT:
                case ITEM_POWER_LENS:
                case ITEM_POWER_BAND:
                case ITEM_POWER_ANKLET:
                case ITEM_POWER_WEIGHT:
                case ITEM_EVERSTONE:
                case ITEM_FLOAT_STONE:
                    holdItem = 0;
                    break;
                case ITEM_CHOICE_BAND:
                case ITEM_MUSCLE_BAND:
                    if (!HasMoveWithSplit(moves, SPLIT_PHYSICAL))
                        holdItem = 0;
                    break;
                case ITEM_CHOICE_SPECS:
                case ITEM_WISE_GLASSES:
                    if (!HasMoveWithSplit(moves, SPLIT_SPECIAL))
                        holdItem = 0;
                    break;
                case ITEM_CHOICE_SCARF:
                case ITEM_SHELL_BELL:
                case ITEM_EXPERT_BELT:
                case ITEM_LIFE_ORB:
                case ITEM_ASSAULT_VEST:
                    if (!HasAttackingMove(moves))
                        holdItem = 0;
                    break;
                case ITEM_BLACK_BELT:
                case ITEM_BLACK_GLASSES:
                case ITEM_CHARCOAL:
                case ITEM_DRAGON_FANG:
                case ITEM_HARD_STONE:
                case ITEM_MAGNET:
                case ITEM_METAL_COAT:
                case ITEM_MIRACLE_SEED:
                case ITEM_MYSTIC_WATER:
                case ITEM_NEVER_MELT_ICE:
                case ITEM_POISON_BARB:
                case ITEM_SHARP_BEAK:
                case ITEM_SILVER_POWDER:
                case ITEM_SPELL_TAG:
                case ITEM_TWISTED_SPOON:
                    effect = ItemId_GetHoldEffect(holdItem);
                    for (j = 0; i < ARRAY_COUNT(gHoldEffectToType); j++)
                    {
                        if (effect == gHoldEffectToType[j][0])
                        {
                            if (!HasAttackingTypeMove(moves, gHoldEffectToType[j][1]))
                                holdItem = 0;
                            break;
                        }
                    }
                    break;
                case ITEM_BLACK_SLUDGE:
                    if (!IsType(species, TYPE_POISON))
                        holdItem = 0;
                case ITEM_LEFTOVERS:
                    if (species == SPECIES_SHEDINJA)
                        holdItem = 0;
                    break;
                case ITEM_WIDE_LENS:
                case ITEM_ZOOM_LENS:
                    if (ability == ABILITY_NO_GUARD || !HasMoveWithMissChance(moves))
                        holdItem = 0;
                    break;
                case ITEM_LIGHT_CLAY:
                    if (!HasMoveWithEffect(moves, EFFECT_REFLECT) && !HasMoveWithEffect(moves, EFFECT_LIGHT_SCREEN) && !HasMoveWithEffect(moves, EFFECT_AURORA_VEIL))
                        holdItem = 0;
                    break;
                case ITEM_ICY_ROCK:
                    if (ability != ABILITY_SNOW_WARNING && !HasMoveWithEffect(moves, EFFECT_HAIL))
                        holdItem = 0;
                    break;
                case ITEM_SMOOTH_ROCK:
                    if (ability != ABILITY_SAND_STREAM && !HasMoveWithEffect(moves, EFFECT_SANDSTORM))
                        holdItem = 0;
                    break;
                case ITEM_HEAT_ROCK:
                    if (ability != ABILITY_DROUGHT && !HasMoveWithEffect(moves, EFFECT_SUNNY_DAY))
                        holdItem = 0;
                    break;
                case ITEM_DAMP_ROCK:
                    if (ability != ABILITY_DRIZZLE && !HasMoveWithEffect(moves, EFFECT_RAIN_DANCE))
                        holdItem = 0;
                    break;
                case ITEM_TOXIC_ORB:
                    switch (ability)
                    {
                    case ABILITY_POISON_HEAL:
                        break;
                    case ABILITY_MAGIC_GUARD:
                    case ABILITY_GUTS:
                    case ABILITY_TOXIC_BOOST:
                        if (!HasAttackingMove(moves))
                            holdItem = 0;
                        break;
                    default:
                        holdItem = 0;
                        break;
                    }
                    break;
                case ITEM_FLAME_ORB:
                    switch (ability)
                    {
                    case ABILITY_MAGIC_GUARD:
                    case ABILITY_GUTS:
                    case ABILITY_FLARE_BOOST:
                        if (!HasAttackingMove(moves))
                            holdItem = 0;
                        break;
                    default:
                        holdItem = 0;
                        break;
                    }
                    break;
                case ITEM_STICKY_BARB:
                    if (ability != ABILITY_MAGIC_GUARD && Random() & 1)
                        holdItem = 0;
                    break;
                case ITEM_LAGGING_TAIL:
                case ITEM_IRON_BALL:
                case ITEM_RING_TARGET:
                    if (!HasMoveWithEffect(moves, EFFECT_TRICK))
                        holdItem = 0;
                    break;
                case ITEM_BIG_ROOT:
                    if (!HasMoveWithEffect(moves, EFFECT_LEECH_SEED) && !HasMoveWithEffect(moves, EFFECT_STRENGTH_SAP) && !HasMoveWithEffect(moves, EFFECT_ABSORB) && !HasMoveWithEffect(moves, EFFECT_AQUA_RING))
                        holdItem = 0;
                    break;
                case ITEM_EVIOLITE:
                    if (!CanEvolve(species))
                        holdItem = 0;
                    break;
                case ITEM_AIR_BALLOON:
                    if (IsType(species, TYPE_FLYING) || ability == ABILITY_LEVITATE)
                        holdItem = 0;
                    break;
                case ITEM_BINDING_BAND:
                case ITEM_GRIP_CLAW:
                    if (!HasMoveWithEffect(moves, EFFECT_TRAP))
                        holdItem = 0;
                    break;
                case ITEM_TERRAIN_EXTENDER:
                    if (ability != ABILITY_PSYCHIC_SURGE && !HasMoveWithEffect(moves, EFFECT_PSYCHIC_TERRAIN)
                        && ability != ABILITY_ELECTRIC_SURGE && !HasMoveWithEffect(moves, EFFECT_ELECTRIC_TERRAIN)
                        && ability != ABILITY_GRASSY_SURGE && !HasMoveWithEffect(moves, EFFECT_GRASSY_TERRAIN)
                        && ability != ABILITY_MISTY_SURGE && !HasMoveWithEffect(moves, EFFECT_MISTY_TERRAIN))
                        holdItem = 0;
                    break;
                case ITEM_PSYCHIC_SEED:
                    if (ability != ABILITY_PSYCHIC_SURGE && !HasMoveWithEffect(moves, EFFECT_PSYCHIC_TERRAIN))
                        holdItem = 0;
                    break;
                case ITEM_ELECTRIC_SEED:
                    if (ability != ABILITY_ELECTRIC_SURGE && !HasMoveWithEffect(moves, EFFECT_ELECTRIC_TERRAIN))
                        holdItem = 0;
                    break;
                case ITEM_GRASSY_SEED:
                    if (ability != ABILITY_GRASSY_SURGE && !HasMoveWithEffect(moves, EFFECT_GRASSY_TERRAIN))
                        holdItem = 0;
                    break;
                case ITEM_MISTY_SEED:
                    if (ability != ABILITY_MISTY_SURGE && !HasMoveWithEffect(moves, EFFECT_MISTY_TERRAIN))
                        holdItem = 0;
                    break;
                }
            }

            if (holdItem != 0)
                return holdItem;
        }
    }

    return 0;
}

static void TrySetMonMoves(struct Pokemon *mon, const u16 *moves)
{
    int i;
    if (moves)
    {
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            if (moves[i] != 0)
            {
                SetMonData(mon, MON_DATA_MOVE1 + i, &moves[i]);
                SetMonData(mon, MON_DATA_PP1 + i, &gBattleMoves[moves[i]].pp);
            }
        }
    }
}

static void CreateTrainerMon(struct Pokemon *mon, u32 trainerId, u32 species, u32 level, u32 holdItem, u32 fixedIvs, const u16 *moves)
{
    struct IdVal stats[NUM_STATS];
    u32 nature, difficulty = gSaveBlock2Ptr->optionsDifficulty;
    int i, value, totalEvs = 0, badgeCount = CountBadges(trainerId);

    if (difficulty-- != DIFFICULTY_EASY)
    {
        RankBaseStats(stats, species);
        // Choose nature
        if (PercentChance(sNatureOdds[difficulty] + (badgeCount * 6)))
        {
            // Get difference between best and worst stat except HP
            int bestId, worstId;
            for (i = 0; i < NUM_STATS; i++)
            {
                if (stats[i].id != STAT_HP)
                {
                    bestId = i;
                    break;
                }
            }
            for (i = NUM_STATS - 1; i >= 0; i--)
            {
                if (stats[i].id != STAT_HP)
                {
                    worstId = i;
                    break;
                }
            }

            if (stats[bestId].value - stats[worstId].value <= 40) // If best and worst are similar choose a neutral nature
            {
                nature = (Random() % 5) * NATURE_DOCILE;
            }
            else // Find best+, worst-
            {
                for (i = 0; i < NATURE_QUIRKY; i++)
                {
                    if (gNatureStatTable[i][stats[bestId].id - 1] == +1 && gNatureStatTable[i][stats[worstId].id - 1] == -1)
                        break;
                }
                nature = i;
            }
        }
        else
        {
            nature = (Random() % NATURE_QUIRKY);
        }

        CreateMonWithNature(mon, species, GetTrainerMonLevel(trainerId, level), 0, nature);
        // Set ivs and evs
        for (i = 0; i < NUM_STATS; i++)
        {
            if (PercentChance(sIvsOdds[difficulty][i]))
                value = sIvsRanges[difficulty][badgeCount][1];
            else
                value = GetRandomRange(sIvsRanges[difficulty][badgeCount][0], sIvsRanges[difficulty][badgeCount][1]);

            SetMonData(mon, MON_DATA_HP_IV + stats[i].id, &value);

            if (totalEvs < MAX_TOTAL_EVS)
            {
                value = GetRandomRange(sEvsRanges[difficulty][badgeCount][0], sEvsRanges[difficulty][badgeCount][1]);
                if (value + totalEvs > MAX_TOTAL_EVS)
                    value = MAX_TOTAL_EVS - totalEvs;

                totalEvs += value;
                SetMonData(mon, MON_DATA_HP_EV + stats[i].id, &value);
            }
        }

        CalculateMonStats(mon);
        TrySetMonMoves(mon, moves);
        holdItem = ChooseHeldItem(mon, species, trainerId, difficulty, badgeCount);
        if (holdItem)
            SetMonData(mon, MON_DATA_HELD_ITEM, &holdItem);
    }
    else // Regular Emerald Mom
    {
        CreateMon(mon, species, level, fixedIvs, 0, 0, OT_ID_RANDOM_NO_SHINY, 0);
        if (holdItem)
            SetMonData(mon, MON_DATA_HELD_ITEM, &holdItem);
        TrySetMonMoves(mon, moves);
    }

}

void CreateTrainersMons(struct Pokemon *party, u32 trainerId, int monsCount)
{
    int i, fixedIV;

    sHasMegaStone = FALSE;
    for (i = 0; i < monsCount; i++)
    {
        switch (gTrainers[trainerId].partyFlags)
        {
        case 0:
        {
            const struct TrainerMonNoItemDefaultMoves *partyData = gTrainers[trainerId].party.NoItemDefaultMoves;

            fixedIV = partyData[i].iv * 31 / 255;
            CreateTrainerMon(&party[i], trainerId, partyData[i].species, partyData[i].lvl, 0, fixedIV, NULL);
            break;
        }
        case F_TRAINER_PARTY_CUSTOM_MOVESET:
        {
            const struct TrainerMonNoItemCustomMoves *partyData = gTrainers[trainerId].party.NoItemCustomMoves;

            fixedIV = partyData[i].iv * 31 / 255;
            CreateTrainerMon(&party[i], trainerId, partyData[i].species, partyData[i].lvl, 0, fixedIV, partyData[i].moves);
            break;
        }
        case F_TRAINER_PARTY_HELD_ITEM:
        {
            const struct TrainerMonItemDefaultMoves *partyData = gTrainers[trainerId].party.ItemDefaultMoves;

            fixedIV = partyData[i].iv * 31 / 255;
            CreateTrainerMon(&party[i], trainerId, partyData[i].species, partyData[i].lvl, partyData[i].heldItem, fixedIV, NULL);
            break;
        }
        case F_TRAINER_PARTY_CUSTOM_MOVESET | F_TRAINER_PARTY_HELD_ITEM:
        {
            const struct TrainerMonItemCustomMoves *partyData = gTrainers[trainerId].party.ItemCustomMoves;

            fixedIV = partyData[i].iv * 31 / 255;
            CreateTrainerMon(&party[i], trainerId, partyData[i].species, partyData[i].lvl, partyData[i].heldItem, fixedIV, partyData[i].moves);
            break;
        }
        }
    }
}

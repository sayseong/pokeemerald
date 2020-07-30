#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <dirent.h>

                                /* EDIT THESE */

// Abilities config.
#define RANDOMIZE_ABILITIES             true    // Randomizes abilities if true, does not if false.

#define ALL_MONS_TWO_ABILITIES          true    // If true, all pokemon are going to have two different abilities. If false, pokemon which previously had only one ability, remain with only one.
#define ALLOW_WONDER_GUARD              false   // If false, no pokemon are allowed to have wonder guard. If true...well, you better not encounter a pokemon with this ability in the wild.
#define NO_FORM_ABILITIES               true    // If true, pokemon won't have abilities such as Schooling, Zen Mode, etc, which only work on specific species. Recommended to keep true, otherwise pokemon may end up with useless abilities.

// Learned moves config.
#define RANDOMIZE_MOVES                 true    // Randomizes learned moves if true, does not if false.

#define ALLOW_REPEATED_MOVES            false   // If true, it's possible for a pokemon to learn the same move at multiple levels. Recommended to keep false, as it gives more variety to learnsets.
#define ALLOW_LEARN_HMS                 false   // If true, pokemon may randomly learn various HMs.

// Wild mons config.
#define RANDOMIZE_WILD                  true    // Randomizes wild pokemon if true, does not if false.

#define SAME_WILD_MONS_COUNT            true    // If true, it changes all species in one area to another species. For example in route 101 all Poochies become Dustox. If false, each map may have the maximum number(12 for land grass) of DIFFERENT wild mons available.
#define SANE_WATER_MONS                 true    // If true, wild water mons(surf/fishing) will only be of water type. If false, you can encounter anything in the water.
#define SAME_WILD_BASE_STATS            true    // If true, each wild pokemon can only be substituted to another in range specified below. Setting it to true allows for a more balanced randomness. If false, it's possible to have third forms mons in the beginning of the game.
#define SAME_WILD_STATS_RANGE           50      // Extension of the above. Range of 50 means that a pokemon with BST of 300 can be replaced with another one with BST in range 250-350. Note, don't go lower than 19, because it may be impossible to find valid mons then.

// Trainer Mons
#define RANDOMIZE_TRAINERS              true    // If true, randomizes trainers' pokemons.

#define RANDOMIZE_TRAINER_MOVES         false   // If true, randomizes not only trainers' pokemon, but also their mons' moves(for trainers whose pokemon have specific mpves).
#define SAME_TRAINER_BASE_STATS         true    // The same as with wild mons, new random mons will have similar base stat total if true.
#define SAME_TRAINER_STATS_RANGE_LOW    25      // Same as with wild mons, but low range. High value -> weaker mons allowed
#define SAME_TRAINER_STATS_RANGE_HIGH   100     // Same as with wild mons, but high range. High value -> stronger mons allowed
#define SAME_TRAINERS_TYPES             true    // If true, the new random pokemon will share a type with the original mon. Note, this is not fault-proof as for example a bug catcher may end up with a pure poison pokemon, because it had a bug-poison before. Still offers at least some control over random types.

// Starter mons
#define RANDOMIZE_STARTERS              true
#define STARTERS_KEEP_TYPE_TRIANGLE     true    // Each is weak against one and strong against other
#define STARTERS_HAS_TO_EVOLVE          true    // If true, each starter has to be able to evolve at least once
#define STARTERS_CANT_BE_EVOLVED        true    // If true, each starter cannot be an already evolved pokemon

// Items
#define RANDOMIZE_ITEMS                 true    // Randomizes items received from people, or found in pokeballs. Key items and TMs/HMs excluded.
#define RANDOMIZE_HIDDEN_ITEMS          true    // Same as above, but with the hidden items.

                                /* STOP EDITING */

#define ARRAY_COUNT(array) (size_t)(sizeof(array) / sizeof((array)[0]))

// Forward declarations.

uint32_t ConstantNameToArrId(char *name, uint32_t count, uint32_t constantsId);
bool ModifyAbilitiesInBaseStats(FILE *file, uint32_t *constantCounts, char *allocStr);
bool ModifyLearnsets(FILE *file, uint32_t *constantCounts, char *allocStr);
bool ModifyWildMons(FILE *file, uint32_t *constantCounts, char *allocStr);
bool ModifyBallItems(FILE *file, uint32_t *constantCounts, char *allocStr);
bool ModifyStarters(FILE *file, uint32_t *constantCounts, char *allocStr);

enum
{
    CONSTANTS_ABILITIES,
    CONSTANTS_MOVES,
    CONSTANTS_SPECIES,
    CONSTANTS_ITEMS,
    CONSTANTS_COUNT,
};


#define STR_MAX_LEN     30 // Max string length to deal with.
#define CHR_BUFF_BIG    0x500
#define CHR_BUFF_SMALL  0x100

struct ConstantIdName
{
    char *str;
    int id;
};

static struct ConstantIdName sMoveConstants[1000] = {};
static struct ConstantIdName sAbilityConstants[1000] = {};
static struct ConstantIdName sSpeciesConstants[1000] = {};
static struct ConstantIdName sItemsConstants[1000] = {};

static char sMonTypes[1000][2][20] = {};
static uint16_t sMonBaseStats[1000] = {0};

struct
{
    uint16_t evoTo;
    uint16_t evoFrom;
    uint16_t canMegaEvo;
} static sEvoData[1000] = {0};

static const char *const sFormAbilities[] =
{
    "ABILITY_BATTLE_BOND",
    "ABILITY_DISGUISE",
    "ABILITY_FLOWER_GIFT",
    "ABILITY_FORECAST",
    "ABILITY_MULTITYPE",
    "ABILITY_POWER_CONSTRUCT",
    "ABILITY_RKS_SYSTEM",
    "ABILITY_SCHOOLING",
    "ABILITY_SHIELDS_DOWN",
    "ABILITY_STANCE_CHANGE",
    "ABILITY_ZEN_MODE",
};

static const char *const sHMMoves[] =
{
    "MOVE_CUT",
    "MOVE_SURF",
    "MOVE_WATERFALL",
    "MOVE_STRENGTH",
    "MOVE_FLASH",
    "MOVE_DIVE",
    "MOVE_FLY",
    "MOVE_ROCK_SMASH",
};

static const char sBaseStatsDir[] = "src/data/pokemon/base_stats.h";
static const char sLearnsetsDir[] = "src/data/pokemon/level_up_learnsets.h";
static const char sEvolutionDir[] = "src/data/pokemon/evolution.h";
static const char sTrainerPartiesDir[] = "src/data/trainer_parties.h";
static const char sWildMonsDir[] = "src/data/wild_encounters.json";
static const char sItemBallsDir[] = "data/scripts/item_ball_scripts.inc";
static const char sAbilitiesDir[] = "include/constants/abilities.h";
static const char sMovesDir[] = "include/constants/moves.h";
static const char sSpeciesDir[] = "include/constants/species.h";
static const char sItemsDir[] = "include/constants/items.h";
static const char sStarterChooseDir[] = "src/starter_choose.c";
static const char sTempPokemonDir[] = "src/data/pokemon/temp.txt";

static const char *const sTriangleTypes[][3] =
{
    {"TYPE_GRASS", "TYPE_FIRE", "TYPE_WATER"},
    {"TYPE_FIGHTING", "TYPE_PSYCHIC", "TYPE_DARK"},
    {"TYPE_GROUND", "TYPE_GRASS", "TYPE_POISON"},
    {"TYPE_FIRE", "TYPE_ROCK", "TYPE_STEEL"},
    {"TYPE_FLYING", "TYPE_ROCK", "TYPE_FIGHTING"},
};

struct
{
    const char *const dir;
    const char *const prefix;
    uint32_t prefixLen;
    const char *const msgPrefix;
    struct ConstantIdName *structPtr;
}
static const sConstantsData[CONSTANTS_COUNT] =
{
    [CONSTANTS_ABILITIES]   = {sAbilitiesDir,   "ABILITY_", ARRAY_COUNT("ABILITY_"),    "Abilities",    sAbilityConstants},
    [CONSTANTS_MOVES]       = {sMovesDir,       "MOVE_",    ARRAY_COUNT("MOVE_"),       "Moves",        sMoveConstants},
    [CONSTANTS_SPECIES]     = {sSpeciesDir,     "SPECIES_", ARRAY_COUNT("SPECIES_"),    "Species",      sSpeciesConstants},
    [CONSTANTS_ITEMS]       = {sItemsDir,       "ITEM_",    ARRAY_COUNT("ITEM_"),       "Items",        sItemsConstants},
};

struct
{
    const char *const dir;
    const char *const name;
    const char *const randomizedThingName;
    bool toEdit;
    bool (*func)(FILE *, uint32_t *count, char *buffer);
}
static const sFilesToEdit[] =
{
    {sBaseStatsDir, "Base Stats", "abilities", RANDOMIZE_ABILITIES, ModifyAbilitiesInBaseStats},
    {sLearnsetsDir, "Learnsets", "learnsets", RANDOMIZE_MOVES, ModifyLearnsets},
    {sWildMonsDir, "Wild Mons", "wild encounters", RANDOMIZE_WILD, ModifyWildMons},
#if (RANDOMIZE_HIDDEN_ITEMS == true)
    {sItemBallsDir, "Items",  "items found in poke balls and hidden items", RANDOMIZE_ITEMS, ModifyBallItems},
#else
    {sItemBallsDir, "Item Balls",  "items found in poke balls", RANDOMIZE_ITEMS, ModifyBallItems},
    #endif
    {sStarterChooseDir, "starter_choose.c", "starter mons", RANDOMIZE_STARTERS, ModifyStarters},
};

#define SKIP_WHTSPACE(str) {while (*str == ' ' || *str == '\t') str++;}
#define SKIP_TILL(str, c) {while (*str != c) str++;}
#define SAME_STRINGS(str1, str2)((strcmp(str1, str2) == 0))
#define RAND_ID(count)(((rand() % (count - 1)) + 1))

uint32_t BeginsWithStr(char **str, const char *toCmpTo, bool advanceCursor)
{
    uint32_t i;
    for (i = 0; i < toCmpTo[i] != '\0'; i++)
    {
        if ((*str)[i] != toCmpTo[i])
            break;
    }
    if (toCmpTo[i] == '\0')
    {
        if (advanceCursor)
            *str += i;
        return i;
    }
    return 0;
}

uint32_t CopyTill(char *dst, const char *src, char c)
{
    uint32_t i;
    for (i = 0; src[i] != c; i++)
        dst[i] = src[i];
    dst[i] = '\0';
    return i;
}

uint32_t CopyTillNotPrintable(char *dst, const char *src, bool allowSpace)
{
    uint32_t i;
    if (allowSpace)
    {
        for (i = 0; isprint(src[i]); i++)
            dst[i] = src[i];
    }
    else
    {
        for (i = 0; isprint(src[i]) && src[i] != ' '; i++)
            dst[i] = src[i];
    }
    dst[i] = '\0';
    return i;
}

uint32_t GetDefines(FILE *file, const char *prefix, struct ConstantIdName *constants)
{
    char *str, (*allocChars)[][STR_MAX_LEN], num[5], *allocStr;
    uint32_t count = 0, i;

    allocChars = malloc(STR_MAX_LEN * CHR_BUFF_BIG);
    allocStr = malloc(CHR_BUFF_BIG);
    if (allocChars == NULL || allocStr == NULL)
        return 0;

    while (1)
    {
        str = allocStr;
        if (fgets(str, CHR_BUFF_BIG, file) == NULL)
            break;

        // Ignore forms, unowns, megas, etc.
        if (strstr(str, "SPECIES_EGG ") || strstr(str, "SPECIES_MEGA_"))
            break;

        SKIP_WHTSPACE(str);
        if (!BeginsWithStr(&str, "#define", true))
            continue;

        SKIP_WHTSPACE(str);
        //we're at #define _____ Get define name, and make sure the id does not repeat

        // Make sure it starts with ABILITY_, ITEM_ or MOVE_
        if (!BeginsWithStr(&str, prefix, false))
            continue;

        // Ignore all entries with COUNT in it
        if (strstr(str, "_COUNT_") || strstr(str, "_COUNT "))
            continue;

        // Copy constant define name.
        i = CopyTill((*allocChars)[count], str, ' ');

        // Get id
        str += i;
        SKIP_WHTSPACE(str);

        // If it's not a number, ignore
        if (!isdigit(*str))
            continue;

        for (i = 0; str[i] != '\0' && str[i] != '\n'; i++)
        {
            if (!isdigit(str[i]))
                break;
        }
        str[i+1] = '\0';

        constants[count].str = (*allocChars)[count];
        if (SAME_STRINGS(constants[count].str, "ITEM_MACH_BIKE")) // Don't count key items and tms as valid items
            break;
        constants[count].id = atoi(str);
        // Check if the id was used before
        for (i = 0; i < count; i++)
        {
            if (constants[count].id == constants[i].id)
                break;
        }
        if (i != count)
            continue;
        count++;
    }
    return count;
}

bool IsStringInList(const char *const *const names, uint32_t count, char *str)
{
    uint32_t i;
    for (i = 0; i < count; i++)
    {
        if (SAME_STRINGS(str, names[i]))
            return true;
    }
    return false;
}

bool IsValueInList(const uint16_t *list, uint32_t count, uint32_t val)
{
    uint32_t i;
    for (i = 0; i < count; i++)
    {
        if (list[i] == val)
            return true;
    }
    return false;
}

char *RandomizeAbility(uint32_t count)
{
    char *ret = NULL;
    do
    {
        ret = sAbilityConstants[RAND_ID(count)].str;
    } while (SAME_STRINGS(ret, "ABILITY_CACOPHONY")
             || (!ALLOW_WONDER_GUARD && SAME_STRINGS(ret, "ABILITY_WONDER_GUARD"))
             || (NO_FORM_ABILITIES && IsStringInList(sFormAbilities, ARRAY_COUNT(sFormAbilities), ret))
             );

    return ret;
}

void GatherMonBaseStatData(FILE *baseStatFile, uint32_t speciesCount)
{
    char *allocStr = malloc(CHR_BUFF_BIG), *str, bsString[5];
    bool speciesFound = false, rewinded = false;
    uint32_t i, j, speciesId, typeFound = 0, statsFound = 0, bsCharId;
    if (allocStr == NULL || baseStatFile == NULL)
        return;

    rewind(baseStatFile);
    for (speciesId = 1; speciesId < speciesCount; speciesId++)
    {
        // Find wanted species
        while (1)
        {
            str = allocStr;
            if (fgets(str, CHR_BUFF_BIG, baseStatFile) == NULL)
            {
                if (rewinded)
                    break;
                // Hmm, we need to start searching from the beginning.
                // This should happen only if the order of mon is not the species order.
                rewinded = true;
                rewind(baseStatFile);
                continue;
            }

            SKIP_WHTSPACE(str);
            if (speciesFound)
            {
                if (*str == '.') // Struct field
                {
                    // Check if type
                    if (BeginsWithStr(&str, ".type1", false))
                        typeFound++;
                    else if (BeginsWithStr(&str, ".type2", false))
                        typeFound++;

                    if (typeFound != 0) // Is type
                    {
                        // Get things from = to comma
                        while (*(str++) != '=')
                            ;
                        SKIP_WHTSPACE(str);

                        for (i = 0; str[i] != ',' && str[i] != ' '; i++)
                            sMonTypes[speciesId][typeFound - 1][i] = str[i];
                        sMonTypes[speciesId][typeFound - 1][i] = '\0';
                    }
                    else // Check if it's base stat
                    {
                        static const char *statNames[] = {".baseHP", ".baseAttack", ".baseDefense", ".baseSpeed", ".baseSpAttack", ".baseSpDefense"};
                        for (i = 0; i < ARRAY_COUNT(statNames); i++)
                        {
                            if (BeginsWithStr(&str, statNames[i], false))
                            {
                                // Get things from = to comma
                                while (*(str++) != '=')
                                    ;
                                SKIP_WHTSPACE(str);

                                for (bsCharId = 0, i = 0; str[i] != ',' && str[i] != ' '; i++)
                                {
                                    if (isdigit(str[i]))
                                        bsString[bsCharId++] = str[i];
                                }
                                bsString[bsCharId++] = '\0';
                                sMonBaseStats[speciesId] += atoi(bsString);
                                statsFound++;
                                break;
                            }
                        }
                    }
                    // Two types found, increment species.
                    if (typeFound == 2 && statsFound == 6)
                    {
                        speciesFound = false;
                        break;
                    }
                }
            }
            else if (strstr(str, sSpeciesConstants[speciesId].str))
            {
                speciesFound = true;
                typeFound = 0, statsFound = 0;
            }
        }
    }
}

void GatherEvolutionData(FILE *evoDataFile, uint32_t speciesCount)
{
    char *allocStr = malloc(CHR_BUFF_BIG), *str, name[20];
    uint32_t i, id;
    if (allocStr == NULL || evoDataFile == NULL)
        return;

    while (1)
    {
        str = allocStr;
        if (fgets(str, CHR_BUFF_BIG, evoDataFile) == NULL)
            break;

        SKIP_WHTSPACE(str);
        if (*(str++) == '[') // Get species id
        {
            // Copy species which has evolutions name
            i = CopyTill(name, str, ']');
            id = ConstantNameToArrId(name, speciesCount, CONSTANTS_SPECIES);
            if (id == 0)
                continue;

            // Find evo method
            for (str += i; *str != '\0'; str++)
            {
                if (BeginsWithStr(&str, "EVO_", true))
                {
                    if (BeginsWithStr(&str, "MEGA", false))
                    {
                        sEvoData[id].canMegaEvo = 1;
                    }
                    else // Regular evolution
                    {
                        // Find species which they evolve into
                        for (; *str != '\0'; str++)
                        {
                            if (*str == 'S' && BeginsWithStr(&str, "SPECIES_", false))
                            {
                                char *namePtr = name;
                                for (i = 0; str[i] != '\0'; i++)
                                {
                                    if (str[i] == ' ')
                                        continue;
                                    if (str[i] == '}')
                                    {
                                        *(namePtr++) = '\0';
                                        sEvoData[id].evoTo = ConstantNameToArrId(name, speciesCount, CONSTANTS_SPECIES);
                                        sEvoData[sEvoData[id].evoTo].evoFrom = id;
                                        goto LOOP_CONTINUE;
                                    }
                                    if (str[i] == ',') // Species was used as an argument, abort
                                    {
                                        str += i - 1;
                                        break;
                                    }
                                    *(namePtr++) = str[i];
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
    LOOP_CONTINUE:
        ;
    }
}

uint32_t ConstantNameToArrId(char *name, uint32_t count, uint32_t constantsId)
{
    uint32_t i;
    char *str = name;

    // All constants begin with a prefix, so skip past that.
    BeginsWithStr(&str, sConstantsData[constantsId].prefix, true);
    for (i = 1; i < count; i++)
    {
        if (SAME_STRINGS(str, sConstantsData[constantsId].structPtr[i].str + (sConstantsData[constantsId].prefixLen - 1)))
            return i;
    }
    return 0;
}

bool IsInBSRange(char *oldName, uint32_t newId, uint32_t speciesCount, uint32_t rangeLow, uint32_t rangeHigh)
{
    uint32_t oldId = ConstantNameToArrId(oldName, speciesCount, CONSTANTS_SPECIES);
    //printf("\nComparing %s %u with %s %u.", oldName, sMonBaseStats[oldId], sSpeciesConstants[newId].str, sMonBaseStats[newId]);
    if (oldId != 0)
    {
        if (sMonBaseStats[newId] < sMonBaseStats[oldId])
        {
            if (sMonBaseStats[newId] + rangeLow >= sMonBaseStats[oldId])
                return true;
        }
        else
        {
            if (sMonBaseStats[newId] - rangeHigh <= sMonBaseStats[oldId])
                return true;
        }
    }
    return false;
}

bool ShareType(uint32_t id1, uint32_t id2)
{
    uint32_t i;
    for (i = 0; i < 2; i++)
    {
        if (SAME_STRINGS(sMonTypes[id1][i], sMonTypes[id2][0])
            || SAME_STRINGS(sMonTypes[id1][i], sMonTypes[id2][1]))
            return true;
    }
    return false;
}

enum
{
    WILD_ENCOUNTERS = 1,
    WILD_LAND,
    WILD_SURF,
    WILD_FISH,
    WILD_SMASH,
};

struct WildChange
{
    char old[50];
    char new[50];
};

void UpdateFile(FILE *file, FILE *tempFile, const char *fileDir, const char *tempDir)
{
    fclose(file);
    fclose(tempFile);
    remove(fileDir);
    rename(tempDir, fileDir);
}

bool ModifyWildMons(FILE *file, uint32_t *constantCounts, char *allocStr)
{
    FILE *dstFile;
    char *str, oldName[50], *newName;
    struct WildChange *mons = malloc(sizeof(struct WildChange) * 15);
    uint16_t speciesInMap[15], inMapSpeciesId = 0;
    uint32_t i, id, mapMonsCount = 0, state = 0, count = constantCounts[CONSTANTS_SPECIES];

    if (allocStr == NULL || mons == NULL)
        return false;

    dstFile = fopen(sTempPokemonDir, "w+");
    if (dstFile == NULL)
        return false;

    while (1)
    {
        str = allocStr;
        if (fgets(str, CHR_BUFF_BIG, file) == NULL)
            break;

        if (state == 0)
        {
            // Find encounters
            if (strstr(str, "encounters"))
                state = WILD_ENCOUNTERS;
        }
        else if (state == WILD_ENCOUNTERS)
        {
            static const char *locations[] =
            {
                "land_mons", "water_mons", "fishing_mons", "rock_smash_mons"
            };
            for (i = 0; i < ARRAY_COUNT(locations); i++)
            {
                if (strstr(str, locations[i]))
                    state = WILD_LAND + i;
            }
        }
        else
        {
            // ], means we're over all mons
            if (strstr(str, "]"))
            {
                state = WILD_ENCOUNTERS;
                mapMonsCount = 0, inMapSpeciesId = 0;
            }
            else if (strstr(str, "species")) // Change species in the map
            {
                // Get old mon name.
                for (i = 0; str[i] != '\0' && str[i] != '\n'; i++)
                {
                    if (str[i] == ':' && str[i + 1] == ' ' && str[i + 2] == '"')
                    {
                        str += i + 3;
                        for (i = 0; str[i] != '"' && str[i] != '\n' && str[i] != '\0'; i++)
                            oldName[i] = str[i];
                        oldName[i] = '\0';
                        break;
                    }
                }

                // Randomize a wild mon
                do
                {
                    id = RAND_ID(count);
                    newName = sSpeciesConstants[id].str;
                } while ((SANE_WATER_MONS && (state == WILD_FISH || state == WILD_SURF)
                            && (!SAME_STRINGS(sMonTypes[id][0], "TYPE_WATER") && !SAME_STRINGS(sMonTypes[id][1], "TYPE_WATER")))
                         || (SAME_WILD_BASE_STATS && !IsInBSRange(oldName, id, count, SAME_WILD_STATS_RANGE, SAME_WILD_STATS_RANGE))
                         || (SAME_WILD_MONS_COUNT && IsValueInList(speciesInMap, inMapSpeciesId, id))) // This is to make sure, a different species doesn't become the same as the previous one.
                         ;

                if (SAME_WILD_MONS_COUNT)
                {
                    for (i = 0; i < mapMonsCount; i++)
                    {
                        if (SAME_STRINGS(mons[i].old, oldName))
                        {
                            newName = mons[i].new;
                            break;
                        }
                    }
                    // A new substitute mon
                    if (i == mapMonsCount)
                    {
                        strcpy(mons[mapMonsCount].old, oldName);
                        strcpy(mons[mapMonsCount].new, newName);
                        mapMonsCount++;
                    }
                }
                speciesInMap[inMapSpeciesId++] = ConstantNameToArrId(newName, count, CONSTANTS_SPECIES);
                fprintf(dstFile, "                \"species\": \"%s\"\n", newName);
                continue;
            }
        }

        fputs(allocStr, dstFile);
    }

    UpdateFile(file, dstFile, sWildMonsDir, sTempPokemonDir);
    free(mons);
    return true;
}

void ClearConsole(int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        putchar(8); // Back space
        putchar(' ');
        putchar(8);
    }
}

bool ModifyHiddenItems(uint32_t itemsCount, char *allocStr)
{
    int state, oldId, newId, i, prevCharsCount = 0;
    bool hasHidden;
    struct dirent *entry;
    FILE *mapFile;
    char oldName[50];
    DIR *dirAllMaps = opendir("data/maps"), *dirMap;
    char *dirName = malloc(CHR_BUFF_BIG);
    static const char text[] = "   Randomizing hidden items in  ";

    if (dirAllMaps == NULL || dirName == NULL)
        return false;

    // Loop through all directories.
    while ((entry = readdir(dirAllMaps)))
    {
        sprintf(dirName, "data/maps/%s/map.json", entry->d_name);
        if ((mapFile = fopen(dirName, "r+")))
        {
            FILE *dstFile = fopen(sTempPokemonDir, "w+");
            state = 0;
            hasHidden = false;
            while (1)
            {
                char *str = allocStr;
                if (fgets(str, CHR_BUFF_BIG, mapFile) == NULL)
                    break;
                SKIP_WHTSPACE(str);

                switch (state)
                {
                case 0: // Search for "hidden_item"
                    if (strstr(str, "\"hidden_item\""))
                        state++;
                    break;
                case 1: // Search for item field
                    if (BeginsWithStr(&str, "\"item\"", true))
                    {
                        state = 0;
                        SKIP_TILL(str, '"');
                        CopyTill(oldName, ++str, '"');
                        // We found a valid item
                        if ((oldId = ConstantNameToArrId(oldName, itemsCount, CONSTANTS_ITEMS)))
                        {
                            do
                            {
                                newId = RAND_ID(itemsCount);
                            } while (newId == oldId);
                            fprintf(dstFile, "      \"item\": \"%s\",\n", sItemsConstants[newId].str);
                            hasHidden = true;
                            continue;
                        }
                    }
                    break;
                }
                fputs(allocStr, dstFile);
            }
            // Update files
            fclose(dstFile);
            fclose(mapFile);
            if (hasHidden)
            {
                remove(dirName);
                rename(sTempPokemonDir, dirName);
                if (prevCharsCount == 0)
                    printf(text);
                else
                    ClearConsole(prevCharsCount);

                prevCharsCount = strlen(dirName);
                printf(dirName);
            }
        }
    }
    closedir(dirAllMaps);
    free(dirName);
    ClearConsole(prevCharsCount + ARRAY_COUNT(text) - 1);
    return true;
}

bool ModifyBallItems(FILE *file, uint32_t *constantCounts, char *allocStr)
{
    FILE *dstFile;
    char *str, name[25];
    uint32_t oldId, newId, itemsCount = constantCounts[CONSTANTS_ITEMS];

    dstFile = fopen(sTempPokemonDir, "w+");
    if (dstFile == NULL || allocStr == NULL)
        return false;

    while (1)
    {
        str = allocStr;
        if (fgets(str, CHR_BUFF_BIG, file) == NULL)
            break;
        SKIP_WHTSPACE(str);
        if (BeginsWithStr(&str, "finditem", true))
        {
            SKIP_WHTSPACE(str);
            CopyTillNotPrintable(name, str, false);
            // Check if not a key item or tm
            oldId = ConstantNameToArrId(name, itemsCount, CONSTANTS_ITEMS);
            if (oldId != 0)
            {
                do
                {
                    newId = RAND_ID(itemsCount);
                } while (newId == oldId);
                fprintf(dstFile, "	finditem %s\n", sItemsConstants[newId].str);
                continue;
            }
        }
        fputs(allocStr, dstFile);
    }

    UpdateFile(file, dstFile, sItemBallsDir, sTempPokemonDir);
    if (RANDOMIZE_HIDDEN_ITEMS)
        ModifyHiddenItems(itemsCount, allocStr);
    return true;
}

bool ModifyAbilitiesInBaseStats(FILE *file, uint32_t *constantCounts, char *allocStr)
{
    FILE *dstFile;
    uint32_t i, count = constantCounts[CONSTANTS_ABILITIES];
    bool theSameAbilities;
    char *newAbility1, *newAbility2;

    char *ability1 = malloc(CHR_BUFF_SMALL), *ability2 = malloc(CHR_BUFF_SMALL);
    char *str;
    if (allocStr == NULL || ability1 == NULL || ability2 == NULL)
        return false;

    dstFile = fopen(sTempPokemonDir, "w+");
    if (dstFile == NULL || file == NULL)
        return false;

    while (1)
    {
        str = allocStr;
        if (fgets(str, CHR_BUFF_BIG, file) == NULL)
            break;

        SKIP_WHTSPACE(str);
        // Search for .abilities
        while (*str != '\0' && *str != '\n')
        {
            if (*str == '.') // Field begins there
            {
                if (BeginsWithStr(&str, ".abilities", true))
                {
                    // We're at .abilities
                    SKIP_TILL(str, '{');
                    *str = '\0';
                    fprintf(dstFile, allocStr); // Copy everything excetp {ability1, ability 2},
                    str++;

                    theSameAbilities = false;
                    // Find whether we want two different abilities.
                    if (!ALL_MONS_TWO_ABILITIES)
                    {
                        SKIP_WHTSPACE(str);
                        // Copy first ability
                        for (i = 0; str[i] != ',' && str[i] != '}' && str[i] != ' '; i++)
                            ability1[i] = str[i];
                        ability1[i] = '\0';

                        SKIP_WHTSPACE(str);

                        if (str[i] == ',') // Copy second ability
                        {
                            str += i + 1;
                            SKIP_WHTSPACE(str);

                            for (i = 0; str[i] != ',' && str[i] != '}' && str[i] != ' '; i++)
                                ability2[i] = str[i];
                            ability2[i] = '\0';

                            if (ability2[0] == '0' || SAME_STRINGS(ability1, ability2) || strstr(ability2, "NONE") != NULL)
                                theSameAbilities = true;
                        }
                        else
                        {
                            theSameAbilities = true;
                        }
                    }

                    newAbility1 = RandomizeAbility(count);
                    if (!theSameAbilities)
                    {
                        do
                        {
                            newAbility2 = RandomizeAbility(count);
                        } while (newAbility1 == newAbility2);
                    }
                    else
                    {
                        newAbility2 = newAbility1;
                    }
                    fprintf(dstFile, "{%s, %s},\n", newAbility1, newAbility2);
                    goto LOOP_END;
                }

                // Different struct field, abort line
                break;
            }
            str++;
        }

        // Just copy the line
        fputs(allocStr, dstFile);
    LOOP_END:
        ;
    }

    UpdateFile(file, dstFile, sBaseStatsDir, sTempPokemonDir);
    free(ability1);
    free(ability2);
    return true;
}

bool ModifyLearnsets(FILE *file, uint32_t *constantCounts, char *allocStr)
{
    uint16_t *learnedMoves = malloc(2 * 100);
    char *str, *move;
    FILE *dstFile;
    uint32_t i, id, learnedCounter = 0, count = constantCounts[CONSTANTS_MOVES];

    if (allocStr == NULL || learnedMoves == NULL)
        return false;

    dstFile = fopen(sTempPokemonDir, "w+");
    if (dstFile == NULL)
        return false;

    while (1)
    {
        str = allocStr;
        if (fgets(str, CHR_BUFF_BIG, file) == NULL)
            break;

        SKIP_WHTSPACE(str);
        // ignore macros
        if (*str != '#')
        {
            // Search for LEVEL_UP_MOVE or LEVEL_UP_END
            while (*str != '\0' && *str != '\n')
            {
                SKIP_WHTSPACE(str);
                // We're at LEVEL_UP_MOVE
                if ((i = BeginsWithStr(&str, "LEVEL_UP_MOVE", false)))
                {
                    str += i;
                    SKIP_TILL(str, ',');
                    SKIP_WHTSPACE(str);
                    *str = '\0';
                    fprintf(dstFile, allocStr); // Copy everything except MOVE_X),
                    str++;
                    // Choose a random move
                    do
                    {
                        id = RAND_ID(count);
                        move = sMoveConstants[id].str;
                    } while ((!ALLOW_REPEATED_MOVES && IsValueInList(learnedMoves, learnedCounter, id))
                             || (!ALLOW_LEARN_HMS && IsStringInList(sHMMoves, ARRAY_COUNT(sHMMoves), move))
                             );

                    learnedMoves[learnedCounter++] = sMoveConstants[id].id;
                    fprintf(dstFile, ", %s),\n", move);
                    goto LOOP_END;
                }
                // We're at LEVEL_UP_END
                else if (BeginsWithStr(&str, "LEVEL_UP_END", false))
                {
                    learnedCounter = 0;
                    break;
                }
                str++;
            }
        }

        fputs(allocStr, dstFile);
    LOOP_END:
        ;
    }

    UpdateFile(file, dstFile, sLearnsetsDir, sTempPokemonDir);
    free(learnedMoves);
    return true;
}

bool ModifyTrainerMons(FILE *file, uint32_t *constantCounts, char *allocStr)
{
    FILE *dstFile;
    char *str, oldName[30], *newName;
    uint16_t knownMoves[4], movesId, prevMovesCount;
    uint32_t i, id, speciesCount = constantCounts[CONSTANTS_SPECIES], movesCount = constantCounts[CONSTANTS_MOVES];

    dstFile = fopen(sTempPokemonDir, "w+");
    if (dstFile == NULL || allocStr == NULL)
        return false;

    while (1)
    {
        str = allocStr;
        if (fgets(str, CHR_BUFF_BIG, file) == NULL)
            break;

        SKIP_WHTSPACE(str);
        while (*str != '\0' && *str != '\n')
        {
            if (*str == '.') // Field begins there
            {
                if (BeginsWithStr(&str, ".species", true))
                {
                    // We're at .species
                    SKIP_TILL(str, '=');
                    *str = '\0';
                    fprintf(dstFile, allocStr); // Copy everything SPECIES_X,
                    str++;

                    SKIP_WHTSPACE(str);
                    // species name
                    for (i = 0; str[i] != ',' && str[i] != ' '; i++)
                        oldName[i] = str[i];
                    oldName[i] = '\0';
                    do
                    {
                        id = RAND_ID(speciesCount);
                        newName = sSpeciesConstants[id].str;
                    } while (SAME_STRINGS(oldName, newName)
                             || (SAME_TRAINER_BASE_STATS && !IsInBSRange(oldName, id, speciesCount, SAME_TRAINER_STATS_RANGE_LOW, SAME_TRAINER_STATS_RANGE_HIGH))
                             || (SAME_TRAINERS_TYPES && !ShareType(id, ConstantNameToArrId(oldName, speciesCount, CONSTANTS_SPECIES))));
                    fprintf(dstFile, "= %s,\n", newName);
                    goto LOOP_END;
                }
                // Randomize moves
                else if (RANDOMIZE_TRAINER_MOVES && BeginsWithStr(&str, ".moves", true))
                {
                    movesId = 0;
                    // We're at .moves
                    SKIP_TILL(str, '=');
                    *str = '\0';
                    fprintf(dstFile, allocStr); // Copy everything MOves,
                    str++;
                    SKIP_WHTSPACE(str);
                    // Should always be before array, but in case it's not there.
                    if (*str == '{')
                        str++;

                    SKIP_WHTSPACE(str);
                    // Count how many moves were there previously
                    prevMovesCount = 0;
                    while (*str != '}' && *str != '\0' && *str !='\n')
                    {
                        SKIP_WHTSPACE(str);
                        str = strstr(str, "MOVE_");
                        if (str == NULL)
                            break;
                        if (BeginsWithStr(&str, "MOVE_NONE", false))
                            break;
                        str++;
                        prevMovesCount++;
                    }
                    for (i = 0; i < prevMovesCount; i++)
                    {
                        do
                        {
                            id = RAND_ID(movesCount);
                        } while (IsValueInList(knownMoves, movesId, id));
                        knownMoves[movesId++] = id;
                    }
                    *allocStr = '\0';
                    for (i = 0; i < movesId; i++)
                    {
                        strcat(allocStr, sMoveConstants[knownMoves[i]].str);
                        if (i + 1 < movesId)
                            strcat(allocStr, ", ");
                    }
                    fprintf(dstFile, "= \{%s},\n", allocStr);
                    goto LOOP_END;
                }

                // Different struct field, abort line
                break;
            }
            str++;
        }

        fputs(allocStr, dstFile);
    LOOP_END:
        ;
    }

    UpdateFile(file, dstFile, sTrainerPartiesDir, sTempPokemonDir);
    return true;
}

bool ModifyStarters(FILE *starterFile, uint32_t *constantCounts, char *allocStr)
{
    FILE *dstFile;
    char *str;
    uint32_t i, j, triangleId, state = 0, ids[3], speciesCount = constantCounts[CONSTANTS_SPECIES];

    dstFile = fopen(sTempPokemonDir, "w+");
    if (dstFile == NULL || allocStr == NULL)
        return false;

    while (1)
    {
        str = allocStr;
        if (fgets(str, CHR_BUFF_BIG, starterFile) == NULL)
            break;

        SKIP_WHTSPACE(str);
        switch (state)
        {
        case 0: // Find static const u16 sStarterMon
            if (BeginsWithStr(&str, "static const u16 sStarterMon", true))
                state++;
            else
                break;
        case 1: // Don't copy lines until } is found which signifies the array end
            for (; *str != '\0'; str++)
            {
                if (*str == '}')
                {
                    state = 2;
                    break;
                }
            }
            if (state != 2)
                continue;
        case 2:
            for (i = 0; i < 3; i++)
            {
                while (1)
                {
                    ids[i] = RAND_ID(speciesCount);
                    // Check if it's not the same mon and not their pre-evo or post evo
                    for (j = 0; j < i; j++)
                    {
                        if (ids[i] == ids[j] || sEvoData[ids[i]].evoTo == ids[j] || sEvoData[ids[i]].evoFrom == ids[j])
                            break;
                    }
                    if (j != i)
                        continue;

                    if (STARTERS_KEEP_TYPE_TRIANGLE)
                    {
                        if (i == 0) // Choose type triangle
                        {
                            for (j = 0; j < ARRAY_COUNT(sTriangleTypes); j++)
                            {
                                if (SAME_STRINGS(sMonTypes[ids[i]][0], sTriangleTypes[j][0]) || SAME_STRINGS(sMonTypes[ids[i]][1], sTriangleTypes[j][0]))
                                    break;
                            }
                            triangleId = j;
                            if (triangleId == ARRAY_COUNT(sTriangleTypes)) // Wrong type
                                continue;
                        }
                        else // Check if matches type
                        {
                            if (!SAME_STRINGS(sMonTypes[ids[i]][0], sTriangleTypes[triangleId][i]) && !SAME_STRINGS(sMonTypes[ids[i]][1], sTriangleTypes[triangleId][i]))
                                continue;
                        }
                    }

                    if (STARTERS_HAS_TO_EVOLVE && sEvoData[ids[i]].evoTo == 0)
                        continue;
                    if (STARTERS_CANT_BE_EVOLVED && sEvoData[ids[i]].evoFrom != 0)
                        continue;

                    break;
                }
            }
            fprintf(dstFile, "static const u16 sStarterMon[3] = {%s, %s, %s};\n", sSpeciesConstants[ids[0]].str, sSpeciesConstants[ids[1]].str, sSpeciesConstants[ids[2]].str);
            state++;
            continue;
        case 3: // Everything worked perfectly
            break;
        }

        fputs(allocStr, dstFile);
    }

    if (state == 3)
        UpdateFile(starterFile, dstFile, sStarterChooseDir, sTempPokemonDir);
    return (state == 3);
}

void RandomizeGame(void)
{
    uint32_t count[CONSTANTS_COUNT], i, j;
    FILE *file = NULL;
    char *stringBuffer = malloc(CHR_BUFF_BIG);

    srand(time(NULL));

    // Gather sConstantsData.
    for (i = 0; i < CONSTANTS_COUNT; i++)
    {
        file = fopen(sConstantsData[i].dir, "r");
        printf("%s constants...", sConstantsData[i].msgPrefix);
        if (file == NULL || (count[i] = GetDefines(file, sConstantsData[i].prefix, sConstantsData[i].structPtr)) == 0)
            printf("missing\n");
        else
            printf("found\n");
        fclose(file);
    }

    // Gather all types/bs data earlier to avoid lag for wild mons.
    file = fopen(sBaseStatsDir, "r");
    GatherMonBaseStatData(file, count[CONSTANTS_SPECIES]);
    fclose(file);

    // Gather evolution data
    file = fopen(sEvolutionDir, "r");
    GatherEvolutionData(file, count[CONSTANTS_SPECIES]);
    fclose(file);

    // Edit wanted files.
    for (i = 0; i < ARRAY_COUNT(sFilesToEdit); i++)
    {
        if (sFilesToEdit[i].toEdit)
        {
            file = fopen(sFilesToEdit[i].dir, "r+");
            printf("%s file...", sFilesToEdit[i].name);
            if (file == NULL)
            {
                 printf("missing\n");
            }
            else
            {
                printf("found ");
                if (sFilesToEdit[i].func(file, count, stringBuffer))
                    printf("- successfully randomized %s!\n", sFilesToEdit[i].randomizedThingName);
                else
                    printf("...failed to randomize %s!\n");
            }
        }
    }

    // Free allocated chars.
    for (i = 0; i < CONSTANTS_COUNT; i++)
    {
        if (sConstantsData[i].structPtr[0].str != NULL)
            free(sConstantsData[i].structPtr[0].str);
    }
    free(stringBuffer);
}

int main()
{
    RandomizeGame();
    return 0;
}

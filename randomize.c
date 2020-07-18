#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

                                /* EDIT THESE */

// Abilities config.
#define RANDOMIZE_ABILITIES     true    // Randomizes abilities if true, does not if false.
#define ALL_MONS_TWO_ABILITIES  true    // If true, all pokemon are going to have two different abilities. If false, pokemon which previously had only one ability, remain with only one.
#define ALLOW_WONDER_GUARD      false   // If false, no pokemon are allowed to have wonder guard. If true...well, you better not encounter a pokemon with this ability in the wild.
#define NO_FORM_ABILITIES       true    // If true, pokemon won't have abilities such as Schooling, Zen Mode, etc, which only work on specific species. Recommended to keep true, otherwise pokemon may end up with useless abilities.

// Learned moves config.
#define RANDOMIZE_MOVES         true    // Randomizes learned moves if true, does not if false.
#define ALLOW_REPEATED_MOVES    false   // If true, it's possible for a pokemon to learn the same move at multiple levels. Recommended to keep false, as it gives more variety to learnsets.
#define ALLOW_LEARN_HMS         false   // If true, pokemon may randomly learn various HMs.


                                /* STOP EDITING */

#define ARRAY_COUNT(array) (size_t)(sizeof(array) / sizeof((array)[0]))

struct DefineName
{
    char *str;
    int id;
};

struct DefineName sDefNames[999];

char sChrBuff[0x1000];

#define CHR_LEN 30

static const char *sFormAbilities[] =
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

static const char *sHMMoves[] =
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
static const char sAbilitiesDir[] = "include/constants/abilities.h";
static const char sMovesDir[] = "include/constants/moves.h";
static const char sTempPokemonDir[] = "src/data/pokemon/temp.txt";

// Finds start of #define X
char *FindDefineStart(char *str)
{
    int i;
    static const char name[] = "#define ";
    for (i = 0; i < ARRAY_COUNT(name) - 1; i++) // - 1 because we don't want eos
    {
        if ((*(str++) != name[i]))
            break;
    }
    if (i == ARRAY_COUNT(name) - 1)
        return str;
    return NULL;
}

uint32_t GetDefines(FILE *file, const char *defName)
{
    char *str, (*allocChars)[][CHR_LEN], num[5];
    uint32_t count = 0, i;

    allocChars = malloc(CHR_LEN * 1000);
    if (allocChars == NULL)
        return 0;

    while (1)
    {
        if (fgets(sChrBuff, sizeof(sChrBuff), file) == NULL)
            break;

        str = FindDefineStart(sChrBuff);
        if (str == NULL)
            continue;

        //we're at #define _____ Get define name, and make sure the id does not repeat

        // Make sure it starts with ABILITY_, ITEM_ or MOVE_
        for (i = 0; defName[i] != '\0'; i++)
        {
            if (str[i] != defName[i])
                break;
        }
        if (defName[i] != '\0')
            continue;

        // Get whole name
        for (i = 0; str[i] != ' '; i++)
            (*allocChars)[count][i] = str[i];
        (*allocChars)[count][i] = '\0';

        // Get id
        str += i;
        while (*str == ' ')
            str++;

        if (!isdigit(*str))
            continue;

        for (i = 0; str[i] != '\0' && str[i] != '\n'; i++)
        {
            if (!isdigit(str[i]))
                break;
        }
        str[i+1] = '\0';

        sDefNames[count].str = (*allocChars)[count];
        sDefNames[count].id = atoi(str);
        // Check if the id was used before
        for (i = 0; i < count; i++)
        {
            if (sDefNames[count].id == sDefNames[i].id)
                break;
        }
        if (i != count)
            continue;
        count++;
    }
    return count;
}

bool IsStringInList(const char **names, uint32_t count, char *str)
{
    uint32_t i;
    for (i = 0; i < count; i++)
    {
        if (strcmp(str, names[i]) == 0)
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
        ret = sDefNames[(rand() % (count - 1)) + 1].str;
    } while (strcmp(ret, "ABILITY_CACOPHONY") == 0
             || (!ALLOW_WONDER_GUARD && strcmp(ret, "ABILITY_WONDER_GUARD") == 0)
             || (NO_FORM_ABILITIES && IsStringInList(sFormAbilities, ARRAY_COUNT(sFormAbilities), ret))
             );

    return ret;
}

void ModifyAbilitiesInBaseStats(FILE *file, uint32_t count)
{
    static const char abilityStr[] = ".abilities";
    FILE *dst;
    uint32_t i;
    bool theSameAbilities;
    char *newAbility1, *newAbility2;
    char *ability1 = malloc(0x100), *ability2 = malloc(0x100);

    char *allocStr = malloc(0x1000), *str;
    if (allocStr == NULL || ability1 == NULL || ability2 == NULL)
        return;

    dst = fopen(sTempPokemonDir, "w+");
    if (dst == NULL)
        return;

    while (1)
    {
        str = allocStr;
        if (fgets(str, 0x1000, file) == NULL)
            break;

        // Skip whitespace
        while (*str == ' ')
            str++;

        // Search for .abilities
        while (*str != '\0' && *str != '\n')
        {
            if (*str == abilityStr[0])
            {
                for (i = 1; i < ARRAY_COUNT(abilityStr) - 1; i++)
                {
                    if (str[i] != abilityStr[i])
                        break;
                }

                // We're at .abilities
                if (i == ARRAY_COUNT(abilityStr) - 1)
                {
                    str += i;
                    // Skip until '{' is found
                    while (*str != '{')
                        str++;
                    *str = '\0';
                    fprintf(dst, allocStr); // Copy everything excetp {ability1, ability 2},
                    str++;

                    theSameAbilities = false;
                    // Find whether we want two different abilities.
                    if (!ALL_MONS_TWO_ABILITIES)
                    {
                        // Skip whitespace
                        while (*str == ' ')
                            str++;

                        // Copy first ability
                        for (i = 0; str[i] != ',' && str[i] != '}' && str[i] != ' '; i++)
                            ability1[i] = str[i];
                        ability1[i] = '\0';

                         // Skip whitespace
                        while (*str == ' ')
                            str++;

                        if (str[i] == ',') // Copy second ability
                        {
                            str += i + 1;
                            // Skip whitespace
                            while (*str == ' ')
                                str++;
                            for (i = 0; str[i] != ',' && str[i] != '}' && str[i] != ' '; i++)
                                ability2[i] = str[i];
                            ability2[i] = '\0';

                            if (ability2[0] == '0' || strcmp(ability1, ability2) == 0 || strstr(ability2, "NONE") != NULL)
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
                    fprintf(dst, "{%s, %s},\n", newAbility1, newAbility2);
                    goto LOOP_END;
                }

                // Different struct field, abort line
                break;
            }
            str++;
        }

        // Just copy the line
        fputs(allocStr, dst);
    LOOP_END:
        ;
    }

    fclose(dst);
    fclose(file);

    remove(sBaseStatsDir);
    rename(sTempPokemonDir, sBaseStatsDir);

    free(allocStr);
    free(ability1);
    free(ability2);
}

void ModifyLearnsets(FILE *file, uint32_t count)
{
    static const char moveString[] = "LEVEL_UP_MOVE";
    static const char moveEnd[] = "LEVEL_UP_END";
    uint16_t *learnedMoves = malloc(2 * 100);
    char *allocStr = malloc(0x1000), *str, *move;
    FILE *dst;
    uint32_t i, id, learnedCounter = 0;

    if (allocStr == NULL || learnedMoves == NULL)
        return;

    dst = fopen(sTempPokemonDir, "w+");
    if (dst == NULL)
        return;

    while (1)
    {
        str = allocStr;
        if (fgets(str, 0x1000, file) == NULL)
            break;

        // Skip whitespace
        while (*str == ' ')
            str++;

        // ignore macros
        if (*str != '#')
        {
            // Search for .abilities
            while (*str != '\0' && *str != '\n')
            {
                if (*str == moveString[0])
                {
                    for (i = 1; i < ARRAY_COUNT(moveString) - 1; i++)
                    {
                        if (str[i] != moveString[i])
                            break;
                    }
                    // We're at LEVEL_UP_MOVE
                    if (i == ARRAY_COUNT(moveString) - 1)
                    {
                        str += i;
                         // Skip until ',' is found
                        while (*str != ',')
                            str++;
                        // Skip whitespace
                        while (*str == ' ')
                            str++;
                        *str = '\0';
                        fprintf(dst, allocStr); // Copy everything except MOVE_X),
                        str++;
                        // Choose a random move
                        do
                        {
                            id = (rand() % (count - 1)) + 1;
                            move = sDefNames[id].str;
                        } while ((!ALLOW_REPEATED_MOVES && IsValueInList(learnedMoves, learnedCounter, id))
                                 || (!ALLOW_LEARN_HMS && IsStringInList(sHMMoves, ARRAY_COUNT(sHMMoves), move))
                                 );

                        learnedMoves[learnedCounter++] = sDefNames[id].id;
                        fprintf(dst, ", %s),\n", move);
                        goto LOOP_END;
                    }

                    for (i = 1; i < ARRAY_COUNT(moveEnd) - 1; i++)
                    {
                        if (str[i] != moveEnd[i])
                            break;
                    }
                    // We're at LEVEL_UP_END
                    if (i == ARRAY_COUNT(moveEnd) - 1)
                    {
                        learnedCounter = 0;
                    }
                    break;
                }
                str++;
            }
        }

        fputs(allocStr, dst);
    LOOP_END:
        ;
    }

    fclose(dst);
    fclose(file);

    remove(sLearnsetsDir);
    rename(sTempPokemonDir, sLearnsetsDir);

    free(allocStr);
    free(learnedMoves);
}

void RandomizeGame(void)
{
    uint32_t count, i;
    FILE *file;

    srand(time(NULL));

    // Abilities
    if (RANDOMIZE_ABILITIES)
    {
        file = fopen(sAbilitiesDir, "r");
        if (file == NULL)
        {
            printf("Cannot find abilities.h.\n");
        }
        else
        {
            count = GetDefines(file, "ABILITY_");
            fclose(file);

            // Modify abilities in base_stats.h
            file = fopen(sBaseStatsDir, "r+");
            if (file == NULL)
                printf("Cannot find base stats.\n");
            else
                ModifyAbilitiesInBaseStats(file, count);
        }
    }
    // Moves
    if (RANDOMIZE_MOVES)
    {
        file = fopen(sMovesDir, "r");
        if (file == NULL)
        {
            printf("Cannot find moves.h.\n");
        }
        else
        {
            count = GetDefines(file, "MOVE_");
            fclose(file);

            // Modify moves.
            file = fopen(sLearnsetsDir, "r+");
            if (file == NULL)
                printf("Cannot find learnsets.\n");
            else
                ModifyLearnsets(file, count);
        }
    }
}

int main()
{
    RandomizeGame();
    return 0;
}

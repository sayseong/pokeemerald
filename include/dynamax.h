#ifndef ENGINEUPDATED_DYNAMAX_H
#define ENGINEUPDATED_DYNAMAX_H

bool32 IsEvolutionHappened(u32 battlerId);
bool32 CanBattlerEvo (u8 battlerId);

enum BattleEvolutionType {
    EvolutionNone = 0, EvolutionMega = 1, EvolutionDynamax = 2, EvolutionMegaHappend = 3, EvolutionDynamaxHappend = 4,
};
enum BattleEvolutionType GetEvolutionTypeForBattler(u8 battlerId);
#include "battle_evocpp.h"

#endif //ENGINEUPDATED_DYNAMAX_H

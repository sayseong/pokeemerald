#ifndef ENGINEUPDATED_DYNAMAX_H
#define ENGINEUPDATED_DYNAMAX_H

bool32 IsEvolutionHappened(u32 battlerId);
bool32 CanBattlerEvo (u8 battlerId);

void CreateTrigger(u8 battlerId,u8 palId);
void DistoryTrigger(u32 battlerId);
void HideTriggerSprite();
u8 CreateIndicator(u32 battlerId);
void PrepareEvolution(u32 battlerId);
void DoEvolution(u32 battlerId);
void AfterEvolutionChangeStates(u32 battlerId, struct Pokemon* pokemon);
void UndoEvolution(u32 monId);
void ChangeTriggerSprite(u8 battlerId, u8 state);
u8 GetIndicatorSpriteId(u32 healthboxSpriteId);
enum BattleEvolutionType GetEvolutionTypeForBattler(u8 battlerId);
enum BattleEvolutionType {
    EvolutionNone = 0, EvolutionMega = 1, EvolutionDynamax = 2, EvolutionMegaHappend = 3, EvolutionDynamaxHappend = 4,
};

#endif //ENGINEUPDATED_DYNAMAX_H

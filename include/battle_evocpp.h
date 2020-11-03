#ifndef ENGINEUPDATED_BATTLE_EVOCPP_H
#define ENGINEUPDATED_BATTLE_EVOCPP_H



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
void InitBattleStruct();





#endif //ENGINEUPDATED_BATTLE_EVOCPP_H

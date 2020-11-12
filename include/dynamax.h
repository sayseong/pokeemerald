#ifndef ENGINEUPDATED_DYNAMAX_H
#define ENGINEUPDATED_DYNAMAX_H

bool32 IsEvolutionHappened(u32 battlerId);
bool32 CanBattlerEvo (u8 battlerId);

void CreateTrigger(u8 battlerId,u8 palId);
void HideTriggerSprite();
u8 CreateIndicator(u32 battlerId);
void DoEvolution(u32 battlerId);
void UndoEvolution(u32 monId);
void ChangeTriggerSprite(u8 battlerId, u8 state);
u8 GetIndicatorSpriteId(u32 healthboxSpriteId);
enum BattleEvolutionType GetEvolutionTypeForBattler(u8 battlerId);
bool32 HandleDynamaxEndTurnEffect();
enum BattleEvolutionType {
    EvolutionNone = 0, EvolutionMega = 1, EvolutionDynamax = 2
};
void HandleDynamaxMoveEffect();
#define DYNAMAX_SET_RAIN 0
#define DYNAMAX_SET_SUNNY 1
#define DYNAMAX_SET_SANDSTORM 2
#define DYNAMAX_SET_HAIL 3
#define DYNAMAX_SET_MISTY_TERRAIN 4
#define DYNAMAX_SET_GRASS_TERRAIN 5
#define DYNAMAX_SET_ELECTRIC_TERRAIN 6
#define DYNAMAX_SET_PSYCHIC_TERRAIN 7
#endif //ENGINEUPDATED_DYNAMAX_H

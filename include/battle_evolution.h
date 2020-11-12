#ifndef ENGINEUPDATED_BATTLE_EVOLUTION_H
#define ENGINEUPDATED_BATTLE_EVOLUTION_H


#define BATTLE_TYPE_DYNAMAX				BATTLE_TYPE_x80000000
#define TYPE_ICON_TAG 0x2720
#define TYPE_ICON_TAG_2 0x2721
typedef bool32 (*TypeCanPokemonEvolution) (struct Pokemon* poke);
typedef void (*TypeCreateOrShowTrigger)(u8 battlerId, u8 palId);
typedef u32 (*TypeCreateIndicator)(u32 battlerId);
typedef void (*TypeDoEvolution)(u32 battlerId);
typedef void (*TypeUndoEvolution)(u32 monId);
typedef void (*TypeHideTriggerSprite)();
typedef void (*TypeChangeTriggerSprite)(u8 state);
struct BattleEvolutionFunc {
    TypeCanPokemonEvolution CanPokemonEvolution;
    TypeCreateOrShowTrigger CreateOrShowTrigger;
    TypeCreateIndicator CreateIndicator;
    TypeDoEvolution DoEvolution;
    TypeUndoEvolution UndoEvolution;
    TypeHideTriggerSprite HideTriggerSprite;
    TypeChangeTriggerSprite ChangeTriggerSprite;
};

enum BattleEvolutionType GetEvolutionType(u8 battlerId);
enum BattleEvolutionType CalEvolutionType(u8 battlerId);
const struct BattleEvolutionFunc* GetBattleEvolutionFunc(u8 battlerId);
const struct BattleEvolutionFunc* GetBattlerFuncByEvolutionType(u32 monId, u8 side);
void SetEvolutionType(struct BattleStruct* battleStruct, u8 battlerId, enum BattleEvolutionType value);
#endif //ENGINEUPDATED_BATTLE_EVOLUTION_H

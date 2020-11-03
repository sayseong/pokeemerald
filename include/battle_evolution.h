#ifndef ENGINEUPDATED_BATTLE_EVOLUTION_H
#define ENGINEUPDATED_BATTLE_EVOLUTION_H


#define BATTLE_TYPE_DYNAMAX				BATTLE_TYPE_x80000000
#define TYPE_ICON_TAG 0x2720
#define TYPE_ICON_TAG_2 0x2721

struct BattleEvolutionFunc {
    bool32 (*CanPokemonEvolution) (struct Pokemon* poke);
    bool32  (*IsEvolutionHappened)(u32 battlerId);

    void (*CreateOrShowTrigger)(u8 battlerId, u8 palId);
    void (*DistoryTrigger)(u32 battlerId);
    u32 (*CreateIndicator)(u32 battlerId);
    void (*PrepareEvolution) (u32 battlerId);
    void (*DoEvolution)(u32 battlerId);
    void (*UndoEvolution)(u32 monId);
    void (*HideTriggerSprite)();
    void (*ChangeTriggerSprite)(u8 state);
};

bool32 CheckEvolutionType(struct BattleEvolutionData* evolutionData, u8 battlerId);
const struct BattleEvolutionFunc* GetBattleEvolutionFunc(u8 battlerId);
const struct BattleEvolutionFunc* GetBattleEvolutionFuncByPos(u32 monId, u8 side);
void SetEvolutionType(struct BattleStruct* battleStruct, u8 battlerId, enum BattleEvolutionType value);
#endif //ENGINEUPDATED_BATTLE_EVOLUTION_H

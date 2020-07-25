#include "global.h"
#include "crt0.h"
#include "malloc.h"
#include "link.h"
#include "link_rfu.h"
#include "librfu.h"
#include "m4a.h"
#include "bg.h"
#include "rtc.h"
#include "scanline_effect.h"
#include "overworld.h"
#include "random.h"
#include "dma3.h"
#include "gba/flash_internal.h"
#include "load_save.h"
#include "gpu_regs.h"
#include "agb_flash.h"
#include "sound.h"
#include "battle.h"
#include "battle_controllers.h"
#include "text.h"
#include "intro.h"
#include "main.h"
#include "trainer_hill.h"

static void VBlankIntr(void);
static void HBlankIntr(void);
static void VCountIntr(void);
static void SerialIntr(void);
static void IntrDummy(void);

const u8 gGameVersion = GAME_VERSION;

const u8 gGameLanguage = GAME_LANGUAGE; // English

const IntrFunc gIntrTableTemplate[] =
{
    VCountIntr, // V-count interrupt
    SerialIntr, // Serial interrupt
    Timer3Intr, // Timer 3 interrupt
    HBlankIntr, // H-blank interrupt
    VBlankIntr, // V-blank interrupt
    IntrDummy,  // Timer 0 interrupt
    IntrDummy,  // Timer 1 interrupt
    IntrDummy,  // Timer 2 interrupt
    IntrDummy,  // DMA 0 interrupt
    IntrDummy,  // DMA 1 interrupt
    IntrDummy,  // DMA 2 interrupt
    IntrDummy,  // DMA 3 interrupt
    IntrDummy,  // Key interrupt
    IntrDummy,  // Game Pak interrupt
};

#define INTR_COUNT ((int)(sizeof(gIntrTableTemplate)/sizeof(IntrFunc)))

static u16 gUnknown_03000000;
static u8 sPlayTimeCounterState;

u16 gKeyRepeatStartDelay;
bool8 gLinkTransferringData;
struct Main gMain;
u16 gKeyRepeatContinueDelay;
bool8 gSoftResetDisabled;
IntrFunc gIntrTable[INTR_COUNT];
u8 gLinkVSyncDisabled;
u32 IntrMain_Buffer[0x200];
s8 gPcmDmaCounter;

static EWRAM_DATA u16 gTrainerId = 0;

//EWRAM_DATA void (**gFlashTimerIntrFunc)(void) = NULL;

static void UpdateLinkAndCallCallbacks(void);
static void InitMainCallbacks(void);
static void CallCallbacks(void);
static void SeedRngWithRtc(void);
void InitIntrHandlers(void);
void EnableVCountIntrAtLine150(void);

enum
{
    STOPPED,
    RUNNING,
    MAXED_OUT
};

static void PlayTimeCounter_SetToMax(void);
static void PlayTimeCounter_Stop(void);

#define B_START_SELECT (B_BUTTON | START_BUTTON | SELECT_BUTTON)

NAKED void AgbMain()
{
    RegisterRamReset(RESET_ALL);
    *(vu16 *)BG_PLTT = 0x7FFF;
    InitGpuRegManager();
    REG_WAITCNT = WAITCNT_PREFETCH_ENABLE | WAITCNT_WS0_S_1 | WAITCNT_WS0_N_3;
    InitKeys();
    InitIntrHandlers();
    m4aSoundInit();
    EnableVCountIntrAtLine150();
    InitRFU();
    RtcInit();
    CheckForFlashMemory();
    InitMainCallbacks();
    InitMapMusic();
    SeedRngWithRtc();
    ClearDma3Requests();
    ResetBgs();
    SetDefaultFontsPointer();
    InitHeap(gHeap, HEAP_SIZE);

    gSoftResetDisabled = FALSE;

    if (gFlashMemoryPresent != TRUE)
        SetMainCallback2(IntrDummy);

    gLinkTransferringData = FALSE;
    gUnknown_03000000 = 0xFC0;

    asm("\n\
ldr r3, .L_KEYINPUT\n\
mov r8, r3\n\
ldr r4, .L_KEYMASK\n\
mov r9, r4\n\
ldr r4, .L_MAIN\n\
ldr r5, .L_SAVBLCK2\n\
ldr r6, .L_PLAYTIME_STATE\n\
mov r7, #0\n\
\n\
.LOOP:\n\
	@ Read Keys\n\
	mov r3, r8 @ Key Input\n\
	mov r2, r9 @ key mask\n\
	ldrh r1, [r3]\n\
	eor r1, r2 @ r1 - key input\n\
	ldrh r2, [r4, %[heldKeysRaw]]\n\
    mov r0, r1 \n\
	bic r0, r2\n\
	strh r0, [r4, %[newKeysRaw]]\n\
	strh r0, [r4, %[newKeys]]\n\
	strh r0, [r4, %[newAndRepeatedKeys]]\n\
	tst r1, r1 @ if keyinput is 0\n\
	beq .KEY_COUNTER_RESET\n\
	ldrh r0, [r4, %[heldKeys]]\n\
	cmp r0, r1\n\
	beq .UPDATE_KEY_COUNTER\n\
.KEY_COUNTER_RESET:\n\
	ldr r0, .L_KEY_REPEAT_START_DELAY\n\
	ldrh r0, [r0]\n\
	strh r0, [r4, %[keyRepeatCounter]]\n\
.UPDATE_HELD_KEYS:\n\
	strh r1, [r4, %[heldKeysRaw]]\n\
	strh r1, [r4, %[heldKeys]]\n\
	\n\
	@the same code in two places for speed boost\n\
	@ call callbacks\n\
	ldr r3, [r4, %[callback1]]\n\
	cmp r3, #0\n\
	beq .CALLBACK2\n\
	bl .call_via_r3\n\
	\n\
.CALLBACK2:\n\
	ldr r3, [r4, %[callback2]]\n\
	bl .call_via_r3\n\
	\n\
@ Play time update\n\
	ldrb r3, [r6]\n\
	cmp r3, %[playTimeRunning]\n\
	bne .MUSIC\n\
	ldrb r3, [r5, %[playTimeVBlanks]]\n\
	add r3, r3, #1\n\
	cmp r3, #59\n\
	bls .TIME_VBLANKS\n\
	strb r7, [r5, %[playTimeVBlanks]]\n\
	ldrb r3, [r5, %[playTimeSeconds]]\n\
	add r3, r3, #1\n\
	cmp r3, #59\n\
	bls .TIME_SECONDS\n\
	strb r7, [r5, %[playTimeSeconds]]\n\
	ldrb r3, [r5, %[playTimeMinutes]]\n\
	add r3, r3, #1\n\
	cmp r3, #59\n\
	bls .TIME_MINUTES\n\
	strb r7, [r5, %[playTimeMinutes]]\n\
	ldrh r3, [r5, %[playTimeHours]]\n\
	add r3, r3, #1\n\
	ldr r2, .L_999\n\
	strh r3, [r5, %[playTimeHours]]\n\
	cmp r3, r2\n\
	bcc .MUSIC\n\
	bl PlayTimeCounter_SetToMax\n\
	b .MUSIC\n\
	\n\
.TIME_VBLANKS:\n\
	strb r3, [r5, %[playTimeVBlanks]]\n\
.MUSIC:\n\
	bl	MapMusicMain\n\
	mov r2, %[intrFlagVblank]\n\
	ldrh r3, [r4, %[intrCheck]]\n\
	bic r3, r2\n\
	strh r3, [r4, %[intrCheck]]\n\
	swi 0x5\n\
	b .LOOP\n\
	\n\
.TIME_SECONDS:\n\
	strb r3, [r5, %[playTimeSeconds]]\n\
	bl	MapMusicMain\n\
	mov r2, %[intrFlagVblank]\n\
	ldrh r3, [r4, %[intrCheck]]\n\
	bic r3, r2\n\
	strh r3, [r4, %[intrCheck]]\n\
	swi 0x5\n\
	b .LOOP\n\
	\n\
.TIME_MINUTES:\n\
	strb r3, [r5, %[playTimeMinutes]]\n\
	bl	MapMusicMain\n\
	mov r2, %[intrFlagVblank]\n\
	ldrh r3, [r4, %[intrCheck]]\n\
	bic r3, r2\n\
	strh r3, [r4, %[intrCheck]]\n\
	swi 0x5\n\
	b .LOOP\n\
	\n\
.UPDATE_KEY_COUNTER:\n\
	ldrh r0, [r4, %[keyRepeatCounter]]\n\
	sub r0, r0, #1\n\
	strh r0, [r4, %[keyRepeatCounter]]\n\
	bne .UPDATE_HELD_KEYS\n\
	strh r1, [r4, %[newAndRepeatedKeys]]\n\
	ldr r0, .L_KEY_REPEAT_CONTINUE_DELAY\n\
	ldrh r0, [r0]\n\
	strh r0, [r4, %[keyRepeatCounter]]\n\
	strh r1, [r4, %[heldKeysRaw]]\n\
	strh r1, [r4, %[heldKeys]]\n\
	\n\
	@ same code as above\n\
	ldr r3, [r4, %[callback1]]\n\
	cmp r3, #0\n\
	beq ._CALLBACK2\n\
	bl .call_via_r3\n\
	\n\
._CALLBACK2:\n\
	ldr r3, [r4, %[callback2]]\n\
	bl .call_via_r3\n\
	\n\
@ Play time update\n\
	ldrb r3, [r6]\n\
	cmp r3, %[playTimeRunning]\n\
	bne .MUSIC\n\
	ldrb r3, [r5, %[playTimeVBlanks]]\n\
	add r3, r3, #1\n\
	cmp r3, #59\n\
	bls .TIME_VBLANKS\n\
	strb r7, [r5, %[playTimeVBlanks]]\n\
	ldrb r3, [r5, %[playTimeSeconds]]\n\
	add r3, r3, #1\n\
	cmp r3, #59\n\
	bls .TIME_SECONDS\n\
	strb r7, [r5, %[playTimeSeconds]]\n\
	ldrb r3, [r5, %[playTimeMinutes]]\n\
	add r3, r3, #1\n\
	cmp r3, #59\n\
	bls .TIME_MINUTES\n\
	strb r7, [r5, %[playTimeMinutes]]\n\
	ldrh r3, [r5, %[playTimeHours]]\n\
	add r3, r3, #1\n\
	ldr r2, .L_999\n\
	strh r3, [r5, %[playTimeHours]]\n\
	cmp r3, r2\n\
	bcc .MUSIC\n\
	bl PlayTimeCounter_SetToMax\n\
	b .MUSIC\n\
\n\
.align 2\n\
.L_KEY_REPEAT_START_DELAY:\n\
	.word gKeyRepeatStartDelay\n\
.L_KEY_REPEAT_CONTINUE_DELAY:\n\
	.word gKeyRepeatContinueDelay\n\
.L_KEYINPUT:\n\
	.word 0x04000130 @ REG KEY INPUT\n\
.L_KEYMASK:\n\
	.word 0x03FF\n\
.L_999:\n\
	.word 999\n\
.L_MAIN:\n\
	.word gMain\n\
.L_SAVBLCK2:\n\
	.word gSaveblock2\n\
.L_PLAYTIME_STATE:\n\
	.word sPlayTimeCounterState\n\
	\n\
.align	1\n\
.thumb\n\
.call_via_r3:\n\
	bx r3"
    : /* no outputs */
    : [heldKeysRaw] "i" (offsetof(struct Main, heldKeysRaw)),
     [newKeysRaw] "i" (offsetof(struct Main, newKeysRaw)),
     [newKeys] "i" (offsetof(struct Main, newKeys)),
     [newAndRepeatedKeys] "i" (offsetof(struct Main, newAndRepeatedKeys)),
     [heldKeys] "i" (offsetof(struct Main, heldKeys)),
     [keyRepeatCounter] "i" (offsetof(struct Main, keyRepeatCounter)),
     [callback1] "i" (offsetof(struct Main, callback1)),
     [callback2] "i" (offsetof(struct Main, callback2)),
     [intrCheck] "i" (offsetof(struct Main, intrCheck)),
     [playTimeVBlanks] "i" (offsetof(struct SaveBlock2, playTimeVBlanks)),
     [playTimeSeconds] "i" (offsetof(struct SaveBlock2, playTimeSeconds)),
     [playTimeMinutes] "i" (offsetof(struct SaveBlock2, playTimeMinutes)),
     [playTimeHours] "i" (offsetof(struct SaveBlock2, playTimeHours)),
     [playTimeRunning] "i" (RUNNING),
     [intrFlagVblank] "i" (INTR_FLAG_VBLANK)
    );
}

static void UpdateLinkAndCallCallbacks(void)
{
    if (!HandleLinkConnection())
        CallCallbacks();
}

static void InitMainCallbacks(void)
{
    gMain.vblankCounter1 = 0;
    gTrainerHillVBlankCounter = NULL;
    gMain.vblankCounter2 = 0;
    gMain.callback1 = NULL;
    SetMainCallback2(CB2_InitCopyrightScreenAfterBootup);
}

static void CallCallbacks(void)
{
    if (gMain.callback1)
        gMain.callback1();

    if (gMain.callback2)
        gMain.callback2();
}

void SetMainCallback2(MainCallback callback)
{
    gMain.callback2 = callback;
    gMain.state = 0;
}

void StartTimer1(void)
{
    REG_TM1CNT_H = 0x80;
}

void SeedRngAndSetTrainerId(void)
{
    u16 val = REG_TM1CNT_L;
    SeedRng(val);
    REG_TM1CNT_H = 0;
    gTrainerId = val;
}

u16 GetGeneratedTrainerIdLower(void)
{
    return gTrainerId;
}

void EnableVCountIntrAtLine150(void)
{
    u16 gpuReg = (GetGpuReg(REG_OFFSET_DISPSTAT) & 0xFF) | (150 << 8);
    SetGpuReg(REG_OFFSET_DISPSTAT, gpuReg | DISPSTAT_VCOUNT_INTR);
    EnableInterrupts(INTR_FLAG_VCOUNT);
}

static void SeedRngWithRtc(void)
{
    u32 seed = RtcGetMinuteCount();
    seed = (seed >> 16) ^ (seed & 0xFFFF);
    SeedRng(seed);
}

void InitKeys(void)
{
    gKeyRepeatContinueDelay = 5;
    gKeyRepeatStartDelay = 40;

    gMain.heldKeys = 0;
    gMain.newKeys = 0;
    gMain.newAndRepeatedKeys = 0;
    gMain.heldKeysRaw = 0;
    gMain.newKeysRaw = 0;
}

void InitIntrHandlers(void)
{
    int i;

    for (i = 0; i < INTR_COUNT; i++)
        gIntrTable[i] = gIntrTableTemplate[i];

    DmaCopy32(3, IntrMain, IntrMain_Buffer, sizeof(IntrMain_Buffer));

    INTR_VECTOR = IntrMain_Buffer;

    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
    SetSerialCallback(NULL);

    REG_IME = 1;

    EnableInterrupts(0x1);
}

void SetVBlankCallback(IntrCallback callback)
{
    gMain.vblankCallback = callback;
}

void SetHBlankCallback(IntrCallback callback)
{
    gMain.hblankCallback = callback;
}

void SetVCountCallback(IntrCallback callback)
{
    gMain.vcountCallback = callback;
}

void RestoreSerialTimer3IntrHandlers(void)
{
    gIntrTable[1] = SerialIntr;
    gIntrTable[2] = Timer3Intr;
}

void SetSerialCallback(IntrCallback callback)
{
    gMain.serialCallback = callback;
}

static void VBlankIntr(void)
{
    if (gTrainerHillVBlankCounter && *gTrainerHillVBlankCounter < 0xFFFFFFFF)
        (*gTrainerHillVBlankCounter)++;

    if (gMain.vblankCallback)
        gMain.vblankCallback();

    CopyBufferedValuesToGpuRegs();
    ProcessDma3Requests();

    gPcmDmaCounter = gSoundInfo.pcmDmaCounter;

    m4aSoundMain();

    if (!gMain.inBattle || !(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_FRONTIER | BATTLE_TYPE_RECORDED)))
        Random();

    INTR_CHECK |= INTR_FLAG_VBLANK;
    gMain.intrCheck |= INTR_FLAG_VBLANK;
}

void InitFlashTimer(void)
{
    SetFlashTimerIntr(2, gIntrTable + 0x7);
}

static void HBlankIntr(void)
{
    if (gMain.hblankCallback)
        gMain.hblankCallback();

    INTR_CHECK |= INTR_FLAG_HBLANK;
    gMain.intrCheck |= INTR_FLAG_HBLANK;
}

static void VCountIntr(void)
{
    if (gMain.vcountCallback)
        gMain.vcountCallback();

    m4aSoundVSync();
    INTR_CHECK |= INTR_FLAG_VCOUNT;
    gMain.intrCheck |= INTR_FLAG_VCOUNT;
}

static void SerialIntr(void)
{
    if (gMain.serialCallback)
        gMain.serialCallback();

    INTR_CHECK |= INTR_FLAG_SERIAL;
    gMain.intrCheck |= INTR_FLAG_SERIAL;
}

static void IntrDummy(void)
{}

static void WaitForVBlank(void)
{
    gMain.intrCheck &= ~INTR_FLAG_VBLANK;

    asm("swi 0x5");
}

void SetTrainerHillVBlankCounter(u32 *counter)
{
    gTrainerHillVBlankCounter = counter;
}

void ClearTrainerHillVBlankCounter(void)
{
    gTrainerHillVBlankCounter = NULL;
}

void DoSoftReset(void)
{
    REG_IME = 0;
    m4aSoundVSyncOff();
    ScanlineEffect_Stop();
    DmaStop(1);
    DmaStop(2);
    DmaStop(3);
    SiiRtcProtect();
    SoftReset(RESET_ALL);
}

void ClearPokemonCrySongs(void)
{
    CpuFill16(0, gPokemonCrySongs, MAX_POKEMON_CRIES * sizeof(struct PokemonCrySong));
}

// play time

void PlayTimeCounter_Reset(void)
{
    sPlayTimeCounterState = STOPPED;

    gSaveBlock2Ptr->playTimeHours = 0;
    gSaveBlock2Ptr->playTimeMinutes = 0;
    gSaveBlock2Ptr->playTimeSeconds = 0;
    gSaveBlock2Ptr->playTimeVBlanks = 0;
}

void PlayTimeCounter_Start(void)
{
    sPlayTimeCounterState = RUNNING;

    if (gSaveBlock2Ptr->playTimeHours > 999)
        PlayTimeCounter_SetToMax();
}

static void PlayTimeCounter_Stop(void)
{
    sPlayTimeCounterState = STOPPED;
}

static void PlayTimeCounter_SetToMax(void)
{
    sPlayTimeCounterState = MAXED_OUT;

    gSaveBlock2Ptr->playTimeHours = 999;
    gSaveBlock2Ptr->playTimeMinutes = 59;
    gSaveBlock2Ptr->playTimeSeconds = 59;
    gSaveBlock2Ptr->playTimeVBlanks = 59;
}

@ commands

	.macro loadspritegfx tag:req
	.byte 0x0
	.2byte \tag
	.endm

	.macro unloadspritegfx tag:req
	.byte 0x1
	.2byte \tag
	.endm

.macro createsprite template:req, anim_battler:req, subpriority_offset:req, argv:vararg
	.byte 0x02
	.4byte \template
	.if \anim_battler == ANIM_TARGET
	.byte 0x80 | (\subpriority_offset & 0x7F)
	.else
	.byte (\subpriority_offset & 0x7F)
	.endif
	.byte (.Lsprite_\@_2 - .Lsprite_\@_1) / 2
.Lsprite_\@_1:
	.2byte \argv
.Lsprite_\@_2:
	.endm

	.macro createvisualtask addr:req, priority:req, argv:vararg
	.byte 0x03
	.4byte \addr
	.byte \priority
	.byte (.Lcreatetask_\@_2 - .Lcreatetask_\@_1) / 2
.Lcreatetask_\@_1:
	.2byte \argv
.Lcreatetask_\@_2:
	.endm

	.macro delay param0:req
	.byte 0x4
	.byte \param0
	.endm

	.macro waitforvisualfinish
	.byte 0x5
	.endm

	.macro hang1
	.byte 0x6
	.endm

	.macro hang2
	.byte 0x7
	.endm

	.macro end
	.byte 0x8
	.endm

	.macro playse se:req
	.byte 0x9
	.2byte \se
	.endm

	.macro monbg battler:req
	.byte 0xa
	.byte \battler
	.endm

	.macro clearmonbg battler:req
	.byte 0xb
	.byte \battler
	.endm

	.macro setalpha eva:req, evb:req
	.byte 0x0C
	.2byte ((\evb) << 8) | (\eva)
	.endm

	.macro blendoff
	.byte 0xd
	.endm

	.macro call param0:req
	.byte 0xe
	.4byte \param0
	.endm

	.macro return
	.byte 0xf
	.endm

	.macro setarg param0:req, param1:req
	.byte 0x10
	.byte \param0
	.2byte \param1
	.endm

	.macro choosetwoturnanim param0:req, param1:req
	.byte 0x11
	.4byte \param0
	.4byte \param1
	.endm

	.macro jumpifmoveturn param0:req, ptr:req
	.byte 0x12
	.byte \param0
	.4byte \ptr
	.endm

	.macro goto ptr:req
	.byte 0x13
	.4byte \ptr
	.endm

	.macro fadetobg bg:req
	.byte 0x14
	.byte \bg
	.endm

	.macro restorebg
	.byte 0x15
	.endm

	.macro waitbgfadeout
	.byte 0x16
	.endm

	.macro waitbgfadein
	.byte 0x17
	.endm

	.macro changebg bg:req
	.byte 0x18
	.byte \bg
	.endm

	.macro playsewithpan se:req, pan:req
	.byte 0x19
	.2byte \se
	.byte \pan
	.endm

	.macro setpan pan:req
	.byte 0x1a
	.byte \pan
	.endm

	.macro panse_1B se:req, param1:req, param2:req, param3:req, param4:req
	.byte 0x1b
	.2byte \se
	.byte \param1
	.byte \param2
	.byte \param3
	.byte \param4
	.endm

	.macro loopsewithpan se:req, pan:req, wait:req, times:req
	.byte 0x1c
	.2byte \se
	.byte \pan
	.byte \wait
	.byte \times
	.endm

	.macro waitplaysewithpan se:req, pan:req, wait:req
	.byte 0x1d
	.2byte \se
	.byte \pan
	.byte \wait
	.endm

	.macro setbldcnt param0:req
	.byte 0x1e
	.2byte \param0
	.endm

	.macro createsoundtask addr:req, argv:vararg
	.byte 0x1F
	.4byte \addr
	.byte (.Lcreatetask_1F_\@_2 - .Lcreatetask_1F_\@_1) / 2
.Lcreatetask_1F_\@_1:
	.2byte \argv
.Lcreatetask_1F_\@_2:
	.endm

	.macro waitsound
	.byte 0x20
	.endm

	.macro jumpargeq param0:req, param1:req, ptr:req
	.byte 0x21
	.byte \param0
	.2byte \param1
	.4byte \ptr
	.endm

	.macro monbg_22 battler:req
	.byte 0x22
	.byte \battler
	.endm

	.macro clearmonbg_23 battler:req
	.byte 0x23
	.byte \battler
	.endm

	.macro jumpifcontest ptr:req
	.byte 0x24
	.4byte \ptr
	.endm

	.macro fadetobgfromset param0:req, param1:req, param2:req
	.byte 0x25
	.byte \param0
	.byte \param1
	.byte \param2
	.endm

	.macro panse_26 se:req, param1:req, param2:req, param3:req, param4:req
	.byte 0x26
	.2byte \se
	.byte \param1
	.byte \param2
	.byte \param3
	.byte \param4
	.endm

	.macro panse_27 se:req, param1:req, param2:req, param3:req, param4:req
	.byte 0x27
	.2byte \se
	.byte \param1
	.byte \param2
	.byte \param3
	.byte \param4
	.endm

	.macro monbgprio_28 battler:req
	.byte 0x28
	.byte \battler
	.endm

	.macro monbgprio_29
	.byte 0x29
	.endm

	.macro monbgprio_2A battler:req
	.byte 0x2a
	.byte \battler
	.endm

	.macro invisible battler:req
	.byte 0x2b
	.byte \battler
	.endm

	.macro visible battler:req
	.byte 0x2c
	.byte \battler
	.endm

	.macro doublebattle_2D battler:req
	.byte 0x2d
	.byte \battler
	.endm

	.macro doublebattle_2E battler:req
	.byte 0x2e
	.byte \battler
	.endm

	.macro stopsound
	.byte 0x2f
	.endm
	
@ useful macros
	.macro jumpreteq value:req, ptr:req
	jumpargeq ARG_RET_ID, \value, \ptr
	.endm
	
	.macro jumprettrue ptr:req
	jumpreteq TRUE, \ptr
	.endm
	
	.macro jumpretfalse ptr:req
	jumpreteq FALSE, \ptr
	.endm
	
	.macro jumpifdoublebattle ptr:req
	createvisualtask AnimTask_IsDoubleBattle, 0
	jumprettrue \ptr
	.endm
    
@ keep CFRU macros cause I'm lazy. todo: update to createsprite and createvisualtask, respectively
.macro animparticle offset size tag
.word \offset
.hword \size
.hword \tag
.endm

.macro animparticlepal offset tag hword
.word \offset
.hword \tag
.hword 0x0
.endm

@commands
.macro loadparticle loadparticleID
.byte 0x0
.hword \loadparticleID
.endm

.macro unloadparticle unloadparticleID
.byte 0x1
.hword \unloadparticleID
.endm

.macro launchtemplate launchtemplatePtr launchtemplatePriority launchtemplateArgsNo launchtemplatearg0 launchtemplatearg1 launchtemplatearg2 launchtemplatearg3 launchtemplatearg4 launchtemplatearg5 launchtemplatearg6 launchtemplatearg7 launchtemplatearg8
.byte 0x2
.word \launchtemplatePtr
.byte \launchtemplatePriority
.byte \launchtemplateArgsNo
.hword \launchtemplatearg0
.hword \launchtemplatearg1
.hword \launchtemplatearg2
.hword \launchtemplatearg3
.hword \launchtemplatearg4
.hword \launchtemplatearg5
.hword \launchtemplatearg6
.hword \launchtemplatearg7
.hword \launchtemplatearg8
.endm

.macro launchtask launchtaskPtr launchtaskPriority launchtaskArgsNo launchtaskarg0 launchtaskarg1 launchtaskarg2 launchtaskarg3 launchtaskarg4 launchtaskarg5 launchtaskarg6 launchtaskarg7 launchtaskarg8
.byte 0x3
.word \launchtaskPtr
.byte \launchtaskPriority
.byte \launchtaskArgsNo
.hword \launchtaskarg0
.hword \launchtaskarg1
.hword \launchtaskarg2
.hword \launchtaskarg3
.hword \launchtaskarg4
.hword \launchtaskarg5
.hword \launchtaskarg6
.hword \launchtaskarg7
.hword \launchtaskarg8
.endm

.macro pause pause_cmdToWait
.byte 0x4
.byte \pause_cmdToWait
.endm

.macro waitanimation
.byte 0x5
.endm

.macro endanimation
.byte 0x8
.endm

.macro playsound1 playsound1ID
.byte 0x9
.hword \playsound1ID
.endm

.macro pokespritetoBG animABank
.byte 0xA
.byte \animABank
.endm

.macro pokespritefromBG animBBank
.byte 0xB
.byte \animBBank
.endm

.macro setblends setblends_value
.byte 0xC
.hword \setblends_value
.endm

.macro resetblends
.byte 0xD
.endm

.macro loadBG1 loadBG1ID
.byte 0x14
.byte \loadBG1ID
.endm

.macro loaddefaultBG
.byte 0x15
.endm

.macro loadBG2 loadBG2ID
.byte 0x18
.byte \loadBG2ID
.endm

.macro playsound2 playsound2ID playsound2Unkown
.byte 0x19
.hword \playsound2ID
.byte \playsound2Unkown
.endm

.macro pancontrol anim1Aarg1
.byte 0x1A
.byte \anim1Aarg1
.endm

.macro playsoundpanchange anim1BsongID anim1BArg3 anim1BArg4 anim1BArg5 anim1BArg6
.byte 0x1B
.hword \anim1BsongID
.byte \anim1BArg3
.byte \anim1BArg4
.byte \anim1BArg5
.byte \anim1BArg6
.endm

.macro soundcomplex anim1Carg1 anim1Carg2 anim1Carg3 anim1Carg4
.byte 0x1C
.hword \anim1Carg1
.byte \anim1Carg2
.byte \anim1Carg3
.byte \anim1Carg4
.endm

.macro playsoundwait anim1Darg1 anim1Darg2 anim1Darg3
.byte 0x1D
.hword \anim1Darg1
.byte \anim1Darg2
.byte \anim1Darg3
.endm

.macro set_BLDCNT set_BLDCNTvalue
.byte 0x1E
.hword \set_BLDCNTvalue
.endm

.macro launchsoundtask launchsoundtaskPtr launchsoundtaskArgsNo launchsoundtaskarg0 launchsoundtaskarg1 launchsoundtaskarg2 launchsoundtaskarg3 launchsoundtaskarg4 launchsoundtaskarg5 launchsoundtaskarg6 launchsoundtaskarg7 launchsoundtaskarg8
.byte 0x1F
.word \launchsoundtaskPtr
.byte \launchsoundtaskArgsNo
.hword \launchsoundtaskarg0
.hword \launchsoundtaskarg1
.hword \launchsoundtaskarg2
.hword \launchsoundtaskarg3
.hword \launchsoundtaskarg4
.hword \launchsoundtaskarg5
.hword \launchsoundtaskarg6
.hword \launchsoundtaskarg7
.hword \launchsoundtaskarg8
.endm

.macro waitforsound
.byte 0x20
.endm

.macro jumpifargmatches jumpifargmatchesargID jumpifargmatchesValue jumpifargmatchesPtr
.byte 0x21
.byte \jumpifargmatchesargID
.hword \jumpifargmatchesValue
.word \jumpifargmatchesPtr
.endm

.macro pokespritetoBG2 animABank
.byte 0x22
.byte \animABank
.endm

.macro pokespritefromBG2 animBBank
.byte 0x23
.byte \animBBank
.endm

.macro jumpifnotinbattle jumpifnotinbattlePTR
.byte 0x24
.word \jumpifnotinbattlePTR
.endm

.macro chooseBG chooseBG1 chooseBG2 chooseBG3
.byte 0x25
.byte \chooseBG1
.byte \chooseBG2
.byte \chooseBG3
.endm

.macro playsoundpanchange2 anim26H0 anim26B0 anim26B1 anim26B2 anim26B3
.byte 0x26
.hword \anim26H0
.byte \anim26B0
.byte \anim26B1
.byte \anim26B2
.byte \anim26B3
.endm

.macro playsoundpanchange3 anim27H0 anim27B0 anim27B1 anim27B2 anim27B3
.byte 0x27
.hword \anim27H0
.byte \anim27B0
.byte \anim27B1
.byte \anim27B2
.byte \anim27B3
.endm

.macro leftbankBG_over_partnerBG anim28Bank
.byte 0x28
.byte \anim28Bank
.endm

.macro bankBG_over_partnerBG
.byte 0x29
.endm

.macro leftopponentbankBG_over_partnerBG anim2Aarg
.byte 0x2A
.byte \anim2Aarg
.endm

.macro makebankinvisible makebankinvisibleBank
.byte 0x2B
.byte \makebankinvisibleBank
.endm

.macro makebankvisible makebankvisibleBank
.byte 0x2C
.byte \makebankvisibleBank
.endm

.macro stopmusic
.byte 0x2F
.endm

@objtemplate

.macro objtemplate objtemplatetiletag objtemplatepaltag objtemplateoam objtemplateimageanim objtemplategfx objtemplaterotscale objtemplatecallback
.hword \objtemplatetiletag
.hword \objtemplatepaltag
.word \objtemplateoam
.word \objtemplateimageanim
.word \objtemplategfx
.word \objtemplaterotscale
.word \objtemplatecallback
.endm


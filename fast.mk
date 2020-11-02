
include Makefile

build/emerald/data/event_scripts.o:data/event_scripts.s data/maps/Route101/scripts.inc
	$(PREPROC) $< charmap.txt | $(CPP) -I include | $(GBAEXE) | $(AS) $(ASFLAGS) -o $@

$(ELF): $(C_OBJS) $(DATA_ASM_OBJS)
	@cd $(OBJ_DIR) && $(LD) $(LDFLAGS) -T ld_script.ld -o ../../$@ $(OBJS_REL) $(LIB)
	@$(FIX) $@ -t"$(TITLE)" -c$(GAME_CODE) -m$(MAKER_CODE) -r$(REVISION) --silent

$(ROM): $(ELF)
	$(OBJCOPY) -O binary $< $@
	$(FIX) $@ -p --silent

elf : $(ROM);

.PHONY: elf
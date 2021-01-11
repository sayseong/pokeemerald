if [ "$1" == "fast" ]; then
  #rm build/emerald/src/pokemon.o
  make DINFO=1 -j8 -f fast.mk elf
elif [ "$1" == "fastn" ]; then
  make DINFO=1 -j8 NODEP=0 -f fast.mk elf
elif [ "$1" == "full" ]; then
  make -j8 DINFO=1
elif [ "$1" == "final" ]; then
  make -j8 MODERN=1
elif [ "$1" == "finalf" ]; then
  make -j8 MODERN=1  -f fast.mk elf
elif [ "$1" == "md" ]; then
  make -j8 MODERN=1 DINFO=1
elif [ "$1" == "mdf" ]; then
  make -j8 MODERN=1 DINFO=1 -f fast.mk elf
elif [ "$1" == "fastrun" ]; then
  make DINFO=1 -f fast.mk elf
  mgba --gdb pokeemerald.gba &
elif [ "$1" == "run" ]; then
  mgba --gdb pokeemerald.gba &
fi
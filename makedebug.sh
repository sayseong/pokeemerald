if [ "$1" == "fast" ]; then
  #rm build/emerald/src/pokemon.o
  make DINFO=1 MODERN=1 -f fast.mk elf
elif [ "$1" == "fastn" ]; then
  make DINFO=1 MODERN=1 NODEP=0 -f fast.mk elf
elif [ "$1" == "full" ]; then
  make DINFO=1 -j8 MODERN=1
elif [ "$1" == "final" ]; then
  rm -rf build
  make -j8
elif [ "$1" == "fastrun" ]; then
  make DINFO=1 -f fast.mk elf
  mgba --gdb pokeemerald.gba &
elif [ "$1" == "run" ]; then
  mgba --gdb pokeemerald.gba &
fi
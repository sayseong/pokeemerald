import os
import subprocess
import sys

data = open("D:\\Data\\1.txt", encoding="utf-8")
i = 35
for line in data:
    line = line.strip().split()
    print("#define MOVE_" + line[3].upper() + "_" + line[4].upper() + " (MOVES_COUNT_GEN7 + %d)" % i)
    i=i+1

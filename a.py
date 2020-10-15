import sys
from sys import stdin
from typing import TextIO


def init_table():
    reader = open("p.txt", 'r')
    dic = {}
    while True:
        line = reader.readline()
        if line == "":
            break
        key, value = line.strip().split('=')
        dic[key] = value
    return dic


class AsmGenerator:
    def __init__(self, reader: TextIO):
        self.table = init_table()
        self.reader = reader

    def process(self):
        while True:
            line = self.reader.readline()
            if line == "": return
            line = line.rstrip()
            print(line)
            proccessline = line
            if not proccessline.endswith(":"): continue
            symbol = proccessline[0:len(proccessline) - 1]
            value = self.table.get(symbol)
            if value is None: continue
            if value.startswith(".incbin"):
                value, _type = value.split(";")
                if _type == "STRUCT":
                    self.print_struct(value)
                    continue
            # ARRAY AND ARRAY2
            self.print_array(symbol, value)

    def print_struct(self, value: str):
        file, skip, count = value[8:].split(",")
        file = open(file.strip("\""), 'rb')
        file.seek(int(skip))
        data = file.read(int(count))
        file.close()
        count = 0
        while True:
            line = self.reader.readline().strip()
            if line.startswith(".byte"):
                print(".byte " + hex(data[count]))
                count = count + 1
            elif line.startswith(".short"):
                print(line)
                count = count + 2
            elif line.startswith(".space"):
                print(".byte ", end='')
                c = int(line[7:].strip())
                for d in data[count:count + c - 1]:
                    print(hex(d), end=',')
                print(hex(data[count + c - 1]))
                count = count + c
            elif line.startswith(".word"):
                print(line)
                count = count + 4
            else:
                print(line)
                return

    def print_array(self, symbol: str, value: str):
        print(value)
        while True:
            line = self.reader.readline().strip()
            if not (line.startswith(".byte") or line.startswith(".space")):
                break
        if line.startswith(".size"):
            if value.startswith(".incbin"):
                print("\t.size " + symbol + ',' + value[8:].split(",")[2])
            else:
                print("\t.size " + symbol + ',' + str(((len(line) - 9) // 3 + 1)))
        else:
            print(line)


if len(sys.argv) == 1:
    AsmGenerator(stdin).process()
else:
    AsmGenerator(open(sys.argv[1], 'r')).process()

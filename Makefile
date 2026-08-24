CC := g++
STD := -std=c++20
DEBUG := -g -DDEBUG -fsanitize=address
WARNINGS := -Wall -Wextra -Wpedantic

SRC_FILES := $(wildcard src/*.cc src/*/*.cc)
OBJ_FILES := $(SRC_FILES:.cc=.o)

EXECUTABLES := ababiu

all: $(EXECUTABLES) libababiu.a

ababiu: src/main.o $(OBJ_FILES)
	$(CC) $^ -o $@ $(DEBUG)
%.o: %.cc
	$(CC) -c $< -o $@ $(STD) $(DEBUG) $(WARNINGS)

libababiu.a: libababiu/*
	cd libababiu && gcc -c io.c -o io.o
	cd libababiu && nasm -f elf64 start.asm -o start.o
	cd libababiu && nasm -f elf64 pow.asm -o pow.o
	cd libababiu && ar rcs ../$@ io.o start.o pow.o

clean:
	rm -f $(OBJ_FILES) $(EXECUTABLES)
	rm -f libababiu.a libababiu/*.o

.PHONY: all clean

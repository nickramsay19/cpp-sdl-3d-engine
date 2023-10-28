SDL = /home/sizzler/Documents/Packages/SDL

CC = g++ -std=gnu++20 -I $(SDL)/include -L $(SDL)/build -l SDL2

all: bin/main

bin/main: build/main.o bin
	$(CC) -o $@ $<

build/main.o: src/main.cpp build
	$(CC) -o $@ -c $<

bin:
	mkdir bin

build:
	mkdir build

.PHONY: clean
clean: 
	rm -rf bin build

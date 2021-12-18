# INC_DIR=src/logging
CFLAGS=-c -W -ansi -pedantic -Wall -Isrc -std=gnu99

.PHONY: clean directories

all: clean directories cred

cred: main.o flags.o logging.o config.o
	gcc -o bin/cred obj/main.o obj/flags.o obj/logging.o obj/config.o

main.o: src/main.c src/flags.h
	gcc -o obj/main.o src/main.c $(CFLAGS)

flags.o: src/flags.h src/flags.c
	gcc -o obj/flags.o src/flags.c $(CFLAGS)

logging.o: src/logging/logging.h src/logging/logging.c
	gcc -o obj/logging.o src/logging/logging.c $(CFLAGS)

config.o: src/config/config.h src/config/config.c
	gcc -Isrc -o obj/config.o src/config/config.c $(CFLAGS)

clean:
	rm -rf bin/* obj/*

directories:
	mkdir -p bin/ obj/
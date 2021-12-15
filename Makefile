all: cred

cred: main.o flags.o logging.o
	gcc -o bin/cred obj/main.o obj/flags.o obj/logging.o

main.o: src/main.c src/flags.h
	gcc -o obj/main.o src/main.c -c -W -Wall -ansi -pedantic

flags.o: src/flags.h src/flags.c
	gcc -o obj/flags.o src/flags.c -c -W -Wall -ansi -pedantic

logging.o: src/logging/logging.h src/logging/logging.c
	gcc -o obj/logging.o src/logging/logging.c -c -W -Wall -ansi -pedantic

clean:
	rm -rf bin/* obj/*
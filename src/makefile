all: program.bin

program.bin: program.o

program.o: wright_cs390program3.c
	gcc wright_cs390program3.c -o ../bin/program.bin -ansi

start: program.bin
	../bin/program.bin

format:
	clang-format -i *.c
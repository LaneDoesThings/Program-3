all: program.bin

program.bin: program.o

program.o: wright_cs390program3.c
	gcc wright_cs390program3.c -o ../bin/program.bin -ansi

format:
	clang-format -i *.c
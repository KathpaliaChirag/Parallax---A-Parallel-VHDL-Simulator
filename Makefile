# Parallax --- A Parallel VHDL Simulator
# CHIRAG 2025MCS2098 :: IIT Delhi COP7001
# NOTE: this Makefile was built with AI assistance (Claude)

CC = gcc
CFLAGS = -Wall -g

SRC = src
PARSER = $(SRC)/parser
TESTS = tests/circuit/basic

all: sim parser
	@echo "build complete! run 'make test' to test parser, 'make run' to run simulator"

sim: main.c
	$(CC) $(CFLAGS) main.c -o main.exe

parser: $(PARSER)/parser.y $(PARSER)/lexer.l $(PARSER)/ast.c $(PARSER)/ast.h
	bison -d $(PARSER)/parser.y -o $(PARSER)/parser.c
	flex -o $(PARSER)/lexer.c $(PARSER)/lexer.l
# 	$(CC) $(CFLAGS) $(PARSER)/parser.c $(PARSER)/lexer.c $(PARSER)/ast.c -o parser_test.exe
	$(CC) $(CFLAGS) $(PARSER)/parser.c $(PARSER)/lexer.c $(PARSER)/ast.c $(PARSER)/ast_walker.c -o parser_test.exe

test: parser
	./parser_test.exe < $(TESTS)/and_gate.vhdl

run: sim
	./main.exe

clean:
	rm -f main.exe parser_test.exe
	rm -f $(PARSER)/parser.c $(PARSER)/parser.h $(PARSER)/lexer.c

help:
	@echo "make       → build everything"
	@echo "make test  → test parser on and_gate.vhdl"
	@echo "make run   → run simulator"
	@echo "make clean → remove generated files"

.PHONY: all sim parser test run clean help
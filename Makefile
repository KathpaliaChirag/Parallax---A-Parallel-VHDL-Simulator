# Parallax --- A Parallel VHDL Simulator
# CHIRAG 2025MCS2098 :: IIT Delhi COP7001
# NOTE: this Makefile was built with AI assistance (Claude)

CC = gcc
CFLAGS = -Wall -g
# CHIRAG 02-04-26 :: added fopenmp flag ready for phase 2 parallel engine
OFLAGS = -fopenmp
SRC = src
CORE = $(SRC)/core
SIM = $(SRC)/sim
PARSER = $(SRC)/parser
OUTPUT = $(SRC)/output
ANALYSIS = $(SRC)/analysis
TESTS = tests/circuit/basic

all: sim parser
	@echo "build complete! run 'make test' to test parser, 'make run' to run simulator"

sim: main.c
	$(CC) $(CFLAGS) main.c -o main.exe

# CHIRAG 02-04-26 :: proper sim target that links object files separately
# this is needed for phase 2 ... cant link openmp correctly with the include-everything approach
sim_proper: main.c
	$(CC) $(CFLAGS) $(OFLAGS) \
		main.c \
		$(CORE)/event_queue.c \
		$(CORE)/signal.c \
		$(CORE)/process.c \
		$(CORE)/scheduler.c \
		$(CORE)/delta.c \
		$(SIM)/sequential.c \
		$(OUTPUT)/vcd.c \
		$(OUTPUT)/trace.c \
		-o main.exe

parser: $(PARSER)/parser.y $(PARSER)/lexer.l $(PARSER)/ast.c $(PARSER)/ast_walker.c
	bison -d $(PARSER)/parser.y -o $(PARSER)/parser.c
	flex -o $(PARSER)/lexer.c $(PARSER)/lexer.l
	$(CC) $(CFLAGS) \
		$(PARSER)/parser.c \
		$(PARSER)/lexer.c \
		$(PARSER)/ast.c \
		$(PARSER)/ast_walker.c \
		$(CORE)/signal.c \
		$(CORE)/process.c \
		$(CORE)/scheduler.c \
		$(CORE)/event_queue.c \
		$(CORE)/delta.c \
		$(SIM)/sequential.c \
		$(OUTPUT)/vcd.c \
		$(OUTPUT)/trace.c \
		$(ANALYSIS)/graph.c \
		$(ANALYSIS)/dependency.c \
		-o parser_test.exe

test: parser
	./parser_test.exe and-gate < tests/circuit/basic/multi_and.vhdl

test_or: parser
	./parser_test.exe or-gate < tests/circuit/basic/or_gate.vhdl

run: sim
	./main.exe

# CHIRAG 02-04-26 :: gtkwave targets to visualize VCD output directly from make
wave_dff:
	gtkwave output-DFF.vcd

wave_and:
	gtkwave output-AND.vcd

wave_parser:
	gtkwave output-and-gate.vcd

# CHIRAG 13-04-26 :: generic test target ... usage: make circuit FILE=tests/circuit/basic/multi_and.vhdl
# pass any vhdl file from any directory ... output vcd auto named after file
circuit: parser
	./parser_test.exe $(basename $(notdir $(FILE))) < $(FILE)
# CHIRAG 13-04-26 :: run circuit with optional testbench
# usage: make circuit FILE=tests/circuit/basic/multi_and.vhdl
# usage: make circuit FILE=tests/circuit/basic/multi_and.vhdl TB=tests/circuit/basic/multi_and_tb.txt
circuit: parser
	./parser_test.exe $(basename $(notdir $(FILE))) $(TB) < $(FILE)
clean:
	rm -f main.exe parser_test.exe
	rm -f $(PARSER)/parser.c $(PARSER)/parser.h $(PARSER)/lexer.c

help:
	@echo "make            -> build everything"
	@echo "make test       -> test parser on and_gate.vhdl"
	@echo "make run        -> run simulator"
	@echo "make sim_proper -> build with openmp support"
	@echo "make wave_dff   -> open DFF waveform in gtkwave"
	@echo "make wave_and   -> open AND gate waveform in gtkwave"
	@echo "make wave_parser-> open parser output waveform in gtkwave"
	@echo "make clean      -> remove generated files"

.PHONY: all sim sim_proper parser test test_or run wave_dff wave_and wave_parser clean help
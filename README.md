# Parallax---A-Parallel-VHDL-Simulator
This repo is the final project of COP7001/COD7001 under Prof. Kolin Paul. Parallax is a parallel discrete event simulator that redesigns VHDL simulation for multicore systems while preserving cycle-accurate and deterministic semantics.


plan in details

## phase 2: days 21-40 — sequential simulator

### day 21 — signal representation
what even is a signal in VHDL? its basically a wire that holds a value and has a history. need to represent that in C.

- figure out what fields a signal needs — name, type, current value, time of last change (done)
- write signal.h with the struct (donee)
- write signal.c with a basic update function (done)
- done when: can create a signal, set its value, read it back

### day 22 — process representation
a process in VHDL wakes up when signals it cares about change. need to represent that.

- write process.h — process struct with sensitivity list and a function pointer
- write process.c — register a process, list what signals it watches
- done when: can create a process with a sensitivity list

### day 23 — connect signals and processes
when a signal changes, the right processes need to wake up. this is the connection between day 21 and day 22.

- write scheduler.h/c — given a signal change, find which processes care
- done when: signal change correctly notifies the right processes

### day 24 — delta cycles, understanding first
delta cycles are the trickiest part of VHDL. zero time, cascading updates. need to actually understand this before coding it.

- read about delta cycles properly, not just skim
- extend event struct to have (time, delta) ordering
- write delta.h/c — delta counter, distinguish delta event from time event
- done when: events have proper (time, delta) ordering

### day 25 — delta cycle engine
now actually make the simulation loop handle delta cycles correctly.

- keep processing delta events until there are none left
- only advance simulation time when delta queue is empty
- done when: a simple cascading signal update works without time advancing

### day 26 — hardcode an AND gate
no parser yet. manually describe a circuit in C and simulate it. this is the sanity check.

- hardcode AND gate as signals + processes in main.c
- run simulation, check truth table
- done when: AND gate gives correct output for all input combinations

### day 27 — hardcode a flip flop
flip flop will actually exercise delta cycles. good test.

- hardcode D flip flop
- check that it only updates on clock edge
- done when: flip flop toggles correctly

### day 28 — VCD output
VCD is a standard waveform format. GTKWave can read it. this is how we visualize simulation and also how we demo.

- read VCD format spec, its simple
- write vcd.h/c — open file, write header, write signal changes
- done when: GTKWave opens the file without errors

### day 29 — trace hashing
need a way to check that sequential and parallel produce identical output. hash the signal change sequence.

- write trace.h/c — record signal changes in order, hash them
- done when: same simulation always gives same hash, different simulation gives different hash

### day 30 — flex lexer start
finally starting the parser. lexer first, tokenize the VHDL subset.

- review the VHDL subset grammar we defined
- write lexer.l — tokens for entity, architecture, signal, process, <=, after, operators
- done when: lexer tokenizes a simple VHDL file without errors

### day 31 — flex lexer complete
- handle all keywords, operators, identifiers, numbers
- test on a few different VHDL files
- done when: no tokenization errors on any valid subset VHDL

### day 32 — bison parser start + AST
- write parser.y — start with entity declaration rule
- write ast.h — define AST node types
- done when: parser builds AST for just the entity part

### day 33 — parser — signals and processes
- add grammar rules for signal declarations
- add grammar rules for process with sensitivity list
- done when: parser builds full AST for a complete architecture

### day 34 — parser — expressions and assignments
- add rules for <= assignment
- add rules for and, or, not, xor, if statements
- add after delay parsing
- done when: parser handles the complete VHDL subset

### day 35 — AST to simulator
the parser gives us an AST. now walk it and create actual signal/process structs from it.

- write AST walker — create signals from AST signal nodes
- write AST walker — create processes from AST process nodes
- done when: parser output feeds directly into the simulator engine

### day 36 — full pipeline test
first time running VHDL file to waveform output end to end.

- write AND gate in actual VHDL
- run: VHDL file -> lexer -> parser -> AST -> simulator -> VCD
- done when: GTKWave shows correct AND gate waveform from a real VHDL file

### day 37 — test more circuits
- OR gate, NOT gate, XOR gate
- D flip flop
- fix whatever breaks
- done when: all basic circuits produce correct waveforms

### day 38 — FSM test
- write a simple 2 state FSM in VHDL
- simulate, verify state transitions
- done when: FSM waveform matches expected behavior

### day 39 — bug fixing and cleanup
- fix remaining bugs
- clean up code, add comments where confusing
- compile with -Wall, fix all warnings
- done when: clean build, no known bugs

### day 40 — submit sequential simulator
- final test run on all circuits
- clean commit
- DELIVERABLE: sequential baseline due today

---

## phase 3: days 41-70 — parallel implementation

### day 41 — dependency graph data structure
before we can parallelize, we need to know which processes can run together. need a graph for that.

- write graph.h/c — adjacency list, nodes are processes, edges are shared signals
- done when: can add nodes, add edges, query neighbors

### day 42 — dependency analysis
- write dependency.h/c — analyze which processes share signals at the same timestamp
- done when: given a set of processes, returns the dependency graph

### day 43 — find independent process sets
- from the dependency graph, find groups that can run in parallel
- done when: returns list of independent process groups per timestamp

### day 44 — OpenMP setup
- add OpenMP flags to Makefile
- write a simple parallel hello world to confirm it works
- done when: multiple threads run correctly

### day 45 — parallel simulation loop first draft
- write parallel.h/c — parallel version of simulation loop
- use OpenMP parallel sections for independent process groups
- done when: parallel loop runs without crashing (correctness not required yet)

### day 46 — barrier placement
- add barriers at delta cycle boundaries
- no process should cross a delta boundary before all others finish
- done when: barriers placed correctly, no early crossing

### day 47-48 — signal update atomicity
- find all places where signals are written
- make those writes atomic or mutex protected
- done when: no data races (test with -fsanitize=thread)

### day 49-50 — correctness check on simple circuits
- run AND gate and flip flop on both sequential and parallel
- compare trace hashes
- done when: hashes match

### day 51-52 — correctness on harder circuits
- FSM and pipeline through both simulators
- fix correctness bugs
- done when: all circuits produce identical hashes

### day 53-54 — performance measurement
- time sequential vs parallel on each circuit
- test on 1, 2, 4 cores
- done when: have actual speedup numbers

### day 55-56 — optimize where possible
- identify bottlenecks — is it lock contention? barrier overhead?
- try to reduce overhead
- done when: speedup improves on at least one circuit

### day 57-58 — pipeline circuit
- write a 2 stage pipeline in VHDL
- simulate on both, verify correctness, measure speedup
- done when: pipeline works correctly in parallel

### day 59-60 — scalability analysis
- understand which circuits benefit from parallelism and which dont
- wide ALU should scale, ripple carry adder probably wont
- done when: can explain the results, not just report numbers

### day 61-62 — 8 bit ALU benchmark
- write 8 bit ALU in VHDL
- run benchmark, measure speedup
- done when: ALU simulates correctly in parallel

### day 63-64 — stress testing
- large circuits, long simulations
- valgrind for memory leaks
- done when: no crashes, no leaks

### day 65 — performance report draft
- write up performance results
- speedup table per circuit per core count
- done when: performance section written

### day 66-68 — parallel code cleanup
- remove debug prints
- clean up comments
- make sure parallel code is readable and reproducible

### day 69 — final correctness sweep
- run everything one more time
- compare all hashes
- done when: 100% correct on all test circuits

### day 70 — submit parallel implementation
- clean commit
- DELIVERABLE: parallel implementation due today

---

## phase 4: days 71-80 — demo + gui + benchmarks

### day 71-72 — GTKWave output cleanup
- make VCD output clean and demo ready
- test all circuits in GTKWave

### day 73-74 — demo circuits
- pick 3 circuits that show speedup clearly
- make sure they look good in GTKWave

### day 75 — performance writeup
- final speedup numbers
- overhead analysis
- scalability explanation

### day 76-77 — final documentation
- README, design doc updates, code comments
- someone else should be able to read this codebase

### day 78 — demo rehearsal
- run through everything end to end
- time it

### day 79 — buffer
- fix whatever broke last minute
- no new features

### day 80 — submit everything
- DELIVERABLE: demo + GUI + benchmarks + final


---

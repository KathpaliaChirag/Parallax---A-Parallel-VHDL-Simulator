# CHIRAG 21-04-26 :: testbench generator for wide_and128
# generates 500 time steps with random input toggles
# each step toggles random subset of inputs ... forces many process firings
# 500 steps x 128 processes = ~64000 potential process firings
# enough work to dominate OMP overhead and show real speedup
# CHIRAG 21-04-26 :: WIDE_AND128 ... 128-wide parallel AND gate array
# =======================================================================
#
# WHAT IS THIS CIRCUIT?
# a bank of 128 independent single-bit AND gates
# each gate takes two inputs (Ai, Bi) and produces one output (Yi = Ai AND Bi)
# all 128 gates are completely independent ... no gate reads another gate's output
# this is the hardware equivalent of a SIMD (Single Instruction Multiple Data) unit
#
# REAL WORLD USE:
# wide comparators in CPU branch predictors ... compare 128 bits simultaneously
# parallel CRC computation ... each bit of CRC computed independently
# SIMD logic units in GPUs ... 128 lanes of AND in one clock cycle
# content-addressable memory (CAM) ... match logic across wide data words
# network packet filtering ... 128-bit mask AND applied in one cycle
#
# WHY THIS CIRCUIT FOR PARALLAX BENCHMARKING?
# this is the BEST CASE circuit for our parallel simulator
# dependency graph has 128 nodes and 0 edges ... no conflicts at all
# Welsh-Powell coloring assigns all 128 to color 0 ... one batch
# all 128 processes run simultaneously in a single OMP parallel for
# serial fraction S is purely OMP overhead ... no algorithmic serialization
# Amdahl theoretical max speedup approaches N (number of threads)
#
# ARCHITECTURE NOTE:
# we reuse A0-A31 and B0-B31 as inputs for all 128 outputs
# this keeps port count manageable (64 inputs instead of 256)
# while still giving us 128 independent processes to parallelize
# in a real design each Yi would have its own dedicated Ai Bi inputs
#
# KNOWN LIMITATION:
# each process does only 1 AND operation ... ~5 nanoseconds of work
# OMP thread overhead is ~10-50 microseconds ... overhead still dominates
# use with generated testbench (generate_tb.py) for 500+ time steps
# at that scale process firings accumulate enough to show real speedup
#
# AI ASSISTANCE NOTE:
# circuit structure and port declarations were designed with Claude (Anthropic)
# the idea of reusing A0-A31 B0-B31 across 128 outputs to keep port count
# manageable while maximizing process count came from AI discussion
# parallelism analysis and Amdahl story are mine
# =======================================================================
import random
random.seed(42)  # fixed seed for reproducibility

signals = [f"A{i}" for i in range(32)] + [f"B{i}" for i in range(32)]
steps = 500

with open("tests/circuit/basic/wide_and128_tb.txt", "w") as f:
    f.write("# CHIRAG 21-04-26 :: generated testbench for wide_and128\n")
    f.write("# 500 time steps ... random input toggles ... forces heavy parallel load\n")
    for t in range(1, steps + 1):
        # toggle a random subset of signals at each time step
        for sig in random.sample(signals, random.randint(4, 16)):
            val = random.randint(0, 1)
            f.write(f"{sig} {val} {t}\n")

print("generated wide_and128_tb.txt with", steps, "time steps")
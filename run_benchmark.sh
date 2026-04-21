#!/bin/bash

declare -A TBS=(
  [chainof4]="tests/circuit/basic/chainof4_tb.txt"
  [eight_channel]="tests/circuit/basic/eight_channel_tb.txt"
  [pipeline8]="tests/circuit/basic/pipeline8_tb.txt"
  [thirty2_channel]="tests/circuit/basic/thirty2_channel_tb.txt"
  [wide_and128]="tests/circuit/basic/wide_and128_tb.txt"
  [dff]="tests/circuit/basic/dff_tb.txt"
  [multi_and]="tests/circuit/basic/multi_and_tb.txt"
  [ripple]="tests/circuit/basic/ripple_tb.txt"
  [traffic_light]="tests/circuit/fsm/traffic_light_tb.txt"
)

declare -A VHDLS=(
  [chainof4]="tests/circuit/basic/chainof4.vhdl"
  [eight_channel]="tests/circuit/basic/eight_channel.vhdl"
  [pipeline8]="tests/circuit/basic/pipeline8.vhdl"
  [thirty2_channel]="tests/circuit/basic/thirty2_channel.vhdl"
  [wide_and128]="tests/circuit/basic/wide_and128.vhdl"
  [dff]="tests/circuit/basic/dff.vhdl"
  [multi_and]="tests/circuit/basic/multi_and.vhdl"
  [ripple]="tests/circuit/basic/ripple.vhdl"
  [traffic_light]="tests/circuit/fsm/traffic_light.vhdl"
)

for C in "${!TBS[@]}"; do
  for S in 0 10000 50000 100000; do
    echo "========================================"
    echo "CIRCUIT=$C  STRESS=$S"
    echo "========================================"
    echo -n "  SEQ T=1 : "
    make circuit FILE=${VHDLS[$C]} TB=${TBS[$C]} MODE=--seq THREADS=1 STRESS=$S 2>/dev/null | grep -E "simulation time|trace hash|process firings|delta cycles"
    for T in 1 2 4 8; do
      echo -n "  PAR T=$T : "
      make circuit FILE=${VHDLS[$C]} TB=${TBS[$C]} MODE=--par THREADS=$T STRESS=$S 2>/dev/null | grep -E "simulation time|trace hash|process firings|delta cycles"
    done
    echo ""
  done
done

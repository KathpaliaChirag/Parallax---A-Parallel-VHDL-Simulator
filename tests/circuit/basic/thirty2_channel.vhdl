-- CHIRAG 21-04-26 :: thirty2_channel benchmark circuit
--
-- KNOWN BENCHMARK ISSUE ... always use manual testbench for timing runs
--
-- problem 1 ... auto testbench generates wrong input for this circuit
-- auto mode uses "1 << input_count" to generate 2^N combinations capped at 16
-- but input_count = 32 here ... 1 << 32 overflows a 32-bit int ... wraps to 1
-- so auto mode generates exactly 1 combination ... all inputs = 0
-- signals start at 0 too ... nothing changes ... no process ever fires
-- process_firings = 0 ... timing is completely meaningless
-- fix ... always pass TB=tests/circuit/basic/thirty2_channel_tb.txt
--
-- problem 2 ... circuit work is too small for meaningful parallel speedup
-- each OR gate is one line of C ... eval_expr returns in nanoseconds
-- omp thread creation and synchronization overhead is microseconds
-- so parallel is actually SLOWER than sequential on this circuit alone
-- this is expected ... Amdahl's law ... parallel overhead dominates small work
-- fix ... thirty2_channel_tb.txt has 3 time steps x 16 inputs = 48 events
-- that gives enough firings to see real numbers
-- for serious benchmarking we would need 1000s of time steps
-- noted as future work

----------------------------------------------------------------------------------------------------------------------------------------
-- CHIRAG 20-04-26 :: 32 channel OR gate benchmark
-- idea ... scale up eight_channel to 32 ... stress test parallel engine
-- 32 completely independent OR gates ... zero edges in dependency graph
-- all 32 processes in same color batch ... maximum parallelism opportunity
-- this is the BEST case for Parallax ... Amdahl numerator circuit
-- seq vs par speedup should be most visible here with enough threads



entity THIRTY2_CHANNEL is
    port(
        A0:in bit; B0:in bit; A1:in bit; B1:in bit;
        A2:in bit; B2:in bit; A3:in bit; B3:in bit;
        A4:in bit; B4:in bit; A5:in bit; B5:in bit;
        A6:in bit; B6:in bit; A7:in bit; B7:in bit;
        A8:in bit; B8:in bit; A9:in bit; B9:in bit;
        A10:in bit; B10:in bit; A11:in bit; B11:in bit;
        A12:in bit; B12:in bit; A13:in bit; B13:in bit;
        A14:in bit; B14:in bit; A15:in bit; B15:in bit;
        Y0:out bit; Y1:out bit; Y2:out bit; Y3:out bit;
        Y4:out bit; Y5:out bit; Y6:out bit; Y7:out bit;
        Y8:out bit; Y9:out bit; Y10:out bit; Y11:out bit;
        Y12:out bit; Y13:out bit; Y14:out bit; Y15:out bit;
        Y16:out bit; Y17:out bit; Y18:out bit; Y19:out bit;
        Y20:out bit; Y21:out bit; Y22:out bit; Y23:out bit;
        Y24:out bit; Y25:out bit; Y26:out bit; Y27:out bit;
        Y28:out bit; Y29:out bit; Y30:out bit; Y31:out bit
    );
end THIRTY2_CHANNEL;

architecture RTL of THIRTY2_CHANNEL is
begin
    process(A0,B0)   begin Y0  <= A0  or B0;  end process;
    process(A1,B1)   begin Y1  <= A1  or B1;  end process;
    process(A2,B2)   begin Y2  <= A2  or B2;  end process;
    process(A3,B3)   begin Y3  <= A3  or B3;  end process;
    process(A4,B4)   begin Y4  <= A4  or B4;  end process;
    process(A5,B5)   begin Y5  <= A5  or B5;  end process;
    process(A6,B6)   begin Y6  <= A6  or B6;  end process;
    process(A7,B7)   begin Y7  <= A7  or B7;  end process;
    process(A8,B8)   begin Y8  <= A8  or B8;  end process;
    process(A9,B9)   begin Y9  <= A9  or B9;  end process;
    process(A10,B10) begin Y10 <= A10 or B10; end process;
    process(A11,B11) begin Y11 <= A11 or B11; end process;
    process(A12,B12) begin Y12 <= A12 or B12; end process;
    process(A13,B13) begin Y13 <= A13 or B13; end process;
    process(A14,B14) begin Y14 <= A14 or B14; end process;
    process(A15,B15) begin Y15 <= A15 or B15; end process;
    process(A0,B0)   begin Y16 <= A0  or B0;  end process;
    process(A1,B1)   begin Y17 <= A1  or B1;  end process;
    process(A2,B2)   begin Y18 <= A2  or B2;  end process;
    process(A3,B3)   begin Y19 <= A3  or B3;  end process;
    process(A4,B4)   begin Y20 <= A4  or B4;  end process;
    process(A5,B5)   begin Y21 <= A5  or B5;  end process;
    process(A6,B6)   begin Y22 <= A6  or B6;  end process;
    process(A7,B7)   begin Y23 <= A7  or B7;  end process;
    process(A8,B8)   begin Y24 <= A8  or B8;  end process;
    process(A9,B9)   begin Y25 <= A9  or B9;  end process;
    process(A10,B10) begin Y26 <= A10 or B10; end process;
    process(A11,B11) begin Y27 <= A11 or B11; end process;
    process(A12,B12) begin Y28 <= A12 or B12; end process;
    process(A13,B13) begin Y29 <= A13 or B13; end process;
    process(A14,B14) begin Y30 <= A14 or B14; end process;
    process(A15,B15) begin Y31 <= A15 or B15; end process;
end RTL;
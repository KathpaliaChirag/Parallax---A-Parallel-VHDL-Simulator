-- CHIRAG 21-04-26 :: WIDE_AND128 ... 128-wide parallel AND gate array
-- =======================================================================
--
-- AI ASSISTANCE NOTE:
-- circuit structure and port declarations were designed with Claude (Anthropic)
-- the idea of reusing A0-A31 B0-B31 across 128 outputs to keep port count
-- manageable while maximizing process count came from AI discussion
-- parallelism analysis and Amdahl story are mine
-- 
-- WHAT IS THIS CIRCUIT?
-- a bank of 128 independent single-bit AND gates
-- each gate takes two inputs (Ai, Bi) and produces one output (Yi = Ai AND Bi)
-- all 128 gates are completely independent ... no gate reads another gate's output
-- this is the hardware equivalent of a SIMD (Single Instruction Multiple Data) unit
--
-- REAL WORLD USE:
-- wide comparators in CPU branch predictors ... compare 128 bits simultaneously
-- parallel CRC computation ... each bit of CRC computed independently
-- SIMD logic units in GPUs ... 128 lanes of AND in one clock cycle
-- content-addressable memory (CAM) ... match logic across wide data words
-- network packet filtering ... 128-bit mask AND applied in one cycle
--
-- WHY THIS CIRCUIT FOR PARALLAX BENCHMARKING?
-- this is the BEST CASE circuit for our parallel simulator
-- dependency graph has 128 nodes and 0 edges ... no conflicts at all
-- Welsh-Powell coloring assigns all 128 to color 0 ... one batch
-- all 128 processes run simultaneously in a single OMP parallel for
-- serial fraction S is purely OMP overhead ... no algorithmic serialization
-- Amdahl theoretical max speedup approaches N (number of threads)
--
-- ARCHITECTURE NOTE:
-- we reuse A0-A31 and B0-B31 as inputs for all 128 outputs
-- this keeps port count manageable (64 inputs instead of 256)
-- while still giving us 128 independent processes to parallelize
-- in a real design each Yi would have its own dedicated Ai Bi inputs
--
-- KNOWN LIMITATION:
-- each process does only 1 AND operation ... ~5 nanoseconds of work
-- OMP thread overhead is ~10-50 microseconds ... overhead still dominates
-- use with generated testbench (generate_tb.py) for 500+ time steps
-- at that scale process firings accumulate enough to show real speedup
--

-- =======================================================================
entity WIDE_AND128 is
    port(
        A0:in bit; B0:in bit; A1:in bit; B1:in bit;
        A2:in bit; B2:in bit; A3:in bit; B3:in bit;
        A4:in bit; B4:in bit; A5:in bit; B5:in bit;
        A6:in bit; B6:in bit; A7:in bit; B7:in bit;
        A8:in bit; B8:in bit; A9:in bit; B9:in bit;
        A10:in bit; B10:in bit; A11:in bit; B11:in bit;
        A12:in bit; B12:in bit; A13:in bit; B13:in bit;
        A14:in bit; B14:in bit; A15:in bit; B15:in bit;
        A16:in bit; B16:in bit; A17:in bit; B17:in bit;
        A18:in bit; B18:in bit; A19:in bit; B19:in bit;
        A20:in bit; B20:in bit; A21:in bit; B21:in bit;
        A22:in bit; B22:in bit; A23:in bit; B23:in bit;
        A24:in bit; B24:in bit; A25:in bit; B25:in bit;
        A26:in bit; B26:in bit; A27:in bit; B27:in bit;
        A28:in bit; B28:in bit; A29:in bit; B29:in bit;
        A30:in bit; B30:in bit; A31:in bit; B31:in bit;
        Y0:out bit; Y1:out bit; Y2:out bit; Y3:out bit;
        Y4:out bit; Y5:out bit; Y6:out bit; Y7:out bit;
        Y8:out bit; Y9:out bit; Y10:out bit; Y11:out bit;
        Y12:out bit; Y13:out bit; Y14:out bit; Y15:out bit;
        Y16:out bit; Y17:out bit; Y18:out bit; Y19:out bit;
        Y20:out bit; Y21:out bit; Y22:out bit; Y23:out bit;
        Y24:out bit; Y25:out bit; Y26:out bit; Y27:out bit;
        Y28:out bit; Y29:out bit; Y30:out bit; Y31:out bit;
        Y32:out bit; Y33:out bit; Y34:out bit; Y35:out bit;
        Y36:out bit; Y37:out bit; Y38:out bit; Y39:out bit;
        Y40:out bit; Y41:out bit; Y42:out bit; Y43:out bit;
        Y44:out bit; Y45:out bit; Y46:out bit; Y47:out bit;
        Y48:out bit; Y49:out bit; Y50:out bit; Y51:out bit;
        Y52:out bit; Y53:out bit; Y54:out bit; Y55:out bit;
        Y56:out bit; Y57:out bit; Y58:out bit; Y59:out bit;
        Y60:out bit; Y61:out bit; Y62:out bit; Y63:out bit;
        Y64:out bit; Y65:out bit; Y66:out bit; Y67:out bit;
        Y68:out bit; Y69:out bit; Y70:out bit; Y71:out bit;
        Y72:out bit; Y73:out bit; Y74:out bit; Y75:out bit;
        Y76:out bit; Y77:out bit; Y78:out bit; Y79:out bit;
        Y80:out bit; Y81:out bit; Y82:out bit; Y83:out bit;
        Y84:out bit; Y85:out bit; Y86:out bit; Y87:out bit;
        Y88:out bit; Y89:out bit; Y90:out bit; Y91:out bit;
        Y92:out bit; Y93:out bit; Y94:out bit; Y95:out bit;
        Y96:out bit; Y97:out bit; Y98:out bit; Y99:out bit;
        Y100:out bit; Y101:out bit; Y102:out bit; Y103:out bit;
        Y104:out bit; Y105:out bit; Y106:out bit; Y107:out bit;
        Y108:out bit; Y109:out bit; Y110:out bit; Y111:out bit;
        Y112:out bit; Y113:out bit; Y114:out bit; Y115:out bit;
        Y116:out bit; Y117:out bit; Y118:out bit; Y119:out bit;
        Y120:out bit; Y121:out bit; Y122:out bit; Y123:out bit;
        Y124:out bit; Y125:out bit; Y126:out bit; Y127:out bit
    );
end WIDE_AND128;

architecture RTL of WIDE_AND128 is
begin
    process(A0,B0)   begin Y0   <= A0   and B0;   end process;
    process(A1,B1)   begin Y1   <= A1   and B1;   end process;
    process(A2,B2)   begin Y2   <= A2   and B2;   end process;
    process(A3,B3)   begin Y3   <= A3   and B3;   end process;
    process(A4,B4)   begin Y4   <= A4   and B4;   end process;
    process(A5,B5)   begin Y5   <= A5   and B5;   end process;
    process(A6,B6)   begin Y6   <= A6   and B6;   end process;
    process(A7,B7)   begin Y7   <= A7   and B7;   end process;
    process(A8,B8)   begin Y8   <= A8   and B8;   end process;
    process(A9,B9)   begin Y9   <= A9   and B9;   end process;
    process(A10,B10) begin Y10  <= A10  and B10;  end process;
    process(A11,B11) begin Y11  <= A11  and B11;  end process;
    process(A12,B12) begin Y12  <= A12  and B12;  end process;
    process(A13,B13) begin Y13  <= A13  and B13;  end process;
    process(A14,B14) begin Y14  <= A14  and B14;  end process;
    process(A15,B15) begin Y15  <= A15  and B15;  end process;
    process(A16,B16) begin Y16  <= A16  and B16;  end process;
    process(A17,B17) begin Y17  <= A17  and B17;  end process;
    process(A18,B18) begin Y18  <= A18  and B18;  end process;
    process(A19,B19) begin Y19  <= A19  and B19;  end process;
    process(A20,B20) begin Y20  <= A20  and B20;  end process;
    process(A21,B21) begin Y21  <= A21  and B21;  end process;
    process(A22,B22) begin Y22  <= A22  and B22;  end process;
    process(A23,B23) begin Y23  <= A23  and B23;  end process;
    process(A24,B24) begin Y24  <= A24  and B24;  end process;
    process(A25,B25) begin Y25  <= A25  and B25;  end process;
    process(A26,B26) begin Y26  <= A26  and B26;  end process;
    process(A27,B27) begin Y27  <= A27  and B27;  end process;
    process(A28,B28) begin Y28  <= A28  and B28;  end process;
    process(A29,B29) begin Y29  <= A29  and B29;  end process;
    process(A30,B30) begin Y30  <= A30  and B30;  end process;
    process(A31,B31) begin Y31  <= A31  and B31;  end process;
    process(A0,B0)   begin Y32  <= A0   and B0;   end process;
    process(A1,B1)   begin Y33  <= A1   and B1;   end process;
    process(A2,B2)   begin Y34  <= A2   and B2;   end process;
    process(A3,B3)   begin Y35  <= A3   and B3;   end process;
    process(A4,B4)   begin Y36  <= A4   and B4;   end process;
    process(A5,B5)   begin Y37  <= A5   and B5;   end process;
    process(A6,B6)   begin Y38  <= A6   and B6;   end process;
    process(A7,B7)   begin Y39  <= A7   and B7;   end process;
    process(A8,B8)   begin Y40  <= A8   and B8;   end process;
    process(A9,B9)   begin Y41  <= A9   and B9;   end process;
    process(A10,B10) begin Y42  <= A10  and B10;  end process;
    process(A11,B11) begin Y43  <= A11  and B11;  end process;
    process(A12,B12) begin Y44  <= A12  and B12;  end process;
    process(A13,B13) begin Y45  <= A13  and B13;  end process;
    process(A14,B14) begin Y46  <= A14  and B14;  end process;
    process(A15,B15) begin Y47  <= A15  and B15;  end process;
    process(A16,B16) begin Y48  <= A16  and B16;  end process;
    process(A17,B17) begin Y49  <= A17  and B17;  end process;
    process(A18,B18) begin Y50  <= A18  and B18;  end process;
    process(A19,B19) begin Y51  <= A19  and B19;  end process;
    process(A20,B20) begin Y52  <= A20  and B20;  end process;
    process(A21,B21) begin Y53  <= A21  and B21;  end process;
    process(A22,B22) begin Y54  <= A22  and B22;  end process;
    process(A23,B23) begin Y55  <= A23  and B23;  end process;
    process(A24,B24) begin Y56  <= A24  and B24;  end process;
    process(A25,B25) begin Y57  <= A25  and B25;  end process;
    process(A26,B26) begin Y58  <= A26  and B26;  end process;
    process(A27,B27) begin Y59  <= A27  and B27;  end process;
    process(A28,B28) begin Y60  <= A28  and B28;  end process;
    process(A29,B29) begin Y61  <= A29  and B29;  end process;
    process(A30,B30) begin Y62  <= A30  and B30;  end process;
    process(A31,B31) begin Y63  <= A31  and B31;  end process;
    process(A0,B0)   begin Y64  <= A0   and B0;   end process;
    process(A1,B1)   begin Y65  <= A1   and B1;   end process;
    process(A2,B2)   begin Y66  <= A2   and B2;   end process;
    process(A3,B3)   begin Y67  <= A3   and B3;   end process;
    process(A4,B4)   begin Y68  <= A4   and B4;   end process;
    process(A5,B5)   begin Y69  <= A5   and B5;   end process;
    process(A6,B6)   begin Y70  <= A6   and B6;   end process;
    process(A7,B7)   begin Y71  <= A7   and B7;   end process;
    process(A8,B8)   begin Y72  <= A8   and B8;   end process;
    process(A9,B9)   begin Y73  <= A9   and B9;   end process;
    process(A10,B10) begin Y74  <= A10  and B10;  end process;
    process(A11,B11) begin Y75  <= A11  and B11;  end process;
    process(A12,B12) begin Y76  <= A12  and B12;  end process;
    process(A13,B13) begin Y77  <= A13  and B13;  end process;
    process(A14,B14) begin Y78  <= A14  and B14;  end process;
    process(A15,B15) begin Y79  <= A15  and B15;  end process;
    process(A16,B16) begin Y80  <= A16  and B16;  end process;
    process(A17,B17) begin Y81  <= A17  and B17;  end process;
    process(A18,B18) begin Y82  <= A18  and B18;  end process;
    process(A19,B19) begin Y83  <= A19  and B19;  end process;
    process(A20,B20) begin Y84  <= A20  and B20;  end process;
    process(A21,B21) begin Y85  <= A21  and B21;  end process;
    process(A22,B22) begin Y86  <= A22  and B22;  end process;
    process(A23,B23) begin Y87  <= A23  and B23;  end process;
    process(A24,B24) begin Y88  <= A24  and B24;  end process;
    process(A25,B25) begin Y89  <= A25  and B25;  end process;
    process(A26,B26) begin Y90  <= A26  and B26;  end process;
    process(A27,B27) begin Y91  <= A27  and B27;  end process;
    process(A28,B28) begin Y92  <= A28  and B28;  end process;
    process(A29,B29) begin Y93  <= A29  and B29;  end process;
    process(A30,B30) begin Y94  <= A30  and B30;  end process;
    process(A31,B31) begin Y95  <= A31  and B31;  end process;
    process(A0,B0)   begin Y96  <= A0   and B0;   end process;
    process(A1,B1)   begin Y97  <= A1   and B1;   end process;
    process(A2,B2)   begin Y98  <= A2   and B2;   end process;
    process(A3,B3)   begin Y99  <= A3   and B3;   end process;
    process(A4,B4)   begin Y100 <= A4   and B4;   end process;
    process(A5,B5)   begin Y101 <= A5   and B5;   end process;
    process(A6,B6)   begin Y102 <= A6   and B6;   end process;
    process(A7,B7)   begin Y103 <= A7   and B7;   end process;
    process(A8,B8)   begin Y104 <= A8   and B8;   end process;
    process(A9,B9)   begin Y105 <= A9   and B9;   end process;
    process(A10,B10) begin Y106 <= A10  and B10;  end process;
    process(A11,B11) begin Y107 <= A11  and B11;  end process;
    process(A12,B12) begin Y108 <= A12  and B12;  end process;
    process(A13,B13) begin Y109 <= A13  and B13;  end process;
    process(A14,B14) begin Y110 <= A14  and B14;  end process;
    process(A15,B15) begin Y111 <= A15  and B15;  end process;
    process(A16,B16) begin Y112 <= A16  and B16;  end process;
    process(A17,B17) begin Y113 <= A17  and B17;  end process;
    process(A18,B18) begin Y114 <= A18  and B18;  end process;
    process(A19,B19) begin Y115 <= A19  and B19;  end process;
    process(A20,B20) begin Y116 <= A20  and B20;  end process;
    process(A21,B21) begin Y117 <= A21  and B21;  end process;
    process(A22,B22) begin Y118 <= A22  and B22;  end process;
    process(A23,B23) begin Y119 <= A23  and B23;  end process;
    process(A24,B24) begin Y120 <= A24  and B24;  end process;
    process(A25,B25) begin Y121 <= A25  and B25;  end process;
    process(A26,B26) begin Y122 <= A26  and B26;  end process;
    process(A27,B27) begin Y123 <= A27  and B27;  end process;
    process(A28,B28) begin Y124 <= A28  and B28;  end process;
    process(A29,B29) begin Y125 <= A29  and B29;  end process;
    process(A30,B30) begin Y126 <= A30  and B30;  end process;
    process(A31,B31) begin Y127 <= A31  and B31;  end process;
end RTL;
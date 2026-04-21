-- CHIRAG 22-04-26 :: Hamming encoder 32-bit
-- 32 independent parity check processes
-- each process computes XOR of 3 data bits
-- all 32 processes completely independent ... 0 edges in dependency graph
-- best case for parallel speedup after wide_and128
--
-- real world use ....
-- ECC for 32-bit data bus ... standard in DDR4/DDR5 memory
-- each parity bit protects different subset of data bits

entity HAMMING32 is
    port(
        D0:in bit; D1:in bit; D2:in bit; D3:in bit;
        D4:in bit; D5:in bit; D6:in bit; D7:in bit;
        D8:in bit; D9:in bit; D10:in bit; D11:in bit;
        D12:in bit; D13:in bit; D14:in bit; D15:in bit;
        D16:in bit; D17:in bit; D18:in bit; D19:in bit;
        D20:in bit; D21:in bit; D22:in bit; D23:in bit;
        D24:in bit; D25:in bit; D26:in bit; D27:in bit;
        D28:in bit; D29:in bit; D30:in bit; D31:in bit;
        P0:out bit; P1:out bit; P2:out bit; P3:out bit;
        P4:out bit; P5:out bit; P6:out bit; P7:out bit;
        P8:out bit; P9:out bit; P10:out bit; P11:out bit;
        P12:out bit; P13:out bit; P14:out bit; P15:out bit;
        P16:out bit; P17:out bit; P18:out bit; P19:out bit;
        P20:out bit; P21:out bit; P22:out bit; P23:out bit;
        P24:out bit; P25:out bit; P26:out bit; P27:out bit;
        P28:out bit; P29:out bit; P30:out bit; P31:out bit
    );
end HAMMING32;

architecture RTL of HAMMING32 is
begin
    process(D0,D1,D2)   begin P0  <= D0  xor D1  xor D2;  end process;
    process(D1,D2,D3)   begin P1  <= D1  xor D2  xor D3;  end process;
    process(D2,D3,D4)   begin P2  <= D2  xor D3  xor D4;  end process;
    process(D3,D4,D5)   begin P3  <= D3  xor D4  xor D5;  end process;
    process(D4,D5,D6)   begin P4  <= D4  xor D5  xor D6;  end process;
    process(D5,D6,D7)   begin P5  <= D5  xor D6  xor D7;  end process;
    process(D6,D7,D8)   begin P6  <= D6  xor D7  xor D8;  end process;
    process(D7,D8,D9)   begin P7  <= D7  xor D8  xor D9;  end process;
    process(D8,D9,D10)  begin P8  <= D8  xor D9  xor D10; end process;
    process(D9,D10,D11) begin P9  <= D9  xor D10 xor D11; end process;
    process(D10,D11,D12) begin P10 <= D10 xor D11 xor D12; end process;
    process(D11,D12,D13) begin P11 <= D11 xor D12 xor D13; end process;
    process(D12,D13,D14) begin P12 <= D12 xor D13 xor D14; end process;
    process(D13,D14,D15) begin P13 <= D13 xor D14 xor D15; end process;
    process(D14,D15,D16) begin P14 <= D14 xor D15 xor D16; end process;
    process(D15,D16,D17) begin P15 <= D15 xor D16 xor D17; end process;
    process(D16,D17,D18) begin P16 <= D16 xor D17 xor D18; end process;
    process(D17,D18,D19) begin P17 <= D17 xor D18 xor D19; end process;
    process(D18,D19,D20) begin P18 <= D18 xor D19 xor D20; end process;
    process(D19,D20,D21) begin P19 <= D19 xor D20 xor D21; end process;
    process(D20,D21,D22) begin P20 <= D20 xor D21 xor D22; end process;
    process(D21,D22,D23) begin P21 <= D21 xor D22 xor D23; end process;
    process(D22,D23,D24) begin P22 <= D22 xor D23 xor D24; end process;
    process(D23,D24,D25) begin P23 <= D23 xor D24 xor D25; end process;
    process(D24,D25,D26) begin P24 <= D24 xor D25 xor D26; end process;
    process(D25,D26,D27) begin P25 <= D25 xor D26 xor D27; end process;
    process(D26,D27,D28) begin P26 <= D26 xor D27 xor D28; end process;
    process(D27,D28,D29) begin P27 <= D27 xor D28 xor D29; end process;
    process(D28,D29,D30) begin P28 <= D28 xor D29 xor D30; end process;
    process(D29,D30,D31) begin P29 <= D29 xor D30 xor D31; end process;
    process(D0,D15,D31)  begin P30 <= D0  xor D15 xor D31; end process;
    process(D1,D16,D31)  begin P31 <= D1  xor D16 xor D31; end process;
end RTL;
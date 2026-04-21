-- CHIRAG 22-04-26 :: 32-bit Carry Lookahead Adder
-- same structure as CLA4 scaled to 32 bits
-- G and P ... 64 independent processes ... color 0 ... maximum parallel batch
-- carry chain C1-C31 ... sequential ... 31 colors
-- sum S0-S31 ... 32 processes ... final parallel batch
--
-- why 32 bit ....
-- matches real CPU word size
-- 64 independent G/P processes gives strong parallel speedup story
-- carry chain is the serial bottleneck ... same Amdahl story as pipeline8

entity CLA32 is
    port(
        A0:in bit; A1:in bit; A2:in bit; A3:in bit;
        A4:in bit; A5:in bit; A6:in bit; A7:in bit;
        A8:in bit; A9:in bit; A10:in bit; A11:in bit;
        A12:in bit; A13:in bit; A14:in bit; A15:in bit;
        A16:in bit; A17:in bit; A18:in bit; A19:in bit;
        A20:in bit; A21:in bit; A22:in bit; A23:in bit;
        A24:in bit; A25:in bit; A26:in bit; A27:in bit;
        A28:in bit; A29:in bit; A30:in bit; A31:in bit;
        B0:in bit; B1:in bit; B2:in bit; B3:in bit;
        B4:in bit; B5:in bit; B6:in bit; B7:in bit;
        B8:in bit; B9:in bit; B10:in bit; B11:in bit;
        B12:in bit; B13:in bit; B14:in bit; B15:in bit;
        B16:in bit; B17:in bit; B18:in bit; B19:in bit;
        B20:in bit; B21:in bit; B22:in bit; B23:in bit;
        B24:in bit; B25:in bit; B26:in bit; B27:in bit;
        B28:in bit; B29:in bit; B30:in bit; B31:in bit;
        Cin:in bit;
        S0:out bit; S1:out bit; S2:out bit; S3:out bit;
        S4:out bit; S5:out bit; S6:out bit; S7:out bit;
        S8:out bit; S9:out bit; S10:out bit; S11:out bit;
        S12:out bit; S13:out bit; S14:out bit; S15:out bit;
        S16:out bit; S17:out bit; S18:out bit; S19:out bit;
        S20:out bit; S21:out bit; S22:out bit; S23:out bit;
        S24:out bit; S25:out bit; S26:out bit; S27:out bit;
        S28:out bit; S29:out bit; S30:out bit; S31:out bit;
        Cout:out bit
    );
end CLA32;

architecture RTL of CLA32 is
    signal G0:bit; signal G1:bit; signal G2:bit; signal G3:bit;
    signal G4:bit; signal G5:bit; signal G6:bit; signal G7:bit;
    signal G8:bit; signal G9:bit; signal G10:bit; signal G11:bit;
    signal G12:bit; signal G13:bit; signal G14:bit; signal G15:bit;
    signal G16:bit; signal G17:bit; signal G18:bit; signal G19:bit;
    signal G20:bit; signal G21:bit; signal G22:bit; signal G23:bit;
    signal G24:bit; signal G25:bit; signal G26:bit; signal G27:bit;
    signal G28:bit; signal G29:bit; signal G30:bit; signal G31:bit;
    signal P0:bit; signal P1:bit; signal P2:bit; signal P3:bit;
    signal P4:bit; signal P5:bit; signal P6:bit; signal P7:bit;
    signal P8:bit; signal P9:bit; signal P10:bit; signal P11:bit;
    signal P12:bit; signal P13:bit; signal P14:bit; signal P15:bit;
    signal P16:bit; signal P17:bit; signal P18:bit; signal P19:bit;
    signal P20:bit; signal P21:bit; signal P22:bit; signal P23:bit;
    signal P24:bit; signal P25:bit; signal P26:bit; signal P27:bit;
    signal P28:bit; signal P29:bit; signal P30:bit; signal P31:bit;
    signal C1:bit; signal C2:bit; signal C3:bit; signal C4:bit;
    signal C5:bit; signal C6:bit; signal C7:bit; signal C8:bit;
    signal C9:bit; signal C10:bit; signal C11:bit; signal C12:bit;
    signal C13:bit; signal C14:bit; signal C15:bit; signal C16:bit;
    signal C17:bit; signal C18:bit; signal C19:bit; signal C20:bit;
    signal C21:bit; signal C22:bit; signal C23:bit; signal C24:bit;
    signal C25:bit; signal C26:bit; signal C27:bit; signal C28:bit;
    signal C29:bit; signal C30:bit; signal C31:bit;
begin
    process(A0,B0)   begin G0  <= A0  and B0;  end process;
    process(A1,B1)   begin G1  <= A1  and B1;  end process;
    process(A2,B2)   begin G2  <= A2  and B2;  end process;
    process(A3,B3)   begin G3  <= A3  and B3;  end process;
    process(A4,B4)   begin G4  <= A4  and B4;  end process;
    process(A5,B5)   begin G5  <= A5  and B5;  end process;
    process(A6,B6)   begin G6  <= A6  and B6;  end process;
    process(A7,B7)   begin G7  <= A7  and B7;  end process;
    process(A8,B8)   begin G8  <= A8  and B8;  end process;
    process(A9,B9)   begin G9  <= A9  and B9;  end process;
    process(A10,B10) begin G10 <= A10 and B10; end process;
    process(A11,B11) begin G11 <= A11 and B11; end process;
    process(A12,B12) begin G12 <= A12 and B12; end process;
    process(A13,B13) begin G13 <= A13 and B13; end process;
    process(A14,B14) begin G14 <= A14 and B14; end process;
    process(A15,B15) begin G15 <= A15 and B15; end process;
    process(A16,B16) begin G16 <= A16 and B16; end process;
    process(A17,B17) begin G17 <= A17 and B17; end process;
    process(A18,B18) begin G18 <= A18 and B18; end process;
    process(A19,B19) begin G19 <= A19 and B19; end process;
    process(A20,B20) begin G20 <= A20 and B20; end process;
    process(A21,B21) begin G21 <= A21 and B21; end process;
    process(A22,B22) begin G22 <= A22 and B22; end process;
    process(A23,B23) begin G23 <= A23 and B23; end process;
    process(A24,B24) begin G24 <= A24 and B24; end process;
    process(A25,B25) begin G25 <= A25 and B25; end process;
    process(A26,B26) begin G26 <= A26 and B26; end process;
    process(A27,B27) begin G27 <= A27 and B27; end process;
    process(A28,B28) begin G28 <= A28 and B28; end process;
    process(A29,B29) begin G29 <= A29 and B29; end process;
    process(A30,B30) begin G30 <= A30 and B30; end process;
    process(A31,B31) begin G31 <= A31 and B31; end process;

    process(A0,B0)   begin P0  <= A0  xor B0;  end process;
    process(A1,B1)   begin P1  <= A1  xor B1;  end process;
    process(A2,B2)   begin P2  <= A2  xor B2;  end process;
    process(A3,B3)   begin P3  <= A3  xor B3;  end process;
    process(A4,B4)   begin P4  <= A4  xor B4;  end process;
    process(A5,B5)   begin P5  <= A5  xor B5;  end process;
    process(A6,B6)   begin P6  <= A6  xor B6;  end process;
    process(A7,B7)   begin P7  <= A7  xor B7;  end process;
    process(A8,B8)   begin P8  <= A8  xor B8;  end process;
    process(A9,B9)   begin P9  <= A9  xor B9;  end process;
    process(A10,B10) begin P10 <= A10 xor B10; end process;
    process(A11,B11) begin P11 <= A11 xor B11; end process;
    process(A12,B12) begin P12 <= A12 xor B12; end process;
    process(A13,B13) begin P13 <= A13 xor B13; end process;
    process(A14,B14) begin P14 <= A14 xor B14; end process;
    process(A15,B15) begin P15 <= A15 xor B15; end process;
    process(A16,B16) begin P16 <= A16 xor B16; end process;
    process(A17,B17) begin P17 <= A17 xor B17; end process;
    process(A18,B18) begin P18 <= A18 xor B18; end process;
    process(A19,B19) begin P19 <= A19 xor B19; end process;
    process(A20,B20) begin P20 <= A20 xor B20; end process;
    process(A21,B21) begin P21 <= A21 xor B21; end process;
    process(A22,B22) begin P22 <= A22 xor B22; end process;
    process(A23,B23) begin P23 <= A23 xor B23; end process;
    process(A24,B24) begin P24 <= A24 xor B24; end process;
    process(A25,B25) begin P25 <= A25 xor B25; end process;
    process(A26,B26) begin P26 <= A26 xor B26; end process;
    process(A27,B27) begin P27 <= A27 xor B27; end process;
    process(A28,B28) begin P28 <= A28 xor B28; end process;
    process(A29,B29) begin P29 <= A29 xor B29; end process;
    process(A30,B30) begin P30 <= A30 xor B30; end process;
    process(A31,B31) begin P31 <= A31 xor B31; end process;

    process(G0,P0,Cin)   begin C1  <= G0  or (P0  and Cin); end process;
    process(G1,P1,C1)    begin C2  <= G1  or (P1  and C1);  end process;
    process(G2,P2,C2)    begin C3  <= G2  or (P2  and C2);  end process;
    process(G3,P3,C3)    begin C4  <= G3  or (P3  and C3);  end process;
    process(G4,P4,C4)    begin C5  <= G4  or (P4  and C4);  end process;
    process(G5,P5,C5)    begin C6  <= G5  or (P5  and C5);  end process;
    process(G6,P6,C6)    begin C7  <= G6  or (P6  and C6);  end process;
    process(G7,P7,C7)    begin C8  <= G7  or (P7  and C7);  end process;
    process(G8,P8,C8)    begin C9  <= G8  or (P8  and C8);  end process;
    process(G9,P9,C9)    begin C10 <= G9  or (P9  and C9);  end process;
    process(G10,P10,C10) begin C11 <= G10 or (P10 and C10); end process;
    process(G11,P11,C11) begin C12 <= G11 or (P11 and C11); end process;
    process(G12,P12,C12) begin C13 <= G12 or (P12 and C12); end process;
    process(G13,P13,C13) begin C14 <= G13 or (P13 and C13); end process;
    process(G14,P14,C14) begin C15 <= G14 or (P14 and C14); end process;
    process(G15,P15,C15) begin C16 <= G15 or (P15 and C15); end process;
    process(G16,P16,C16) begin C17 <= G16 or (P16 and C16); end process;
    process(G17,P17,C17) begin C18 <= G17 or (P17 and C17); end process;
    process(G18,P18,C18) begin C19 <= G18 or (P18 and C18); end process;
    process(G19,P19,C19) begin C20 <= G19 or (P19 and C19); end process;
    process(G20,P20,C20) begin C21 <= G20 or (P20 and C20); end process;
    process(G21,P21,C21) begin C22 <= G21 or (P21 and C21); end process;
    process(G22,P22,C22) begin C23 <= G22 or (P22 and C22); end process;
    process(G23,P23,C23) begin C24 <= G23 or (P23 and C23); end process;
    process(G24,P24,C24) begin C25 <= G24 or (P24 and C24); end process;
    process(G25,P25,C25) begin C26 <= G25 or (P25 and C25); end process;
    process(G26,P26,C26) begin C27 <= G26 or (P26 and C26); end process;
    process(G27,P27,C27) begin C28 <= G27 or (P27 and C27); end process;
    process(G28,P28,C28) begin C29 <= G28 or (P28 and C28); end process;
    process(G29,P29,C29) begin C30 <= G29 or (P29 and C29); end process;
    process(G30,P30,C30) begin C31 <= G30 or (P30 and C30); end process;

    process(P0,Cin)  begin S0  <= P0  xor Cin; end process;
    process(P1,C1)   begin S1  <= P1  xor C1;  end process;
    process(P2,C2)   begin S2  <= P2  xor C2;  end process;
    process(P3,C3)   begin S3  <= P3  xor C3;  end process;
    process(P4,C4)   begin S4  <= P4  xor C4;  end process;
    process(P5,C5)   begin S5  <= P5  xor C5;  end process;
    process(P6,C6)   begin S6  <= P6  xor C6;  end process;
    process(P7,C7)   begin S7  <= P7  xor C7;  end process;
    process(P8,C8)   begin S8  <= P8  xor C8;  end process;
    process(P9,C9)   begin S9  <= P9  xor C9;  end process;
    process(P10,C10) begin S10 <= P10 xor C10; end process;
    process(P11,C11) begin S11 <= P11 xor C11; end process;
    process(P12,C12) begin S12 <= P12 xor C12; end process;
    process(P13,C13) begin S13 <= P13 xor C13; end process;
    process(P14,C14) begin S14 <= P14 xor C14; end process;
    process(P15,C15) begin S15 <= P15 xor C15; end process;
    process(P16,C16) begin S16 <= P16 xor C16; end process;
    process(P17,C17) begin S17 <= P17 xor C17; end process;
    process(P18,C18) begin S18 <= P18 xor C18; end process;
    process(P19,C19) begin S19 <= P19 xor C19; end process;
    process(P20,C20) begin S20 <= P20 xor C20; end process;
    process(P21,C21) begin S21 <= P21 xor C21; end process;
    process(P22,C22) begin S22 <= P22 xor C22; end process;
    process(P23,C23) begin S23 <= P23 xor C23; end process;
    process(P24,C24) begin S24 <= P24 xor C24; end process;
    process(P25,C25) begin S25 <= P25 xor C25; end process;
    process(P26,C26) begin S26 <= P26 xor C26; end process;
    process(P27,C27) begin S27 <= P27 xor C27; end process;
    process(P28,C28) begin S28 <= P28 xor C28; end process;
    process(P29,C29) begin S29 <= P29 xor C29; end process;
    process(P30,C30) begin S30 <= P30 xor C30; end process;
    process(P31,C31) begin S31 <= P31 xor C31; end process;

    process(G31,P31,C31) begin Cout <= G31 or (P31 and C31); end process;
end RTL;
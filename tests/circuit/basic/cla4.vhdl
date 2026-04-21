-- CHIRAG 22-04-26 :: 4-bit Carry Lookahead Adder
-- inputs A0-A3, B0-B3, Cin
-- outputs S0-S3 (sum), Cout (carry out)
--
-- how CLA works ....
-- generate G = A and B ... this bit GENERATES a carry regardless of input carry
-- propagate P = A xor B ... this bit PROPAGATES carry from input
-- sum S = P xor C ... final sum bit
-- carry lookahead ... C1 = G0 or (P0 and C0) ... computed in parallel not ripple
--
-- parallelism story ....
-- G and P processes ... 8 total ... all independent ... color 0 ... run in parallel
-- carry chain C1 C2 C3 ... sequential ... colors 1 2 3
-- sum processes ... 4 total ... depend on carry ... final color batch
--
-- real world use ....
-- every modern CPU ALU uses CLA or variants
-- faster addition = faster everything ... CPUs, GPUs, DSPs

entity CLA4 is
    port(
        A0  : in bit;
        A1  : in bit;
        A2  : in bit;
        A3  : in bit;
        B0  : in bit;
        B1  : in bit;
        B2  : in bit;
        B3  : in bit;
        Cin : in bit;
        S0  : out bit;
        S1  : out bit;
        S2  : out bit;
        S3  : out bit;
        Cout : out bit
    );
end CLA4;

architecture RTL of CLA4 is
    signal G0 : bit;
    signal G1 : bit;
    signal G2 : bit;
    signal G3 : bit;
    signal P0 : bit;
    signal P1 : bit;
    signal P2 : bit;
    signal P3 : bit;
    signal C1 : bit;
    signal C2 : bit;
    signal C3 : bit;
begin
    process(A0, B0) begin G0 <= A0 and B0; end process;
    process(A1, B1) begin G1 <= A1 and B1; end process;
    process(A2, B2) begin G2 <= A2 and B2; end process;
    process(A3, B3) begin G3 <= A3 and B3; end process;

    process(A0, B0) begin P0 <= A0 xor B0; end process;
    process(A1, B1) begin P1 <= A1 xor B1; end process;
    process(A2, B2) begin P2 <= A2 xor B2; end process;
    process(A3, B3) begin P3 <= A3 xor B3; end process;

    process(G0, P0, Cin) begin C1 <= G0 or (P0 and Cin); end process;
    process(G1, P1, C1)  begin C2 <= G1 or (P1 and C1);  end process;
    process(G2, P2, C2)  begin C3 <= G2 or (P2 and C2);  end process;

    process(P0, Cin) begin S0 <= P0 xor Cin; end process;
    process(P1, C1)  begin S1 <= P1 xor C1;  end process;
    process(P2, C2)  begin S2 <= P2 xor C2;  end process;
    process(P3, C3)  begin S3 <= P3 xor C3;  end process;

    process(G3, P3, C3) begin Cout <= G3 or (P3 and C3); end process;
end RTL;
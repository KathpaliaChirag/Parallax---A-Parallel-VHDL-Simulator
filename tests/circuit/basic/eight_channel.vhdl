-- CHIRAG 13-04-26 :: eight_channel.vhdl
-- 8 independent OR gates ... maximum parallelism demo circuit
-- all 8 processes are completely independent ... zero shared signals
-- dependency graph: 8 nodes, 0 edges, 8 color classes
-- all 8 run in parallel simultaneously ... best case speedup demo
entity EIGHT_CHANNEL is
    port(
        A0 : in bit; B0 : in bit;
        A1 : in bit; B1 : in bit;
        A2 : in bit; B2 : in bit;
        A3 : in bit; B3 : in bit;
        A4 : in bit; B4 : in bit;
        A5 : in bit; B5 : in bit;
        A6 : in bit; B6 : in bit;
        A7 : in bit; B7 : in bit;
        Y0 : out bit;
        Y1 : out bit;
        Y2 : out bit;
        Y3 : out bit;
        Y4 : out bit;
        Y5 : out bit;
        Y6 : out bit;
        Y7 : out bit
    );
end EIGHT_CHANNEL;

architecture RTL of EIGHT_CHANNEL is
begin
    process(A0, B0)
    begin
        Y0 <= A0 or B0;
    end process;

    process(A1, B1)
    begin
        Y1 <= A1 or B1;
    end process;

    process(A2, B2)
    begin
        Y2 <= A2 or B2;
    end process;

    process(A3, B3)
    begin
        Y3 <= A3 or B3;
    end process;

    process(A4, B4)
    begin
        Y4 <= A4 or B4;
    end process;

    process(A5, B5)
    begin
        Y5 <= A5 or B5;
    end process;

    process(A6, B6)
    begin
        Y6 <= A6 or B6;
    end process;

    process(A7, B7)
    begin
        Y7 <= A7 or B7;
    end process;

end RTL;
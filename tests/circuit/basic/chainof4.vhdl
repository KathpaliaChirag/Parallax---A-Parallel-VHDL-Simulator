-- CHIRAG 13-04-26 :: chain4.vhdl
-- 4 stage pipeline chain ... output of each stage feeds next stage
-- process 0 writes B ... process 1 reads B ... so they conflict
-- process 1 writes C ... process 2 reads C ... so they conflict
-- process 2 writes D ... process 3 reads D ... so they conflict
-- dependency graph: chain A-B-C-D
-- Welsh-Powell coloring gives 2 colors ... {proc0, proc2} and {proc1, proc3}
-- stages 0 and 2 can run in parallel ... stages 1 and 3 can run in parallel
-- idea of this was taken from GPT ...
entity CHAIN4 is
    port(
        A : in bit;
        B : out bit;
        C : out bit;
        D : out bit;
        E : out bit
    );
end CHAIN4;

architecture RTL of CHAIN4 is
begin
    -- stage 0 ... reads A writes B
    process(A)
    begin
        B <= A and A;
    end process;

    -- stage 1 ... reads B writes C
    -- conflicts with stage 0 since stage 0 writes B
    process(B)
    begin
        C <= B and B;
    end process;

    -- stage 2 ... reads C writes D
    -- conflicts with stage 1 since stage 1 writes C
    process(C)
    begin
        D <= C and C;
    end process;

    -- stage 3 ... reads D writes E
    -- conflicts with stage 2 since stage 2 writes D
    process(D)
    begin
        E <= D and D;
    end process;

end RTL;
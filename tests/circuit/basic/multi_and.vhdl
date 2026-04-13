-- CHIRAG 13-04-26 :: multi_and.vhdl
-- Okay so here starts test 2 plan is simple 
-- 4 independent AND gates in one architecture
-- each process watches different signals ... zero shared signals between processes
-- dependency graph should show 4 nodes, 0 edges, 4 color classes
-- best case for parallelism ... all 4 can run simultaneously

-- first we define all ports in an entity 
entity MULTI_AND is
    port(
        A0 : in bit;
        B0 : in bit;
        A1 : in bit;
        B1 : in bit;
        A2 : in bit;
        B2 : in bit;
        A3 : in bit;
        B3 : in bit;
        Y0 : out bit;
        Y1 : out bit;
        Y2 : out bit;
        Y3 : out bit
    );
end MULTI_AND;

architecture RTL of MULTI_AND is
begin
    -- process 0 ... watches A0 and B0 ... writes Y0
    -- completely independent from processes 1 2 3
    process(A0, B0)
    begin
        Y0 <= A0 and B0;
    end process;

    -- process 1 ... watches A1 and B1 ... writes Y1
    process(A1, B1)
    begin
        Y1 <= A1 and B1;
    end process;

    -- process 2 ... watches A2 and B2 ... writes Y2
    process(A2, B2)
    begin
        Y2 <= A2 and B2;
    end process;

    -- process 3 ... watches A3 and B3 ... writes Y3
    process(A3, B3)
    begin
        Y3 <= A3 and B3;
    end process;

end RTL;
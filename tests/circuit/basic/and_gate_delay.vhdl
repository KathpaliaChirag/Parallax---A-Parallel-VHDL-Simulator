-- CHIRAG 21-04-26 :: AND gate with propagation delay
-- real use ... every real gate has propagation delay ... not zero
-- this tests that our simulator correctly schedules future events
-- Y should change 5ns after inputs change ... not immediately

entity AND_GATE_DELAY is
    port(
        A : in bit;
        B : in bit;
        Y : out bit
    );
end AND_GATE_DELAY;

architecture RTL of AND_GATE_DELAY is
begin
    process(A, B)
    begin
        Y <= A and B after 5 ns;
    end process;
end RTL;
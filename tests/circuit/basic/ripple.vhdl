-- CHIRAG 18-04-26 :: ripple carry adder ... first circuit that needs signal declarations
-- why this circuit? ... CARRY0 is an internal wire ... not an input, not an output
-- it connects two full adder stages ... only exists inside the architecture
-- this is exactly what "signal X : bit;" is for ... internal wires between processes
--
-- circuit logic ...
-- stage 0 ... adds A0 + B0 ... produces SUM0 and carry out CARRY0
-- stage 1 ... adds A1 + B1 + CARRY0 ... produces SUM1 and COUT
-- so 1+1=2 means SUM0=0 CARRY0=1 ... then 1+0+1=2 means SUM1=0 COUT=1

entity RIPPLE is
    port(
        A0   : in bit;
        B0   : in bit;
        A1   : in bit;
        B1   : in bit;
        SUM0 : out bit;
        SUM1 : out bit;
        COUT : out bit
    );
end RIPPLE;

architecture RTL of RIPPLE is
signal CARRY0 : bit;
begin
    -- process 0 ... first full adder stage
    -- watches A0 B0 ... writes SUM0 and CARRY0
    -- CARRY0 is an internal signal ... process 1 will read it
    process(A0, B0)
    begin
        SUM0   <= A0 xor B0;
        CARRY0 <= A0 and B0;
    end process;

    -- process 1 ... second full adder stage
    -- watches A1 B1 CARRY0 ... CARRY0 is the internal wire from stage 0
    -- this is why process 1 has CARRY0 in its sensitivity list
    process(A1, B1, CARRY0)
    begin
        SUM1 <= A1 xor B1 xor CARRY0;
        COUT <= (A1 and B1) or (A1 and CARRY0) or (B1 and CARRY0);
    end process;

end RTL;
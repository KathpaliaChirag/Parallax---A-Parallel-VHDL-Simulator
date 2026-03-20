-- CHIRAG 20-03-26 :: OR gate VHDL test circuit
-- same structure as and_gate.vhdl.... only difference is the operator
-- Y <= A or B means: Y gets the value of A OR B

entity OR_GATE is
    port(
        A : in bit;
        B : in bit;
        Y : out bit
    );
end OR_GATE;

architecture RTL of OR_GATE is
begin
    process(A, B)
    begin
        Y <= A or B;
    end process;

end RTL;
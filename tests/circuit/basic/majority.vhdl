-- CHIRAG 21-04-26 :: majority voter using function
-- real use ... fault tolerant systems ... spacecraft ... nuclear controllers
-- takes 3 inputs ... outputs 1 if majority (2 or more) are 1
-- uses a function declaration to keep process clean
-- function majority(A, B, C) return (A and B) or (B and C) or (A and C)

entity MAJORITY is
    port(
        A : in bit;
        B : in bit;
        C : in bit;
        Y : out bit
    );
end MAJORITY;

architecture RTL of MAJORITY is

    function majority(A: bit; B: bit; C: bit) return bit is
    begin
        return (A and B) or (B and C) or (A and C);
    end function;

begin
    process(A, B, C)
    begin
        Y <= majority(A, B, C);
    end process;
end RTL;
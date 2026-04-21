-- CHIRAG 22-04-26 :: Hamming encoder 4-bit
-- encodes 4 data bits D0-D3 into 7-bit hamming code
-- P0 P1 P2 are parity bits ... D0 D1 D2 D3 are data bits
--
-- hamming(7,4) parity equations ....
-- P0 covers bits 1,3,5,7 ... D0 D1 D3
-- P1 covers bits 2,3,6,7 ... D0 D2 D3
-- P2 covers bits 4,5,6,7 ... D1 D2 D3
--
-- all three parity processes are completely independent ....
-- P0 doesnt read P1 or P2 ... P1 doesnt read P0 or P2 ... etc
-- dependency graph has 3 nodes and 0 edges ... perfect for parallelism
--
-- real world use ....
-- ECC RAM ... detects and corrects single bit errors
-- satellite comms ... error correction over noisy channels
-- storage systems ... RAID with error correction

entity HAMMING4 is
    port(
        D0  : in bit;
        D1  : in bit;
        D2  : in bit;
        D3  : in bit;
        P0  : out bit;
        P1  : out bit;
        P2  : out bit
    );
end HAMMING4;

architecture RTL of HAMMING4 is
begin
    process(D0, D1, D3)
    begin
        P0 <= D0 xor D1 xor D3;
    end process;

    process(D0, D2, D3)
    begin
        P1 <= D0 xor D2 xor D3;
    end process;

    process(D1, D2, D3)
    begin
        P2 <= D1 xor D2 xor D3;
    end process;
end RTL;
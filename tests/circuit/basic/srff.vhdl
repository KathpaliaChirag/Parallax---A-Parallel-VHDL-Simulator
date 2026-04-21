-- CHIRAG 22-04-26 :: SR flip flop
-- S = set .... R = reset .... CLK = clock .... Q = output
--
-- how it works ....
-- on rising clock edge ....
-- S=1 R=0 ... Q goes to 1 (set)
-- S=0 R=1 ... Q goes to 0 (reset)
-- S=0 R=0 ... Q holds
-- S=1 R=1 ... undefined ... we hold ... practical choice
--
-- real world use ....
-- basic latch in register files ... set/reset control logic
-- debounce circuits ... S on press R on release
-- interrupt flags ... set by hardware ... reset by software
-- CHIRAG 22-04-26 :: SR flip flop
-- avoids direct bit literal assignment ... uses conditional expression instead
-- S=1 sets Q ... R=1 resets Q ... both 0 holds ... both 1 holds (safe choice)

entity SRFF is
    port(
        S   : in bit;
        R   : in bit;
        CLK : in bit;
        Q   : out bit
    );
end SRFF;

architecture RTL of SRFF is
    signal Q_reg : bit;
begin
    process(CLK)
    begin
        if CLK = '1' then
            if S = '1' then
                Q_reg <= S;
            end if;
            if R = '1' then
                Q_reg <= not R;
            end if;
        end if;
    end process;

    process(Q_reg)
    begin
        Q <= Q_reg;
    end process;
end RTL;
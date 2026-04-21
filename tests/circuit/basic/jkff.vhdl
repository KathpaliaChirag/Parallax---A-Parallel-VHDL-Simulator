-- CHIRAG 22-04-26 :: JK flip flop
-- J = set input .... K = reset input .... CLK = clock .... Q = output
--
-- how it works ....
-- on rising clock edge ....
-- J=0 K=0 ... Q holds
-- J=1 K=0 ... Q goes to 1 (set)
-- J=0 K=1 ... Q goes to 0 (reset)
-- J=1 K=1 ... Q toggles (most powerful state)
--
-- real world use ....
-- most versatile flip flop ... can replace DFF TFF SRFF
-- used in shift registers ... counters ... state machines
-- JK eliminates the undefined state of SR ... J=K=1 is valid here
--
-- implementation ....
-- J=1 K=0 ... same as SRFF set ... Q_reg <= J
-- J=0 K=1 ... same as SRFF reset ... Q_reg <= not K
-- J=1 K=1 ... same as TFF toggle ... Q_reg <= not Q_reg

entity JKFF is
    port(
        J   : in bit;
        K   : in bit;
        CLK : in bit;
        Q   : out bit
    );
end JKFF;

architecture RTL of JKFF is
    signal Q_reg : bit;
begin
    process(CLK)
    begin
        if CLK = '1' then
            if J = '1' then
                if K = '1' then
                    Q_reg <= not Q_reg;
                end if;
            end if;
            if J = '1' then
                if K = '0' then
                    Q_reg <= J;
                end if;
            end if;
            if J = '0' then
                if K = '1' then
                    Q_reg <= not K;
                end if;
            end if;
        end if;
    end process;

    process(Q_reg)
    begin
        Q <= Q_reg;
    end process;
end RTL;
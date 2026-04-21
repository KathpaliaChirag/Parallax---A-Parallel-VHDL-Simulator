-- CHIRAG 22-04-26 :: T flip flop
-- T = toggle input .... CLK = clock .... Q = output
-- Q_reg is internal state register
--
-- how it works ....
-- on rising clock edge .... if T=0 ... Q holds .... if T=1 ... Q toggles
-- thats it .... simplest flip flop after DFF
--
-- real world use ....
-- frequency dividers ... connect Q back to T ... output toggles every clock
-- binary counters ... chain of TFFs ... each divides frequency by 2
-- debounce circuits ... toggle on button press
--
-- relation to DFF ....
-- TFF is a DFF with feedback ... if T=1 then D=NOT Q else D=Q
-- we implement nested if directly ... cleaner than feedback trick

entity TFF is
    port(
        T   : in bit;
        CLK : in bit;
        Q   : out bit
    );
end TFF;

architecture RTL of TFF is
    signal Q_reg : bit;
begin
    process(CLK)
    begin
        if CLK = '1' then
            if T = '1' then
                Q_reg <= not Q_reg;
            end if;
        end if;
    end process;

    process(Q_reg)
    begin
        Q <= Q_reg;
    end process;
end RTL;
-- CHIRAG : 18-04-26 :: D Flipflop in vhdl is here
-- I am still learning basics of vhdl syntax so it was assisted by AI.... i wrote some part and corrections ai helped
-- so its simple if say clk is 1 then Q captures D else nothing happens
-- simplerr than real dff and no rising edge detection .... it fires whenevr clk=1
-- currently problem that i see is that rising eddge detectiion needs prev_value concept and its not in our parser yet 
-- still neeeds to be done 

entity DFF is
    port (
        D : in bit;
        CLK : in bit;
        Q : out bit
    );
end DFF;

architecture RTL of DFF is 
begin 
    process(CLK, D)
    begin  
        if CLK = '1' then
            Q <= D;
        end if;
    end process;
end RTL;
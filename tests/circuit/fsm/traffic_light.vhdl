-- CHIRAG 21-04-26 :: traffic light FSM added to test if our simulator handles FSMs properly
-- real use ... finite state machine controls traffic light sequencing
-- used in every traffic controller, elevator, vending machine, protocol handler
-- this is the hello world of sequential digital design

-- states encoded as 1-bit pairs using two signal registers
-- STATE0 STATE1 ... 00=RED 01=GREEN 10=YELLOW
-- on each clock rising edge ... advance to next state
-- RED -> GREEN -> YELLOW -> RED ... forever
--
-- reason i fell its sort of a perfect tstt is cause of following :
-- uses signal declarations (STATE0, STATE1 internal wires)
-- uses if-else (clock edge detection + state transitions)
-- uses multiple assignments in one process
-- dependency graph has one process watching CLK ... one color ... sequential by nature

entity TRAFFIC_LIGHT is
    port(
        CLK   : in bit;
        RED   : out bit;
        GREEN : out bit;
        YELLOW: out bit
    );
end TRAFFIC_LIGHT;

architecture RTL of TRAFFIC_LIGHT is
signal STATE0 : bit;
signal STATE1 : bit;
begin
    -- state register process ... updates state on clock rising edge
    -- STATE0 STATE1 encoding ...
    -- 0 0 = RED
    -- 0 1 = GREEN  
    -- 1 0 = YELLOW
    process(CLK)
    begin
        if CLK = '1' then
            STATE0 <= STATE1;
            STATE1 <= not STATE0 and not STATE1;
        end if;
    end process;

    -- output decode process ... combinational ... watches state bits
    process(STATE0, STATE1)
    begin
        RED    <= not STATE0 and not STATE1;
        GREEN  <= not STATE0 and STATE1;
        YELLOW <= STATE0 and not STATE1;
    end process;

end RTL;
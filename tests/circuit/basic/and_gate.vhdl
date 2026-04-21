-- CHIRAG 18-03-26 : 23: 45 :: my first VHDL file.... writing it with help of AI to test the parser
-- for 1st file my contribution is only to write comments and understand it and its syntax
-- VHDL comments start with -- just like // in C
-- 
-- VHDL has two main parts for every circuit:
-- 1. ENTITY : defines the circuit's interface.... what pins go in and out
--             think of it like a function signature in C
-- 2. ARCHITECTURE : defines what the circuit actually does
--                   think of it like the function body in C
--
-- so entity says "i have inputs A,B and output Y"
-- architecture says "Y = A AND B"
-- they are separate on purpose.... same entity can have multiple architectures
-- like same interface different implementations

-- ENTITY DECLARATION
-- format: entity NAME is port(port_list); end NAME;
-- port_list: signal_name : direction type
-- direction is either 'in' (input) or 'out' (output)
-- type is 'bit' for single bit signals (0 or 1)
entity AND_GATE is
    port(
        A : in bit;    -- input signal A
        B : in bit;    -- input signal B
        Y : out bit    -- output signal Y.... no semicolon on last one!
    );
end AND_GATE;

-- ARCHITECTURE DECLARATION  
-- format: architecture NAME of ENTITY_NAME is begin ... end NAME;
-- NAME is usually RTL (Register Transfer Level) by convention
-- of AND_GATE connects this architecture to the entity above
architecture RTL of AND_GATE is
begin
    -- PROCESS
    -- format: process(sensitivity_list) begin ... end process;
    -- sensitivity list: signals that wake up this process when they change
    -- so process(A, B) means: whenever A or B changes, run this process
    -- this is exactly like our Process struct with sensitivity_list!
    process(A, B)
    begin
        -- SIGNAL ASSIGNMENT
        -- format: target <= expression;
        -- <= is the signal assignment operator in VHDL (not less-than-equal here!)
        -- Y <= A and B means: Y gets the value of A AND B
        -- this is like our scheduler_notify + process run() combined
        Y <= A and B;
    end process;

end RTL;
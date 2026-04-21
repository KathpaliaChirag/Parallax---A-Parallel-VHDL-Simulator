-- CHIRAG 20-04-26 :: 8 stage pipeline chain
-- idea ... extend chainof4 to 8 stages ... tests deep delta cycle propagation
-- each stage is a buffer ... output of stage N feeds input of stage N+1
-- dependency graph is a linear chain ... 8 nodes ... 7 edges ... 8 colors
-- worst case for parallelism ... all stages depend on previous ... fully sequential
-- but good for testing delta depth ... signal takes 8 deltas to propagate end to end

entity PIPELINE8 is
    port(
        IN0  : in bit;
        OUT7 : out bit
    );
end PIPELINE8;

architecture RTL of PIPELINE8 is
signal W1 : bit;
signal W2 : bit;
signal W3 : bit;
signal W4 : bit;
signal W5 : bit;
signal W6 : bit;
signal W7 : bit;
begin
    process(IN0)  begin W1   <= IN0; end process;
    process(W1)   begin W2   <= W1;  end process;
    process(W2)   begin W3   <= W2;  end process;
    process(W3)   begin W4   <= W3;  end process;
    process(W4)   begin W5   <= W4;  end process;
    process(W5)   begin W6   <= W5;  end process;
    process(W6)   begin W7   <= W6;  end process;
    process(W7)   begin OUT7 <= W7;  end process;
end RTL;
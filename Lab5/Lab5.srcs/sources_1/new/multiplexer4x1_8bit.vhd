library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity multiplexer4x1_8bit is
    Port ( D1 : in std_logic_vector(7 downto 0);
           D2 : in std_logic_vector(7 downto 0);
           D3 : in std_logic_vector(7 downto 0);
           D4 : in std_logic_vector(7 downto 0);
           choose : in std_logic_vector(1 downto 0);
           output : out std_logic_vector(7 downto 0));
end multiplexer4x1_8bit;

architecture Behavioral of multiplexer4x1_8bit is

begin

    with choose select output <=
        D1 when "00",
        D2 when "01",
        D3 when "10",
        D4 when "11",
        D1 when others;
--    process(D1, D2, D3, D4, choose) begin          
--        if choose = "00" then
--            output<=D1;
--        elsif choose = "01" then
--            output<=D2;
--        elsif choose = "10" then
--            output<=D3;
--        elsif choose = "11" then
--            output<=D4;
--        else
--          output <= (others => '0');
--        end if;
--    end process;
end Behavioral;
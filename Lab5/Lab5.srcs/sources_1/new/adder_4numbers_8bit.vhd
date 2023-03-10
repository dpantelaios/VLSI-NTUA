library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;
use IEEE.std_logic_unsigned.all;

entity adder_4numbers_8bit is
    Port ( A : in STD_LOGIC_VECTOR (7 downto 0):=(others=>'0');
           B : in STD_LOGIC_VECTOR (7 downto 0):=(others=>'0');
           C : in STD_LOGIC_VECTOR (7 downto 0):=(others=>'0');
           D : in STD_LOGIC_VECTOR (7 downto 0):=(others=>'0');
           mean : out STD_LOGIC_VECTOR (7 downto 0));
end adder_4numbers_8bit;

architecture adder_arch of adder_4numbers_8bit is

begin
    process(A, B, C, D)
        variable tmp1,tmp2,tmp3: std_logic_vector (9 downto 0);
        begin
            tmp1 := ("00"&A)+("00"&B);
            tmp2 := tmp1+("00"&C);
            tmp3 := tmp2+("00"&D);
            mean<= tmp3(9 downto 2);  --divide by 4
    end process;
end adder_arch;
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned.all;

entity counter_with_limit_behavioral is
    port( clk,
        resetn,
        count_en : in std_logic;
        parallel_in : in std_logic_vector(2 downto 0);
        sum : out std_logic_vector(2 downto 0);
        cout : out std_logic);
end;


architecture rtl_limit of counter_with_limit_behavioral is
signal count: std_logic_vector(2 downto 0);
begin
    process(clk, resetn)
    begin
        if resetn='0' then
            -- ??????? ??? ??? ????????? ??? reset (?????? ??????)
            count <= (others=>'0');
        elsif clk'event and clk='1' then
            if count_en='1' then
                if count>=parallel_in then
                    count<=(others=>'0');
                else
                    count <= count + 1;
                end if;
            end if;
        end if;
    end process;
    -- ??????? ????? ??? ?????? ??????
    sum <= count;
    cout <= '1' when count=parallel_in and count_en='1' else '0';
end rtl_limit;
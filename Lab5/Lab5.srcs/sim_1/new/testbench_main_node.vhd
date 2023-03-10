----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 01.06.2022 10:29:37
-- Design Name: 
-- Module Name: testbanch_main_node - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;
use IEEE.std_logic_unsigned.all;
use std.textio.all;

entity testbench_main_node is
end testbench_main_node;

architecture Behavioral of testbench_main_node is
    signal clk, rst,  valid_in, new_image, valid_out, image_finished : std_logic;
    signal red, green, blue, input_read : std_logic_vector(7 downto 0);
    constant clk1_period : time := 1 ns;
    --signal value_read: std_logic_vector(7 downto 0);
    signal clk_counter : std_logic_vector (12 downto 0) := (others => '0');
    signal counter_in :  std_logic_vector(20 downto 0);
    --debug
    signal next_state: std_logic_vector(1 downto 0);
    --debug

    component main_node is
      generic (N: std_logic_vector(11 downto 0) := "000000100000");
      Port (clk : in std_logic;
            rst, valid_in, new_image : in std_logic;
            input : in std_logic_vector(7 downto 0);
            red : out std_logic_vector(7 downto 0);
            green : out std_logic_vector(7 downto 0);
            blue : out std_logic_vector(7 downto 0);
            --debug
            next_state: out std_logic_vector(1 downto 0);
            counter_in : inout std_logic_vector(20 downto 0);
            --debug
            valid_out, image_finished : out std_logic);
    end component;
    
    
begin

    uut : main_node
      Port map(clk => clk,
            rst => rst, 
            valid_in => valid_in,  
            new_image => new_image,
            input => input_read, 
            red => red,
            blue => blue, 
            green =>  green,
            --debug
            next_state => next_state,
            counter_in => counter_in,
            --debug
            valid_out => valid_out, image_finished => image_finished);
    
    stimulus: process 
    
         file test_file : text open read_mode is "bayer32x32.txt";
        variable row : line;
        variable v_data_read : integer;
    begin
        valid_in <= '1';
        new_image <= '0';
        rst <= '1';
        wait for clk1_period/2;
        valid_in <= '0';
        new_image <= '0';
        wait for clk1_period/2;
        rst <= '1';
        readline(test_file, row);
        read(row, v_data_read);
        input_read <= std_logic_vector(to_unsigned(v_data_read, 8));
         
        valid_in <= '1';
        new_image <= '1';
        wait for clk1_period/2;
                clk_counter <= clk_counter + 1;
                wait for clk1_period/2;
        new_image <= '0';
    
        --for i in 1 to 1023 loop
        --    readline(test_file, row);
        --    read(row, v_data_read);
        --    input_read <= std_logic_vector(to_unsigned(v_data_read, 8));
        --    wait for clk1_period/2;
        --        clk_counter <= clk_counter + 1;
        --        wait for clk1_period/2;
        --end loop;
        for i in 1 to 512 loop
            readline(test_file, row);
            read(row, v_data_read);
            input_read <= std_logic_vector(to_unsigned(v_data_read, 8));
            wait for clk1_period/2;
                clk_counter <= clk_counter + 1;
                wait for clk1_period/2;
        end loop;
        
        valid_in <= '0'; 
        for i in 0 to 20 loop
            wait for clk1_period;
        end loop;
        
        valid_in <= '1';
        for i in 513 to 1023 loop
            readline(test_file, row);
            read(row, v_data_read);
            input_read <= std_logic_vector(to_unsigned(v_data_read, 8));
            wait for clk1_period/2;
            clk_counter <= clk_counter + 1;
            wait for clk1_period/2;
        end loop;
        
        valid_in <= '0';
        
        wait; 
        
            
    end process;
  
    
    clk1_generator: process begin
            clk <= '1';
            wait for clk1_period/2;
            clk <= '0';
            wait for clk1_period/2;
        end process;
end Behavioral;
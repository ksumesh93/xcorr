----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 01/26/2021 07:20:29 PM
-- Design Name: 
-- Module Name: processing_element - Behavioral
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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity processing_element is
    Port ( clock : in STD_LOGIC;
           reset : in STD_LOGIC;
           start : out STD_LOGIC;
           done : in STD_LOGIC;
           
           command_rw : out STD_LOGIC;
           ddr_readdatavalid : in STD_LOGIC;
           ddr_readdata : in STD_LOGIC_VECTOR(511 DOWNTO 0);
           ddr_writedata : out STD_LOGIC_VECTOR(511 DOWNTO 0);
           start_address: out STD_LOGIC_VECTOR(25 DOWNTO 0);
           end_address: out STD_LOGIC_VECTOR(25 DOWNTO 0));
end processing_element;

architecture Behavioral of processing_element is

begin


end Behavioral;

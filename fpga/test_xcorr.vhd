----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 01/26/2021 08:32:39 AM
-- Design Name: 
-- Module Name: test_xcorr - Behavioral
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
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity test_xcorr is
--  Port ( );
end test_xcorr;

architecture Behavioral of test_xcorr is

component xcorr is
    Port ( iCLK : in STD_LOGIC;
           iRST_n : in STD_LOGIC;
           avs_control_address    : in  std_logic_vector(3 downto 0)   := (others => '0'); -- avs_control.address
            avs_control_read       : in  std_logic                      := '0';             --            .read
            avs_control_readdata   : out std_logic_vector(31 downto 0);                     --            .readdata
            avs_control_write      : in  std_logic                      := '0';             --            .write
            avs_control_writedata  : in  std_logic_vector(31 downto 0)  := (others => '0') --            .writedata);
            );
end component;


signal iCLK : STD_LOGIC;
signal iRST_n : STD_LOGIC;
signal avs_control_address    : std_logic_vector(3 downto 0)   := (others => '0'); -- avs_control.address
signal avs_control_read       : std_logic                      := '0';             --            .read
signal avs_control_readdata   : std_logic_vector(31 downto 0);                     --            .readdata
signal avs_control_write      : std_logic                      := '0';             --            .write
signal avs_control_writedata  : std_logic_vector(31 downto 0)  := (others => '0'); --            .writedata);

constant CLK_PERIOD : time := 5ns;

begin

xcorr_test: xcorr 
    Port map
          ( iCLK => iCLK,
           iRST_n => iRST_n,
           avs_control_address => avs_control_address,
            avs_control_read   => avs_control_read,
            avs_control_readdata  => avs_control_readdata,
            avs_control_write    => avs_control_write,
            avs_control_writedata  => avs_control_writedata
            );

clock: process
begin
    iCLK <= '0';
    wait for CLK_PERIOD/2;
    iCLK <= '1';
    wait for CLK_PERIOD/2;
 end process;

stimulus: process
begin
     iRST_n <= '0';
     avs_control_address <= (others => '0');
     avs_control_read <= '0';
     avs_control_write <= '0';
     avs_control_writedata <= (others => '0');
     wait for 50 ns;
     
     iRST_n <= '1';
     wait for 40ns;
     
     avs_control_address <= std_logic_vector(to_unsigned(2, avs_control_address'length));
     avs_control_write <= '1';
     avs_control_writedata <= std_logic_vector(to_unsigned(4, avs_control_writedata'length));
     
     wait for 5ns;
     avs_control_write <= '0';
     
     wait for 40ns;
     avs_control_address <= std_logic_vector(to_unsigned(3, avs_control_address'length));
     avs_control_write <= '1';
     avs_control_writedata <= std_logic_vector(to_unsigned(6, avs_control_writedata'length));
     
     wait for 5ns;
     avs_control_write <= '0';
     
     wait for 40ns;
     avs_control_address <= std_logic_vector(to_unsigned(0, avs_control_address'length));
     avs_control_write <= '1';
     avs_control_writedata <= std_logic_vector(to_unsigned(3, avs_control_writedata'length));
     
     wait for 5ns;
     avs_control_write <= '0';
     
     wait for 40ns;
     avs_control_address <= std_logic_vector(to_unsigned(0, avs_control_address'length));
     avs_control_write <= '1';
     avs_control_writedata <= std_logic_vector(to_unsigned(0, avs_control_writedata'length));
     
     wait for 5ns;
     avs_control_write <= '0';
     
     avs_control_address <= std_logic_vector(to_unsigned(2, avs_control_address'length));
     avs_control_write <= '1';
     avs_control_writedata <= std_logic_vector(to_unsigned(4, avs_control_writedata'length));
     
     wait for 5ns;
     avs_control_write <= '0';
     
     wait for 40ns;
     avs_control_address <= std_logic_vector(to_unsigned(3, avs_control_address'length));
     avs_control_write <= '1';
     avs_control_writedata <= std_logic_vector(to_unsigned(8, avs_control_writedata'length));
     
     wait for 5ns;
     avs_control_write <= '0';
     
     wait for 40ns;
     avs_control_address <= std_logic_vector(to_unsigned(0, avs_control_address'length));
     avs_control_write <= '1';
     avs_control_writedata <= std_logic_vector(to_unsigned(1, avs_control_writedata'length));
     
     wait for 5ns;
     avs_control_write <= '0';
     wait;
end process;


end Behavioral;

----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 01/25/2021 08:47:38 PM
-- Design Name: 
-- Module Name: test_bus - Behavioral
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

entity test_bus is
--  Port ( );
end test_bus;

architecture Behavioral of test_bus is

COMPONENT Avalon_RW_bus IS
	PORT (
		iCLK : IN STD_LOGIC;
		iRST_n : IN STD_LOGIC;
		insert_error : IN STD_LOGIC;
		drv_status_test_complete : OUT STD_LOGIC;

		command_rw : IN STD_LOGIC;
		ddr_readdatavalid : OUT STD_LOGIC;
		ddr_readdata : OUT STD_LOGIC_VECTOR(511 DOWNTO 0);
		ddr_writedata : IN STD_LOGIC_VECTOR(511 DOWNTO 0);
		start_address : IN STD_LOGIC_VECTOR(25 DOWNTO 0);
		end_address : IN STD_LOGIC_VECTOR(25 DOWNTO 0));
END COMPONENT;

signal iCLK : STD_LOGIC;
signal iRST_n : STD_LOGIC;
signal insert_error : STD_LOGIC;
signal drv_status_test_complete : STD_LOGIC;

signal command_rw : STD_LOGIC;
signal ddr_readdatavalid : STD_LOGIC;
signal ddr_readdata : STD_LOGIC_VECTOR(511 DOWNTO 0);
signal ddr_writedata : STD_LOGIC_VECTOR(511 DOWNTO 0);
signal start_address : STD_LOGIC_VECTOR(25 DOWNTO 0);
signal end_address : STD_LOGIC_VECTOR(25 DOWNTO 0);

constant CLK_PERIOD : time := 5ns;

begin

process
begin
    iCLK <= '0';
    wait for CLK_PERIOD/2;
    iCLK <= '1';
    wait for CLK_PERIOD/2;
 end process;
 
 process
 begin
    iRST_n <= '0';
    ddr_writedata <= (others => '0');
    start_address <= (others => '0');
    end_address <= (others => '0');
    insert_error <= '0';
    command_rw <= '0';
    wait for 50ns;
    iRST_n <= '1';
    wait for 50ns;
    if drv_status_test_complete = '0' then
        ddr_writedata <= std_logic_vector(to_unsigned(128, ddr_writedata'length));
        start_address <= std_logic_vector(to_unsigned(3, start_address'length));
        end_address <= (others => '0');
        insert_error <= '1';
        command_rw <= '1';
    end if;
    wait until drv_status_test_complete = '1';
    iRST_n <= '0';
    wait;
 end process;


bus_test : Avalon_RW_bus 
	PORT MAP
	   (
		iCLK => iCLK,
		iRST_n => iRST_n,
		insert_error => insert_error,
		drv_status_test_complete => drv_status_test_complete,

		command_rw => command_rw,
		ddr_readdatavalid => ddr_readdatavalid,
		ddr_readdata => ddr_readdata,
		ddr_writedata => ddr_writedata,
		start_address => start_address,
		end_address => end_address
		);


end Behavioral;

----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 01/26/2021 12:34:51 PM
-- Design Name: 
-- Module Name: xcorr - Behavioral
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
----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 01/26/2021 08:16:28 AM
-- Design Name: 
-- Module Name: xcorr - Behavioral
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

entity xcorr is

    Port ( iCLK : in STD_LOGIC;
           iRST_n : in STD_LOGIC;
           avs_control_address    : in  std_logic_vector(3 downto 0)   := (others => '0'); -- avs_control.address
            avs_control_read       : in  std_logic                      := '0';             --            .read
            avs_control_readdata   : out std_logic_vector(31 downto 0);                     --            .readdata
            avs_control_write      : in  std_logic                      := '0';             --            .write
            avs_control_writedata  : in  std_logic_vector(31 downto 0)  := (others => '0') --            .writedata);
            );
end xcorr;

architecture Behavioral of xcorr is

COMPONENT Avalon_RW_bus IS
    GENERIC ( file_path : string := "file_path");
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

SIGNAL control_address : unsigned(3 DOWNTO 0);
TYPE reg_file IS ARRAY (0 TO 9) OF STD_LOGIC_VECTOR(31 DOWNTO 0);
SIGNAL control_registers : reg_file;
SIGNAL xcorr_control_register : STD_LOGIC_VECTOR(31 DOWNTO 0);
SIGNAL xcorr_status_register : STD_LOGIC_VECTOR(31 DOWNTO 0);

SIGNAL ddr_start_address : STD_LOGIC_VECTOR(25 DOWNTO 0);
SIGNAL ddr_end_address : STD_LOGIC_VECTOR(25 DOWNTO 0);
SIGNAL ddr_readdata_0 : STD_LOGIC_VECTOR(511 DOWNTO 0);
SIGNAL ddr_readdata_1 : STD_LOGIC_VECTOR(511 DOWNTO 0);
SIGNAL ddr_readdata_2 : STD_LOGIC_VECTOR(511 DOWNTO 0);
SIGNAL ddr_readdata_3 : STD_LOGIC_VECTOR(511 DOWNTO 0);
SIGNAL ddr_writedata : STD_LOGIC_VECTOR(511 DOWNTO 0);



SIGNAL core0_readdatavalid : STD_LOGIC;
SIGNAL core1_readdatavalid : STD_LOGIC;
SIGNAL core2_readdatavalid : STD_LOGIC;
SIGNAL core3_readdatavalid : STD_LOGIC;


SIGNAL drv_status_op_complete : STD_LOGIC_VECTOR(3 DOWNTO 0);


begin

xcorr_control_register <= control_registers(0);
control_address <= unsigned(avs_control_address);
avs_control_readdata <= control_registers(to_integer(control_address));
PROCESS (iCLK, iRST_n)
BEGIN
    IF iRST_n = '0' THEN
        FOR ii IN 0 TO 9 LOOP
            control_registers(ii) <= (others => '0');
        END LOOP;
    ELSIF rising_edge(iCLK) THEN
        IF avs_control_write = '1' THEN
            control_registers(to_integer(control_address)) <= avs_control_writedata;
        ELSE
            control_registers(1) <= xcorr_status_register;
        END IF;
    END IF;
END PROCESS;


xcorr_status_register(0) <= drv_status_op_complete(0);
xcorr_status_register(1) <= drv_status_op_complete(1);
xcorr_status_register(2) <= drv_status_op_complete(2);
xcorr_status_register(3) <= drv_status_op_complete(3);
xcorr_status_register(31 DOWNTO 4) <= (OTHERS => '0');

ddr_writedata <= STD_LOGIC_VECTOR(to_unsigned(64, 512));

bus_test1 : Avalon_RW_bus 
    GENERIC MAP ( file_path => "C:\Users\sumes\OneDrive\Documents\Xcorr-Design-files\project-code\xcorr\ddra.txt")
	PORT MAP
	   (
		iCLK => iCLK,
		iRST_n => iRST_n,
		insert_error => xcorr_control_register(0),
		drv_status_test_complete => drv_status_op_complete(0),

		command_rw => xcorr_control_register(1),
		ddr_readdatavalid => core0_readdatavalid,
		ddr_readdata => ddr_readdata_0,
		ddr_writedata => ddr_writedata,
		start_address => control_registers(2)(25 DOWNTO 0),
		end_address => control_registers(3)(25 DOWNTO 0)
		);

bus_test2 : Avalon_RW_bus 
    GENERIC MAP ( file_path => "C:\Users\sumes\OneDrive\Documents\Xcorr-Design-files\project-code\xcorr\ddrb.txt")
	PORT MAP
	   (
		iCLK => iCLK,
		iRST_n => iRST_n,
		insert_error => xcorr_control_register(2),
		drv_status_test_complete => drv_status_op_complete(1),

		command_rw => xcorr_control_register(3),
		ddr_readdatavalid => core1_readdatavalid,
		ddr_readdata => open,
		ddr_writedata => ddr_writedata,
		start_address => control_registers(4)(25 DOWNTO 0),
		end_address => control_registers(5)(25 DOWNTO 0)
		);
		
bus_test3 : Avalon_RW_bus 
    GENERIC MAP ( file_path => "C:\Users\sumes\OneDrive\Documents\Xcorr-Design-files\project-code\xcorr\ddrc.txt")
	PORT MAP
	   (
		iCLK => iCLK,
		iRST_n => iRST_n,
		insert_error => xcorr_control_register(4),
		drv_status_test_complete => drv_status_op_complete(2),

		command_rw => xcorr_control_register(5),
		ddr_readdatavalid => core2_readdatavalid,
		ddr_readdata => open,
		ddr_writedata => ddr_writedata,
		start_address => control_registers(6)(25 DOWNTO 0),
		end_address => control_registers(7)(25 DOWNTO 0)
		);
		
bus_test4 : Avalon_RW_bus 
    GENERIC MAP ( file_path => "C:\Users\sumes\OneDrive\Documents\Xcorr-Design-files\project-code\xcorr\ddrd.txt")
	PORT MAP
	   (
		iCLK => iCLK,
		iRST_n => iRST_n,
		insert_error => xcorr_control_register(6),
		drv_status_test_complete => drv_status_op_complete(3),

		command_rw => xcorr_control_register(7),
		ddr_readdatavalid => core3_readdatavalid,
		ddr_readdata => open,
		ddr_writedata => ddr_writedata,
		start_address => control_registers(8)(25 DOWNTO 0),
		end_address => control_registers(9)(25 DOWNTO 0)
		);				

end Behavioral;




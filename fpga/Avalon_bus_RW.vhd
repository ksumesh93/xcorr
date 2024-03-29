----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 05/12/2020 08:18:04 PM
-- Design Name: 
-- Module Name: Avalon_RW_bus - Behavioral
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
LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
USE IEEE.NUMERIC_STD.ALL;
USE IEEE.STD_LOGIC_TEXTIO.ALL;
USE std.textio.all;


ENTITY Avalon_RW_bus IS
    GENERIC ( file_path : string := "file_path");
	PORT (
		iCLK_out : OUT STD_LOGIC;
		iRST_n : IN STD_LOGIC;
		insert_error : IN STD_LOGIC;
		drv_status_test_complete : OUT STD_LOGIC;

		command_rw : IN STD_LOGIC;
		write_addr : OUT STD_LOGIC_VECTOR(4 DOWNTO 0);
		read_addr : OUT STD_LOGIC_VECTOR(4 DOWNTO 0);
		
		ddr_readdatavalid : OUT STD_LOGIC;
		ddr_readdata : OUT STD_LOGIC_VECTOR(511 DOWNTO 0);
		ddr_writedata : IN STD_LOGIC_VECTOR(511 DOWNTO 0);
		start_address : IN STD_LOGIC_VECTOR(25 DOWNTO 0);
		end_address : IN STD_LOGIC_VECTOR(25 DOWNTO 0));
END Avalon_RW_bus;

ARCHITECTURE Behavioral OF Avalon_RW_bus IS


constant SCRATCH : string := "D:\backup\Xcorr-Design-files\project-code\data\scratch_pad.txt";

constant C_DATA_W : integer := 512;
constant C_CLK_PERIOD : time := 5ns;

signal address_start : unsigned(25 downto 0);
signal address_end : unsigned(25 downto 0);

signal data : std_logic_vector(C_DATA_W-1 downto 0);

signal eof_a : std_logic;
signal eof_b : std_logic;
signal eof_c : std_logic;
signal eof_d : std_logic;

signal iCLK : std_logic;

signal number : unsigned(31 downto 0) := x"00000000";

signal read_addr_w : unsigned(4 downto 0);
signal write_addr_w : unsigned(4 downto 0);
signal write_en : std_logic;

file fptr_read_ddr : text;
file fptr_write_ddr : text;

procedure read_from_file(
    signal line_start : in unsigned(25 downto 0);
    signal line_end : in unsigned(25 downto 0);
    signal done : out std_logic;
    signal dataline : out std_logic_vector(C_DATA_W-1 downto 0);
    signal readdatavalid : out std_logic;
    constant FILE_NAME : in string;
    file fptr : text
    ) is
    variable num : natural range 0 to 3354432:=0;
    variable fstatus : file_open_status;
    variable file_line : line;
    variable var_data : std_logic_vector(C_DATA_W-1 downto 0);
    variable eof : std_logic;
    variable start_reading : std_logic := '0';
    
    begin
        dataline <= (others => '0');
        readdatavalid <= '0';
        num := 0;
        eof := '0';
        done <= '0';
        --Opening the file for reading
        file_open(fstatus, fptr, FILE_NAME, read_mode);
        readdatavalid <= '0';
        while (not endfile(fptr) and eof = '0') loop
            readline(fptr, file_line);
            if num >= to_integer(line_start) and num <= to_integer(line_end) then
                hread(file_line, var_data);
                if start_reading = '0' then
                    start_reading := '1';
                    wait until iCLK = '1';
                    wait until iCLK = '0';
                    readdatavalid <= '1';
                    dataline <= var_data;
                    wait until iCLK = '1';
                    wait for 2 ns;
                else
                    dataline <= var_data;
                    readdatavalid <= '1';
                    wait for 2 ns;
                end if;
            else
                readdatavalid <= '0';
                dataline <= (others => '0');
                eof := start_reading;
            end if;
            num := num + 1;
        end loop;
        done <= '1';
        file_close(fptr);
    end read_from_file;
        
procedure write_to_file(
    signal line_start : in unsigned(25 downto 0);
    signal line_end : in unsigned(25 downto 0);
    signal done : out std_logic;
    signal dataline : in std_logic_vector(C_DATA_W-1 downto 0);
    signal write_begin : out std_logic;
    constant FILE_NAME : in string;
    constant FILE_NAME2 : in string;
    file fptr : text
    ) is
    variable num : natural range 0 to 3354432:=0;
    variable fstatus1 : file_open_status;
    variable fstatus2 : file_open_status;
    variable file_line : line;
    variable file_line2 : line;
    variable var_data : std_logic_vector(C_DATA_W-1 downto 0);
    file fptr_temp : text;
    variable eof : std_logic;
    
    begin
        write_begin <= '0';
        num := 0;
        eof := '0';
        done <= '0';
        --Opening the file for reading
        --file_open(fstatus, fptr, FILE_NAME, write_mode);
        file_open(fstatus1, fptr_temp, FILE_NAME, read_mode);
        file_open(fstatus2, fptr, FILE_NAME2, write_mode);
        while (not endfile(fptr_temp)) loop        
            if num >= to_integer(line_start) and num <= to_integer(line_end) then
                readline(fptr_temp, file_line);
                hwrite(file_line2, dataline);
                writeline(fptr, file_line2);
                write_begin <= '1';
                wait for 2ns;
            else
                readline(fptr_temp, file_line);
                hread(file_line, var_data);
                hwrite(file_line2, var_data);
                writeline(fptr, file_line2);
                write_begin <= '0';
            end if;
            num := num + 1;
        end loop;
        file_close(fptr);
        file_close(fptr_temp);
        
        file_open(fstatus1, fptr_temp, FILE_NAME2, read_mode);
        file_open(fstatus2, fptr, FILE_NAME, write_mode);
        while(not endfile(fptr_temp)) loop
            wait for 1 ps;
            
            eof := '0';
            readline(fptr_temp, file_line);
            hread(file_line, var_data);
            hwrite(file_line2, var_data);
            writeline(fptr, file_line2);
        end loop;
        done <= '1';
        file_close(fptr);
        file_close(fptr_temp);
    end write_to_file;

signal operation_complete : std_logic;
signal readdatavalid : std_logic;
signal readdata : std_logic_vector(511 downto 0);

BEGIN


    
    
    ddr_readdatavalid <= readdatavalid;
    ddr_readdata <= readdata;
    
    process
    begin
        if operation_complete = '1' and iRST_n = '1' then
            wait until insert_error = '0';
            operation_complete <= '0'; 
            readdatavalid <= '0';
        elsif insert_error = '1' and iRST_n = '1' then
            if command_rw = '1' then
            
                write_to_file(address_start, address_end, operation_complete, ddr_writedata, write_en, file_path, SCRATCH, fptr_write_ddr);
            else 
                read_from_file(address_start, address_end, operation_complete, readdata, readdatavalid, file_path, fptr_read_ddr);
            end if;
        else
            operation_complete <= '0';
        end if;
        address_start <= unsigned(start_address);
        address_end <= unsigned(end_address);
        wait for 1ps;
    end process;
    
    process(iCLK,iRST_n)
    begin
        if iRST_n = '0' then 
            drv_status_test_complete <= '0';
        elsif rising_edge(iCLK) then
            drv_status_test_complete <= operation_complete;
        end if;
    end process;
    
    read_addr <= std_logic_vector(read_addr_w);
    write_addr <= std_logic_vector(write_addr_w);
    
    
    process(iCLK,iRST_n)
    begin
        if iRST_n = '0' then 
            read_addr_w <= (others => '0');
        elsif falling_edge(iCLK) then
            if readdatavalid = '1' then
                read_addr_w <= read_addr_w + 1;
            end if;
        end if;
    end process;
    
    process(iCLK,iRST_n)
    begin
        if iRST_n = '0' then 
            write_addr_w <= (others => '0');
        elsif rising_edge(iCLK) then
            if write_en = '1' then
                write_addr_w <= write_addr_w + 1;
            end if;
        end if;
    end process;
    
    
    process
    begin
        iCLK <= '1';
        wait for 1ns;
        iCLK <= '0';
        wait for 1ns;
    end process;
    
    iCLK_out <= iCLK;

END Behavioral;
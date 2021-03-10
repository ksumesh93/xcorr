----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 03/07/2021 08:44:00 PM
-- Design Name: 
-- Module Name: ion_matching_kernel - Behavioral
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

use work.package_xcorr.all;
use IEEE.NUMERIC_STD.ALL;



entity ion_matching_kernel is
    Port(
        packet_in : in STD_LOGIC_VECTOR(511 downto 0);
        theoretical_ion : in STD_LOGIC_VECTOR(15 downto 0);
        ion_out : out STD_LOGIC_VECTOR(15 downto 0);
        new_spectral_chunk : out STD_LOGIC;
        new_theoretical_ion : out STD_LOGIC;
        anamoly_cond1 : out STD_LOGIC;
        anamoly_cond2 : out STD_LOGIC
        );
end ion_matching_kernel;

architecture Behavioral of ion_matching_kernel is


signal spectral_chunk : spectral_ions;

signal comparison : std_logic_vector(15 downto 0);

begin

--Mapping inputs
spectral_chunk <= vector_to_spectral_chunk(packet_in);



--Processing pipelines
--Here 16 experimental spectral ion pairs will be matches with a theoretical ion
--Comparator array 

generate_comparators : for ii in 0 to 15 generate
    comparison(ii) <= '1' when (unsigned(theoretical_ion) >= spectral_chunk(ii).ion_mz) else '0';
end generate;

--Generating mux for selecting the correct m_z value
with comparison select ion_out <=
    std_logic_vector(spectral_chunk(0).ion_intensity) when std_logic_vector(to_unsigned(2**0 - 1, comparison'length)),
    std_logic_vector(spectral_chunk(1).ion_intensity) when std_logic_vector(to_unsigned(2**1 - 1, comparison'length)),
    std_logic_vector(spectral_chunk(2).ion_intensity) when std_logic_vector(to_unsigned(2**2 - 1, comparison'length)),
    std_logic_vector(spectral_chunk(3).ion_intensity) when std_logic_vector(to_unsigned(2**3 - 1, comparison'length)),
    std_logic_vector(spectral_chunk(4).ion_intensity) when std_logic_vector(to_unsigned(2**4 - 1, comparison'length)),
    std_logic_vector(spectral_chunk(5).ion_intensity) when std_logic_vector(to_unsigned(2**5 - 1, comparison'length)),
    std_logic_vector(spectral_chunk(6).ion_intensity) when std_logic_vector(to_unsigned(2**6 - 1, comparison'length)),
    std_logic_vector(spectral_chunk(7).ion_intensity) when std_logic_vector(to_unsigned(2**7 - 1, comparison'length)),
    std_logic_vector(spectral_chunk(8).ion_intensity) when std_logic_vector(to_unsigned(2**8 - 1, comparison'length)),
    std_logic_vector(spectral_chunk(9).ion_intensity) when std_logic_vector(to_unsigned(2**9 - 1, comparison'length)),
    std_logic_vector(spectral_chunk(10).ion_intensity) when std_logic_vector(to_unsigned(2**10 - 1, comparison'length)),
    std_logic_vector(spectral_chunk(11).ion_intensity) when std_logic_vector(to_unsigned(2**11 - 1, comparison'length)),
    std_logic_vector(spectral_chunk(12).ion_intensity) when std_logic_vector(to_unsigned(2**12 - 1, comparison'length)),
    std_logic_vector(spectral_chunk(13).ion_intensity) when std_logic_vector(to_unsigned(2**13 - 1, comparison'length)),
    std_logic_vector(spectral_chunk(14).ion_intensity) when std_logic_vector(to_unsigned(2**14 - 1, comparison'length)),
    std_logic_vector(spectral_chunk(15).ion_intensity) when std_logic_vector(to_unsigned(2**15 - 1, comparison'length));


--Conditions when the processing element might need to store the calculated score separately
anamoly_cond2 <= not comparison(0);
anamoly_cond1 <= comparison(15);

--These are duplicated signals named differently because they are serving a different purposes. We have to optimize this part later
new_spectral_chunk <= comparison(15);
new_theoretical_ion <= not comparison(0);



end Behavioral;

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

package package_xcorr is
  
  type ion_pair is record
    ion_mz : unsigned(15 downto 0);
    ion_intensity : unsigned(15 downto 0);
  end record ion_pair;
 
  type spectral_ions is array (0 to 15) of ion_pair; 
  type peptide_sequence is array (0 to 49) of unsigned(7 downto 0);
  type protein_id is array (0 to 7) of unsigned(7 downto 0);
  
  type score_packet is record
    scan_number : unsigned(31 downto 0); --4 bytes
    peptide : peptide_sequence;  --50 bytes
    protein : protein_id;        --8 bytes
    xcorr_score : unsigned(15 downto 0);
  end record score_packet;
  
  type peptide_packet is record
    protein : protein_id;
    peptide_mass : unsigned(31 downto 0);
    peptide : peptide_sequence;
    zero_fill : unsigned(15 downto 0);
  end record peptide_packet;
  
  type spectrum_header is record
    scan_number : unsigned(31 downto 0); --4 bytes
    precursor_mass : unsigned(31 downto 0); --4 bytes
    status : unsigned(7 downto 0); -- 1 bytes
    spectrum_start_address : unsigned(31 downto 0); --4 bytes
    spectrum_end_address : unsigned(31 downto 0); -- 4 bytes
    zero_fill : unsigned(375 downto 0);
  end record spectrum_header;
  
  function ppacket_to_vector(pep_packet : peptide_packet)
        return std_logic_vector;
        
  function shead_to_vector(spec_head : spectrum_header)
        return std_logic_vector;
        
  function spacket_to_vector(s_packet : score_packet)
        return std_logic_vector;
        
  function vector_to_ppacket(vectored_packet : std_logic_vector(511 downto 0))
        return peptide_packet;
  
  function vector_to_shead(vectored_packet : std_logic_vector(511 downto 0))
        return spectrum_header;
        
  function vector_to_spacket(vectored_packet : std_logic_vector(511 downto 0))
        return score_packet;
        
  function vector_to_spectral_chunk(vectored_packet : std_logic_vector(511 downto 0))
        return spectral_ions;
        
  function spectral_chunk_to_vector(spectral_chunk : spectral_ions)
        return std_logic_vector;          
end package package_xcorr;
 
package body package_xcorr is

    function ppacket_to_vector(pep_packet : peptide_packet)
        return std_logic_vector is
        variable vectored_packet : std_logic_vector(511 downto 0);
    begin
        for ii in 0 to 7 loop
            vectored_packet(ii*8+7+448 downto ii*8+448) := std_logic_vector(pep_packet.protein(ii)); 
        end loop;
        vectored_packet(447 downto 416) := std_logic_vector(pep_packet.peptide_mass); 
        for ii in 0 to 49 loop
            vectored_packet(ii*8+7+16 downto ii*8+16) := std_logic_vector(pep_packet.peptide(ii)); 
        end loop;
        vectored_packet(15 downto 0) := std_logic_vector(pep_packet.zero_fill); 
        return vectored_packet;
    end function ppacket_to_vector;
    
    
    function shead_to_vector(spec_head : spectrum_header)
        return std_logic_vector is
        variable vectored_packet : std_logic_vector(511 downto 0);
    begin
        vectored_packet(511 downto 480) := std_logic_vector(spec_head.scan_number); 
        vectored_packet(479 downto 448) := std_logic_vector(spec_head.precursor_mass); 
        vectored_packet(447 downto 440) := std_logic_vector(spec_head.status); 
        vectored_packet(439 downto 408) := std_logic_vector(spec_head.spectrum_start_address);
        vectored_packet(407 downto 376) := std_logic_vector(spec_head.spectrum_end_address);
        vectored_packet(375 downto 0) := std_logic_vector(spec_head.zero_fill); 
        return vectored_packet;
    end function shead_to_vector;
    
    
    function spacket_to_vector(s_packet : score_packet)
        return std_logic_vector is
        variable vectored_packet : std_logic_vector(511 downto 0);
    begin
        vectored_packet(511 downto 480) := std_logic_vector(s_packet.scan_number); 
        for ii in 0 to 49 loop
            vectored_packet(ii*8+7+80 downto ii*8+80) := std_logic_vector(s_packet.peptide(ii)); 
        end loop;
        for ii in 0 to 7 loop
            vectored_packet(ii*8+7+16 downto ii*8+16) := std_logic_vector(s_packet.protein(ii)); 
        end loop;
        vectored_packet(15 downto 0) := std_logic_vector(s_packet.xcorr_score); 
        return vectored_packet;
    end function spacket_to_vector;
    
    function vector_to_ppacket(vectored_packet : std_logic_vector(511 downto 0))
        return peptide_packet is
        variable pep_packet : peptide_packet;
    begin
        for ii in 0 to 7 loop
            pep_packet.protein(ii) := unsigned(vectored_packet(ii*8+7+448 downto ii*8+448)); 
        end loop;
            pep_packet.peptide_mass := unsigned(vectored_packet(447 downto 416)); 
        for ii in 0 to 49 loop
            pep_packet.peptide(ii) := unsigned(vectored_packet(ii*8+7+16 downto ii*8+16));
        end loop;
        pep_packet.zero_fill := unsigned(vectored_packet(15 downto 0));
        return pep_packet;
    end function vector_to_ppacket;
    
    function vector_to_shead(vectored_packet : std_logic_vector(511 downto 0))
        return spectrum_header is
        variable spec_head : spectrum_header;
    begin
        spec_head.scan_number := unsigned(vectored_packet(511 downto 480));
        spec_head.precursor_mass := unsigned(vectored_packet(479 downto 448)); 
        spec_head.status := unsigned(vectored_packet(447 downto 440)); 
        spec_head.spectrum_start_address := unsigned(vectored_packet(439 downto 408));
        spec_head.spectrum_end_address := unsigned(vectored_packet(407 downto 376)); 
        spec_head.zero_fill := unsigned(vectored_packet(375 downto 0)); 
        return spec_head;
    end function vector_to_shead;
    
    function vector_to_spacket(vectored_packet : std_logic_vector(511 downto 0))
        return score_packet is
        variable s_packet : score_packet;
    begin
        s_packet.scan_number := unsigned(vectored_packet(511 downto 480));
        for ii in 0 to 49 loop
            s_packet.peptide(ii) := unsigned(vectored_packet(ii*8+7+80 downto ii*8+80));
        end loop;
        for ii in 0 to 7 loop
            s_packet.protein(ii) := unsigned(vectored_packet(ii*8+7+16 downto ii*8+16));
        end loop;
        s_packet.xcorr_score := unsigned(vectored_packet(15 downto 0)); 
        return s_packet;
    end function vector_to_spacket;
    
    function vector_to_spectral_chunk(vectored_packet : std_logic_vector(511 downto 0))
        return spectral_ions is
        variable spectral_chunk : spectral_ions;
    begin
        for ii in 0 to 15 loop
            spectral_chunk(ii).ion_mz := unsigned(vectored_packet(32*ii+15 downto 32*ii));
            spectral_chunk(ii).ion_intensity := unsigned(vectored_packet(32*ii downto 32*ii+16));
        end loop;
        return spectral_chunk;
    end function vector_to_spectral_chunk;
    
    function spectral_chunk_to_vector(spectral_chunk : spectral_ions)
        return std_logic_vector is
        variable vectored_packet : std_logic_vector(511 downto 0);
    begin
        for ii in 0 to 15 loop
            vectored_packet(32*ii+15 downto 32*ii) := std_logic_vector(spectral_chunk(ii).ion_mz);
            vectored_packet(32*ii downto 32*ii+16) := std_logic_vector(spectral_chunk(ii).ion_intensity); 
        end loop;
        return vectored_packet;
    end function spectral_chunk_to_vector;
    
    
end package body package_xcorr;

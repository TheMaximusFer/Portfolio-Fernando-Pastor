
library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.numeric_std.all;

entity reg is
    generic (	size: natural := 32;
    			Tsup_delay: time := 1 ns;
    			propagation_delay: time := 5 ns
    		);  -- por defecto son de 32 bits, pero se puede usar cualquier tamaño
	Port ( Din : in  STD_LOGIC_VECTOR (size -1 downto 0);
           clk : in  STD_LOGIC;
		   reset : in  STD_LOGIC;
           load : in  STD_LOGIC;
           Dout : out  STD_LOGIC_VECTOR (size -1 downto 0));
end REG;

architecture behavioral of REG is

	signal internal_dout, internal_Din : std_logic_vector(size-1 downto 0);
	signal internal_load : std_logic;

begin

	process(clk)
	begin
		if (rising_edge(clk)) then

			if (reset = '1') then
				internal_dout <= (others => '0');
			elsif (internal_load = '1') then
				internal_dout <= internal_din;
			end if;

		end if;

	end process;
--Delays 
	internal_load <= load after Tsup_delay; 
	internal_Din <= Din after Tsup_delay; 
	Dout <= internal_dout after propagation_delay;

end behavioral ; -- arch

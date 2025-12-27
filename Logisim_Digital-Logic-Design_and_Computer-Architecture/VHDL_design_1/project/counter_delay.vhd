----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    12:32:21 07/23/2014 
-- Design Name: 
-- Module Name:    contado5 - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
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
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity counter is
generic (	size: natural := 32;-- por defecto son de 32 bits, pero se puede usar cualquier tamaño
    		Tsup_delay: time := 1 ns;
    		propagation_delay: time := 6 ns -- retardo de propagación 1ns por defecto
    		);  
Port ( clk : in  STD_LOGIC;
       reset : in  STD_LOGIC;
       count_enable : in  STD_LOGIC;
       count : out  STD_LOGIC_VECTOR (size-1 downto 0));
end counter;

architecture Behavioral of counter is
signal count_int: STD_LOGIC_VECTOR (size-1 downto 0);
signal internal_count_enable : std_logic;
begin
process (clk) 
begin
   if clk='1' and clk'event then
      if reset='1' then 
        -- reset síncrono (se resetea cuando llega el flanco de reloj 
      	count_int <= (others => '0');
      elsif internal_count_enable='1' then
        -- el contador sólo cuenta cuando count_enable vale '1'. 
      	count_int <= count_int + 1;
      end if;
   end if;
end process;
	internal_count_enable <= count_enable after Tsup_delay; 
	--Propagation delay
	count <= count_int after propagation_delay;
end Behavioral;


------------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    13:10:55 03/31/2014 
-- Design Name: 
-- Module Name:    mux4_1_8bits - Behavioral 
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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity mux2_1 is
generic (	
		size: natural := 32;
		mux_data_delay: time := 2 ns; -- delay of the data inputs of the 32 to 1 mux used for the read ports
		mux_ctrl_delay: time := 3 ns -- delay of the ctrl input of the 32 to 1 mux used for the read ports
    		);  
Port (   DIn0 : in  STD_LOGIC_VECTOR (size-1 downto 0);
         DIn1 : in  STD_LOGIC_VECTOR (size-1 downto 0);
		 ctrl : in  STD_LOGIC;
         Dout : out  STD_LOGIC_VECTOR (size-1 downto 0));
end mux2_1;

architecture Behavioral of mux2_1 is
signal Din0_internal, Din1_internal: STD_LOGIC_VECTOR (size-1 downto 0);
signal ctrl_internal: std_logic;
begin
	--delays
	ctrl_internal <= ctrl after mux_ctrl_delay;
	Din0_internal <= DIn0 after mux_data_delay;
	Din1_internal <= DIn1 after mux_data_delay;
	Dout <= Din1_internal when (ctrl_internal ='1') else Din0_internal;
	
end Behavioral;

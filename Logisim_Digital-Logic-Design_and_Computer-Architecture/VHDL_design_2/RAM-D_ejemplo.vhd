----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Fernando Pastor Peralta - 897113
-- 
-- Create Date:    14:12:11 04/04/2014 
-- Design Name: 
-- Module Name:    memoriaRAM - Behavioral 
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
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;


-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;
-- Memoria RAM de 128 oalabras de 32 bits
entity Data_Memory_Subsystem is port (
	CLK : in std_logic;
	reset: in std_logic;
	ADDR : in std_logic_vector (31 downto 0); --Dir solicitada por el Mips
	Din : in std_logic_vector (31 downto 0);--entrada de datos desde el Mips
	WE : in std_logic;		-- write enable	del MIPS
	RE : in std_logic;		-- read enable del MIPS	
	Mem_ready: out std_logic; -- indica si podemos hacer la operaci�n solicitada en el ciclo actual. En esta memoria vale siempre '1'.
	Dout : out std_logic_vector (31 downto 0)); --dato que se env�a al Mips
end Data_Memory_Subsystem;

architecture Behavioral of Data_Memory_Subsystem is
type RamType is array(0 to 127) of std_logic_vector(31 downto 0);

--RAM ORIGINAL
-- signal RAM : RamType := (  X"00000001", X"00000002", X"00000004", X"00000000", X"FFFFFFFF", X"FFFFFFFF", X"FFFFFFFF", X"FFFFFFFF", -- palabras 0,1,2,3,4,5,6,7
-- 									X"000000BB", X"11220044", X"FFFFFFFF", X"FFFFFFFF", X"AA995566", X"20000000", X"30008001", X"00000007", --palabras 8,9,...
-- 									X"20000000", X"20000000", X"30018001", X"02AD6093", X"30008001", X"00000001", X"20000000", X"30002001",
-- 									X"00010900", X"20000000", X"30004000", X"5000102D", X"01000300", X"80000400", X"10000000", X"00000000",
-- 									X"00002000", X"00000000", X"00000000", X"00000002", X"00000000", X"00000000", X"00000000", X"00000002",
-- 									X"00000000", X"00000002", X"00001200", X"00000000", X"00000000", X"00000002", X"00000000", X"00000002",
-- 									X"000006AD", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000",
-- 									X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00100000", X"00000000", X"00000000",
-- 									X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"02000000", X"08010000", X"00000000",
-- 									X"00000000", X"00000000", X"00000010", X"00000000", X"00000000", X"00000000", X"00000002", X"00200000",
-- 									X"00000000", X"20080001", X"00000000", X"00000000", X"00000800", X"40000000", X"00000000", X"00000000",
-- 									X"00000000", X"000009C2", X"00000000", X"00000000", X"00000800", X"00000000", X"00080800", X"00000000",
-- 									X"00080000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000",
-- 									X"00002000", X"00400000", X"00080000", X"00000000", X"00080000", X"00000000", X"50000000", X"00000001",
-- 									X"40020000", X"00000000", X"00000000", X"40000000", X"00000000", X"00000004", X"00000800", X"00800004",
-- 									X"20800000", X"00000004", X"00000000", X"00000004", X"00000000", X"00000000", X"20000000", X"00000004");

--RAM 897113.txt y 897113-R.txt
signal RAM : RamType := (  			X"00000000", X"00000004", X"00000020", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", --word 0,1,...
									X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"000C3500", X"00015F90", --word 8,9,...
									X"00001B58", X"00000064", X"0000000A", X"00000003", X"00000000", X"00000000", X"00000000", X"00000000", --word 16,...
									X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", --word 24,...
									X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", --word 32,...
									X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", --word 40,...
									X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", --word 48,...
									X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", --word 56,...
									X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", --word 64,...
									X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000",--word 72,...
									X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", --word 80,...
									X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", --word 88,...
									X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", --word 96,...
									X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", --word 104,...
									X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", --word 112,...
									X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000", X"00000000");--word 120,...
signal dir_7:  std_logic_vector(6 downto 0); 
begin
 
 dir_7 <= ADDR(8 downto 2); -- como la memoria es de 128 plalabras no usamos la direcci�n completa sino s�lo 7 bits. Como se direccionan los bytes, pero damos palabras no usamos los 2 bits menos significativos
 MEM_ready <= '1'; --En esta versi�n la memoria siempre est� preparada
 process (CLK)
    begin
        if (CLK'event and CLK = '1') then
            if (WE = '1') then -- s�lo se escribe si WE vale 1
                RAM(conv_integer(dir_7)) <= Din;
                -- la siguiente l�nea saca un aviso por pantalla con el dato que se ha escrito
                report "Simulation time : " & time'IMAGE(now) & ".  Data written: " & integer'image(to_integer(unsigned(Din))) & ", in ADDR = " & integer'image(to_integer(unsigned(ADDR)));
            end if;
        end if;
    end process;

    Dout <= RAM(conv_integer(dir_7)) when (RE='1') else "00000000000000000000000000000000"; --s�lo se lee si RE vale 1

end Behavioral;


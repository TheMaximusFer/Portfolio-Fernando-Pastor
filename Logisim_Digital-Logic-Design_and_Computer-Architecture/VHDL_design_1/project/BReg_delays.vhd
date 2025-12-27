----------------------------------------------------------------------------------
-- Company: Universidad de Zaragoza
-- Engineer: Javier Resano
-- 
-- Create Date:    11:03:23 03/07/2023 
-- Design Name: 
-- Module Name:    BR - Behavioral 
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
use ieee.numeric_std.all;
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
-- use IEEE.NUMERIC_STD.ALL;
-- librería para usar la función conv_integer
use IEEE.std_logic_unsigned.all;



entity BReg is
generic (   mux_delay: time := 7 ns; -- delay of the data inputs of the 32 to 1 mux used for the read ports
            dec_delay: time := 3 ns; -- Tsup delay due to the 5 to 32 decoder used for the write enable signal. This is the main factor for the Tsup, other factors are ignored
            register_propagation_delay: time := 5 ns --propagation delay of the registers
            );  
port (
        clk : in std_logic;
        reset : in std_logic;
        RA : in std_logic_vector (4 downto 0); --Dir para el puerto de lectura A
        RB : in std_logic_vector (4 downto 0); --Dir para el puerto de lectura B
        RW : in std_logic_vector (4 downto 0); --Dir para el puerto de escritura
        BusW : in std_logic_vector (31 downto 0);--entrada de datos para el puerto de escritura
        RegWrite : in std_logic;                        
        BusA : out std_logic_vector (31 downto 0);
        BusB : out std_logic_vector (31 downto 0)
    );
end BReg;

architecture Behavioral of BReg is
-- el banco de registros es un array de 32 registros de 32 bits
-- Usamos dos señales: 
--  reg_file_internal se usa para calcular el nuevo valor del banco de registros cuando hay un reset o una escritura; 
--  reg_file es el contenido del banco. Es lo mismo que reg_file_internal, tras el retardo de propagación de los registros
    type reg_array is array (0 to 31) of std_logic_vector(31 downto 0);
    signal RW_internal: std_logic_vector (4 downto 0);
	signal RegWrite_internal: std_logic;
    signal reg_file_internal, reg_file: reg_array;
    
    begin 
     process(clk)
        begin 
-- se escribe en flanco de subida. clk'event vale 1 cuando hay un flanco. Si ha habido un flanco y clk fuera cero sería un flanco de bajada
-- En VHDL podemos usar raising_edge() en vez de este test
            if (clk'event and clk='1') then 
                if reset='1' then   
                    for i in 0 to 31 loop
                        reg_file_internal(i) <= X"00000000";
                     end loop;
                else
                    --if RegWrite_internal is 1, write BusW data in register RW
					if RegWrite_internal = '1' then
                         reg_file_internal(conv_integer(RW_internal)) <= BusW; --forma super compacta de vhdl para hacer el decodificador y la escritura en el banco de registros
                     -- la siguiente línea saca un aviso por pantalla con el dato que se ha escrito
                         report "Simulation time : " & time'IMAGE(now) & ".  Data written: " & integer'image(to_integer(signed(BusW))) & ", in Reg = " & integer'image(to_integer(unsigned(RW)));
                    end if;
                end if;
            end if;
    end process;
    -- Delays:
    reg_file <= reg_file_internal after register_propagation_delay;
    RW_internal <= RW after dec_delay;
	RegWrite_internal <= RegWrite after dec_delay;
    --get data stored at register RA
    BusA <= reg_file(conv_integer(RA)) after mux_delay; -- esto es una forma muy rápida de hacer un Mux en vhdl
     --get data stored at register RA
    BusB <= reg_file(conv_integer(RB)) after mux_delay;
    

end Behavioral;


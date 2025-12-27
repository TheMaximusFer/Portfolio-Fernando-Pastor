----------------------------------------------------------------------------------
-- IMPORTANTE: CADA ESTUDIANTE DEBE COMPLETAR SUS DATOS 
-- Name: Fernando Pastor Peralta
-- NIA: 897113
-- Create Date: 19/2/2025   
-- Module Name:    UC_Mov_Add
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

entity UC_Mov_Add is
	generic (	propagation_delay: time := 3 ns --propagation delay of the UC; The remaining delays (We asume that the delay of the state register, and its Tsup are negligible)
    		);  
    Port ( 	clk : in  STD_LOGIC;
		   	reset : in  STD_LOGIC;
    		op_code : in  STD_LOGIC_VECTOR (1 downto 0);
			PC_ce : out  STD_LOGIC;
    		load_A : out  STD_LOGIC;
			load_B : out  STD_LOGIC;
			load_ALUout : out  STD_LOGIC;          
    		RegWr : out  STD_LOGIC;
           	MUX_ctrl : out  STD_LOGIC
		   );
end UC_Mov_Add;

architecture Behavioral of UC_Mov_Add is

    -- define constantes para mejorar la legibilidad del c�digo
    -- El valor "01 es un ejemplo. Pon los valores que t� hayas usado. 
	CONSTANT MOV_opcode   : STD_LOGIC_VECTOR(1 downto 0) := "00";
	CONSTANT ADD_opcode   : STD_LOGIC_VECTOR(1 downto 0) := "01";
	CONSTANT HALT_opcode  : STD_LOGIC_VECTOR(1 downto 0) := "11";

	-- Asignamos los nombres que queramos a los estados para mejorar la legibilidad del c�digo
	-- Los nombres que aqu� aparecen son ejemplos
	type state_type is (CargarPC, MOV, ADD_CargarAyB, ADD_CargarALUout, ADD_RegWr, HALT);
	signal state, next_state : state_type; 
	signal internal_RegWr, internal_MUX_ctrl, internal_load_A, internal_load_B, internal_load_ALUout, internal_PC_ce : STD_LOGIC; 
	begin
	
	-- State Register
	--We do not use the component register because we do not want to codify the states, but use their names instead.
	State_reg: process (clk)
	   begin
	      if (clk'event and clk = '1') then
	         if (reset = '1') then
	            state <= CargarPC;
	         else
	            state <= next_state;
	         end if;        
	      end if;
	   end process;
	
	   UC_outputs : process (state)
	   begin 
		   -- Inicialización por defecto
		   internal_PC_ce <= '0'; 
		   internal_RegWr <= '0'; 
		   internal_MUX_ctrl <= '0'; 
		   internal_load_A <= '0'; 
		   internal_load_B <= '0'; 
		   internal_load_ALUout <= '0';
		   CASE state IS
			   WHEN MOV=>  
				   internal_MUX_ctrl <= '1';
				   internal_RegWr <= '1';
				   internal_PC_ce <= '1';

			   WHEN ADD_CargarAyB =>
				   internal_load_A <= '1'; 
				   internal_load_B <= '1'; 
				
			   WHEN ADD_CargarALUout =>
				   internal_load_ALUout <= '1';
			   
				WHEN ADD_RegWr =>
			  	   internal_RegWr <= '1';
				   internal_PC_ce <= '1';

			   WHEN HALT =>
				   -- No hacer nada, todas las señales ya están en '0'
	   
			   WHEN OTHERS =>  
				   -- Estado por defecto (opcional)
				   internal_PC_ce <= '0';
				   internal_RegWr <= '0';
				   internal_MUX_ctrl <= '0';
				   internal_load_A <= '0';
				   internal_load_B <= '0';
				   internal_load_ALUout <= '0';
		   END CASE;
	   end process;
	   
	
	UC_next_state : process (state, op_code)
begin         
    CASE state IS
        WHEN CargarPC =>
            IF op_code = MOV_opcode THEN 
                next_state <= MOV;
            ELSIF op_code = ADD_opcode THEN 
                next_state <= ADD_CargarAyB;
            ELSE
                next_state <= HALT;
            END IF;

		WHEN MOV =>  
            next_state <= CargarPC;
		
        WHEN ADD_CargarAyB =>
            next_state <= ADD_CargarALUout;

        WHEN ADD_CargarALUout =>
            next_state <= ADD_RegWr;

		WHEN ADD_RegWr =>
			next_state <= CargarPC;

        WHEN HALT =>
            next_state <= HALT;

        WHEN OTHERS =>  
            next_state <= CargarPC; 
    END CASE;
end process;


	-- Delays
	RegWr <= internal_RegWr after propagation_delay;
	MUX_ctrl <= internal_MUX_ctrl after propagation_delay;
	PC_ce <= internal_PC_ce after propagation_delay;
    load_A <= internal_load_A after propagation_delay;
    load_B <= internal_load_B after propagation_delay;
	load_ALUout <= internal_load_ALUout after propagation_delay;      

end Behavioral;

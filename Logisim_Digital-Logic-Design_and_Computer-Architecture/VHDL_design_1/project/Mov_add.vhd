----------------------------------------------------------------------------------
-- IMPORTANTE: CADA ESTUDIANTE DEBE COMPLETAR SUS DATOS 
-- Name: Fernando Pastor Peralta
-- NIA: 897113
-- Create Date: 19/2/2025    
-- Module Name: Mov_Add
-- Additional Comments: 
--
----------------------------------------------------------------------------------

  LIBRARY ieee;
  USE ieee.std_logic_1164.ALL;
  USE ieee.numeric_std.ALL;

  ENTITY Mov_add IS
  	Port ( 	clk : in  STD_LOGIC;
           	reset : in  STD_LOGIC
           	);
  END Mov_add;

  ARCHITECTURE behavior OF Mov_add IS 
-------------------------------------------------------------
  -- Component Declaration
  -- BR:
	COMPONENT BReg is
	generic (	
		mux_delay: time := 7 ns; -- delay of the 32 to 1 mux used for the read ports
		dec_delay: time := 3 ns; -- delay of the 5 to 32 decoder used for thw write enable signal
		register_propagation_delay: time := 5 ps --propagation delay of the registers
    );  
	port (
        clk : in std_logic;
		reset : in std_logic;
        RA : in std_logic_vector (4 downto 0); --Dir para el puerto de lectura A
        RB : in std_logic_vector (4 downto 0); --Dir para el puerto de lectura A
        RW : in std_logic_vector (4 downto 0); --Dir para el puerto de escritura
        BusW : in std_logic_vector (31 downto 0);--entrada de datos para el puerto de escritura
        RegWrite : in std_logic;						
        BusA : out std_logic_vector (31 downto 0);
        BusB : out std_logic_vector (31 downto 0)
    );
	END COMPONENT;
	-- Register
	COMPONENT reg is
    generic (	size: natural := 32;
    			Tsup_delay: time := 1 ns;
    			propagation_delay: time := 5 ns
    		);  -- por defecto son de 32 bits, pero se puede usar cualquier tama�o
	Port ( Din : in  STD_LOGIC_VECTOR (size -1 downto 0);
           clk : in  STD_LOGIC;
		   reset : in  STD_LOGIC;
           load : in  STD_LOGIC;
           Dout : out  STD_LOGIC_VECTOR (size -1 downto 0));
    END COMPONENT;
    -- Adder
	COMPONENT adder32 is
	generic (	propagation_delay: time := 26 ns
    			);
    Port ( Din0 : in  STD_LOGIC_VECTOR (31 downto 0);
           Din1 : in  STD_LOGIC_VECTOR (31 downto 0);
           Dout : out  STD_LOGIC_VECTOR (31 downto 0));
	end component;
	-- Counter
	COMPONENT counter is
	generic (	size: natural := 32;-- por defecto son de 32 bits, pero se puede usar cualquier tama�o
	    		Tsup_delay: time := 1 ns;
				propagation_delay: time := 6 ns -- retardo de propagaci�n 1ns por defecto
	    		);  
	Port ( clk : in  STD_LOGIC;
	       reset : in  STD_LOGIC;
	       count_enable : in  STD_LOGIC;
	       count : out  STD_LOGIC_VECTOR (size-1 downto 0));
	end component;   
	--ROM
	COMPONENT memoriaROM_I generic (	propagation_delay: time := 8 ns --propagation delay of the registers
    		);  
	port (
		ADDR : in std_logic_vector (6 downto 0); --Dir 
        RE : in std_logic;		-- read enable		  
		Dout : out std_logic_vector (31 downto 0)
		);
    end component;   
    -- MUX 2 a 1
    COMPONENT mux2_1 is
	generic (	
		size: natural := 32;
		mux_data_delay: time := 2 ns; -- delay of the data inputs of the 32 to 1 mux used for the read ports
		mux_ctrl_delay: time := 3 ns -- delay of the ctrl input of the 32 to 1 mux used for the read ports
    		);  
	Port (   DIn0 : in  STD_LOGIC_VECTOR (size-1 downto 0);
         DIn1 : in  STD_LOGIC_VECTOR (size-1 downto 0);
		 ctrl : in  STD_LOGIC;
         Dout : out  STD_LOGIC_VECTOR (size-1 downto 0));
    end component;  
    
    COMPONENT UC_Mov_Add is
	generic (	propagation_delay: time := 3 ns --propagation delay of the UC
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
	end component;  
------------------------------------------------           
-- Internal signals:    
	SIGNAL RegWrite, MUX_ctrl, PC_ce, load_A, load_B, load_ALUout :  std_logic;
    SIGNAL BusA,BusB, Adder_out, BusW, Instruction, K_ext, RA_out, RB_out, ALU_out  :  std_logic_vector(31 downto 0);
    SIGNAL RA, RB, RW: std_logic_vector (4 downto 0);
    SIGNAL PC_out: std_logic_vector (6 downto 0);
------------------------------------------------       

  BEGIN

-- Component Instantiation
-- TODO: conect the modules assigning the proper signals to the open ports.
-- Example: We can connect PC_out with the ADDR of the ROM memory writing: Mem_I: memoriaROM_I PORT MAP (ADDR => PC_out,  	
	-- Contador de programa
	pc: counter 	generic map (size => 7)
					port map (clk => clk, reset => reset, count_enable => PC_ce, count => PC_out);
	-- se lee la memoria todos los ciclos
	Mem_I: memoriaROM_I PORT MAP (ADDR => PC_out, RE => '1', Dout => Instruction);
					
	UC: UC_Mov_Add PORT MAP (clk => clk, reset => reset, op_code => Instruction (31 downto 30), PC_ce => PC_ce, load_A => load_A, load_B => load_B, load_ALUout => load_ALUout, RegWr => RegWrite, MUX_ctrl => MUX_ctrl);
	
	Register_bank: BReg PORT MAP (clk => clk, reset => reset, RA => Instruction (29 downto 25), RB => Instruction (24 downto 20), RW => Instruction (19 downto 15), BusW => BusW, RegWrite => RegWrite, BusA => BusA, BusB => BusB);
	
	Reg_A: reg generic map (size => 32)
			port map (	Din => BusA, clk => clk, reset => reset, load => load_A, Dout => RA_out);
			
	Reg_B: reg generic map (size => 32)
			port map (	Din => BusB, clk => clk, reset => reset, load => load_B, Dout => RB_out);
	
	adder: adder32 port map (Din0 => RA_out, Din1 => RB_out, Dout => Adder_out);
	
	ALUout: reg generic map (size => 32)
			port map (	Din => Adder_out, clk => clk, reset => reset, load => load_ALUout, Dout => ALU_out);
	
	-- signed extension of K
	Sign_ext: mux2_1 generic map (size => 16) port map (Din0 => "0000000000000000", Din1 => "1111111111111111", ctrl => Instruction(29), Dout => K_ext(31 downto 16));
	K_ext(15 downto 6) <= Instruction(29 downto 20);
	K_ext(5 downto 0) <= Instruction(14 downto 9);
	
	mux: mux2_1 generic map (size => 32) port map (Din0 => ALU_out, Din1 => K_ext, ctrl => MUX_ctrl, Dout => BusW);
	
END;

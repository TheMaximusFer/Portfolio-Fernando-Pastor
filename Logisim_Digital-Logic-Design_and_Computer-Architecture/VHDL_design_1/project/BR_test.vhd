-- TestBench Template 

  LIBRARY ieee;
  USE ieee.std_logic_1164.ALL;
  USE ieee.numeric_std.ALL;

  ENTITY BR_testbench IS
  END BR_testbench;

  ARCHITECTURE behavior OF BR_testbench IS 

  -- Component Declaration
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

    SIGNAL clk, reset, RegWrite :  std_logic;
    SIGNAL BusA,BusB, BusW  :  std_logic_vector(31 downto 0);
    SIGNAL RA, RB, RW: std_logic_vector (4 downto 0);
          
  -- Clock period definitions
   constant CLK_period : time := 10 ns;
  BEGIN

  -- Component Instantiation
   Register_bank: BReg PORT MAP (clk => clk, reset => reset, RA => RA, RB => RB, RW => RW, BusW => BusW, RegWrite => RegWrite, BusA => BusA, BusB => BusB);

-- Clock process definitions
   CLK_process :process
   begin
		CLK <= '0';
		wait for CLK_period/2;
		CLK <= '1';
		wait for CLK_period/2;
   end process;

 stim_proc: process
   begin		  	
   		RA		<= "00000";
   		RB		<= "00000";
   		BusW	<= x"00000000";
   		RW		<= "00000";
   		reset 	<= '1';
   		RegWrite <= '0';	
    	wait for CLK_period*1;
--------Test 0---------------------------------------------------------------------------------------------------------
	  	-- Se escribe un 3 en el registro 2
    	reset <= '0';
    	RW		<= "00010";
    	BusW	<= x"00000003";
    	RegWrite <= '1';
		wait for CLK_period;
--------Test 1---------------------------------------------------------------------------------------------------------
	  	-- Se escribe un 1 en el registro 5
    	reset <= '0';
    	RW		<= "00101";
    	BusW	<= x"00000001";
    	RegWrite <= '1';
		wait for CLK_period;		
--------Test 2---------------------------------------------------------------------------------------------------------
	  	-- Se leen los registros 2 y 5 en los bancos A y B tras el retardo de propagación de los muxes
	  	-- Se cambia el valor del puerto de escritura, pero se desactiva Regwrite: no se debe escribir
    	reset <= '0';
    	RW		<= "00101";
    	BusW	<= x"EEEEEEEE";
    	RA		<= "00010";
   		RB		<= "00101";
    	RegWrite <= '0';
    	wait for CLK_period;	
--------Test 3---------------------------------------------------------------------------------------------------------
	  	-- Se leen los registros 2 y 5 en los bancos A y B
	  	-- Se escribe en el registro 5 un 7, inicialmente se debe ver el valor antiguo, y tras el retardo de propagación del registro aparecerá el nuevo
		reset <= '0';
    	RW		<= "00101";
    	BusW	<= x"00000007";
    	RA		<= "00010";
   		RB		<= "00101";
    	RegWrite <= '1';
 		wait;
   end process;

  END;

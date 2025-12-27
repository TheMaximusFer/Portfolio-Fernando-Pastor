-- TestBench Template 

  LIBRARY ieee;
  USE ieee.std_logic_1164.ALL;
  USE ieee.numeric_std.ALL;

  ENTITY Mov_add_testbench IS
  END Mov_add_testbench;

  ARCHITECTURE behavior OF Mov_add_testbench IS 

  -- Component Declaration
	COMPONENT Mov_add is
		Port ( 	clk : in  STD_LOGIC;
           	reset : in  STD_LOGIC
           	);
	END COMPONENT;

          SIGNAL clk, reset:  std_logic;
         
          
  -- Clock period definitions
   constant CLK_period : time := 100 ns;
  BEGIN

  -- Component Instantiation
   uut: Mov_add PORT MAP(clk => clk, reset => reset);

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
      	-- activamos reset
   		reset <= '1';
    	wait for CLK_period*2;
		-- desactivamos reset. Se ejecutarán las instrucciones que haya en la ROM
    	reset <= '0';
		wait;
   end process;

  END;

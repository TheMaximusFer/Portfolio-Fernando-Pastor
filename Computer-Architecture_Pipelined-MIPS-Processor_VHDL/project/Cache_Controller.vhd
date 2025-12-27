---------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    13:38:18 05/15/2014 
-- Design Name: 
-- Module Name:    UC_slave - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: la UC incluye un contador de 2 bits para llevar la cuenta de las transferencias de bloque y una m�quina de estados
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

entity UC_MC is
    Port ( 	
        clk : in  STD_LOGIC;
		reset : in  STD_LOGIC;
		
		-- MIPS instructions
		RE : in  STD_LOGIC; 
		WE : in  STD_LOGIC;
		
		-- NEW: signal for fetch_inc
		Fetch_inc: in std_logic;
		invalidate_bit: out std_logic;
		
		-- Response to MIPS
		ready : out  STD_LOGIC; -- indicates whether we can process the current MIPS instruction in this cycle. Otherwise, the MIPS will have to be stalled.
		
		-- Signals from the Cache Memory (MC)
		hit0 : in  STD_LOGIC; -- activated if there is a hit in way 0
		hit1 : in  STD_LOGIC; -- activated if there is a hit in way 1
		via_2_rpl :  in  STD_LOGIC; -- indicates which way will be replaced
		addr_non_cacheable: in STD_LOGIC; -- indicates that the address should not be stored in the cache (e.g., belongs to scratchpad)
		internal_addr: in STD_LOGIC; -- indicates that the requested address refers to an internal MC register

		MC_WE0 : out  STD_LOGIC;
        MC_WE1 : out  STD_LOGIC;
        
        -- Signals to indicate the operation to be performed on the bus
        MC_bus_Read : out  STD_LOGIC; -- to request the bus for a read access
		MC_bus_Write : out  STD_LOGIC; -- to request the bus for a write access
		MC_bus_Fetch_inc : out  STD_LOGIC; -- to request the bus for a Fetch_inc operation
		MC_tags_WE : out  STD_LOGIC; -- to write the tag into the tag memory

        palabra : out  STD_LOGIC_VECTOR (1 downto 0); -- indicates the current word within a block transfer (1st, 2nd, etc.)

        mux_origen: out STD_LOGIC; -- used to select whether the source of the address and data is the MIPS (when 0) or the controller and bus (when 1)
		block_addr : out  STD_LOGIC; -- indicates whether the address to send is a block address (rm) or a word address (w)
		mux_output: out  std_logic_vector(1 downto 0); -- to select whether to send the output from the cache (value 0), the data on the bus (value 1), or an internal register (value 2)

		-- Performance counters for the Cache Memory
		inc_m : out STD_LOGIC; -- indicates a miss occurred in cache
		inc_w : out STD_LOGIC; -- indicates a write occurred in cache
		inc_r : out STD_LOGIC; -- indicates a read occurred in cache
		inc_inv :out STD_LOGIC; -- indica que ha habido una invalidaci�n

		-- Error management
		unaligned: in STD_LOGIC; -- indicates the address requested by MIPS is not aligned
		Mem_ERROR: out std_logic; -- activated if in the last transfer the slave did not respond to its address
		load_addr_error: out std_logic; -- used to record the address that caused an error

		-- Bus transfer management
		bus_TRDY : in  STD_LOGIC; -- indicates that the memory can perform the requested operation in this cycle
		Bus_DevSel: in  STD_LOGIC; -- indicates that the memory has acknowledged the address as within its range
		Bus_grant :  in  STD_LOGIC; -- indicates bus usage has been granted
		MC_send_addr_ctrl : out  STD_LOGIC; -- commands sending the address and control signals to the bus
        MC_send_data : out  STD_LOGIC; -- commands sending data
        Frame : out  STD_LOGIC; -- indicates that the operation is not yet complete
        last_word : out  STD_LOGIC; -- indicates that this is the last data of the transfer
        Bus_req :  out  STD_LOGIC -- indicates a bus request to the arbiter
	);
end UC_MC;


architecture Behavioral of UC_MC is
 
component counter is 
	generic (
	   size : integer := 10
	);
	Port ( clk : in  STD_LOGIC;
	       reset : in  STD_LOGIC;
	       count_enable : in  STD_LOGIC;
	       count : out  STD_LOGIC_VECTOR (size-1 downto 0)
					  );
end component;		           
-- Examples of state names. Name your states with descriptive names, either one of these, or others. This makes debugging easier. Remove unused names
type state_type is (Inicio, send_addr, send_data); 
type error_type is (memory_error, No_error); 
signal state, next_state : state_type; 
signal error_state, next_error_state : error_type; 
signal last_word_block: STD_LOGIC; -- is activated when the last word of a block is being requested.
signal count_enable: STD_LOGIC; -- Increments the word counter. It is activated when a new word has been received
signal hit: std_logic;
signal palabra_UC : STD_LOGIC_VECTOR (1 downto 0);
begin

hit <= hit0 or hit1;	
 
-- The counter reports how many words have been received. It is used to know when the block transfer is finished and to generate the address of the word in which the data read from the bus is written in the MC.
word_counter: counter 	generic map (size => 2)
						port map (clk, reset, count_enable, palabra_UC); -- indicates the current word within a block transfer (1st, 2nd...)

last_word_block <= '1' when palabra_UC="11" else '0';-- is activated when we are asking for the last transfer

palabra <= palabra_UC;

   State_reg: process (clk)
   begin
      if (clk'event and clk = '1') then
         if (reset = '1') then
            state <= Inicio;
         else
            state <= next_state;
         end if;        
      end if;
   end process;
 
---------------------------------------------------------------------------
-- State machine for the error bit
---------------------------------------------------------------------------

error_reg: process (clk)
   begin
      if (clk'event and clk = '1') then
         if (reset = '1') then           
            error_state <= No_error;
        else
            error_state <= next_error_state;
         end if;   
      end if;
   end process;
   
--Mem Error output
Mem_ERROR <= '1' when (error_state = memory_error) else '0';

--Mealy State-Machine - Outputs based on state and inputs
   
   --MEALY State-Machine - Outputs based on state and inputs
   -- Important: check that the signals used as inputs are included in the sensitivity list
   OUTPUT_DECODE: process (state, error_state, RE, WE, Fetch_inc, unaligned, internal_addr, Bus_grant, hit, addr_non_cacheable, Bus_DevSel, hit0, hit1, bus_TRDY, via_2_rpl, last_word_block)
   begin
			  -- default values, if no other value is assigned in a state these are the defaut values
	MC_WE0 <= '0';
	MC_WE1 <= '0';
	MC_bus_Read <= '0';
	MC_bus_Write <= '0';
	MC_bus_Fetch_inc <= '0';
	MC_tags_WE <= '0';
    ready <= '0';
    mux_origen <= '0';
    MC_send_addr_ctrl <= '0';
    MC_send_data <= '0';
    next_state <= state;  
	count_enable <= '0';
	Frame <= '0';
	block_addr <= '0';
	inc_m <= '0';
	inc_w <= '0';
	inc_r <= '0';
	inc_inv <= '0';
	Bus_req <= '0';
	mux_output <= "00";
	last_word <= '0';
	next_error_state <= error_state; 
	load_addr_error <= '0';
	invalidate_bit <= '0';
				
        -- Inicio state          
		CASE state is 
		when Inicio => 
			If (RE = '0' and WE = '0' and Fetch_inc = '0') then -- if Mips ask for nothing, we do nothing
				next_state <= Inicio;
				ready <= '1';
			elsif ((RE = '1') or (WE = '1') or (Fetch_inc = '1')) and  (unaligned ='1') then -- if the processor wants to read an unaligned address
				-- The error is processed and the request is ignored.
				next_state <= Inicio;
				ready <= '1';
				next_error_state <= memory_error; -- last address incorrect (not aligned)
				load_addr_error <= '1';
			elsif (RE= '1' and  internal_addr ='1') then -- if Mips wants to read an MC internal register
				next_state <= Inicio;
				ready <= '1';
				mux_output <= "10"; -- The output is an internal MC record
				next_error_state <= No_error; -- When the internal register is read, the controller removes the error signal.
			elsif (((WE = '1') or (Fetch_inc = '1')) and  internal_addr ='1') then -- if you want to write or fetch_inc to the internal MC register, an error is generated because it is read-only.
				next_state <= Inicio;
				ready <= '1';
				next_error_state <= memory_error; -- Attempt to write a read-only internal register
				load_addr_error <= '1';
			elsif (Fetch_inc= '1') then -- fetch_inc 
				Bus_req <= '1';
				If (Bus_grant = '0') then -- si no nos dan el bus no hacemos nada
					next_state <= Inicio;
      			else  -- si nos dan el bus enviamos la direcci�n
					if ( hit = '1') then
						invalidate_bit <= '1';
						inc_inv <= '1';
					-- else
					-- 	if (addr_non_cacheable = '0') then
					-- 		inc_m <= '1'; -- MC miss				
					-- 	end if;
					end if;
         			next_state <= send_addr;
      			end if; 
				
			elsif (RE= '1' and  hit='1') then -- read hit
				-- ORDER MATTERS: IF THIS OPTION GOES BEFORE fetch_inc if fetch_inc and RE are activated it would be treated as RE. It depends on how the fetch_inc signals are handled, this may or may not happen.
				next_state <= Inicio;
				ready <= '1';
				inc_r <= '1'; -- MC read
				mux_output <= "00"; -- This is the default value. There is no need to set it. The output is a data stored in the MC
		
			elsif (WE= '1' and  hit='1') then -- write hit
				Bus_req <= '1';
				MC_WE0 <= hit0;
				MC_WE1 <= hit1;
				If (Bus_grant = '0') then -- si no nos dan el bus no hacemos nada
					next_state <= Inicio;
      			else  -- si nos dan el bus enviamos la direcci�n
         			next_state <= send_addr;
      			end if; 
				
			elsif (hit='0') then  -- if the MIPS asks for a well aligned memory @ and it is not a hit or access to a register
				Bus_req <= '1';
				If (Bus_grant = '0') then -- si no nos dan el bus no hacemos nada
					next_state <= Inicio;
      			else  -- si nos dan el bus enviamos la direcci�n
         			next_state <= send_addr;
      			end if; 
			end if;
			
		when send_addr =>
			if ((RE = '1' or WE = '1') and (hit = '0') and addr_non_cacheable = '0' and Fetch_inc= '0') then
				Frame <= '1';
				MC_send_addr_ctrl <= '1';
				MC_bus_Read <= '1';
				block_addr <= '1';
				
				if (Bus_DevSel = '1') then
					next_state <= send_data;
					next_error_state <= No_error;
				else
					next_state <= Inicio;
					load_addr_error <= '1';
					next_error_state <= memory_error;
					ready <= '1';
					mux_output <= "01"; -- The output is the data on the bus
				end if;
			elsif (WE = '1' and hit = '1' and addr_non_cacheable = '0' and Fetch_inc= '0') then			
				Frame <= '1';
				MC_send_addr_ctrl <= '1';
				MC_bus_Write <= '1';
				if (Bus_DevSel = '1') then
					next_state <= send_data;
				else
					next_state <= Inicio;
					load_addr_error <= '1';
					next_error_state <= memory_error;
					ready <= '1';
					mux_output <= "01"; -- The output is the data on the bus
				end if;
				
			elsif (addr_non_cacheable = '1') then --el orden importa, si se hace fetch_in en addr_non_cacheable=1 entra aquí y no en el if de fetch_inc
				Frame <= '1';
				MC_send_addr_ctrl <= '1';	
				if(RE = '1') then 
					MC_bus_Read <= '1';
					if (Bus_DevSel = '1') then
						next_state <= send_data;
						next_error_state <= No_error;
					else
						load_addr_error <= '1';
						next_state <= Inicio;
						next_error_state <= memory_error;
						ready <= '1';
						mux_output <= "01"; -- The output is the data on the bus
						load_addr_error <= '1';

						
					end if;	
				elsif (WE = '1') then
					MC_bus_Write <= '1';
					if (Bus_DevSel = '1') then
						next_state <= send_data;
						next_error_state <= No_error;
					else
						load_addr_error <= '1';
						next_state <= Inicio;
						next_error_state <= memory_error;
						ready <= '1';
						mux_output <= "01"; -- The output is the data on the bus
						
					end if;	
				end if;
			elsif (Fetch_inc= '1') then											
				Frame <= '1';
				MC_send_addr_ctrl <= '1';
				MC_bus_Fetch_inc <= '1';	
				if (Bus_DevSel = '1') then
					next_state <= send_data;
					next_error_state <= No_error;
				else
					load_addr_error <= '1';
					next_state <= Inicio;
					next_error_state <= memory_error;
					ready <= '1';
					mux_output <= "01"; -- The output is the data on the bus
				end if;
				
			end if;

				
		when send_data =>
			
			if ((RE = '1' or WE = '1') and (hit = '0') and addr_non_cacheable = '0' and Fetch_inc= '0') then
				if (bus_TRDY = '0') then
					next_state <= send_data;
					Frame <= '1';
					MC_bus_Read <= '1';
				elsif((bus_TRDY = '1') and (last_word_block = '0') and (via_2_rpl = '0')) then
					next_state <= send_data;
					Frame <= '1';
					MC_bus_Read <= '1';
					count_enable <= '1';
					mux_origen <= '1';
					MC_WE0 <= '1';
				elsif((bus_TRDY = '1') and (last_word_block = '0') and (via_2_rpl = '1')) then	
					next_state <= send_data;
					Frame <= '1';
					MC_bus_Read <= '1';
					count_enable <= '1';
					mux_origen <= '1';
					MC_WE1 <= '1';
				elsif((bus_TRDY = '1') and (last_word_block = '1') and (via_2_rpl = '0')) then
					next_state <= Inicio;
					Frame <= '1';
					MC_bus_Read <= '1';
					last_word <= '1';
					mux_origen <= '1';
					MC_WE0 <= '1';
					MC_tags_WE <= '1';
					count_enable <= '1';
					inc_m <= '1'; -- MC miss	
				elsif((bus_TRDY = '1') and (last_word_block = '1') and (via_2_rpl = '1')) then
					next_state <= Inicio;
					Frame <= '1';
					MC_bus_Read <= '1';
					last_word <= '1';
					mux_origen <= '1';
					MC_WE1 <= '1';
					MC_tags_WE <= '1';
					count_enable <= '1';
					inc_m <= '1'; -- MC miss
				end if;

			elsif (WE = '1' and hit = '1' and addr_non_cacheable = '0' and Fetch_inc= '0') then
				Frame <= '1';
				MC_send_data <= '1';
				MC_bus_Write <= '1';
				if (bus_TRDY = '1') then
					last_word <= '1';
					inc_w <= '1'; -- MC write
					ready <= '1';
					next_state <= Inicio;
				else
					next_state <= send_data; -- Espera hasta que bus_TRDY = '1'
				end if;

			elsif (addr_non_cacheable = '1') then --el orden importa, si se hace fetch_in en addr_non_cacheable=1 entra aquí y no en el if de fetch_inc
				Frame <= '1';
				if(RE = '1') then
					MC_bus_Read <= '1';
					if (bus_TRDY = '1') then
						mux_output <= "01"; -- Selecciona dato del bus
						last_word <= '1';
						ready <= '1';       -- Dato válido
						next_state <= Inicio;
						--inc_r <= '1';       -- Contador de lectura
					else
						next_state <= send_data; -- Espera hasta bus_TRDY = '1'
					end if;
				elsif (WE = '1') then
					MC_bus_Write <= '1';
					MC_send_data <= '1';
					if (bus_TRDY = '1') then
						last_word <= '1';
						ready <= '1';        -- Operación completada
						--inc_w <= '1';        -- Incrementa contador de escrituras
						next_state <= Inicio;
					else
						next_state <= send_data; -- Espera hasta bus_TRDY = '1'
					end if;
				end if;
			
			elsif (Fetch_inc= '1') then	
				Frame <= '1';
				MC_bus_Fetch_inc <= '1';
				if (bus_TRDY = '1') then
					last_word <= '1';
					mux_output <= "01";
					ready <= '1';
					--inc_r <= '1';       -- Contador de lectura
					next_state <= Inicio;
				else
					next_state <= send_data; -- Espera hasta bus_TRDY = '1'
				end if;
			end if;
		
	WHEN others => 	
	end CASE;
		
   end process;
 
   
end Behavioral;

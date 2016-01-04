library IEEE;
use IEEE.STD_LOGIC_1164.all;
use ieee.numeric_std.all;

entity adc is
	port (
		CLOCK_50 : in std_logic;
		RESETN : in std_logic;
		ADDR : in std_logic_vector(1 downto 0);
		RD_EN : in std_logic;
		WR_EN : in std_logic;
		READDATA : out std_logic_vector(7 downto 0);
		WRITEDATA : in std_logic_vector(7 downto 0);
		GPIO_1 : INOUT std_logic_vector(18 downto 0)
		);
	end adc;
	
architecture behaviour of adc is 

	signal test : std_logic;
	signal state : integer := 1;
	signal channel : std_logic_vector(2 downto 0) := "000";
	signal EOC_flag : std_logic;
	signal cas : std_logic := '0';
	signal count : integer;
	signal newclock : std_LOGIC;
	signal slownewclock : std_LOGIC;
			
	component clock 
	PORT(
        clock_in   : in STD_LOGIC;
        clock_out     : out STD_LOGIC		
	);
	END COMPOnent;

	component slowclock 
	PORT(
        clock_in   : in STD_LOGIC;
        clock_out     : out STD_LOGIC
		
	);
	END COMPOnent;
		
begin

	adc_clock: clock -- this is a component for the clock that is sent to the ADC
	port map (CLOCK_50, newclock);
	
	adcslowclock: slowclock -- this component is the delay to make sure the ADC reads the signals properly before going to next state
	port map(CLOCK_50, slownewclock);

	GPIO_1(18) <= newclock; -- this is a 620hz(?) that is sent to the ADC
	
	process(CLOCK_50, newclock, slownewclock)
		begin

			if(rising_edge(slownewclock)) then -- delay
				
				if(state = 1) then       -- select which channel to use
					GPIO_1(12 downto 10) <= channel;
					
					GPIO_1(14) <= '0'; -- ALE
					GPIO_1(15) <= '0'; -- output enable
					GPIO_1(16) <= '0'; -- start
					
					state <= 2;
				
				elsif(state = 2) then   -- comfirm channel with ALE

					GPIO_1(12 downto 10) <= channel;
					GPIO_1(14) <= '1'; -- ALE
					GPIO_1(15) <= '0'; -- output enable
					GPIO_1(16) <= '0'; -- start
					
					state <= 3;
					
				elsif(state = 3) then   -- start conversion
					
					GPIO_1(14) <= '1'; -- ALE
					GPIO_1(15) <= '0'; -- output enable
					GPIO_1(16) <= '1'; -- start
					
					state <= 4;
					
				elsif(state = 4) then  -- waiting for conversion to finish
					
					GPIO_1(14) <= '0'; -- ALE
					GPIO_1(15) <= '0'; -- output enable
					GPIO_1(16) <= '1'; -- start
					
					EOC_flag <= GPIO_1(17); -- this is the finish flag from the ADC
					
					if(EOC_flag = '0') then -- when finish flag is activelow (then it means its done)
					
						GPIO_1(14) <= '0'; -- ALE
						GPIO_1(15) <= '1'; -- output enable
						GPIO_1(16) <= '0'; -- start
						
						state <= 5;
					elsif(EOC_flag = '1') then -- still waiting for ADC to finish conversion
						GPIO_1(14) <= '0'; -- ALE
						GPIO_1(15) <= '0'; -- output enable
						GPIO_1(16) <= '0'; -- start
					end if;	
					
				elsif(state = 5) then   -- reading from the output pins from the ADC here
					
					GPIO_1(14) <= '0'; -- ALE
					GPIO_1(15) <= '1'; -- output enable
					GPIO_1(16) <= '0'; -- start
				
					-- the GPIO_1 are the outputs form the ADC, ADC_output(0) is MOST SIGNIFICANT BIT and ADC_output(7) is LEAST SIGNIFICANT BIT
					READDATA(0) <= GPIO_1(7);
					READDATA(1) <= GPIO_1(6);
					READDATA(2) <= GPIO_1(5);
					READDATA(3) <= GPIO_1(4);
					READDATA(4) <= GPIO_1(3);
					READDATA(5) <= GPIO_1(2);
					READDATA(6) <= GPIO_1(1);
					READDATA(7) <= GPIO_1(0);
					
					
					state <= 1;
					
--				elsif(state == 6) then   -- this state will be the other 2 sliders
--					
--					GPIO_1(14) <= '0'; -- ALE
--					GPIO_1(15) <= '0'; -- output enable
--					GPIO_1(16) <= '0'; -- start
					else 
					state <= 1;
					
				end if;

			end if;

	end process;

end behaviour;
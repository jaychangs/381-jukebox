library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity slowclock is
    port(
        clock_in   : in STD_LOGIC; -- 50 Mhz
        clock_out     : out STD_LOGIC
    );
end slowclock;

architecture Behavioral of slowclock is
    -- prescaler should be (clock_speed/desired_clock_speed)/2 because you want a rising edge every period
    signal prescaler: integer := 800;
    signal prescaler_counter: integer := 0;
    signal new_clock : std_logic := '0';
begin

    countClock: process(clock_in)
	 
    begin
        if rising_edge(clock_in) then
            prescaler_counter <= prescaler_counter + 1;
            if(prescaler_counter > prescaler) then
                -- Iterate
					 
                new_clock <= not new_clock;

                prescaler_counter <= 0;
            end if;
			clock_out <= new_clock;
        end if;
    end process;


end Behavioral;
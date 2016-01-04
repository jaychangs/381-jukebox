LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_arith.all;
USE ieee.std_logic_unsigned.all;

ENTITY DE2 IS
	PORT (
		SW : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
		KEY : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
		CLOCK_50 : IN STD_LOGIC;
		DRAM_CLK, DRAM_CKE : OUT STD_LOGIC;
		DRAM_ADDR : OUT STD_LOGIC_VECTOR(11 DOWNTO 0);
		DRAM_BA_0, DRAM_BA_1 : BUFFER STD_LOGIC;
		DRAM_CS_N, DRAM_CAS_N, DRAM_RAS_N, DRAM_WE_N : OUT STD_LOGIC;
		DRAM_DQ : INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
		DRAM_UDQM, DRAM_LDQM : BUFFER STD_LOGIC;
		GPIO_0 : INOUT STD_LOGIC_VECTOR(19 DOWNTO 0);
		GPIO_1 : INOUT STD_LOGIC_VECTOR(18 DOWNTO 0)
		);
END DE2;

ARCHITECTURE Structure OF DE2 IS
	COMPONENT nios_i2c
	PORT (
		clk_clk : IN STD_LOGIC;
		reset_reset_n : IN STD_LOGIC;
		sdram_clk_clk : OUT STD_LOGIC;
		sdram_wire_addr : OUT STD_LOGIC_VECTOR(11 DOWNTO 0);
		sdram_wire_ba : BUFFER STD_LOGIC_VECTOR(1 DOWNTO 0);
		sdram_wire_cas_n : OUT STD_LOGIC;
		sdram_wire_cke : OUT STD_LOGIC;
		sdram_wire_cs_n : OUT STD_LOGIC;
		sdram_wire_dq : INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
		sdram_wire_dqm : BUFFER STD_LOGIC_VECTOR(1 DOWNTO 0);
		sdram_wire_ras_n : OUT STD_LOGIC;
		sdram_wire_we_n : OUT STD_LOGIC;
		buffer_out_export : OUT   std_logic_vector(7 downto 0);                     -- export
      write_flag_export : OUT   std_logic;                                        -- export
      read_flag_export  : IN    std_logic;
		buffer_in_export  : IN    std_logic_vector(7 downto 0);
		push_buttons_export : in    std_logic_vector(3 downto 0);
		i2c_export_scl_pad_io : inout std_logic;                                  -- scl_pad_io
      i2c_export_sda_pad_io : inout std_logic;                                 -- sda_pad_io
		gpio_export           : inout std_logic_vector(18 downto 0)
		);
	END COMPONENT;
	
SIGNAL DQM : STD_LOGIC_VECTOR(1 DOWNTO 0);
SIGNAL BA : STD_LOGIC_VECTOR(1 DOWNTO 0);

BEGIN

		DRAM_BA_0 <= BA(0);
		DRAM_BA_1 <= BA(1);
		DRAM_UDQM <= DQM(1);
		DRAM_LDQM <= DQM(0);

	NiosII: nios_i2c
	PORT MAP (
		clk_clk => CLOCK_50,
		reset_reset_n => KEY(0),
		sdram_clk_clk => DRAM_CLK,
		sdram_wire_addr => DRAM_ADDR,
		sdram_wire_ba => BA,
		sdram_wire_cas_n => DRAM_CAS_N,
		sdram_wire_cke => DRAM_CKE,
		sdram_wire_cs_n => DRAM_CS_N,
		sdram_wire_dq => DRAM_DQ,
		sdram_wire_dqm => DQM,
		sdram_wire_ras_n => DRAM_RAS_N,
		sdram_wire_we_n => DRAM_WE_N,
      buffer_out_export =>  GPIO_0(9 DOWNTO 2),
      write_flag_export =>  GPIO_0(0),
      read_flag_export  =>  GPIO_0(1),
		buffer_in_export  =>  GPIO_0(19 DOWNTO 12),
		push_buttons_export => KEY(3 DOWNTO 0),
		i2c_export_scl_pad_io => GPIO_0(10), --   i2c_export.scl_pad_io
      i2c_export_sda_pad_io => GPIO_0(11),  --             .sda_pad_io
		gpio_export => GPIO_1(18 downto 0)
		);
END Structure;
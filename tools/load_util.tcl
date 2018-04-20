proc reset { } {
	puts -nonewline "Reseting"

	targets -set -nocase -filter {name =~ "*PSU*"}
	stop
	rst -system
	after 2000
	puts -nonewline "."

	targets -set -nocase -filter {name =~ "*PMU*"}
	stop
	rst -system
	after 2000
	puts -nonewline "."

	targets -set -nocase -filter {name =~ "*PSU*"}
	stop
	rst -system
	after 2000
	puts -nonewline "."

	# Works ^^^^

	# 0001_1100_000
	# 00_011_100_000
	# mask_write 0xFFCA0038 0x1C0 0x1C0
	# This ^ looks wrong, try vvv
	#1_1111_111
	#0xFFCA0038 - jtag_sec (CSU) Register Description
	mwr 0xFFCA0038 0x1ff
	puts ". done"
}

proc run_pmufw { bin_dir pmufw_elf } {
	targets -set -nocase -filter {name =~ "*MicroBlaze PMU*"}
	puts stderr "INFO: Downloading ELF file to the target."
	dow $pmufw_elf
	after 2000
	con
	puts "PMUFW running on PMU..."
	# Works ^^^^
}

proc run_fsbl { bin_dir psu_init_tcl zynqmp_fsbl_elf } {
}

proc turn_on_switch { } {
	puts -nonewline "Turning on switch... "

	mwr 0xff0a0244 0x1000000
	mwr 0xff0a0244 0x0
	mwr 0xff0a0244 0x1000000
	mwr 0xff0a0248 0x1000000
	mwr 0xff0a0248 0x0
	mwr 0xff0a0248 0x1000000
	mwr 0xff0a0044 0x03000fff
	mwr 0xff0a0044 0x02000fff
	mwr 0xff0a0044 0x03000fff

	# Read network_config for GEM1
	set network_config [mrd 0xFF0C0004]
	puts stderr ">>>>>>>>> network_config is: $network_config"

	mwr 0xFF0C0004 0x013F2482

	# Read network_config for GEM1
	set network_config [mrd 0xFF0C0004]
	puts stderr ">>>>>>>>> network_config is: $network_config"

	puts "switch on"
}

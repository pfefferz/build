set bin_dir [file join $env(PETALINUX_BUILD_OUT)]

connect -url "$env(TARGET)"

targets -set -nocase -filter {name =~ "*PSU*"}
stop
rst -system
after 2000
targets -set -nocase -filter {name =~ "*PMU*"}
stop
rst -system
after 2000
targets -set -nocase -filter {name =~ "*PSU*"}
stop
rst -system
after 2000
# Works ^^^^

# 0001_1100_000
# 00_011_100_000
# mask_write 0xFFCA0038 0x1C0 0x1C0
# This ^ looks wrong, try vvv
#1_1111_111
#0xFFCA0038 - jtag_sec (CSU) Register Description
mwr 0xFFCA0038 0x1ff
targets -set -nocase -filter {name =~ "*MicroBlaze PMU*"}
puts stderr "INFO: Downloading ELF file to the target."
dow [file join $bin_dir "pmufw.elf"]
after 2000
con
# Works ^^^^

# at this point the PMU is running
targets -set -nocase -filter {name =~ "*APU*"}
#0xffff0000
#Write 0x14000000 to what looks like the reset vector 
mwr 0xffff0000 0x14000000
#0xFD1A0104 - CRF_APB - Software Controlled APU MPCore Resets
# 0x0101_0000_0001
# 0x0_1_0_1_0_0_0_0_0_0_0_1
# acpu0_reset - APU core0 system reset
# apu_l2_reset - L2 Cache reset 
# acpu0_pwron_reset - APU core0 POR reset.
#mask_write 0xFD1A0104 0x501 0x0
mwr 0xFD1A0104 0x501
after 2000
mwr 0xFD1A0104 0x0
after 2000
targets -set -nocase -filter {name =~ "*A53*#0"}
source [file join $bin_dir "psu_init.tcl"]
# After this ^^^ DDR timings have been set up

puts stderr "INFO: Downloading ELF file to the target."
dow [file join $bin_dir "zynqmp_fsbl.elf"]
after 2000
con
# At this point, fsbl is running

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

after 4000; stop; catch {stop}; psu_ps_pl_isolation_removal; psu_ps_pl_reset_config
targets -set -nocase -filter {name =~ "*A53*#0"}
dow -data [file join $bin_dir "Image"] 0x00080000
after 2000
targets -set -nocase -filter {name =~ "*A53*#0"}
dow -data [file join $bin_dir "system.dtb"] 0x1407f000
after 2000
#Works

targets -set -nocase -filter {name =~ "*A53*#0"}
puts stderr "INFO: Downloading ELF file to the target."
dow [file join $bin_dir "linux-boot.elf"]
after 2000
#Works

targets -set -nocase -filter {name =~ "*A53*#0"}
puts stderr "INFO: Downloading ELF file to the target."
dow [file join $bin_dir "bl31.elf"]
after 2000
#works
con

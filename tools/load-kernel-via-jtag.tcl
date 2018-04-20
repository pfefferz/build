set bin_dir [file join $env(PETALINUX_BUILD_OUT)]

set pmufw_elf [file join $bin_dir "pmufw.elf"]
set psu_init_tcl [file join $bin_dir "psu_init.tcl"]
set zynqmp_fsbl_elf [file join $bin_dir "zynqmp_fsbl.elf"]
set Image [file join $bin_dir "Image"]
set system_dtb [file join $bin_dir "system.dtb"]
set linux_boot_elf [file join $bin_dir "linux-boot.elf"]
set bl31_elf [file join $bin_dir "bl31.elf"]

# Uncomment to test handling
#if {![file exist $no_file]} {puts "$no_file doesn't exist. Exiting"}

if {![file exist $pmufw_elf]} {puts "$pmufw_elf doesn't exist. Exiting"}
if {![file exist $psu_init_tcl]} {puts "$psu_init_tcl doesn't exist. Exiting"}
if {![file exist $zynqmp_fsbl_elf]} {puts "$zynqmp_fsbl_elf doesn't exist. Exiting"}
if {![file exist $Image]} {puts "$Image doesn't exist. Exiting"}
if {![file exist $system_dtb]} {puts "$system_dtb doesn't exist. Exiting"}
if {![file exist $linux_boot_elf]} {puts "$linux_boot_elf doesn't exist. Exiting"}
if {![file exist $bl31_elf]} {puts "$bl31_elf doesn't exist. Exiting"}

puts -nonewline "Connecting to $env(TARGET)... "
connect -url "$env(TARGET)"
puts "connected"

source [file join $env(PETALINUX_BUILD_TOOLS) load_util.tcl]
reset

run_pmufw $bin_dir $pmufw_elf



# trying to put this block into run_fsbl
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

puts "sourcing $psu_init_tcl"
source $psu_init_tcl
# After this ^^^ DDR timings have been set up

puts stderr "INFO: Downloading ELF file to the target."
dow $zynqmp_fsbl_elf
after 2000
con
# At this point, fsbl is running
puts "FSBL running on A53 core..."

after 4000; stop; catch {stop}; psu_ps_pl_isolation_removal; psu_ps_pl_reset_config



turn_on_switch

targets -set -nocase -filter {name =~ "*A53*#0"}
dow -data $Image 0x00080000
after 2000

targets -set -nocase -filter {name =~ "*A53*#0"}
dow -data $system_dtb 0x1407f000
after 2000
#Works

targets -set -nocase -filter {name =~ "*A53*#0"}
puts stderr "INFO: Downloading ELF file to the target."
dow $linux_boot_elf
after 2000
#Works

targets -set -nocase -filter {name =~ "*A53*#0"}
puts stderr "INFO: Downloading ELF file to the target."
dow $bl31_elf
after 2000
#works
con

puts "System loaded"
return "Success"

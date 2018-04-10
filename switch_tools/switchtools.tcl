# switchtools.tcl
#
# The MIT License (MIT)
#
# Copyright(c) 2018 Centennial Software Solutions LLC.
#
# inquiries@centennialsoftwaresolutions.com
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


# Notes
#
# The read/write functions are meant to be called in Xilinx's xsct.
# You _can_ call them in tclsh by using the "show" versions.
#
# References
#
# Zynq UltraScale+ MPSoC Register Reference
# https://www.xilinx.com/html_docs/registers/ug1087/ug1087-zynq-ultrascale-registers.html
#
# MIT License text from:
# https://en.wikipedia.org/w/index.php?title=MIT_License&oldid=833561742


# Read or write the Marvell 88E6321 switch on the MDIO line of
# Zynq UltraScale+ MPSoC GEM1
#
# Arguments
#
# do_rw:
#         "yes" actually do the read or the write using mwr and mrd
#         If anything else, then don't do anything
#
# phy_addr_arg_hex:
#         The address of the phy device address in hex
#
# reg_arg_hex:
#         The register of the phy device address to read or write
#
# data_arg_hex:
#         The data to be written in hex
#
#
# Return Value
#
# The value read from the last 16 bits of the MDIO register:
#         phy_write_read_data
#
proc r_or_w {do_rw op phy_addr_arg reg_arg data_arg} {

	#CLAUSE_22
	#0b0100_
	#0x40000000

	set CLAUSE_22 0x40000000

	if {$op == "read"} {

		#READ_OP
		#0b0010_
		#0x20000000

		set OP 0x20000000

	} else { ;# "write"

		#WRITE_OP
		#0x0001_
		#0x10000000

		set OP 0x10000000
	}

	#PHY_ADDR
	#(x & 0x1F) << 23

	set PHY_MASK 0x1F
	set  phy_addr [expr [expr $phy_addr_arg & $PHY_MASK] << 23]

	#REG_ADDR
	#(x & 0x1F) << 18

	set REG_MASK 0x1F
	set reg [expr [expr $reg_arg & $REG_MASK] << 18]

	#WRITE10
	#0b10_0000000000000000
	#0x20000

	set WRITE10 0x20000

	#DATA
	#d & 0x0000FFFF

	set DATA_MASK 0xFFFF
	set data [expr $data_arg & $DATA_MASK]

	set to_write $CLAUSE_22
	set to_write [expr $to_write | $OP]
	set to_write [expr $to_write | $phy_addr]
	set to_write [expr $to_write | $reg]
	set to_write [expr $to_write | $WRITE10]
	set to_write [expr $to_write | $data]

	#puts $to_write

	set val 0x[format %04X $to_write]
        # puts "mwr 0xFF0C0034 $val"
        # puts "mrd 0xFF0C0008"
        # puts "mrd 0xFF0C0034"

	if {$do_rw == "yes"} {
		mwr 0xFF0C0034 $val
		mrd 0xFF0C0008
		set ret [mrd 0xFF0C0034]
		set split_ret [regexp -all -inline {\S+} $ret]
		set data_str [lindex $split_ret 1]
		scan $data_str %x data
		set retval 0x[format %04X [expr $data & $DATA_MASK]]
		return $retval
	} else {
		return 0
	}
}

proc rs {phy_addr_arg reg_arg} {
	set retval [r_or_w "yes" "read" $phy_addr_arg $reg_arg 0]
        # puts "read $retval"
	return $retval
}

proc ws {phy_addr_arg reg_arg data_arg} {
	set retval [r_or_w "yes" "write" $phy_addr_arg $reg_arg $data_arg]
        # puts "wrote $retval"
}

# Show the commands that would be executed to read the switch, but don't execute them
proc showrs {phy_addr_arg reg_arg} {
	r_or_w "no" "read" $phy_addr_arg $reg_arg 0
}

# Show the commands that would be executed to write the switch, but don't execute them
proc showws {phy_addr_arg reg_arg data_arg} {
	r_or_w "no" "write" $phy_addr_arg $reg_arg $data_arg
}

# Dump all the registers in the Marvell switch
#
# Arguments
#
# action:
#         "show" just show what would happen with dummy register reads
#          anything else, dump the registers
#
proc dump {{action "do"}} {
	foreach port_reg {0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x1B 0x1C 0x1D} {
		dump_port $port_reg $action
	}
}

# Dump all the registers of a port in the Marvell switch
#
# Arguments
#
# port_reg:
#         0x10, 0x11, etc...
#
# action:
#         "show" just show what would happen with dummy register reads
#          anything else, dump the registers
#
proc dump_port {port_reg {action "do"}} {
	puts "Device $port_reg Registers:"
	for {set i 0 } {$i <= 0x1F} {incr i 1} {
		if {$action == "show"} {
			puts "[format "%#0.2x" $i] -> [format "%#0.4x" 0x0000]"
		} elseif {$action == "do" } {
			set readval [r_or_w "yes" "read" $port_reg $i 0]
			puts "[format "%#0.2x" $i] -> [format "%#0.4x" $readval]"
		}
	}
}

proc reset_switch { } {

	# Make the GPIO pin that will reset the chip an output
	# Configure Direction mode (GPIO Bank1, MIO),  MIO[51:26]
	# DIRECTION_1: 25:0: 0: input, 1: output
	# 0x1000000
	# 0001_0000_0000_0000_0000_0000_0000
	# bit 24 is set
	# MIO 50

	mwr 0xff0a0244 0x1000000
	#mwr 0xff0a0244 0x0
	#mwr 0xff0a0244 0x1000000

	# Enable output

	mwr 0xff0a0248 0x1000000
	#mwr 0xff0a0248 0x0
	#mwr 0xff0a0248 0x1000000

	# Send a reset, reset is active low, this sends a high, then low then
	# high

	mwr 0xff0a0044 0x03000fff
	mwr 0xff0a0044 0x02000fff
	mwr 0xff0a0044 0x03000fff
}

# Check that we can read the switch
# Do this by reading the Switch Identifier Register 0x3 of the first port 0x10
# We should read 0x3102
# Product Num 0x310
# Rev 0x2
#
# Return Value
#
# "okay" if the Switch Identifier Register returned 0x3102
# "error" if it didn't
#
proc check_switch { } {
	set regval [rs 0x10 0x3]
	puts $regval
	if {$regval == 0x3102} {
		puts "Switch okay, could read Product Num and Rev"
		return "okay"
	} else {
		puts "ERROR: Switch not okay, could not read Product Num and Rev. Read $regval."
		return "error"
	}

}

# Set the RGMII timing for port 2,5 and 6
#
# Note: function should force the link down when it does this, but it doesn't
#
# Return Value
# If error
# If the Rx and Tx timing bits are not set return "error"
# If okay return "okay"
proc set_rgmii_timing_for_port_2_5_6 { } {

	set RGMII_Rx_Timing 0x8000
	set RGMII_Tx_Timing 0x4000

	foreach port_reg {0x12 0x15 0x16} {
		set read_reg [expr [rs $port_reg 0x1]]
		# puts [format %04X $read_reg]
		set val_to_write [expr $read_reg | $RGMII_Tx_Timing | $RGMII_Rx_Timing]
		# puts [format %04X $val_to_write]
		ws $port_reg 0x1 $val_to_write
		set read_reg [expr [rs $port_reg 0x1]]
		if {[expr $read_reg & [expr $RGMII_Tx_Timing | $RGMII_Rx_Timing]]} {
			set port [expr $port_reg & 0xF]
			puts "RGMII Tx and Rx Timing set for port $port"
		} else {
			return "error"
		}
	}
	return "okay"
}

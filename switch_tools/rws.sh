# rws.sh
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


# Read or write the Marvell 88E6321 switch on the MDIO line of
# Zynq UltraScale+ MPSoC GEM1 using devmem
#
# To use
# source rws.sh
#
# To write the chip
# w 0x10 0x1 0x0000
#
# To read the chip
# r 0x10 0x2
#
# To show the commands that would be executed to write the chip
# showw 0x10 0x1 0x0000
#
# To show the commands that would be executed to read the chip
# showr 0x10 0x2
#

# 1st arg: "yes" to actually execute the devmem
# 2nd arg: 2 is read, 1 is write
# 3rd arg: phy_address
# 4th arg: register_address
# 5th arg: data
function o {
        # set up all the fields to write the MDIO value
        write0_m=0x1;
        write0_s=31;
        write0=$(((0 & $write0_m) << $write0_s));
        write1_m=0x1;
        write1_s=30;
        CLAUSE_22=1;
        write1=$(((CLAUSE_22 & $write1_m) << $write1_s));
        operation_m=0x3;
        operation_s=28;
        operation=$((($2 & operation_m) << operation_s));
        phy_address_m=0x1f;
        phy_address_s=23;
        phy_address=$((($3 & phy_address_m) << phy_address_s));
        register_address_m=0x1f;
        register_address_s=18;
        register_address=$((($4 & register_address_m) << register_address_s));
        write10_m=0x3;
        write10_s=16;
        write10=$(((2 & write10_m) << write10_s));
        phy_write_read_data_m=0xffff;
        phy_write_read_data_s=0;
        phy_write_read_data=$((($5 & phy_write_read_data_m) << phy_write_read_data_s));
        val=$(($write0 | $write1 | $operation | $phy_address | $register_address | $write10 | $phy_write_read_data));

        # Some debug prints
        #printf '0x%x\n' $write0
        #printf '0x%x\n' $write1
        #printf '0x%x\n' $operation
        #printf '0x%x\n' $phy_address
        #printf '0x%x\n' $register_address
        #printf '0x%x\n' $write10
        #printf '0x%x\n' $phy_write_read_data
        #printf '0x%x\n' $val

        if [ "$1" == "yes" ]
        then
                devmem 0xFF0C0034 32 $val;

                # need to read the man_done field of the network_status reg
                # it'll be 1 when the transaction is complete
                # normally its one on the first read
                man_done=0;
                while [ $man_done -eq 0 ]; do
                        man_done_m=0x0004;
                        man_done_s=2;
                        reg_read=$(devmem 0xFF0C0008);
                        man_done=$(((reg_read & man_done_m) >> man_done_s));
                done

                val=$(devmem 0xFF0C0034);
        else
                printf "devmem 0xFF0C0034 32 0x%x\n" $val;
                printf "devmem 0xFF0C0008 #waiting for bit 2 to be one\n";
                printf "devmem 0xFF0C0034\n";

                val=0;
        fi
        printf '0x%04x' $(($val & 0xFFFF));
}

# Read a register on the switch chip
#
# 1st arg: phy_address
# 2nd arg: register_address
function r {
        val=$(o "yes" 2 $1 $2 0);
        echo $val;
}

# Write a register on the switch chip
#
# 1st arg: phy_address
# 2nd arg: register_address
# 3rd arg: data
function w {
        val=$(o "yes" 1 $1 $2 $3);
        echo $val;
}

# Show the devmem transactions that would happen to read a register
# on the switch chip
#
# 1st arg: phy_address
# 2nd arg: register_address
function showr {
        o "no" 2 $1 $2 0;
}

# Show the devmem transactions that would happen to write a register
# on the switch chip
#
# 1st arg: phy_address
# 2nd arg: register_address
# 3rd arg: data
function showw {
        o "no" 1 $1 $2 $3;
}

# Show the commands that would be used to dump the device registers
# 
# 1st arg: SMI Device Address
function show_dump_dev {
        printf 'Device 0x%02x Registers:\n' $1
        for reg in `seq 0 0x1F`;
        do
                showr $1 $reg;
        done
}

# Show the commands to dump all the registers in the chip 
# 
# No arguments
function show_dump_all {
        for dev in 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x1B 0x1C 0x1D;
        do
                echo $dev;
                show_dump_dev $dev;
        done
}

# Show the commands that would be used to dump the device
# 
# 1st arg: SMI Device Address
function dump_dev {
        printf 'Device 0x%02x Registers:\n' $1
        for reg in `seq 0 0x1F`;
        do
                val=$(r $1 $reg);
                printf '0x%02x -> 0x%04x\n' $reg $(($val & 0xFFFF));
        done
}

# Dump all the registers 
# 
# No arguments
function dump_all {
        for dev in 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x1B 0x1C 0x1D;
        do
                dump_dev $dev;
        done
}

# Reset the switch
#
# No arguments
function reset_switch {

        # Make the GPIO pin that will reset the chip an output
        # Configure Direction mode (GPIO Bank1, MIO),  MIO[51:26]
        # DIRECTION_1: 25:0: 0: input, 1: output
        # 0x1000000
        # 0001_0000_0000_0000_0000_0000_0000
        # bit 24 is set
        # MIO 50

        devmem 0xff0a0244 32 0x1000000;
        #devmem 0xff0a0244 32 0x0;
        #devmem 0xff0a0244 32 0x1000000;

        # Enable output

        devmem 0xff0a0248 32 0x1000000;
        #devmem 0xff0a0248 32 0x0;
        #devmem 0xff0a0248 32 0x1000000;

        # Send a reset, reset is active low, this sends a high, then low then
        # high

        devmem 0xff0a0044 32 0x03000fff;
        devmem 0xff0a0044 32 0x02000fff;
        devmem 0xff0a0044 32 0x03000fff;
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
function check_switch {
        regval=$(r 0x10 0x3)
        echo $regval
        if [[ $regval -eq 0x3102 ]]
        then
                echo "Switch okay, could read Product Num and Rev"
                echo "okay"
        else
                #echo "ERROR Switch not okay"
                #echo "Could not read Product Num and Rev."
                #echo "Read $regval."
                echo "error"
        fi
}


# Set the RGMII timing for port 2,5 and 6
#
# Note: function should force the link down when it does this, but it doesn't
#
# Return Value
# If error
# If the Rx and Tx timing bits are not set return "error"
# If okay return "okay"
function set_rgmii_timing_for_port_2_5_6 {

        RGMII_Rx_Timing=0x8000
        RGMII_Tx_Timing=0x4000

        for port_reg in 0x12 0x15 0x16;
        do
                read_reg=$(r $port_reg 0x1)
                val_to_write=$(($read_reg |  $RGMII_Tx_Timing |  $RGMII_Rx_Timing))
                w $port_reg 0x1 $val_to_write
                read_reg=$(r $port_reg 0x1)
                if [ $(($read_reg & ($RGMII_Tx_Timing | $RGMII_Rx_Timing))) ]
                then
                        port=$(($port_reg & 0xF))
                        echo "RGMII Tx and Rx Timing set for port $port"
                else
                        echo "error"
                fi
        done
        echo "okay"
}

function create_bridge {
	# You should have called set_rgmii_timing_for_port_2_5_6 before this
	# You can also test ping after this is called,
	# call ifconfig enp3s0 192.169.1.11 on the other end of the connection

	brctl addbr br0
	brctl addif br0 lan3
	ifconfig eth0 up
	ifconfig lan3 up
	ifconfig br0 192.169.1.10 up
}

function destroy_bridge {

	ifconfig br0 0.0.0.0 down
	ifconfig lan3 down
	ifconfig eth0 down
	brctl delif br0 lan3
	brctl delbr br0
}

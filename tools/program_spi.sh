if [ "$SETUP_ENV" != "1" ]; then
        echo "Environment not set, exiting."
        return
fi

export DISPLAY=dummy; TARGET=$CONNECT_TO $PATH_TO_XSCT/xsct $PETALINUX_BUILD_TOOLS/reset_target.tcl

P="$PATH_TO_XSCT/program_flash -f $PETALINUX_BUILD_OUT/BOOT.bin -fsbl $PETALINUX_BUILD_OUT/zynqmp_fsbl.elf -flash_type qspi_single -blank_check -verify -cable type xilinx_tcf url $CONNECT_TO"
echo $P
echo "Running program_flash. To speed things up, cancel with Control-C, remove -blank_check -verify from the command and rerun"
$P

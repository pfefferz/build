if [ "$SETUP_ENV" != "1" ]; then
	echo "Environment not set, exiting."
	return
fi

TO_RM="
$PETALINUX_BUILD_OUTPUT_BSP_DIR/mhd_w_hw.bsp
$PETALINUX_BUILD_OUTPUT_BSP_DIR/mhd_w_hw.bsp.name.txt
$PETALINUX_BUILD_OUT/Image
$PETALINUX_BUILD_OUT/bl31.elf
$PETALINUX_BUILD_OUT/linux-boot.elf
$PETALINUX_BUILD_OUT/pmufw.elf
$PETALINUX_BUILD_OUT/psu_init.tcl
$PETALINUX_BUILD_OUT/system.dtb
$PETALINUX_BUILD_OUT/zynqmp_fsbl.elf
"

for file in $TO_RM; do
	echo $file
	rm $file
done

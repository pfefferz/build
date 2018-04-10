if [ "$SETUP_ENV" != "1" ]; then
	echo "Environment not set, exiting."
	return
fi

BINS="
project-spec/hw-description/psu_init.tcl
images/linux/pmufw.elf
images/linux/zynqmp_fsbl.elf
images/linux/Image
images/linux/system.dtb
build/misc/linux-boot/linux-boot.elf
images/linux/bl31.elf
"

for file in $BINS; do
	fullpath=$PETALINUX_PROJS_DIR/$PETALINUX_PROJ_NAME/$file
	if [ ! -e $fullpath ]; then
		echo ERROR: $fullpath does not exist	
		# https://unix.stackexchange.com/questions/293940/bash-how-can-i-make-press-any-key-to-continue?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
		read -n 1 -s -r -p "Press any key to continue"
		exit -1
	fi
done

for file in $BINS; do
	fullpath=$PETALINUX_PROJS_DIR/$PETALINUX_PROJ_NAME/$file
	cp -vf $fullpath $PETALINUX_BUILD_OUT
done

echo Files copied to $PETALINUX_BUILD_OUT

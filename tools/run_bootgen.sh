if [ "$SETUP_ENV" != "1" ]; then
	echo "Environment not set, exiting."
	return
fi

cp -f $PETALINUX_BUILD_OUT/uImage $PETALINUX_BUILD_OUT/uImage.bin

# Method from https://stackoverflow.com/questions/23929235/multi-line-string-with-extra-space-preserved-indentation?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa

printf "%s\n" \
"image : {" \
"        [bootloader,destination_cpu=a53-0]$PETALINUX_BUILD_OUT/zynqmp_fsbl.elf" \
"        [pmufw_image]$PETALINUX_BUILD_OUT/pmufw.elf" \
"        [destination_cpu=a53-0, exception_level=el-3, trustzone] $PETALINUX_BUILD_OUT/bl31.elf" \
"        [destination_cpu=a53-0, exception_level=el-2] $PETALINUX_BUILD_OUT/u-boot.elf" \
"        [load=0x03000000]$PETALINUX_BUILD_OUT/uImage.bin" \
"        [load=0x1407f000]$PETALINUX_BUILD_OUT/system.dtb" \
"        [load=0x01000000]$PETALINUX_BUILD_OUT/uramdisk.image.gz" \
"}" \
> $PETALINUX_BUILD_OUT/bootimage.bif

pushd $PETALINUX_BUILD_OUT
P="$PATH_TO_XSCT/bootgen -arch zynqmp -image $PETALINUX_BUILD_OUT/bootimage.bif -w -o $PETALINUX_BUILD_OUT/BOOT.bin"
echo $P
$P
popd

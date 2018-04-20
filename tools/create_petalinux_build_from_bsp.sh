if [ "$SETUP_ENV" != "1" ]; then
	echo "Environment not set, exiting."
	return
fi

mkdir -p $PETALINUX_PROJS_DIR
pushd $PETALINUX_PROJS_DIR
pwd
P="petalinux-create --type project -s $PETALINUX_BUILD_BSP_DIR/$PETALINUX_BUILD_BSP"
$P
pushd $PETALINUX_PROJ_NAME

git init
git add .
git status
git commit -m "$P"

P="petalinux-build"
$P
git add .
git status
git commit -m "$P"


# need to run this to produce build/misc/linux-boot/linux-boot.elf
P="petalinux-boot --jtag --kernel --tcl kernel.tcl"
$P
git add .
git status
git commit -m "$P"

popd
popd

source $PETALINUX_BUILD_TOOLS/prep_image_ramdisk_for_uboot.sh
pushd $PETALINUX_PROJS_DIR
pushd $PETALINUX_PROJ_NAME
git add .
git status
git commit -m "Created U-Boot loadables"
popd
popd

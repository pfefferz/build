if [ "$SETUP_ENV" != "1" ]; then
	echo "Environment not set, exiting."
	return
fi

mkdir -p $PETALINUX_PROJS_DIR
pushd $PETALINUX_PROJS_DIR
pwd
P="petalinux-create --type project --template zynqMP --name $PETALINUX_PROJ_NAME"
$P
pushd $PETALINUX_PROJ_NAME

git init
git add .
git status
git commit -m "$P"

P="petalinux-config --get-hw-description=$PETALINUX_BUILD_HDF_DIR"
$P
git add .
git status
git commit -m "$P"

P="petalinux-config -c kernel"
$P
# Add NET_SWITCHDEV "Switch (and switch-ish) device support"
# Add NET_DSA "Distributed Switch Architecture (NEW)"
# Add NET_DSA_MV88E6XXX "Marvell 88E6xxx Ethernet switch fabric support (NEW)"
git add .
git status
git commit -m "$P"

P="petalinux-build"
$P
# The following doesn't do anything:
git add .
git status
git commit -m "$P"

# Need to run this to generate linux-boot.elf
P="petalinux-boot --jtag --kernel --tcl kernel.txt"
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

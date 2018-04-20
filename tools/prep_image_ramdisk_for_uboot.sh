if [ "$SETUP_ENV" != "1" ]; then
        echo "Environment not set, exiting."
        return
fi

pushd $PETALINUX_PROJS_DIR
pushd $PETALINUX_PROJ_NAME

image='./images/linux/Image'
if [ ! -f $image ]; then
	echo "Linux Image at $image doesn't exist. Exiting."
	return;
fi

rootfs='./images/linux/rootfs.cpio.gz'
if [ ! -f $rootfs ]; then
	echo "Root FS at $rootfs doesn't exist. Exiting."
	return;
fi



echo "Converting Image to uImage so U-Boot can load it (no rootfs)"
P="petalinux-package --image -c kernel --format uImage"
$P

echo "Creating ramdisk that U-Boot can load"
P="./build/tmp/sysroots/x86_64-linux/usr/bin/mkimage -A arm64 -T ramdisk -C gzip -a 0x1000000 -e 0x1000000 -d $rootfs ./images/linux/uramdisk.image.gz"
$P

popd
popd

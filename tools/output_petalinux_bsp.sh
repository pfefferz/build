if [ "$SETUP_ENV" != "1" ]; then
	echo "Environment not set, exiting."
	return
fi

echo "Packaging $PETALINUX_PROJS_DIR/$PETALINUX_PROJ_NAME"
echo "Outputting to $PETALINUX_BUILD_OUTPUT_BSP_DIR"

mkdir -p $PETALINUX_BUILD_OUTPUT_BSP_DIR

P="petalinux-package --bsp -p $PETALINUX_PROJS_DIR/$PETALINUX_PROJ_NAME --hwsource $PETALINUX_BUILD_HDF_DIR --output $PETALINUX_BUILD_OUTPUT_BSP_DIR/$PETALINUX_BUILD_BSP"
$P

# We need to do this because we loose the name of the BSP and we need it later in a script
echo $PETALINUX_PROJ_NAME > $PETALINUX_BUILD_OUTPUT_BSP_DIR/$PETALINUX_BUILD_BSP_FILE_W_NAME

if [ "$SETUP_ENV" != "1" ]; then
        echo "Environment not set, exiting."
        return
fi

# get access to bitbake
source $PETALINUX_TOOLS_INSTALL_DIR/components/yocto/source/aarch64/environment-setup-aarch64-xilinx-linux
echo "Changing directory from $PWD to $PETALINUX_PROJS_DIR/$PETALINUX_PROJ_NAME"
cd $PETALINUX_PROJS_DIR/$PETALINUX_PROJ_NAME
source $PETALINUX_TOOLS_INSTALL_DIR/components/yocto/source/aarch64/layers/core/oe-init-build-env
export BB_ENV_EXTRAWHITE="$BB_ENV_EXTRAWHITE PETALINUX"
which bitbake

if [ "$SETUP_ENV" != "1" ]; then
        echo "Environment not set, exiting."
        return
fi

export DISPLAY=dummy; TARGET=$CONNECT_TO $PATH_TO_XSCT/xsct $PETALINUX_BUILD_TOOLS/reset_to_jtag_boot.tcl

if [ "$SETUP_ENV" == "1" ]; then
	echo "GOOD ENV"

	echo PETALINUX_TOOLS_INSTALL_DIR=$PETALINUX_TOOLS_INSTALL_DIR
	echo PETALINUX_PROJS_DIR=$PETALINUX_PROJS_DIR
	echo PETALINUX_PROJ_NAME=$PETALINUX_PROJ_NAME
	echo PATH_TO_XSCT=$PATH_TO_XSCT
	echo CONNECT_TO=$CONNECT_TO


	echo PETALINUX_BUILD_TOOLKIT=$PETALINUX_BUILD_TOOLKIT
	echo PETALINUX_BUILD_TOOLS=$PETALINUX_BUILD_TOOLS
	echo PETALINUX_BUILD_OUT=$PETALINUX_BUILD_OUT
	echo PETALINUX_BUILD_BIN=$PETALINUX_BUILD_BIN
	echo PETALINUX_BUILD_HDF_DIR=$PETALINUX_BUILD_HDF_DIR
	echo PETALINUX_BUILD_HDF=$PETALINUX_BUILD_HDF
	echo PETALINUX_BUILD_BSP_DIR=$PETALINUX_BUILD_BSP_DIR
	echo PETALINUX_BUILD_BSP=$PETALINUX_BUILD_BSP
	echo PETALINUX_BUILD_BSP_FILE_W_NAME=$PETALINUX_BUILD_BSP_FILE_W_NAME
	echo PETALINUX_BUILD_OUTPUT_BSP_DIR=$PETALINUX_BUILD_OUTPUT_BSP_DIR

	echo PETALINUX_BUILD_HDF=$PETALINUX_BUILD_HDF
	echo PETALINUX_BUILD_BSP=$PETALINUX_BUILD_BSP


	echo 1
else
	echo "BAD ENV, source setup_env.sh"
	echo 0
fi

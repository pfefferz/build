# setup_env.sh
#
# The MIT License (MIT)
#
# Copyright(c) 2018 Centennial Software Solutions LLC.
#
# inquiries@centennialsoftwaresolutions.com
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# These should be updated to reflect the PetaLinux Tools install
# where to install the PetaLinux project and the project name

# Where PetaLinux Tools is installed
export PETALINUX_TOOLS_INSTALL_DIR=/home/pfefferz/tools/opt/pkg/petalinux
# A directory that will contain the PetaLinux project
export PETALINUX_PROJS_DIR=/home/pfefferz/plprjs5
# The path to the SDK
export PATH_TO_XSCT=/hdd/opt/Xilinx/SDK/2017.4/bin/
# How to connect to the JTAG
export CONNECT_TO=tcp:localhost:3121

git config --global user.email "zach.pfeffer@centennialsoftwaresolutions.com"
git config --global user.name "Zach Pfeffer"

# These should be left alone
export PETALINUX_PROJ_NAME=mtd_board # This gets created in PROJ
export PETALINUX_BUILD_TOOLKIT=$PWD
export PETALINUX_BUILD_TOOLS=$PETALINUX_BUILD_TOOLKIT/tools
export PETALINUX_BUILD_OUT=$PETALINUX_BUILD_TOOLKIT/out
export PETALINUX_BUILD_BIN=$PETALINUX_BUILD_TOOLKIT/overlay/bin
export PETALINUX_BUILD_HDF_DIR=$PETALINUX_BUILD_TOOLKIT/hdf
export PETALINUX_BUILD_HDF=mhd_be_test_wrapper.hdf
export PETALINUX_BUILD_BSP_DIR=$PETALINUX_BUILD_TOOLKIT/bsps
export PETALINUX_BUILD_BSP=mhd_w_hw.bsp
export PETALINUX_BUILD_BSP_FILE_W_NAME=mhd_w_hw.bsp.name.txt
export PETALINUX_BUILD_OUTPUT_BSP_DIR=$PETALINUX_BUILD_TOOLKIT/bsps_out


dirs="$PETALINUX_TOOLS_INSTALL_DIR
	$PETALINUX_PROJS_DIR
	$PATH_TO_XSCT
	$PETALINUX_BUILD_TOOLKIT
	$PETALINUX_BUILD_OUT
	$PETALINUX_BUILD_BIN
	$PETALINUX_BUILD_HDF_DIR
	$PETALINUX_BUILD_BSP_DIR
	$PETALINUX_BUILD_OUTPUT_BSP_DIR"

# Uncomment to test error handling
#TEST_SCRIPT=$PETALINUX_BUILD_TOOLKIT/doestexist
#dirs="$dirs $TEST_SCRIPT"

for dir in $dirs; do
	if [ ! -d $dir ]; then
		echo "ERROR $dir does not exist!"
		echo "Environment not set, exiting."
		return;
	fi
done

export PETALINUX_BUILD_HDF=mhd_be_test_wrapper.hdf
export PETALINUX_BUILD_BSP=mhd_w_hw.bsp

files="$PETALINUX_BUILD_HDF_DIR/$PETALINUX_BUILD_HDF
	$PETALINUX_BUILD_BSP_DIR/$PETALINUX_BUILD_BSP
	$PETALINUX_BUILD_BSP_DIR/$PETALINUX_BUILD_BSP_FILE_W_NAME"

for file in $files; do
	if [ ! -f $file ]; then
		echo "ERROR $file does not exist!"
		echo "Environment not set, exiting."
		return;
	fi
done


export PETALINUX_SAVE_PATH=$PATH
source $PETALINUX_TOOLS_INSTALL_DIR/settings.sh
export PATH=$PETALINUX_BUILD_BIN:$PATH
export PATH=$PETALINUX_BUILD_TOOLS:$PATH
echo "Using $(which make)"
echo "Path to petalinux-create $(which petalinux-create)"

export SETUP_ENV=1

Welcome to the MHD build!  

_Overview_

This package has everything you need to work with the build except:
* PetaLinux Tools 2017.4
* The SDK (via Vivado 2017.4)

Download the 2017.4 release of PetaLinux Tools and the SDK @ 
https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-design-tools/2017-4.html

Download the 2017.4 release of Vivado @ 
https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/2017-4.html

If you need any help installing these feel free to look at:
https://www.zachpfeffer.com/single-post/Download-and-Install-Xilinxs-20174-PetaLinux-Tools
https://www.zachpfeffer.com/single-post/Installing-20174-Vivado-and-SDK-on-Linux

At the moment the only thing support is a non-U-Boot JTAG loaded build


__How things Work__

Overall you'll:
A. Setup the Environment 
B. Load the Build
C. Enable Networking
D. Package a BSP



__Setup the Environment__

1. Start by setting up the environment:

Edit setup_env.sh

Set the following variables:

# Where PetaLinux Tools is installed
export PETALINUX_TOOLS_INSTALL_DIR=/home/pfefferz/tools
# A directory that will contain the PetaLinux project
export PETALINUX_PROJS_DIR=/home/pfefferz/plprjs3
# The path to the SDK
export PATH_TO_XSCT=/hdd/opt/Xilinx/SDK/2017.4/bin/
# How to connect to the JTAG
export CONNECT_TO=tcp:localhost:3121

git config --global user.email "zach.pfeffer@centennialsoftwaresolutions.com"
git config --global user.name "Zach Pfeffer"

The script checks that all directories exist. 


2. Next source the script:

source ./setup_env.sh

This sets SETUP_ENV which all the other scripts check.


3. Now create the PetaLinux project from the packaged BSP:

source tools/create_petalinux_build_from_bsp.sh

This will build the PetaLinux project from the included BSP.

This can take some time.

At this point you have a full PetaLinux Project to hack on. Your environment has been set up so you can petalinux-build, petalinux... etc... in the PetaLinux Project directory.



__Load the Build__

1. Copy all the artifacts to the out directory with:

source tools/cp_bins_for_jtag_load.sh


2. And load it

source tools/jtag_load.sh


3. To load using the U-Boot boot loader do

source tools/jtag_load_u-boot.sh

Note: Its takes U-Boot 2-3 minutes to get "un-stuck" from the line "zynqmp_qspi_ofdata_to_platdata: CLK 299999997." I haven't had time to fix this yet. It will eventually get unstuck. Once unstuck, type bootm 00080000 01000000 1407f000.

If you have an existing build or anytime you do a petalinux-build and want to load via U-Boot type:

source tools/prep_image_ramdisk_for_uboot.sh

then

source tools/cp_bins_for_jtag_load.sh



__Enable Networking__

1. Start minicom on the host attached to the target via serial

minicom -o -w -C Apr-11th-2018.log

Check out https://www.zachpfeffer.com/single-post/Arg-Nothing-I-type-shows-up-in-the-Minicom-console if you need help setting up the com port


2. Log in. Username is root and password is root.


3. Now make a terminal 160 chars wide and cat rws.sh

cat switch_tools/rws.sh


4. Copy and paste the whole script into the terminal


5. Run set_rgmii_timing_for_port_2_5_6 and create_bridge


6. To test, ping 192.169.1.11



__Packaging a BSP__

Once you're done hacking on the BSP you can repackage it to bsp_out/

Just run output_petalinux_bsp.sh

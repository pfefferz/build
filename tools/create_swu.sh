if [ "$SETUP_ENV" != "1" ]; then
	echo "Environment not set, exiting."
	return
fi

if [ ! -f $PETALINUX_BUILD_OUT/BOOT.bin ]; then
	echo "Please create BOOT.bin with: source tools/run_bootgen.sh"
	return
fi

cp -f $PETALINUX_BUILD_OUT/BOOT.bin $PETALINUX_BUILD_OUT/BOOT.bin.orig
cp -f $PETALINUX_BUILD_OUT/BOOT.bin $PETALINUX_BUILD_OUT/BOOT.bin.new

# Call with:
# swupdate_unstripped -v -H"0.1":"1.0" -i ~/my-software_1.0.swu -e stable,alt

#printf "%s\n" \
SWDESCRIPT="
software =
{ 
	/* version is required. Does it matter if
         * the value doesn't match /etc/hwrevision?
         */

	version = \"0.1\";

	/* hardware-compatibility is also required
	 * Again, does the value matter
     	 */

	hardware-compatibility: [ \"1.0\" ]; 
 
	stable:
	{
		alt:
		{

 	       		images: (
       	       		{
				filename = \"BOOT.bin.new\";
                        	device = \"/dev/mtd3\";
               		}
			);
		};
		main:
		{
			images: (
                	{
				filename = \"BOOT.bin.orig\";
                        	device = \"/dev/mtd4\";
                	}
			);
		};
        };
}
"
echo "$SWDESCRIPT" > "$PETALINUX_BUILD_OUT/sw-description"

CONTAINER_VER="1.0"
PRODUCT_NAME="my-software"
FILES="sw-description \
       BOOT.bin.new \
       BOOT.bin.orig"
pushd $PETALINUX_BUILD_OUT
for i in $FILES;do
        echo $i;done | cpio -ov -H crc > ${PRODUCT_NAME}_${CONTAINER_VER}.swu
popd

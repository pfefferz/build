# Put a custom bin folder in place and update the path to check it first
LOCALTOOLS=$(pwd)/tools
echo $LOCALTOOLS
mkdir -p $LOCALTOOLS  
echo export PATH=$LOCALTOOLS/bin:\$PATH > set_petalinux_env.sh
. ./set_petalinux_env.sh
# Test it
mkdir -p $LOCALTOOLS/bin
echo echo Hello > $LOCALTOOLS/bin/test-set_petalinux_env.sh
chmod +x $LOCALTOOLS/bin/test-set_petalinux_env.sh
test-set_petalinux_env.sh

#Get and install make-3.81 locally
CURDIRBEFOREMAKE=$(pwd)
LOCALPACKAGE=$(pwd)/package
echo $LOCALPACKAGE
mkdir -p $LOCALPACKAGE
cd $LOCALPACKAGE
wget https://ftp.gnu.org/gnu/make/make-3.81.tar.gz
tar -xvzf $LOCALPACKAGE/make-3.81.tar.gz
cd make-3.81
./configure --prefix=$LOCALTOOLS
make
sudo make install
cd $CURDIRBEFOREMAKE

#Test make
which make # you should see $LOCALTOOLS/bin/make
make --version # should now show GNU Make 3.81

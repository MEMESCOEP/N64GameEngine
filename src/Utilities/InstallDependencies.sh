#!/bin/bash
### OVERVIEW ###
# This script downloads a prebuilt MIPS toolchain and installs it. It also installs libdragon, tiny3d, and ares.


## VARIABLES ##
ProcessorCount=$(nproc --all)
InstallPath='/opt/libdragon'


## MAIN CODE ##
echo Detected $ProcessorCount processors.
echo Setting N64_INST to \"$InstallPath\"...
export N64_INST=$InstallPath

echo Installing build dependencies...
sudo apt-get install build-essential texinfo git pkg-config libgtk-3-dev libcanberra-gtk-module libgl-dev libasound2-dev

echo ""
echo Downloading and installing prebuild toolchain...
wget -O toolchain.dpkg https://github.com/DragonMinded/libdragon/releases/download/toolchain-continuous-prerelease/gcc-toolchain-mips64-x86_64.deb
sudo dpkg -i toolchain.dpkg

echo ""
echo Cloning repositories...
git clone https://github.com/DragonMinded/libdragon
git clone https://github.com/hailtododongo/tiny3d
git clone https://github.com/ares-emulator/ares

echo ""
echo Switching to libdragon preview branch...
git -C ./libdragon checkout preview

echo ""
echo Compiling libdragon, tiny3d, and ares...
cd libdragon
./build.sh

cd ../tiny3d
./build.sh

cd ../ares
make -j$ProcessorCount cores="n64"
mv -f desktop-ui/out/ ../Ares

echo "Cleaning up..."
cd ../
rm toolchain.dpkg
rm -rf libdragon tiny3d ares

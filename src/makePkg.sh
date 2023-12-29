#!/bin/bash
# makePkg.sh: Copyright 2005-2023 Valerio Messina efa@iol.it
# makePkg is part of ReSolve - solve math expressions using discrete values
# ReSolve is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# ReSolve is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with ReSolve. If not, see <http://www.gnu.org/licenses/>.
#
# Script to generate a Linux|Mingw|MXE|OSX package of 'ReSolve'
# used on: Linux=>bin64, Linux=>bin32, Linux=>macOS64
#          MinGw64=>bin64, MinGw32=>bin32, MXE64=>bin64, MXE32=>bin32
#
# Syntax: $ makePkg.sh Linux|WinMxe|WinMgw|MacOS [32|64]

makever=2023-12-29

DEPSPATHMGW64="/mingw64/bin" # path of DLLs needed to generate the Mingw64 package
DEPSPATHMGW32="/mingw32/bin" # path of DLLs needed to generate the Mingw32 package
DEPSLISTMGW="" # list of dll for MinGW

DSTPATH=".." # path where create the Linux|WinMxe|WinMgw|MacOS package directory

echo "makePkg.sh: create a Linux|WinMxe|WinMgw|MacOS package for ReSolve ..."

# check for external dependency compliance
flag=0
for extCmd in 7z cp cut grep mkdir mv rm tar uname ; do
   exist=`which $extCmd 2> /dev/null`
   if (test "" = "$exist") then
      echo "Required external dependency: "\"$extCmd\"" unsatisfied!"
      flag=1
   fi
done
if [[ "$flag" = 1 ]]; then
   echo "ERROR: Install the required packages and retry. Exit"
   exit
fi

if [[ "$1" = "" || "$1" != "Linux" && "$1" != "WinMxe" && "$1" != "WinMgw" && "$1" != "MacOS" ]]; then
   echo "makePkg ERROR: need the target platform to create package"
   echo "Syntax: $ makePkg.sh Linux|WinMxe|WinMgw|MacOS [32|64]"
   exit
fi

exist=`which gtk-mac-bundler 2> /dev/null`
if (test "$1" = "MacOS" && test "" = "$exist") then
   echo "ERROR: makePkg depend on 'gtk-mac-bundler' to generate for macOS. Exit"
   exit
fi

PKG="$1"
CPU=`uname -m` # i686 or x86_64
if (test "" = "$2") then
   BIT=$(getconf LONG_BIT)
else
   BIT="$2"
fi
if (test "$CPU" = "x86_64" && test "$BIT" = "32") then
   CPU=i686
fi
if (test "$PKG" = "WinMxe" || test "$PKG" = "WinMgw") then
   EXT=".exe"
fi
OS=`uname`
if (test "$OS" != "Darwin") then
   OS=`uname -o`  # Msys or GNU/Linux, illegal on macOS
fi
VER=`grep SourceVersion reSolveLib.h | cut -d' ' -f3 | tr -d '."'`
DATE=`date -I`
SRC=`pwd`
TGT=$PKG
if [[ "$OS" = "Msys" ]]; then
   if [[ "$BIT" = "64" ]]; then
      DEPSRC=$DEPSPATHMGW64
   fi
   if [[ "$BIT" = "32" ]]; then
      DEPSRC=$DEPSPATHMGW32
   fi
fi
DMG=""
if [[ "$PKG" = "MacOS" ]]; then
   DMG="DiskImage/ReSolve.app/Contents/MacOS"
fi
DST="ReSolve${VER}_${DATE}_${TGT}_${CPU}_${BIT}bit"

if [[ "$OS" != "Msys" && "$OS" != "GNU/Linux" ]]; then
   echo "ERROR: work in WinMgw|WinMxe(Linux) only"
   exit
fi
if [[ "$OS" = "Msys" && "$PKG" != "WinMgw" ]]; then
   echo "ERROR makePkg: Unsupported target package:$PKG on MinGW/MSYS2"
   exit
fi

if [[ "$OS" = "GNU/Linux" && "$PKG" != "Linux" && "$PKG" != "WinMxe" && "$PKG" != "MacOS" ]]; then
   echo "ERROR makePkg: Unsupported target package:$PKG on Linux"
   exit
fi

echo "PKG : $PKG"
echo "CPU : $CPU"
echo "BIT : $BIT"
echo "OS  : $OS"
echo "VER : $VER"
echo "SRC : $SRC"
echo "TGT : $TGT"
echo "DEP : $DEPSRC"
echo "DST : $DSTPATH/$DST"
echo "DATE: $DATE"
read -p "Proceed? A key to continue"
echo ""

echo "makePkg: Creating ReSolve $VER package for $CPU $TGT $BIT bit ..."
cp -a ../reSolveReadme.txt ../README.md

if (test "$PKG" = "MacOS") then
   if (test "$BIT" = "32") then
      echo "Unsupported 32 bit on MacOS"
      exit
   fi
   gtk-mac-bundler ReSolve.bundle
   cd ../..
   AppName=reSolveGuiMacOS64
   ls -l $AppName.app
   tar -cf $AppName.app.tgz $AppName.app
   wp=0 # write protect
   echo "Generating uncompressesd DMG ..."
   rm -rf DiskImage
   mkdir DiskImage
   mv $AppName.app DiskImage
   rm $AppName$VER.dmg 2> /dev/null
   if (test "$wp" = 1) then
      genisoimage -V $AppName -D -r -apple -no-pad -o $AppName$VER.dmg DiskImage
   else
      genisoimage -V $AppName -D -R -apple -no-pad -o $AppName$VER.dmg DiskImage
   fi
   rm -rf DiskImage
   echo "Compressing DMG ..."
   mv $AppName$VER.dmg uncompressed.dmg
   dmg uncompressed.dmg $AppName$VER.dmg
   rm uncompressed.dmg
   echo "$AppName$VER.dmg created."
   exit
fi

rm -rf AppDir
mkdir -p AppDir/usr/bin
rm -rf ReSolve
mkdir -p ReSolve/src
cd ..
cp -a reSolveConf.txt reSolve.glade circuit*.png ReSolve.png src/AppDir/usr/bin
cp -a reSolveConf.txt reSolve.glade circuit*.png ReSolve.png src/ReSolve
cp -a reSolveReadme.txt LICENSE src/AppDir/usr/bin
cp -a reSolveReadme.txt LICENSE src/ReSolve
cp -a reSolve${PKG}${BIT}${EXT}    src/AppDir/usr/bin/reSolve${PKG}${BIT}${EXT}
cp -a reSolve${PKG}${BIT}${EXT}    src/ReSolve/reSolve${PKG}${BIT}${EXT}
cp -a reSolveGui${PKG}${BIT}${EXT} src/AppDir/usr/bin/reSolveGui${PKG}${BIT}${EXT}
cp -a reSolveGui${PKG}${BIT}${EXT} src/ReSolve/reSolveGui${PKG}${BIT}${EXT}
cd src
cp -a *.h *.c Makefile* ReSolve/src
cp -aP reSolveConf.txt reSolve.glade circuit*.png ReSolve.png ReSolve/src
cp -a makePkg.sh ReSolve.ico ReSolve.icns ReSolve.bundle reSolve.desktop Info.plist ReSolve/src

if (test "$PKG" = "Linux" && (test "$CPU" = "x86_64" || test "$CPU" = "i686")) then # skip on ARM&RISC-V
   echo "makePkg.sh: generating the AppImage for ReSolve (about 2') ..."
   if (test -f logWget$DATE.txt) then { rm logWget$DATE.txt ; } fi
   if (test "$BIT" = "64") then
      if (! test -x linuxdeploy-x86_64.AppImage) then
         wget -nv "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage" 2>> logWget$DATE.txt
         chmod +x linuxdeploy-x86_64.AppImage
      fi
      if (! test -x linuxdeploy-plugin-gtk.sh) then
         wget -nv "https://raw.githubusercontent.com/linuxdeploy/linuxdeploy-plugin-gtk/master/linuxdeploy-plugin-gtk.sh" 2>> logWget$DATE.txt
         chmod +x linuxdeploy-plugin-gtk.sh
      fi
      pwd
      linuxdeploy-x86_64.AppImage -e ../reSolveGuiLinux64 --appdir AppDir -p gtk -i ReSolve.png -d reSolve.desktop --output appimage > logLinuxdeploy$DATE.txt
      file=reSolve${VER}_${DATE}_Linux_${CPU}_${BIT}bit.AppImage
      mv ReSolveGui-x86_64.AppImage ../$file
      cp -a ../$file ../..
      echo "AppImage created: $file"
   fi
   if (test "$BIT" = "32") then
      echo "As now skip AppImage at 32 bit"
   fi
fi
if (test "$PKG" = "Linux") then
   file=reSolve${VER}_${DATE}_Linux_${CPU}_${BIT}bit.tgz
   echo "Creating package file:'$file' ..."
   if (test -f $file) then { rm $file ; } fi
   tar -cvaf $file ReSolve
   mv $file ../..
   echo "Package file:'$file' done"
fi
if (test "$PKG" = "WinMxe" || test "$PKG" = "WinMgw") then
   file=reSolve${VER}_${DATE}_${PKG}_${BIT}bit.7z
   echo "Creating package file:'$file' ..."
   if (test -f $file) then { rm $file ; } fi
   cd ReSolve/src
   rm reSolveConf.txt reSolve.glade circuit*.png ReSolve.png # remove links
   cd ../..
   7z a -m0=lzma -mx=9 -r $file ReSolve > /dev/null
   mv $file ../..
   echo "Package file:'$file' done"
fi
rm -rf AppDir ReSolve

#!/bin/bash
# makeAppDir.sh: this script generate the AppDir for ReSolve. 2023-10-01
echo "makeAppDir.sh: generating the AppDir for ReSolve ..."
if (test "" = "$1") then
   echo "makeAppDir.sh ERROR: need the target platform to create package"
   echo "Syntax: $ makeAppDir.sh Linux|Win|MacOS [32|64]"
   exit
fi
PKG=$1
if (test "" = "$2") then
   BIT=$(getconf LONG_BIT)
else
   BIT=$2
fi
cp -a ../reSolveReadme.txt ../README.md
CPU=`uname -m`
if (test "$CPU" = "x86_64" && test "$BIT" = "32") then
   CPU=i686
fi
if (test "$PKG" = "Win") then
   EXT=".exe"
fi
echo "makeAppDir.sh: generating $PKG $CPU $BIT bit package ..."
ver=`grep SourceVersion reSolveLib.h | cut -d' ' -f3 | tr -d '."'`
date=`date -I`
echo ver:$ver date:$date

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
   rm $AppName$ver.dmg 2> /dev/null
   if (test "$wp" = 1) then
      genisoimage -V $AppName -D -r -apple -no-pad -o $AppName$ver.dmg DiskImage
   else
      genisoimage -V $AppName -D -R -apple -no-pad -o $AppName$ver.dmg DiskImage
   fi
   rm -rf DiskImage
   echo "Compressing DMG ..."
   mv $AppName$ver.dmg uncompressed.dmg
   dmg uncompressed.dmg $AppName$ver.dmg
   rm uncompressed.dmg
   echo "$AppName$ver.dmg created."
   exit
fi

rm -rf AppDir
mkdir -p AppDir/usr/bin
rm -rf ReSolve
mkdir -p ReSolve/src
cd ..
cp -a reSolveConf.txt reSolve.glade circuit*.png ReSolve.png src/AppDir/usr/bin
cp -a reSolveConf.txt reSolve.glade circuit*.png ReSolve.png src/ReSolve
cp -a reSolveReadme.txt src/AppDir/usr/bin
cp -a reSolveReadme.txt src/ReSolve
cp -a reSolve${PKG}${BIT}${EXT}    src/AppDir/usr/bin/reSolve${PKG}${BIT}${EXT}
cp -a reSolve${PKG}${BIT}${EXT}    src/ReSolve/reSolve${PKG}${BIT}${EXT}
cp -a reSolveGui${PKG}${BIT}${EXT} src/AppDir/usr/bin/reSolveGui${PKG}${BIT}${EXT}
cp -a reSolveGui${PKG}${BIT}${EXT} src/ReSolve/reSolveGui${PKG}${BIT}${EXT}
cd src
cp -a *.h *.c Makefile* ReSolve/src
rm ReSolve/src/reSolveLibOld.c ReSolve/src/engStrTst.c 2> /dev/null
cp -aP reSolveConf.txt reSolve.glade circuit*.png ReSolve.png ReSolve/src

if (test "$PKG" = "Linux" && (test "$CPU" = "x86_64" || test "$CPU" = "i686")) then # skip on ARM&RISC-V
   echo "makeAppDir.sh: generating the AppImage for ReSolve (about 2') ..."
   if (test -f logWget$date.txt) then { rm logWget$date.txt ; } fi
   if (test "$BIT" = "64") then
      if (! test -x linuxdeploy-x86_64.AppImage) then
         wget -nv "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage" 2>> logWget$date.txt
         chmod +x linuxdeploy-x86_64.AppImage
      fi
      if (! test -x linuxdeploy-plugin-gtk.sh) then
         wget -nv "https://raw.githubusercontent.com/linuxdeploy/linuxdeploy-plugin-gtk/master/linuxdeploy-plugin-gtk.sh" 2>> logWget$date.txt
         chmod +x linuxdeploy-plugin-gtk.sh
      fi
      linuxdeploy-x86_64.AppImage -e ../reSolveGuiLinux64 --appdir AppDir -p gtk -i ReSolve.png -d reSolve.desktop --output appimage > logLinuxdeploy$date.txt
      file=reSolve${ver}_${date}_Linux_${CPU}_${BIT}bit.AppImage
      mv ReSolveGui-x86_64.AppImage ../$file
      cp -a ../$file ../..
      echo "AppImage created: $file"
   fi
   if (test "$BIT" = "32") then
      echo "As now skip AppImage at 32 bit"
   fi
fi
if (test "$PKG" = "Linux") then
   file=reSolve${ver}_${date}_Linux_${CPU}_${BIT}bit.tgz
   echo "Creating package file:'$file' ..."
   if (test -f $file) then { rm $file ; } fi
   tar -cvaf $file ReSolve
   mv $file ../..
   echo "Package file:'$file' done"
fi
if (test "$PKG" = "Win") then
   file=reSolve${ver}_${date}_Win_${BIT}bit.7z
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

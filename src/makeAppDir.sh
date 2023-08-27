#!/bin/bash
# makeAppDir.sh: this script generate the AppDir for ReSolve. 2023-08-27
echo "makeAppDir.sh: generating the AppDir for ReSolve ..."
if (test "" = "$1") then
   echo "makeAppDir.sh ERROR: need the target platform to create package"
   echo "Syntax: $ makeAppDir.sh Linux|Win [32|64]"
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
echo "makeAppDir.sh: generating $PKG $CPU $BIT bit package ..."
if (test "$PKG" = "MacOS") then
   echo "As now do not support macOS. Use 'gtk-mac-bundler'. Quit"
   exit
fi
rm -rf AppDir
mkdir -p AppDir/usr/bin
cp -aL reSolveConf.txt reSolve.glade circuit*.png ReSolve.png AppDir/usr/bin
cp -a ../reSolveReadme.txt AppDir/usr/bin

if (test "$PKG" = "Win") then
   EXT=".exe"
fi
cp -a ../reSolve${PKG}${BIT}${EXT}    AppDir/usr/bin/reSolve${PKG}${BIT}${EXT}
cp -a ../reSolveGui${PKG}${BIT}${EXT} AppDir/usr/bin/reSolveGui${PKG}${BIT}${EXT}

date=`date -I`
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
      mv ReSolveGui-x86_64.AppImage ../ReSolveGui-x86_64.AppImage
      cp -a ../ReSolveGui-x86_64.AppImage ../..
      echo "AppImage created: ReSolveGui-x86_64.AppImage"
   fi
   if (test "$BIT" = "32") then
      echo "As now skip AppImage at 32 bit"
   fi
fi

ver=`grep SourceVersion reSolveLib.h | cut -d' ' -f3 | tr -d '."'`
echo ver:$ver date:$date
if (test "$PKG" = "Linux") then
   file=reSolve${ver}_${date}_Linux_${CPU}_${BIT}bit.tgz
   echo "Creating package file:'$file' ..."
   cd ../..
   if (test -f $file) then { rm $file ; } fi
   if (test "$BIT" = "64") then
      EXCL=Linux32
   fi
   if (test "$BIT" = "32") then
      EXCL=Linux64
   fi
   tar --exclude-vcs --exclude=$EXCL --exclude=AppDir --exclude=*.AppImage --exclude=appimagetool* --exclude=*.exe --exclude=ReSolve.ods --exclude=reSolveBack.glade --exclude=reSolve_.glade --exclude=notes.txt -cvaf $file ReSolve
fi

if (test "$PKG" = "Win") then
   file=reSolve${ver}_${date}_Win_${BIT}bit.7z
   echo "Creating package file:'$file' ..."
   mkdir -p AppDir/src
   cp -a *.h *.c Makefile* AppDir/src
   mv AppDir ReSolve
   if (test -f $file) then { rm $file ; } fi
   7z a -m0=lzma -mx=9 -r $file ReSolve > /dev/null
   mv ReSolve AppDir
   mv $file ../..
fi
echo "Package file:'$file' done"

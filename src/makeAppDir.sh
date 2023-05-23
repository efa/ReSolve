#!/bin/bash
# makeAppDir.sh: this script generate the AppDir for ReSolve. 2023-05-23
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
mkdir -p AppDir
if (test "$PKG" = "Linux") then
   mkdir -p AppDir/lib
   if (test "$CPU" = "x86_64" || test "$CPU" = "i686") then # skip on ARM&RISC-V
      if (test "$BIT" = "64") then
         cp -aL /lib/x86_64-linux-gnu/libatk-1.0.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libatk-bridge-2.0.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libatspi.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libblkid.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libbrotlicommon.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libbrotlidec.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libbsd.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libcairo-gobject.so.2 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libcairo.so.2 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libdatrie.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libdbus-1.so.3 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libdl.so.2 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libepoxy.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libexpat.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libffi.so.7 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libfontconfig.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libfreetype.so.6 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libfribidi.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libgcrypt.so.20 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libgdk-3.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libgdk_pixbuf-2.0.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libgio-2.0.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libglib-2.0.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libgmodule-2.0.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libgobject-2.0.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libgpg-error.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libgraphite2.so.3 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libgtk-3.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libharfbuzz.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/liblz4.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/liblzma.so.5 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libmd.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libmount.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libpango-1.0.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libpangocairo-1.0.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libpangoft2-1.0.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libpcre2-8.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libpcre.so.3 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libpixman-1.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libpng16.so.16 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libpthread.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libresolv.so.2 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/librt.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libselinux.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libthai.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libuuid.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libwayland-client.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libwayland-cursor.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libwayland-egl.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libX11.so.6 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libXau.so.6 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libxcb-render.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libxcb-shm.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libxcb.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libXcomposite.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libXcursor.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libXdamage.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libXdmcp.so.6 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libXext.so.6 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libXfixes.so.3 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libXinerama.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libXi.so.6 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libxkbcommon.so.0 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libXrandr.so.2 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libXrender.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libz.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libzstd.so.1 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libm.so.6 AppDir/lib
         cp -aL /lib/x86_64-linux-gnu/libc.so.6 AppDir/lib
         #cp -aL /lib/x86_64-linux-gnu/libsystemd.so.0 AppDir/lib
         cat AppRun | sed 's/is32whenLinux32pkgAndRunOnLinux64/64/' > AppDir/AppRun
      fi # 64 bit
      if (test "$BIT" = "32") then
         cp -aL /lib/i386-linux-gnu/libatk-1.0.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libatk-bridge-2.0.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libatspi.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libblkid.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libbrotlicommon.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libbrotlidec.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libbsd.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libcairo-gobject.so.2 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libcairo.so.2 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libdatrie.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libdbus-1.so.3 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libdl.so.2 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libepoxy.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libexpat.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libffi.so.7 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libfontconfig.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libfreetype.so.6 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libfribidi.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libgcrypt.so.20 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libgdk-3.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libgdk_pixbuf-2.0.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libgio-2.0.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libglib-2.0.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libgmodule-2.0.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libgobject-2.0.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libgpg-error.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libgraphite2.so.3 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libgtk-3.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libharfbuzz.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/liblz4.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/liblzma.so.5 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libmd.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libmount.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libpango-1.0.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libpangocairo-1.0.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libpangoft2-1.0.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libpcre2-8.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libpcre.so.3 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libpixman-1.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libpng16.so.16 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libpthread.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libresolv.so.2 AppDir/lib
         cp -aL /lib/i386-linux-gnu/librt.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libselinux.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libthai.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libuuid.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libwayland-client.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libwayland-cursor.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libwayland-egl.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libX11.so.6 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libXau.so.6 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libxcb-render.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libxcb-shm.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libxcb.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libXcomposite.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libXcursor.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libXdamage.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libXdmcp.so.6 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libXext.so.6 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libXfixes.so.3 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libXinerama.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libXi.so.6 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libxkbcommon.so.0 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libXrandr.so.2 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libXrender.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libz.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libzstd.so.1 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libm.so.6 AppDir/lib
         cp -aL /lib/i386-linux-gnu/libc.so.6 AppDir/lib
         #cp -aL /lib/i386-linux-gnu/libsystemd.so.0 AppDir/lib
         cat AppRun | sed 's/is32whenLinux32pkgAndRunOnLinux64/32/' > AppDir/AppRun
      fi # 32 bit
      chmod +x AppDir/AppRun
   fi # x86_64 || i686
   cp -aL reSolve.desktop AppDir
fi # PKG Linux libs
cp -aL reSolveConf.txt reSolve.glade circuit*.png ReSolve.png AppDir
cp -a ../reSolveReadme.txt AppDir
if (test "$PKG" = "Win") then
   EXT=".exe"
fi
cp -a ../reSolve${PKG}${BIT}${EXT}    AppDir/reSolve${PKG}${BIT}${EXT}
cp -a ../reSolveGui${PKG}${BIT}${EXT} AppDir/reSolveGui${PKG}${BIT}${EXT}
if (test "$PKG" = "Linux" && (test "$CPU" = "x86_64" || test "$CPU" = "i686")) then # skip on ARM&RISC-V
   echo "makeAppDir.sh: Generating the AppImage for ReSolve ..."
   if (test "$BIT" = "64") then
      if (! test -x appimagetool-x86_64.AppImage) then
         wget "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"
         chmod +x appimagetool-x86_64.AppImage
      fi
      appimagetool-x86_64.AppImage AppDir
      mv ReSolveGui-x86_64.AppImage ..
      cp -a ../ReSolveGui-x86_64.AppImage ../..
   fi
   if (test "$BIT" = "32") then
      if (! test -x appimagetool-i686.AppImage) then
         wget "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-i686.AppImage"
         chmod +x appimagetool-i686.AppImage
      fi
      appimagetool-i686.AppImage AppDir
      mv ReSolveGui-i386.AppImage ..
      cp -a ../ReSolveGui-i386.AppImage ../..
   fi
fi
ver=`grep SourceVersion reSolveLib.h | cut -d' ' -f3 | tr -d '."'`
date=`date -I`
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

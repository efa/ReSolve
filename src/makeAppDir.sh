#!/bin/bash
# makeAppDir.sh: this script generate the AppDir for ReSolve
mkdir -p AppDir/lib
BITS=$(getconf LONG_BIT)
if (test "$BITS" = "64") then
   cp -a /lib/x86_64-linux-gnu/libatk-1.0.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libatk-bridge-2.0.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libatspi.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libblkid.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libbrotlicommon.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libbrotlidec.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libbsd.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libcairo-gobject.so.2 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libcairo.so.2 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libdatrie.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libdbus-1.so.3 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libdl.so.2 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libepoxy.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libexpat.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libffi.so.7 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libfontconfig.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libfreetype.so.6 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libfribidi.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libgcrypt.so.20 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libgdk-3.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libgdk_pixbuf-2.0.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libgio-2.0.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libglib-2.0.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libgmodule-2.0.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libgobject-2.0.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libgpg-error.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libgraphite2.so.3 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libgtk-3.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libharfbuzz.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/liblz4.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/liblzma.so.5 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libmd.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libmount.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libpango-1.0.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libpangocairo-1.0.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libpangoft2-1.0.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libpcre2-8.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libpcre.so.3 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libpixman-1.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libpng16.so.16 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libpthread.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libresolv.so.2 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/librt.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libselinux.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libthai.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libuuid.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libwayland-client.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libwayland-cursor.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libwayland-egl.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libX11.so.6 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libXau.so.6 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libxcb-render.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libxcb-shm.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libxcb.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libXcomposite.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libXcursor.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libXdamage.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libXdmcp.so.6 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libXext.so.6 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libXfixes.so.3 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libXinerama.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libXi.so.6 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libxkbcommon.so.0 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libXrandr.so.2 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libXrender.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libz.so.1 AppDir/lib
   cp -a /lib/x86_64-linux-gnu/libzstd.so.1 AppDir/lib
   #cp -a /lib/x86_64-linux-gnu/libm.so.6 AppDir/lib
   #cp -a /lib/x86_64-linux-gnu/libc.so.6 AppDir/lib
   #cp -a /lib/x86_64-linux-gnu/libsystemd.so.0 AppDir/lib
fi
if (test "$BITS" = "32") then
   cp -a /lib/i386-linux-gnu/libatk-1.0.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libatk-bridge-2.0.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libatspi.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libblkid.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libbrotlicommon.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libbrotlidec.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libbsd.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libcairo-gobject.so.2 AppDir/lib
   cp -a /lib/i386-linux-gnu/libcairo.so.2 AppDir/lib
   cp -a /lib/i386-linux-gnu/libdatrie.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libdbus-1.so.3 AppDir/lib
   cp -a /lib/i386-linux-gnu/libdl.so.2 AppDir/lib
   cp -a /lib/i386-linux-gnu/libepoxy.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libexpat.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libffi.so.7 AppDir/lib
   cp -a /lib/i386-linux-gnu/libfontconfig.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libfreetype.so.6 AppDir/lib
   cp -a /lib/i386-linux-gnu/libfribidi.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libgcrypt.so.20 AppDir/lib
   cp -a /lib/i386-linux-gnu/libgdk-3.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libgdk_pixbuf-2.0.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libgio-2.0.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libglib-2.0.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libgmodule-2.0.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libgobject-2.0.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libgpg-error.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libgraphite2.so.3 AppDir/lib
   cp -a /lib/i386-linux-gnu/libgtk-3.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libharfbuzz.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/liblz4.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/liblzma.so.5 AppDir/lib
   cp -a /lib/i386-linux-gnu/libmd.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libmount.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libpango-1.0.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libpangocairo-1.0.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libpangoft2-1.0.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libpcre2-8.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libpcre.so.3 AppDir/lib
   cp -a /lib/i386-linux-gnu/libpixman-1.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libpng16.so.16 AppDir/lib
   cp -a /lib/i386-linux-gnu/libpthread.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libresolv.so.2 AppDir/lib
   cp -a /lib/i386-linux-gnu/librt.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libselinux.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libthai.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libuuid.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libwayland-client.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libwayland-cursor.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libwayland-egl.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libX11.so.6 AppDir/lib
   cp -a /lib/i386-linux-gnu/libXau.so.6 AppDir/lib
   cp -a /lib/i386-linux-gnu/libxcb-render.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libxcb-shm.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libxcb.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libXcomposite.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libXcursor.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libXdamage.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libXdmcp.so.6 AppDir/lib
   cp -a /lib/i386-linux-gnu/libXext.so.6 AppDir/lib
   cp -a /lib/i386-linux-gnu/libXfixes.so.3 AppDir/lib
   cp -a /lib/i386-linux-gnu/libXinerama.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libXi.so.6 AppDir/lib
   cp -a /lib/i386-linux-gnu/libxkbcommon.so.0 AppDir/lib
   cp -a /lib/i386-linux-gnu/libXrandr.so.2 AppDir/lib
   cp -a /lib/i386-linux-gnu/libXrender.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libz.so.1 AppDir/lib
   cp -a /lib/i386-linux-gnu/libzstd.so.1 AppDir/lib
   #cp -a /lib/i386-linux-gnu/libm.so.6 AppDir/lib
   #cp -a /lib/i386-linux-gnu/libc.so.6 AppDir/lib
   #cp -a /lib/i386-linux-gnu/libsystemd.so.0 AppDir/lib
fi
cp reSolveConf.txt reSolve.glade circuit*.png AppDir
cp reSolve.desktop ReSolve.png AppRun AppDir
if (test "$BITS" = "64") then
   cp -a reSolveLinux64    AppDir/reSolve
   cp -a reSolveGuiLinux64 AppDir/reSolveGui
fi
if (test "$BITS" = "32") then
   cp -a reSolveLinux32    AppDir/reSolve
   cp -a reSolveGuiLinux32 AppDir/reSolveGui
fi
if (test "$BITS" = "64") then
   if (! test -x appimagetool-x86_64.AppImage) then
      wget "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"
      chmod +x appimagetool-x86_64.AppImage
   fi
   appimagetool-x86_64.AppImage AppDir
   mv ReSolveGui-x86_64.AppImage ..
fi
if (test "$BITS" = "32") then
   if (! test -x appimagetool-i686.AppImage) then
      wget "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-i686.AppImage"
      chmod +x appimagetool-i686.AppImage
   fi
   appimagetool-i686.AppImage AppDir
   mv ReSolveGui-i686.AppImage ..
fi

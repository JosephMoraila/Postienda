# wxWidgets Setup Guide

This guide explains how to download, compile, and install **wxWidgets 3.3.1+** on **Windows** and **Linux** for static builds.

---

## Download

Download the source code from [wxWidgets official website](https://www.wxwidgets.org/downloads/) and extract it. Use version **3.3.1 or higher**.

---

## Windows

1- Extract and add root to path environment and name it WXWIN

2- Go to ```\build\msw```

3- Compile to x64 release static: ```nmake /f makefile.vc BUILD=release SHARED=0 CFG=release TARGET_CPU=amd64 OBJDIR_SUFFIX=rl```

4- Compile to x64 debug static: ```nmake /f makefile.vc BUILD=debug SHARED=0 CFG=debug TARGET_CPU=amd64 OBJDIR_SUFFIX=drl```

5- After that folders with vc_x64_librelease and vc_x64_libdebug will be created


## Linux:

1- Install:   ```gcc-c++ make automake autoconf libtool \
  gtk3-devel \
  libjpeg-turbo-devel \
  libpng-devel \
  libtiff-devel \
  zlib-devel \
  expat-devel \
  webp-devel \
  pcre2-devel \
  gstreamer1-devel gstreamer1-plugins-base-devel```

2- Download source code and extract

3- ```cd wxWidgets-3.3.1```, then ```mkdir gtk-build``` and ```cd gtk-build```

4- ```../configure \
  --prefix=/opt/wxlinux \
  --with-gtk=3 \
  --disable-shared \
  --enable-richtext \
  --enable-stc \
  --enable-xrc \
  --enable-html \
  --with-libpng=sys \
  --with-libjpeg=sys \
  --with-libtiff=sys \
  --with-regex=sys```

5- Compile: ```make -j$(nproc)```

6- Install wxWidgets on /opt/wxlinux: ```sudo make install```

7- Update cache: ```sudo ldconfig```

8- Add to path, so cmake can find it, edit path with: ```nano ~/.bashrc```

9- Add at the end of the file: ```export PATH=/opt/wxlinux/bin:$PATH```

10- Save and reload with: ```source ~/.bashrc```


# OpenSSL Setup Guide

This guide explains how to download, compile, and install **OpenSSL** on **Windows** for static builds.


## Download

Clone with: ```git clone https://github.com/microsoft/vcpkg.git``` on any folder and **vcpkg.exe** from https://github.com/microsoft/vcpkg-tool/releases and place it on root vcpkg\ you just created


## Windows

1.- On your root \vcpkg copy this command: ```.\vcpkg.exe install openssl:x64-windows``` (If you there's an error by command it go to https://github.com/PowerShell/PowerShell/releases and download the latest version of this file: PowerShell-7.5.4-win-x64.msi)

2.- Then run this command: ```.\vcpkg.exe integrate install```. This links vcpkg to Visual Studio

3.- Export root folder to the path. You can do it with this command on PowerShell: ```setx VCPKG_ROOT {YourUserPath}\vcpkg```

4.- If previously ```.\vcpkg.exe install openssl:x64-windows``` didn't work you can try again. Download OpenSSL and compile can take time.


## Demonstration:

Linux:

![Linux](images/demostration_products_ticket.png)


Windows


![Windows](images/demostration_products_ticket_windows.png)
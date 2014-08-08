#
# ============================================================================
# COMCAST C O N F I D E N T I A L AND PROPRIETARY
# ============================================================================
# This file (and its contents) are the intellectual property of Comcast.  It may
# not be used, copied, distributed or otherwise  disclosed in whole or in part
# without the express written permission of Comcast.
# ============================================================================
# Copyright (c) 2014 Comcast. All rights reserved.
# ============================================================================
#
PWD=`pwd`
export RDK_DIR=`echo $PWD/..`
source soc/build/soc_env.sh


copyLibs=0
instalLibs=0
setupSDK=0
buildMode="build"
instaldir=build

for option in $@
do
  echo option = $option
  if [ "$option" = "clean" ]; then
    buildMode="clean"
  elif [ "$option" = "install" -o  "$option" = "instal" ] ; then
    instalLibs=1
  elif [ "$option" = "copy"  ] ; then
    copyLibs=1
  elif [ "$option" = "setup" ] ; then
    setupSDK=1
 fi


done

if [ $setupSDK -eq 1 ];
then
  echo $PLATFORM_SDK
  if [ -d "$PLATFORM_SDK" ]
  then
	  echo "toolchain is already installed..."
  else
	  echo Installing toolchain, it may take few seconds depends on your system
	  tar zxf $RDK_DIR/sdk/toolchain/staging_dir.tgz -C $RDK_DIR/sdk/toolchain
	  echo "toolchain installed $PLATFORM_SDK"
  fi

  echo $ROOTFS
  if [ -d "$ROOTFS" ]
  then
	  echo "ROOT FS is already extracted..."
  else
	  echo extracting ROOT FS, it may take few seconds depends on your system
	  sudo tar zxf $RDK_DIR/sdk/fsroot/fsroot.tgz -C $RDK_DIR/sdk/fsroot
	  sudo tar zxf $RDK_DIR/sdk/fsroot/curl.tgz -C $RDK_DIR/sdk/fsroot
	  sudo tar zxf $RDK_DIR/sdk/fsroot/mafLib.tgz -C $RDK_DIR/sdk/fsroot
	  echo "ROOT FS is extracted $ROOTFS"
  fi

fi

if [ "$buildMode" = "clean" ];
then
  echo "Cleaning the build and $instaldir"
  make clean
fi

if [ $copyLibs -ne 1 ] ;
then
  echo =========================================================================================================================================================
  echo --------------------------BUILDING RDK LOGGER ---------------------------------
  echo =========================================================================================================================================================
  make
  if [ $? -ne 0 ] ;
  then
      echo RDK LOGGER build failed 
      exit -1
  fi
  echo ========================================================================================================================================================
  echo --------------------------BUILDING RDK LOGGER DONE ---------------------------------
  echo ========================================================================================================================================================
fi

if [ $instalLibs -eq 1 ];
then
  echo installing binaries....
  mkdir -p $instaldir/env
  mkdir -p $ROOTFS/mnt/nfs/env
  mkdir -p $ROOTFS/mnt/nfs/lib

  cp *.ini $instaldir/env
  cp log4crc $instaldir/env
  cp -a $instaldir/env/* $ROOTFS/mnt/nfs/env
  mkdir -p $ROOTFS/usr/lib
  cp -a $instaldir/lib/* $ROOTFS/usr/lib
fi

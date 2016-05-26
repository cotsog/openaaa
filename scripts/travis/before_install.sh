#!/bin/sh
set -e
./scripts/version.sh $TRAVIS_BRANCH
export VERSION="$BUILD_MAJOR"
export PATCHLEVEL="$BUILD_MINOR"
export SUBLEVEL="$BUILD_REVISION"
echo "build-version: $VERSION"
echo "build-target: $BUILD_TARGET"
if [ "$BUILD_TARGET" == "win32" ]; then 
  unset CC 
  export CROSS_COMPILE="i686-w64-mingw32-"
  export MINGW=/opt/mingw64 
  export PATH=$MINGW/bin:$PATH 
fi 
if [ "$BUILD_TARGET" == "win64" ]; then 
  unset CC 
  export CROSS_COMPILE="x86_64-w64-mingw32-"
  export MINGW=/opt/mingw64 
  export PATH=$MINGW/bin:$PATH 
fi
if [ "$TRAVIS_OS_NAME" == "linux" ]; then
  sudo apt-get -qq update 
  sudo apt-get install flex bison gperf libncurses-dev pkg-config 
  sudo apt-get install binutils-mingw-w64-i686 gcc-mingw-w64-i686 
  sudo apt-get install binutils-mingw-w64 gcc-mingw-w64 
  fi
if [ "$TRAVIS_OS_NAME" == "osx" ]; then
  unset CROSS_COMPILE 
  brew update 
  brew install flex bison gperftools 
fi



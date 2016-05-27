#!/bin/sh
set -e
export BUILD_MAJOR=0
export BUILD_MINOR=0
export BUILD_REVISION=1

num=`echo $TRAVIS_BRANCH | cut -d. -f1`
if [ $num = *[[:digit:]]* ]; then export BUILD_MAJOR=$num ; fi
num=`echo $TRAVIS_BRANCH | cut -d. -f2`
if [ $num = *[[:digit:]]* ]; then export BUILD_MINOR=$num ; fi
num=`echo $TRAVIS_BRANCH | cut -d. -f3`
if [ $num = *[[:digit:]]* ]; then export BUILD_REVISION=$num ; fi

export BUILD_OS_RELEASE=$(uname -r)
export BUILD_OS_NAME=$(uname -s)

export VERSION="$BUILD_MAJOR"
export PATCHLEVEL="$BUILD_MINOR"
export SUBLEVEL="$BUILD_REVISION"

if [ "$BUILD_TARGET" == "win32" ]; then 
  unset CC 
  export CROSS_COMPILE="i686-w64-mingw32-"
  export MINGW=/opt/mingw64 
  export PATH=$PATH:$MINGW/bin
  export BUILD_OS_NAME="win"
  export BUILD_OS_RELEASE="x86"
fi 
if [ "$BUILD_TARGET" == "win64" ]; then 
  unset CC 
  export CROSS_COMPILE="x86_64-w64-mingw32-"
  export MINGW=/opt/mingw64 
  export PATH=$PATH:$MINGW/bin
  export BUILD_OS_NAME="win"
  export BUILD_OS_RELEASE="x86_64"
fi
if [ "$TRAVIS_OS_NAME" == "osx" ]; then
  unset CROSS_COMPILE 
  export BUILD_OS_NAME="osx"
  brew update 
  brew install flex bison gperftools 
fi

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
  export BUILD_OS_NAME="linux"
fi

echo "build-version: $VERSION"
echo "build-target: $BUILD_TARGET"
echo "build-release: $BUILD_OS_RELEASE"
echo "build-name: $BUILD_OS_NAME"
echo "build-release: $BUILD_MAJOR $BUILD_MINOR $BUILD_REVISION"

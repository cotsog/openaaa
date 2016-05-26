#!/bin/sh
export BUILD_MAJOR=`echo $1 | cut -d. -f1`
export BUILD_MINOR=`echo $1 | cut -d. -f2`
export BUILD_REVISION=`echo $1 | cut -d. -f3`
echo "release: $BUILD_MAJOR $BUILD_MINOR $BUILD_REVISION"

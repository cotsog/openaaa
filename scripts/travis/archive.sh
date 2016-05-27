#!/bin/sh
#files:=$(find . -regex ".*\.\(dll\|so\|dylib\|exe\)")
files=$(find ./obj -perm -111 -type f)
echo "Deploying $1 to GitHub Releases"
echo "$files" | xargs tar -czvf $1

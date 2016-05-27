#!/bin/sh
#files:=$(find . -regex ".*\.\(dll\|so\|dylib\|exe\)")
#files=$(find ./obj -perm -u=x -type f)
files=$(find ./obj -type f -exec test -x {} \; -print)
echo "archive-files: $files"
echo "$files" | xargs tar -czvf $1
echo "Deploying $1 to GitHub Releases"

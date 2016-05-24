#!/bin/bash
PID=`ps -ef | grep "firefox" | grep -v "grep" | awk '{print $2}'`
lsof -p $PID | awk '{print $9}' | grep '\.so'

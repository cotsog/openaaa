#!/bin/bash
kill -9 `ps -ef | grep "marvin/idle" | grep -v "grep" | awk '{print $2}'`
kill -9 `ps -ef | grep "marvin/cpu" | grep -v "grep" | awk '{print $2}'`

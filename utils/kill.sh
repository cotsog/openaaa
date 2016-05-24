#!/bin/bash
kill -9 `ps -ef | grep "marvin.*idle\|marvin.*cpu" | grep -v "grep" | awk '{print $2}'`

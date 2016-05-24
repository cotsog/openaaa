#!/bin/bash
kill -SIGHUP `ps -ef | grep "marvin.*idle" | grep -v "grep" | awk '{print $2}'`

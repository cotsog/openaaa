#!/bin/bash
kill -SIGTERM `ps -ef | grep "marvin/idle" | grep -v "grep" | awk '{print $2}'`

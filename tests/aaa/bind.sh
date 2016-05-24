#!/bin/bash
SID=$@
./run/bin/aaa --id $SID --bind --key user.id --val 1 --set --commit
./run/bin/aaa --id $SID --bind --key user.name --val nobody --set --commit
./run/bin/aaa --id $SID --bind --key user.email --val nobody@example.com --set --commit
./run/bin/aaa --id $SID --bind --dump

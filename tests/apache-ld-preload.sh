#!/bin/bash
#export LD_DEBUG=all
export UNSET_RTLD_DEEPBIND=1
export LD_BIND_NOW=
export LD_PRELOAD=/home/n13l/git/aaa-sec/run/lib/libcrypto5705.so.1.0.0
/opt/apache2/bin/httpd -f /opt/apache2/conf/httpd.conf -X

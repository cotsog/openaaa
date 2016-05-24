#!/bin/bash
sudo ipcs -s | sudo perl -alne 'qx(ipcrm -s $F[1])'

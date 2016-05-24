#!/bin/bash
cp $HOME/git/aaa-sec/obj/src/aaa/libaaa.dll .
cp $HOME/git/aaa-sec/obj/src/aaa/net/tls/libaaa_tls.dll .
cp $HOME/git/aaa-sec/obj/src/aaa/net/vpn/libaaa_vpn.dll .
./openvpn.exe --config ../config/alucid-test.ovpn

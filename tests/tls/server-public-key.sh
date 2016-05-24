#!/bin/bash
openssl rsa -in ./tests/ssl/server.pem -pubout
openssl x509 -modulus -noout < ./tests/ssl/server.pem | sed s/Modulus=/0x/

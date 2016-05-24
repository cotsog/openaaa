#!/bin/bash
openssl req -new -nodes -out ca/client.req.pem -keyout ca/client.key.pem -days 365 -config ca/conf/caconfig.cnf
openssl ca -passin pass:password -out ca/client.cert.pem -days 365 -config ca/conf/caconfig.cnf -infiles ca/client.req.pem

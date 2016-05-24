#!/bin/bash
openssl req -new -nodes -out ca/server.req.pem -keyout ca/server.key.pem -config ca/conf/caconfig.cnf
openssl ca -passin pass:password -out ca/server.cert.pem -config ca/conf/caconfig.cnf -infiles ca/server.req.pem

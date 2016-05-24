openssl req -passout pass:password -passin pass:password -new -x509 -extensions v3_ca -keyout ca/cakey.pem -out ca/cacert.pem -days 365 -config ca/conf/caconfig.cnf
openssl ca -passin pass:password -gencrl -keyfile ca/cakey.pem -cert ca/cacert.pem -out ca/cacrl.pem -config ca/conf/caconfig.cnf -crldays 365

openssl req -new -nodes -out ca/server.req.pem -keyout ca/server.key.pem -config ca/conf/caconfig.cnf
openssl ca -passin pass:password -out ca/server.cert.pem -config ca/conf/caconfig.cnf -infiles ca/server.req.pem

openssl req -new -nodes -out ca/client.req.pem -keyout ca/client.key.pem -days 365 -config ca/conf/caconfig.cnf
openssl ca -passin pass:password -out ca/client.cert.pem -days 365 -config ca/conf/caconfig.cnf -infiles ca/client.req.pem

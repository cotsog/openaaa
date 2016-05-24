openssl req -passout pass:password -passin pass:password -new -x509 -extensions v3_ca -keyout ca/cakey.pem -out ca/cacert.pem -days 365 -config ca/conf/caconfig.cnf
openssl ca -passin pass:password -gencrl -keyfile ca/cakey.pem -cert ca/cacert.pem -out ca/cacrl.pem -config ca/conf/caconfig.cnf -crldays 365

OPENSSL_CONF=./openssl.cnf
OPENAAA_CONF=./openaaa.cnf
OPENAAA_HOST=aaa.anect.cz

HTTP_GET="GET / HTTP/1.0\n\n"
echo "$HTTP_GET" | openssl s_client -state -tlsextdebug -connect $OPENAAA_HOST:443

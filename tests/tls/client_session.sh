#/bin/bash
openssl s_client -sess_out ./ssl_sess.id -CApath ../cacerts -cert ./ca/client.cert.pem -key ./ca/client.key.pem -no_ticket -reconnect -state -prexit -connect 127.0.0.1:3000

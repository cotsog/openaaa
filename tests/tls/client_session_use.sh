#/bin/bash
printf "GET / HTTP/1.1\nHost: hostname\nConnection: keep-alive\n\n" \
| openssl s_client -sess_in /tmp/ssl_sess.id -keymatexport EXPERIMENTAL -no_ticket -tlsextdebug -crlf -ign_eof -prexit -auth -state -no_ticket -prexit -state -connect 127.0.0.1:443

#/bin/bash
printf "GET / HTTP/1.1\nHost: hostname\nConnection: keep-alive\n\n"\
| openssl s_client -sess_out /tmp/ssl_sess.id -no_ticket -tlsextdebug -crlf -ign_eof -prexit -auth -keymatexport EXPERIMENTAL -state -connect 127.0.0.1:443 -nextprotoneg tls-auth

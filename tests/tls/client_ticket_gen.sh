#/bin/bash
openssl s_client -sess_out /tmp/ssl_sess_ticket.id -keymatexport EXPERIMENTAL -connect 127.0.0.1:443 -tlsextdebug

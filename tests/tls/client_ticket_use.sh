#/bin/bash
openssl s_client -sess_in /tmp/ssl_sess_ticket.id -keymatexport EXPERIMENTAL -connect 127.0.0.1:4443

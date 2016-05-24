#!/bin/sh
openssl s_client -sess_out /tmp/ssl_sess_ticket.id -keymatexport EXPERIMENTAL -connect 127.0.0.1:4443 -tlsextdebug  -nextprotoneg tls-auth

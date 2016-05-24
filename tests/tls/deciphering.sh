#!/bin/bash
sudo tethereal -r /tmp/tls.pcap -o ssl.keys_list:"0.0.0.0","443","http","/opt/apache2/conf/key.pem" \
               -o ssl.debug_file:"/tmp/tls.log" -V -R http

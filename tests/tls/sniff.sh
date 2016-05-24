#!/bin/bash
sudo tcpdump -i any port 443 -w /tmp/tls.pcap

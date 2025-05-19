#!/bin/sh
#make directory for Unbound logs
mkdir -p /var/log/unbound
#give ownership to pihole
chown pihole:pihole /var/log/unbound

#start Unbound with error checking
echo " [i] Starting Unbound"
#with config mention in https://docs.pi-hole.net/guides/dns/unbound/ 
/usr/sbin/unbound -d -c /etc/unbound/unbound.conf.d/pi-hole.conf & UNBOUND_PID=$!

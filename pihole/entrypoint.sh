#!/bin/sh
#make directory for Unbound logs
mkdir -p /var/log/unbound
#give ownership to pihole
chown pihole:pihole /var/log/unbound

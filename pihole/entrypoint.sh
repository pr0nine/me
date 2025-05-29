#!/bin/sh
#start Unbound with error checking
echo " [i] Starting Unbound"
#with config mention in https://docs.pi-hole.net/guides/dns/unbound/ 
/usr/sbin/unbound -d -c /etc/unbound/unbound.conf.d/pi-hole.conf & UNBOUND_PID=$!

#check if Unbound started successfully
sleep 1
if ps -p "$UNBOUND_PID" > /dev/null 2>&1; then #redirect stderr file descriptor 2 to the same location as stdout file descriptor 1
    echo " [i] Unbound started successfully with PID $UNBOUND_PID"
else
    echo " [i] ERROR: Unbound failed to start. Check /var/log/unbound.log for details."
    #non-zero exit code to fail the container startup
    exit 1
fi

#comment out below line if only using for docker 
echo $'search home\nnameserver 127.0.0.1' >> /etc/resolv.conf

#run the container startup script
exec /usr/bin/start.sh

FROM pihole/pihole:latest
RUN apk update && apk add unbound

COPY unbound.conf /etc/unbound/unbound.conf.d/pi-hole.conf

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]


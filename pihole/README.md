pihole
-------
a custom container image containing pihole and unbound which is a recursive dns server works by recursively hunting through the top-level domains (TLD)s of an url to reach the ip           

in docker this image runs fine but in kubernetes pihole fails to resolve DNS means it can't reach unbound, upon troubleshooting I found out local nameserver record is missing from `/etc/resolv.conf` in the container due overridden by coredns, so add the line below in the `entrypoint.sh` script

```text-x-sh
echo $'search home\nnameserver 127.0.0.1' >> /etc/resolv.conf
```

then start by building the image with the dockerfile with any image tag of choice make sure to update the manifest `pihole.yaml`

```text-x-sh
cd pihole
docker build --tag mepihole .
```

now to use the image in kubernetes we can push to dockerhub or locally export to containerd in our cluster, for the second option, first save the docker image in tar format

```text-x-sh
docker save -o pihole.tar mepihole:latest
```

now to import the tar file in containerd

```text-x-sh
sudo ctr i import pihole.tar
```

to check containerd for the installed image

```text-x-sh
sudo ctr i ls
```

deploying it in kubernetes using

```text-x-sh
kubectl apply -f pihole.yaml
```

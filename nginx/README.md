for deploying the nginx reverse proxy at first create the nginx.conf file stating which domain to listen and pass traffic,

now using this configuration file create a configmap

```text-x-sh
kubectl create configmap nginx --from-file=./nginx.conf
```

at first I thought providing an external ips would make it possible to hit the nginx endpoint on the default web port

```text-x-yaml
 protocol: TCP
      port: 80
      targetPort: 8090
  externalIPs:
    - 198.168.29.39 //ip of loadbalancer or node
```

works at first, then suddenly goes down, guessed that something else is using the port 80, turned out to be traefik ingress controller

deploying a ingress and backend service set to nginx service solves the issue and can reach the nginx endpoint on the default web port
netbird
-------

NetBird is an open-source, zero-trust networking platform that enables the creation of secure, peer-to-peer private networks

first log or sign in to netbird and create a reusable setup key with allow extra dns labels turned on, store it somewhere safe

set the env NB\_SETUP\_KEY to the newly obtained setup key, set the timezone and apply the manifest 

```text-x-sh
kubectl apply -f netbird.yaml
```

a peer should appear on the netbird console
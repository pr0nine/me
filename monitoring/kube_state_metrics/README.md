Kube State Metrics (KSM) is a service that collects and exposes metrics about the state of Kubernetes objects. It listens to the Kubernetes API server and generates metrics for resources like pods, deployments, nodes, and more. Unlike metrics-server, which focuses on resource usage, KSM provides insights into the health and status of Kubernetes objects. This allows for monitoring and alerting based on the state of cluster resources. 

using kustomize to apply manifests all at once 

```text-x-sh
kubectl apply -k .
```
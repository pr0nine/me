prometheus
----------

Prometheus is an open-source systems monitoring and alerting toolkit, it excels at collecting and storing time-series data from various targets within a Kubernetes cluster

create the prometheus-operator custom resource definition, needed for deploying resource of kind prometheus

```text-x-sh
kubectl create -f https://raw.githubusercontent.com/prometheus-operator/prometheus-operator/master/bundle.yaml
```

create the ClusterRole, ServiceAccount and join them with ClusterRoleBinding all in `prom_rbac.yaml`

```text-x-sh
kubectl apply -f prom_rbac.yaml
```

now deploy the prometheus instance 

```text-x-sh
kubectl apply -f prometheus.yaml
```

expose the prometheus instance with it's service

```text-x-sh
kubectl apply -f prom_svc.yaml
```

Prometheus Operator abstracts away the implementation details of configuring Kubernetes service discovery and scrapes targets using this ServiceMonitor resource. A ServiceMonitor defines a set of targets for Prometheus to monitor and scrape.

```text-x-yaml
apiVersion: monitoring.coreos.com/v1
kind: ServiceMonitor
metadata:
  namespace: default
  name: myapp
  labels:
    app: myapp
spec:
  endpoints:
  - interval: 30s
    port: metrics_port #select the port which exposes metrics to scrape
  selector: # add the label on the service that we want to scrape in this selector
    matchLabels:
      app.kubernetes.io/name: service_name #Select the container service to scrape
```
node\_exporter
--------------

Prometheus Node Exporter is a tool that collects and exposes system-level metrics from Linux and Unix-like systems, making them available for Prometheus to scrape and monitor. It provides detailed insights into CPU usage, memory consumption, disk I/O, network statistics, and more, enabling comprehensive monitoring of server health and performance.

apply the manifest to deploy node exporter

```text-x-sh
kubectl apply -f nodeexp.yaml
```
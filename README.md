🚀 me
-----

This home lab 🏡 leverages **Kubernetes** to orchestrate containers and manage services, with a little bit internet of things sprinkled :

*   Containerized services
*   Self-hosted tools
*   Monitoring and observability
*   Internet Of Things (IOT)

### 🧰 Core Technologies

| Component | Tool/Stack |
| --- | --- |
| Container Runtime | Containerd |
| Orchestration | Kubernetes ~k3s |
| Networking | Ingress-TRAEFIK |
| Storage | ~hostPath for now |
| GitOps | ~FluxCD |
| CI/CD | ~GithubActions + FluxCD |
| Monitoring | Prometheus + UptimeKuma |
| DNS | CoreDNS, Pi-hole + Unbound |
| Dashboard | Grafana |

### 🏗️ Architecture

```text-plain
+-------------------+
|    Local DNS      |
|  (Pi-hole/CoreDNS)|
+--------+----------+
         |
+--------v---------+       +---------------+
|  Load Balancer   |<----->| Reverse Proxy |
|     Ingress      |       | 	(NGINX)    |            
|	 (TRAEFIK)     |       +--------+------+
+--------+---------+                |
         |                          v
+--------v---------+       +------------------+
|   Kubernetes     |<----->| Self-hosted Apps |
|   Cluster (k3s)  |       +------------------+
+------------------+
```

### 📦 Deployed Services

| App | Purpose |
| --- | --- |
| Pi-hole | Ad blocker DNS |
| Postgres | Database for services |
| Trilium | Note taking and knowledge base |
| Uptime Kuma | Monitoring endpoints and alerting |
| Wallabag | Read later for webpages |
| Prometheus | Monitoring metrics |
| Grafana | Visualization |
| Adminer | UI for managing Postgres |
| NetBird | VPN Tunnel for remote access |

### 🛠️ Installation

#### Pre-Requisites:

*   Linux-based machines or VMs
*   Basic networking
*   [kubectl](https://kubernetes.io/docs/tasks/tools/)
*   [k3s](https://k3s.io/) or Kubernetes installed

#### Basic Setup:

```text-x-sh
# Install k3s run as root
curl -sfL https://get.k3s.io | sh -

# Check cluster status
kubectl get nodes
```

### 🔐 Security Practices

*   Network policies for pod isolation
*   Regular image updates

### 📈 Monitoring

*   **Prometheus**: Metrics collection
*   **Grafana**: Dashboards for CPU, memory, disk, and services
*   **Uptime Kuma**: HTTP checks and alerts

### 🌐 IOT

Go to the iot directory for full description [click here](https://github.com/pr0nine/me/tree/eff595dfc4e200690e1935b37283c41279e691f1/iot)

### 📜 License

This project is for personal use. Use at your own risk. No warranty provided.

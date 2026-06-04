CI CD
-----

So whenever a new version of pihole comes out I have to manually build the docker image with the latest pihole image and deploy it in my cluster

To get rid of that repetitive task, here I have implemented Continuous Integration (CI) using GitHub Actions, workflows are defined as YAML files located in the [/.github/workflows/](https://github.com/pr0nine/me/tree/56e8768523909caf83aff66c46bff263890f3140/.github/workflows) directory

##### The .github/workflows/update-hole.yml file consist of

1.  the trigger section consist of a cronjob running the pipeline at 4am on day 1 of every month
2.  the first job checks any new pihole version has arrived or not  
3.  on detection of new version the second job build the docker image with the latest pihole image and push it to dockerhub, later also updates the deployment yaml with the new version tag

Now to deploy the container with the latest image in my environment as Continuous Deployment (CD) I am with Flux as the GitOps platform of my choice as it has a minimalistic approach and a smaller footprint than Argo, every resources counts for optimizing performance and cost 

##### So why GitOps over traditional push-based CD 

*   GitOps leverages a declarative, pull-based approach, which makes the git repository single source of truth for the desired state
*   Enhances security by limiting direct access to the production environment for the CI pipeline, GitOps agent operates within the cluster with defined permissions
*   Flexibility, as the both CI and CD pipelines are not dependent of each other, can be swap anytime without effecting the other one

**Start** by installing flux cli by following the official [docs](https://fluxcd.io/flux/installation/#install-the-flux-cli) and bootstrap it for github by following the official [docs](https://fluxcd.io/flux/get-started/#export-your-credentials) 

Currently I don't need the notification-controller and helm-controller CRDs comes with the base installation of Flux 

![](api/attachments/b2EeBbZEltFE/image/Screenshot%20from%202025-12-03%2020-46-11.png)

search for the deployment manifest for both in the gotk-components.yaml file created by flux in the path provided during bootstrap for me it's [fluxcd/cluster/flux-system/gotk-components.yaml](https://github.com/pr0nine/me/blob/56e8768523909caf83aff66c46bff263890f3140/fluxcd/cluster/flux-system/gotk-components.yaml) and change the replicas to 0 , this will keep the deployment but the pods will be evicted

![](api/attachments/i9KfASGPe09y/image/Screenshot%20from%202025-12-03%2020-47-07.png)

Define a source of type `gitrepository.source.toolkit.fluxcd.io`  in the desired namespace that pulls changes from the given repository every 5 minutes inside the cluster

```text-plain
kubectl apply -f fluxcd/cluster/repo.yaml
```

Then define a `kustomization.kustomize.toolkit.fluxcd.io` that uses the  `/pihole/kustomization.yaml` to determine which resources to create, update or delete

```text-plain
kubectl apply -f fluxcd/cluster/pihole-cd.yaml
```
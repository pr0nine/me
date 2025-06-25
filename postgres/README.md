postgres
--------

first create the secret after swapping with your desired credentials in it 

```text-x-sh
kubectl apply -f pgsecret.yaml
```

every script present in the /docker-entrypoint-initdb.d/ directory will be executed on start up, below line will create the replication user on our half at first i need to manually exec into the postgres container to add the replication user, and comment it out if not plan of using the replication server

```text-x-sh
echo "CREATE USER ${REPLICATION_USER} REPLICATION LOGIN ENCRYPTED PASSWORD '${REPLICATION_PASSWORD}';" > /docker-entrypoint-initdb.d/init.sql
```

now deploy the primary postgres instance 

```text-x-sh
kubectl apply -f postgres.yaml
```

to test replication exec into the postgres container and add a test table

```text-x-sh
kubectl exec -it postgres-0 -- bash
```

login with username and database\_name

```text-x-sh
psql --username=postgres postgres
```

to check for replication user

```text-x-sql
\du
```

create the test table

```text-x-sql
CREATE TABLE cars (
  Sl_No serial,
  brand VARCHAR(55),
  model VARCHAR(55),
  engine_cc SMALLINT,
  hp SMALLINT,
  tq_nm SMALLINT,
  doors SMALLINT
);
```

insert some data

```text-x-sql
INSERT INTO cars (brand, model, engine_cc, hp, tq_nm, doors)
VALUES
  ('Ford', 'Mustang_ecoboost', 2300, 315, 475, 2),
  ('Toyota', 'Supra_mk5', 3000, 382, 499, 2);
```

check for the created table 

```text-x-sql
\dt
```

ctrl + d to exit out

now to deploy the replication server 

```text-x-sh
kubectl apply -f postgres-1.yaml
```

use previous steps to exec into the postgres-1-0 container and check for the test table if present, replication is successfull!

if the primary postgres instance fails we can promote the readonly secondary instance as primary, exec into the secondary instance, switch user to superuser of postgres for me it is postgres

```text-x-sh
su postgres
```

use pg\_ctl to promote by

```text-x-sh
pg_ctl promote
```

adminer
-------

a web-based, open-source database management tool written in PHP

i am using a theme for adminer to apply it, first download the css file

```text-x-sh
curl https://raw.githubusercontent.com/Niyko/Hydra-Dark-Theme-for-Adminer/refs/heads/master/adminer.css > adminer.css
```

 create a configmap from it 

```text-x-sh
kubectl create configmap adminer --from-file=./adminer.css
```

if not using the theme just remove the configmap volume and volumemounts from `adminer.yaml`

```text-x-yaml
volumeMounts:
        - name: theme
          mountPath: /var/www/html/adminer.css
          subPath: adminer.css
      volumes:
        - name: theme
          configMap:
            name: adminer
```

now apply the adminer manifest

```text-x-sh
kubectl apply -f adminer.yaml
```
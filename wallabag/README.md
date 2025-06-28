wallabag
--------

Wallabag is a free and open-source, self-hosted read-it-later service, allows us to save web pages for offline reading, removing distractions like ads and navigation

first we need to create a database in postgres let's name it wallabag

```text-x-sql
CREATE DATABASE wallabag;
```

create the user which wallabag will use, naming it wallabag

```text-x-sql
CREATE USER wallabag WITH PASSWORD 'wallapass';
```

now pass the ownership of the database to the user

```text-x-sql
ALTER DATABASE wallabag OWNER TO wallabag;
```

all of the above queries can be run from adminer or by exec into the postgres container

apply the wallabag manifest

```text-x-sh
kubectl apply -f walla.yaml
```

now exec into the wallabag container

```text-x-sh
kubectl exec -it wallabag-0 -- sh
```

run this to setup the tables in the wallabag database

```text-x-sh
/var/www/wallabag/bin/console wallabag:install --env=prod --no-interaction
```
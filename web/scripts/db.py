docker run -d --name timescaledb -p 5432:5432 -e POSTGRES_PASSWORD=password timescale/timescaledb:latest-pg12-oss
docker exec -it timescaledb psql -U postgres

#-v /your/data/dir:/var/lib/postgresql/data
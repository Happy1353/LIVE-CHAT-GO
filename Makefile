postgresinit:
    docker run --name postgres15 -p 5432 -e POSTGRES_USER=postgres -e POSTGRES_PASSWORD=123 -d postgres:15-alpine

postgres:
    docker exec -it postgres15 psql -U postgres

createdb:
    docker exec -it postgres15 createdb --username=postgres --owner=postgres live-chat

dropdb:
    docker exec -it postgres15 dropdb live-chat

.PHONY: postgresinit
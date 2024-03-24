run:
	go run cmd/main.go

postgresinit:
	docker run --name postgres -p 5432:5432 -e POSTGRES_USER=root -e POSTGRES_PASSWORD=123 -d postgres:15-alpine

postgres:
	docker exec -it postgres psql -U root

createdb:
	docker exec -it postgres createdb --username=root --owner=root chat

dropdb:
	docker exec -it postgres dropdb chat

.PHONY: run
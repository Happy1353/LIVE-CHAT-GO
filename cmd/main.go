package main

import (
	"chat/pkg/storage"
	"log"
)

func main() {
	_, err := storage.NewDatabase()
	if err != nil {
		log.Fatal("could not init database connection: &s", err)
	}

}

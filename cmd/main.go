package main

import (
	"live-chat/db"
	"log"
)

func main() {
	_, err := db.NewDatabase()
	if err != nil {
		log.Fatalf("could not init database connection: &s", err)
	}

}

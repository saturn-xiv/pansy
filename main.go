package main

import (
	"log"

	"github.com/saturn-xiv/pansy/app"
)

func main() {
	if err := app.Execute(); err != nil {
		log.Fatalln(err)
	}
}

package main

import (
	"fmt"
	"os"
)

func main() {
	fmt.Println("SUKA")
	target := os.Args[1];
	action := os.Args[2];
	exampleManifestPath := 
	var (
		src string = "input"
		dst string = "output"
		format string = "json"
		manifest string = "manifest.json"
	)
	for i := 3; i < len(os.Args); i++ {
		if os.Args[i] == "--src" || os.Args[i] == "-i" {
			i++
			src = os.Args[i]
		} else if os.Args[i] == "--dst" || os.Args[i] == "-o" {
			i++
			dst = os.Args[i]
		} else if os.Args[i] == "--format" || os.Args[i] == "-f" {
			i++
			format = os.Args[i]
		} else {

		}
	}
	if (target == "manifest" || target == "mani") {
		if action == "-init" {

		}
	}

}

package main

import (
	"fmt"
	"os"
	"io"
	"io/ioutil"
	"strings"
)

func copy(src, dst string) (int64, error) {
	sourceFileStat, err := os.Stat(src)
	if err != nil {
			return 0, err
	}

	if !sourceFileStat.Mode().IsRegular() {
			return 0, fmt.Errorf("%s is not a regular file", src)
	}

	source, err := os.Open(src)
	if err != nil {
			return 0, err
	}
	defer source.Close()

	destination, err := os.Create(dst)
	if err != nil {
			return 0, err
	}
	defer destination.Close()
	nBytes, err := io.Copy(destination, source)
	return nBytes, err
}

func main() {
	exampleManifestPath := "/home/alexthunder/Desktop/ossa/plugin/manifest.json"
	var (
		src string = "input"
		dst string = "output"
		format string = "json"
		manifest string = "./manifest.json"
		showAll bool = false
		subaction string = ""
	)
	startParse := 3
	target := os.Args[1];
	action := os.Args[2];
	if action == "import" || action == "export" {
		subaction = os.Args[3]
		startParse = 4
	}
	if(target == "info"){
		showAll = true;
	}
	for i := startParse; i < len(os.Args); i++ {
		if os.Args[i] == "--src" || os.Args[i] == "-i" {
			i++
			src = os.Args[i]
		} else if os.Args[i] == "--dst" || os.Args[i] == "-o" {
			i++
			dst = os.Args[i]
		} else if os.Args[i] == "--format" || os.Args[i] == "-f" {
			i++
			format = os.Args[i]
		} else if os.Args[i] == "--output-manifest" || os.Args[i] == "-O"{
			i++
			manifest = os.Args[i]
		}
	}
	if (target == "manifest" || target == "mani") {
		if action == "-init" {
			copy(exampleManifestPath, manifest);
		} else if action == "export" {
			file, err := os.Open(manifest);
			if(err != nil){
				fmt.Printf("SUKA");
				// log.Fatal(err);
			}
			data, err := ioutil.ReadAll(file);
			strings.Trim(lines, "\n\r\t")
			lines := strings.Split(string(data), "\"");
			fmt.Println(lines);
			for i := 0; i < len(lines); i++ {
				pline := strings.Trim(lines[i], "{},");
				words := strings.Split(pline, ":");
				fmt.Println()
				fmt.Println()
				fmt.Println()
				fmt.Println()
				fmt.Println()
				fmt.Println(words," ",subaction);
				// strings.Trim(words[0], "\"");
				// strings.Trim(words[1], "\"");
				// if(words[0] == subaction){
				// 	exportData := words[1]
				// 	fmt.Printf("export: %s\n", exportData);
				// }
			}
		}
	}
	if showAll {
		fmt.Printf("Version: 021-A\nsrc: %s\ndst: %s\nformat: %s\nemp: %s\nmanifest:%s\n", src, dst, format, action, exampleManifestPath, manifest)
	}

}

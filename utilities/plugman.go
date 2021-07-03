package main

import ("fmt",
		"os");

func main() {
	target := os.Args[1];
	action := os.Args[2];
	var src, dst, manifetsPath, format string;
	for i := 3; i < len(os.Args); i++ {
		if os.Args[i] == "--src" || os.Args[i] == "-i" {
			src = os.Args[++i]
		} else if os.Args[i] == "--dst" || os.Args[i] == "-o" {
			dst = os.Args[++i]
		} else if os.Args[i] == "--format" || os.Args[i] == "-f" {
			format = os.Args[++i]
		} else {

		}
	}
	if (target == "manifest" || target == "mani") {
		
	}
}

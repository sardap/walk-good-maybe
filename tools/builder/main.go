package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"time"
)

const makeAssetsScriptPath = "make_assets.sh"

func makeAssets() {
	cmd := exec.Command("bash", makeAssetsScriptPath)
	out, err := cmd.CombinedOutput()
	if err != nil {
		fmt.Printf("error running make assets %v\n", err)
		panic(err)
	}
	fmt.Printf("%s", out)
}

func runMake(arg string) {
	cmd := exec.Command("make", arg)
	out, err := cmd.CombinedOutput()
	if err != nil {
		fmt.Printf("error running make %v\n", err)
		fmt.Printf("out %s", out)
		os.Exit(2)
	}
	fmt.Printf("%s", out)
}

func latestModify(path string) time.Time {
	latest := time.Time{}
	err := filepath.Walk(path,
		func(path string, info os.FileInfo, err error) error {
			if err != nil {
				return err
			}
			if info.ModTime().After(latest) {
				latest = info.ModTime()
			}
			return nil
		})
	if err != nil {
		panic(err)
	}

	return latest
}

func main() {
	//Hacked makefile lamo
	if files, _ := ioutil.ReadDir("source/assets"); os.Args[1] != "clean" &&
		(len(files) == 0 || latestModify("assets").After(latestModify("source/assets"))) {
		makeAssets()
	}

	runMake(os.Args[1])
}

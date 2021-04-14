package main

import (
	"builder/assets"
	"bytes"
	"fmt"
	"io"
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"time"
)

func runMake(arg string) {
	fmt.Printf("running make\n")
	cmd := exec.Command("make", arg)

	var stdBuffer bytes.Buffer
	mw := io.MultiWriter(os.Stdout, &stdBuffer)

	stdIn, _ := cmd.StdinPipe()
	stdIn.Write([]byte("y"))
	stdIn.Close()

	cmd.Stdout = mw
	cmd.Stderr = mw

	err := cmd.Run()
	if err != nil {
		fmt.Printf("error running make %v\n", err)
		os.Exit(2)
	}
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
	buildFilePath := os.Args[1]
	assetsPath := os.Args[2]
	targetPath := os.Args[3]
	cmd := os.Args[4]

	fmt.Printf("Assets: %s Target: %s\n", assetsPath, targetPath)

	if _, err := os.Stat(assetsPath); err != nil {
		panic(err)
	}

	if _, err := os.Stat(targetPath); err != nil {
		os.Mkdir(targetPath, os.ModeDir)
	}

	if cmd == "assets" {
		assets.Make(buildFilePath, assetsPath, targetPath)
		return
	}

	//Hacked makefile lamo
	if files, _ := ioutil.ReadDir("source/assets"); cmd != "clean" &&
		(len(files) == 0 || latestModify("assets").After(latestModify("source/assets"))) {
		assets.Make(buildFilePath, assetsPath, targetPath)
	}

	runMake(cmd)
}

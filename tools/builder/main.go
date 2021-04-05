package main

import (
	"bytes"
	"context"
	"fmt"
	"io"
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"time"
)

const makeAssetsScriptPath = "make_assets.sh"

func makeAssets() {
	fmt.Printf("making assets\n")
	ctx, cancel := context.WithTimeout(context.Background(), time.Duration(60)*time.Second)
	defer cancel()
	cmd := exec.CommandContext(ctx, "bash", makeAssetsScriptPath)

	stdIn, _ := cmd.StdinPipe()
	stdIn.Write([]byte("y"))
	stdIn.Close()

	var stdBuffer bytes.Buffer
	mw := io.MultiWriter(os.Stdout, &stdBuffer)

	cmd.Stdout = mw

	err := cmd.Run()
	if err != nil {
		fmt.Printf("error running make assets %v\n", err)
		panic(err)
	}

	fmt.Println(stdBuffer.String())
}

func runMake(arg string) {
	fmt.Printf("running make\n")
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
	if os.Args[1] == "assets" {
		makeAssets()
		return
	}

	//Hacked makefile lamo
	if files, _ := ioutil.ReadDir("source/assets"); os.Args[1] != "clean" &&
		(len(files) == 0 || latestModify("assets").After(latestModify("source/assets"))) {
		makeAssets()
	}

	runMake(os.Args[1])
}

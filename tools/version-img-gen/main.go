package main

import (
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"os"
	"os/exec"
	"strconv"
	"strings"

	"github.com/pbnjay/pixfont"
)

func getVersion() string {
	descCmd := exec.Command("git", "describe", "--exact-match")
	version, err := descCmd.CombinedOutput()
	if err != nil {
		cmd := exec.Command("git", "describe", "--tags", "--abbrev=0")
		out, err := cmd.CombinedOutput()
		if err != nil {
			fmt.Printf("error running make %v\n", err)
			fmt.Printf("out %s", out)
			os.Exit(2)
		}

		splits := strings.Split(string(out), ".")

		bumpNum, _ := strconv.ParseInt(splits[1], 10, 64)

		bumpNum++

		version = []byte(fmt.Sprintf("%s.%d.%s", splits[0], bumpNum, splits[2]))
	}

	return string(version)
}

func main() {
	textImg := image.NewRGBA(image.Rect(0, 0, 64, 16))

	pixfont.DrawString(textImg, 0, 0, getVersion(), color.White)

	for y := 0; y < textImg.Rect.Max.Y; y++ {
		for x := 0; x < textImg.Rect.Max.X; x++ {
			if textImg.RGBAAt(x, y).A == 0 {
				textImg.Set(x, y, color.RGBA{R: 255, G: 0, B: 246, A: 255})
			}
		}
	}

	inF, _ := os.Open(os.Args[1])
	bgImg, _ := png.Decode(inF)
	inF.Close()

	b := bgImg.Bounds()
	dst := image.NewRGBA(image.Rect(0, 0, b.Dx(), b.Dy()))
	draw.Draw(dst, dst.Bounds(), bgImg, b.Min, draw.Src)
	draw.Draw(dst, textImg.Bounds().Add(image.Point{X: 0, Y: 160 - 8}), textImg, image.Point{}, draw.Over)

	os.Remove(os.Args[1])
	f, _ := os.Create(os.Args[1])
	defer f.Close()
	png.Encode(f, dst)
}

package main

import (
	"image"
	"image/color"
	"image/png"
	_ "image/png"
	"os"
)

var (
	coloursMap map[color.Color]bool = make(map[color.Color]bool)
)

func findColours(img image.Image) {
	for x := 0; x < img.Bounds().Size().X; x++ {
		for y := 0; y < img.Bounds().Size().Y; y++ {
			coloursMap[img.At(x, y)] = true
		}
	}
}

func createColourFile(outfile string) {
	newImg := image.NewRGBA(image.Rectangle{
		image.Point{0, 0}, image.Point{255, 1},
	})

	x := 0
	for k := range coloursMap {
		newImg.Set(x, 0, k)
		x++
	}

	f, _ := os.Create(outfile)
	png.Encode(f, newImg)

}

func getAllColours(files []string) {
	for _, v := range files {
		file, err := os.Open(v)
		if err != nil {
			panic(err)
		}
		img, _, err := image.Decode(file)
		if err != nil {
			panic(err)
		}
		findColours(img)
	}
}

func main() {
	getAllColours(os.Args[2:])
	createColourFile(os.Args[1])
}

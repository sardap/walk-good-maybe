package main

import (
	"image"
	"image/color"
	"image/png"
	_ "image/png"
	"os"
)

func findColours(img image.Image) map[color.Color]bool {
	result := make(map[color.Color]bool)

	for x := 0; x < img.Bounds().Size().X; x++ {
		for y := 0; y < img.Bounds().Size().Y; y++ {
			result[img.At(x, y)] = true
		}
	}

	return result
}

func createColourFile(outfile string, coloursMap map[color.Color]bool) {
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

func getAllColours(files []string) map[color.Color]bool {
	result := make(map[color.Color]bool)

	for _, v := range files {
		file, err := os.Open(v)
		if err != nil {
			panic(err)
		}

		img, _, err := image.Decode(file)
		if err != nil {
			panic(err)
		}

		for k, v := range findColours(img) {
			result[k] = v
		}
	}

	return result
}

func main() {
	colours := getAllColours(os.Args[2:])
	createColourFile(os.Args[1], colours)
}

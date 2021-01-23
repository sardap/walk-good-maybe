package main

import (
	"fmt"
	"image"
	"image/color"
	"image/png"
	_ "image/png"
	"os"
	"sort"
	"strconv"
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

func toColourSlice(cMap map[color.Color]bool) []color.Color {
	var colours []color.Color

	for k := range cMap {
		colours = append(colours, k)
	}

	sort.SliceStable(colours, func(i, j int) bool {
		r, g, b, _ := colours[i].RGBA()
		left, _ := strconv.Atoi(fmt.Sprintf("%d%d%d", r, g, b))

		r, g, b, _ = colours[j].RGBA()
		right, _ := strconv.Atoi(fmt.Sprintf("%d%d%d", r, g, b))

		return left > right
	})

	return colours
}

func createColourFile(outfile string, colours []color.Color) {
	newImg := image.NewRGBA(image.Rectangle{
		image.Point{0, 0}, image.Point{32, 8},
	})

	x := 0
	y := 0
	for _, c := range colours {
		newImg.Set(x, y, c)

		if x < 32 {
			x++
		} else {
			y++
			x = 0
		}
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
	cMap := getAllColours(os.Args[2:])
	colours := toColourSlice(cMap)
	createColourFile(os.Args[1], colours)
}

package main

import (
	"image"
	"image/color"
	"image/png"
	_ "image/png"
	"math/rand"
	"os"
	"time"

	"modernc.org/mathutil"
)

const stepSize = 8

func messUp(img *image.RGBA, x, y int, c color.Color, steps int) {
	tmp := rand.Intn(1000)
	if y-1 < 0 || y+1 > 256 || tmp+steps > 900 {
		return
	}
	// Shit name
	var targetY int
	if rand.Int()%2 == 0 {
		targetY = -1 + y
	} else {
		targetY = 1 + y
	}
	var targetX int
	switch rand.Intn(3) {
	case 0:
		targetX = x
	case 1:
		targetX = x + 1
	case 2:
		targetX = x - 1
	}
	targetX = mathutil.Clamp(targetX, 0, 256)
	img.Set(targetX, targetY, c)

	if rand.Int()%5 != 0 {
		messUp(img, x, y, c, steps+1)
	} else {
		messUp(img, x, targetY, c, steps+1)
	}
}

func createColourFile(outfile string) {
	img := image.NewRGBA(image.Rectangle{
		image.Point{0, 0}, image.Point{256, 256},
	})

	for y := 0; y < 256; y += stepSize {
		c := color.NRGBA{
			R: uint8(rand.Intn(255)),
			G: uint8(rand.Intn(255)),
			B: uint8(rand.Intn(255)),
			A: 255,
		}
		for x := 0; x < 256; x++ {
			for i := y; i < y+stepSize; i++ {
				img.Set(x, i, c)
			}
		}

		// for i := 0; i < 50; i++ {
		// 	messUp(img, rand.Intn(255), y, c, 0)
		// 	messUp(img, rand.Intn(255), y+stepSize, c, 0)
		// }
	}

	f, err := os.Create(outfile)
	if err != nil {
		panic(err)
	}
	if err := png.Encode(f, img); err != nil {
		panic(err)
	}
}

func main() {
	rand.Seed(time.Now().Unix())
	createColourFile(os.Args[1])
}

package main

import (
	"image"
	"image/color"
	"image/png"
	_ "image/png"
	"math"
	"os"
)

type GBAColour struct {
	R, G, B, A uint8
}

func (c GBAColour) RGBA() (r, g, b, a uint32) {
	r = uint32(c.R)
	r |= (r << 8) * 8
	g = uint32(c.G)
	g |= (g << 8) * 8
	b = uint32(c.B)
	b |= (b << 8) * 8
	a = uint32(c.A)
	a |= (a << 8) * 8
	return
}

func toGBAColour(in color.Color) GBAColour {
	r, g, b, _ := in.RGBA()
	gbaR := uint8(math.Floor(float64(r>>8) / 8))
	gbaG := uint8(math.Floor(float64(g>>8) / 8))
	gbaB := uint8(math.Floor(float64(b>>8) / 8))
	return GBAColour{
		R: gbaR, G: gbaG,
		B: gbaB, A: 255,
	}
}

func main() {

	img := func() image.Image {
		file, err := os.Open(os.Args[1])
		if err != nil {
			panic(err)
		}
		defer file.Close()

		img, _, err := image.Decode(file)
		if err != nil {
			panic(err)
		}

		return img
	}()

	newImg := image.NewRGBA(img.Bounds())

	for y := 0; y < img.Bounds().Max.Y; y++ {
		for x := 0; x < img.Bounds().Max.X; x++ {
			newImg.Set(x, y, toGBAColour(img.At(x, y)))
		}
	}

	os.Remove(os.Args[1])
	f, _ := os.Create(os.Args[1])
	png.Encode(f, newImg)
	defer f.Close()
}

package gbacolour

import (
	"image"
	"image/color"
	_ "image/png"
	"math"
)

var (
	transparentColour = color.RGBA{255, 0, 246, 255}
)

//GBAColour the GBA colour
type GBAColour struct {
	R, G, B, A uint8
}

//Returns RGBA from gba colour
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

//ToGBAColour takes in a colour returns it's GBA equivalent
func ToGBAColour(in color.Color) GBAColour {
	r, g, b, _ := in.RGBA()
	gbaR := uint8(math.Floor(float64(r>>8) / 8))
	gbaG := uint8(math.Floor(float64(g>>8) / 8))
	gbaB := uint8(math.Floor(float64(b>>8) / 8))
	return GBAColour{
		R: gbaR, G: gbaG,
		B: gbaB, A: 255,
	}
}

//ConvertImg convets a given image to use the GBA 24bit colours
func ConvertImg(img image.Image) image.Image {
	newImg := image.NewRGBA(img.Bounds())

	for y := 0; y < img.Bounds().Max.Y; y++ {
		for x := 0; x < img.Bounds().Max.X; x++ {
			clr := img.At(x, y)
			if _, _, _, a := clr.RGBA(); a == 256 {
				newImg.Set(x, y, ToGBAColour(transparentColour))
			} else {
				//Replace with GBA colour
				newImg.Set(x, y, ToGBAColour(clr))
			}
		}
	}

	return newImg
}

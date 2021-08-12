package assets

import (
	"builder/gbacolour"
	"image"
	"image/color"
	"image/draw"
	"image/png"

	"bytes"
	"context"
	"fmt"
	"io"
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
	"sync"
	"time"

	"github.com/pbnjay/pixfont"

	_ "github.com/oov/psd"

	"github.com/pelletier/go-toml"
)

var (
	// Grit Really doens't like async shit
	gritMutex = &sync.Mutex{}
)

type GraphicsOutput struct {
	Name    string
	Options []string
	Files   []string
	Dirs    []string
}

func (g *GraphicsOutput) addFilesFromDirs(assetsPath string) {
	for _, dir := range g.Dirs {
		dFiles, err := ioutil.ReadDir(filepath.Join(assetsPath, dir))
		if err != nil {
			panic(err)
		}

		for _, f := range dFiles {
			if filepath.Ext(f.Name()) == ".png" {
				g.Files = append(
					g.Files,
					filepath.Join(
						dir,
						strings.TrimSuffix(f.Name(), ".png"),
					),
				)
			}
		}
	}
}

func (g *GraphicsOutput) clearTargetFiles(targetPath string) {
	//Clear shared files
	for _, o := range g.Options {
		if strings.Contains(o, "-O ") {
			sharedFilename := strings.Split(o, " ")[1]
			os.Remove(filepath.Join(
				targetPath, fmt.Sprintf("%s.h", sharedFilename)),
			)
			os.Remove(filepath.Join(
				targetPath, fmt.Sprintf("%s.c", sharedFilename)),
			)
		}
	}

	for _, filename := range g.Files {
		filename := filepath.Base(filename)
		os.Remove(filepath.Join(targetPath, fmt.Sprintf("%s.h", filename)))
		os.Remove(filepath.Join(targetPath, fmt.Sprintf("%s.c", filename)))
	}
}

func (g *GraphicsOutput) changedFiles(
	updatedFiles map[string]bool,
) []string {
	var result []string

	for _, f := range g.Files {
		baseFile := filepath.Base(f)

		if _, ok := updatedFiles[baseFile]; ok {
			result = append(result, baseFile)
		}
	}

	return result
}

//Make makes assets
func Make(buildFilePath, assetsPath, targetPath string) {

	buildFile, err := os.ReadFile(buildFilePath)
	if err != nil {
		panic(err)
	}

	var config struct {
		Targets []GraphicsOutput
	}
	if err := toml.Unmarshal(buildFile, &config); err != nil {
		panic(err)
	}

	ctx, cancel := context.WithTimeout(context.Background(), time.Duration(60)*time.Second)
	defer cancel()

	updatedFiles := make(map[string]bool)
	ch := make(chan map[string]bool)
	qCh := make(chan struct{})
	var wg sync.WaitGroup

	filepath.Walk(assetsPath, func(path string, info os.FileInfo, err error) error {
		if !info.IsDir() {
			return nil
		}

		wg.Add(1)
		go func(ch chan map[string]bool, wg *sync.WaitGroup) {
			defer wg.Done()
			ch <- genPngs(ctx, targetPath, path)
			fmt.Printf("Done pnging %s\n", path)
		}(ch, &wg)
		return nil
	})

	go func() {
		wg.Wait()
		close(qCh)
	}()

	complete := false
	for !complete {
		select {
		case msg := <-ch:
			updatedFiles = mapUnion(updatedFiles, msg)
		case <-qCh:
			complete = true
		}
	}

	fmt.Printf("\nRunning targets\n")

	//Change into dir for grit
	workingDir, _ := os.Getwd()
	os.Chdir(targetPath)
	defer os.Chdir(workingDir)

	for _, target := range config.Targets {
		target.addFilesFromDirs(assetsPath)
		targetChangedFiles := target.changedFiles(updatedFiles)
		if len(targetChangedFiles) == 0 {
			continue
		}

		fmt.Printf("\nBuilding %s\n", target.Name)
		fmt.Printf("Files changed %v\n", targetChangedFiles)

		target.clearTargetFiles(targetPath)

		err := gritCall(ctx, assetsPath, targetPath, &target)
		if err != nil && !strings.Contains(err.Error(), "signal: segmentation fault") {
			fmt.Printf("Grit error Probably doesn't matter %v\n", err)
		}
	}
}

func mapUnion(a, b map[string]bool) map[string]bool {
	for k, v := range b {
		a[k] = v
	}

	return a
}

func toPngFileName(filename string) string {
	return fmt.Sprintf("%s.png", strings.TrimSuffix(filename, ".psd"))
}

func gritCall(ctx context.Context, assetsPath, targetPath string, target *GraphicsOutput) error {
	gritMutex.Lock()
	defer gritMutex.Unlock()

	var arguments []string
	for _, f := range target.Files {
		arguments = append(arguments, fmt.Sprintf("%s.png", filepath.Join(assetsPath, f)))
	}
	for _, option := range target.Options {
		arguments = append(arguments, strings.Split(option, " ")...)
	}

	cmd := exec.CommandContext(ctx, "grit", arguments...)

	var stdBuffer bytes.Buffer
	mw := io.MultiWriter(os.Stdout, &stdBuffer)
	cmd.Stdout = mw
	cmd.Stderr = mw

	return cmd.Run()
}

func toPng(ctx context.Context, filename string) {
	ch := make(chan struct{})

	go func() {
		file, err := os.Open(filename)
		if err != nil {
			panic(err)
		}
		defer file.Close()

		img, _, err := image.Decode(file)
		if err != nil {
			panic(err)
		}

		if strings.Contains(filename, "tsTitleText") {
			img = overlayVersion(img)
		}

		img = gbacolour.ConvertImg(img)

		os.Remove(toPngFileName(filename))
		pngFile, _ := os.Create(toPngFileName(filename))
		defer pngFile.Close()
		png.Encode(pngFile, img)

		close(ch)
	}()

	select {
	case <-ctx.Done():
		if ctx.Err() != nil {
			panic(ctx.Err())
		}
	case <-ch:
	}
}

func getGenratedAsset(generatedAssetsPath, filename string) string {
	baseFile := filepath.Base(filename)
	baseFile = baseFile[:len(baseFile)-4]

	return filepath.Join(generatedAssetsPath, fmt.Sprintf("%s.h", baseFile))
}

func getVersion() string {
	cmd := exec.Command("git", "describe", "--tags", "--abbrev=0")
	out, err := cmd.CombinedOutput()
	if err != nil {
		fmt.Printf("out %s", out)
		os.Exit(2)
	}

	return string(out)
}

func overlayVersion(inImg image.Image) image.Image {
	textImg := image.NewRGBA(image.Rect(0, 0, 64, 16))

	pixfont.DrawString(textImg, 0, 0, getVersion(), color.White)

	for y := 0; y < textImg.Rect.Max.Y; y++ {
		for x := 0; x < textImg.Rect.Max.X; x++ {
			if textImg.RGBAAt(x, y).A == 0 {
				textImg.Set(x, y, color.RGBA{R: 255, G: 0, B: 246, A: 255})
			}
		}
	}

	b := inImg.Bounds()
	dst := image.NewRGBA(image.Rect(0, 0, b.Dx(), b.Dy()))
	draw.Draw(dst, dst.Bounds(), inImg, b.Min, draw.Src)
	draw.Draw(dst, textImg.Bounds().Add(image.Point{X: 0, Y: 160 - 8}), textImg, image.Point{}, draw.Over)

	return dst
}

func genPngs(ctx context.Context, generatedAssetsPath, targetDir string) map[string]bool {
	items, err := ioutil.ReadDir(targetDir)
	if err != nil {
		panic(err)
	}

	result := make(map[string]bool)
	for _, item := range items {
		if item.IsDir() {
			continue
		}

		if filepath.Ext(item.Name()) == ".psd" {
			psdFile := filepath.Join(targetDir, item.Name())
			psdStat, _ := os.Stat(psdFile)

			baseFile := strings.TrimSuffix(filepath.Base(item.Name()), ".psd")

			assetStat, genErr := os.Stat(getGenratedAsset(generatedAssetsPath, item.Name()))
			if genErr != nil || psdStat.ModTime().After(assetStat.ModTime()) {
				toPng(ctx, psdFile)
				result[filepath.Base(baseFile)] = true
			}

		}
	}

	return result
}

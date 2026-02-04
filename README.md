# raytracer

Basic ray tracer implementation in C++ following [_Ray Tracing In One Weekend_](https://raytracing.github.io/books/RayTracingInOneWeekend.html)

## Features

- Full implementation of the ray tracing tutorial
- **Parallel rendering with OpenMP** for significantly faster image generation
- Graceful fallback to single-threaded mode if OpenMP is unavailable

## Rendered Outputs

The following images were rendered using this raytracer (located in `build/`):

| Image | Description |
|-------|-------------|
| `image.ppm` | Basic render output |
| `antialias.ppm` | Antialiasing demonstration |
| `fov.ppm` | Field of view showcase |
| `fuzz.ppm` | Fuzzy/rough metal materials |
| `hollow.ppm` | Hollow glass sphere effect |
| `dof-spheres.ppm` | Depth of field with spheres |
| `final-scene.ppm` | Final scene composition |
| `final-scene-hq.ppm` | High quality final scene |

> **Note:** PPM files can be viewed with image viewers like GIMP, ImageMagick, or converted to PNG/JPG using tools like `convert image.ppm image.png`

## Building

```bash
mkdir build && cd build
cmake ..
make
./main
```

### OpenMP 

This project uses OpenMP for parallel rendering, which significantly speeds up image generation. The build will work without OpenMP, but rendering will be single-threaded and slower.

**macOS:**
```bash
brew install libomp
```

**Ubuntu/Debian:**
```bash
sudo apt install libomp-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install libomp-devel
```

**Windows:**
OpenMP is typically included with Visual Studio. For MinGW, install via MSYS2.

If OpenMP is not found during the build, you'll see a warning but the project will still compile and run in single-threaded mode.

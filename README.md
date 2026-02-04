# raytracer

Basic ray tracer implementation in C++ following [_Ray Tracing In One Weekend_](https://raytracing.github.io/books/RayTracingInOneWeekend.html)

## Features

- Full implementation of the ray tracing tutorial
- **Parallel rendering with OpenMP** for significantly faster image generation
- Fallback to single-threaded mode if OpenMP is unavailable

## Performance

OpenMP parallelization provides a significant speedup for rendering. Below is a comparison of the final high-quality scene render times:

| Mode | Wall Clock Time | CPU Usage |
|------|-----------------|-----------|
| **Multi-threaded** (default) | 6m 7s 
| **Single-threaded** (`OMP_NUM_THREADS=1`) | >20m (I got impatient and ended build lol)

To force single-threaded execution for testing:
```bash
OMP_NUM_THREADS=1 ./main > output.ppm
```

## Rendered Outputs

### Antialiasing
![Antialiasing](images/antialias.png)

### Basic Render
![Basic](images/image.png)

### Field of View
![FOV](images/fov.png)

### Fuzzy Metal Materials
![Fuzz](images/fuzz.png)

### Hollow Glass Sphere
![Hollow](images/hollow.png)

### Depth of Field
![Depth of Field](images/dof-spheres.png)

### Final Scene (samples_per_pixel = 10)
![Final Scene](images/final-scene.png)

### Final Scene (samples_per_pixel = 500)
![Final Scene HQ](images/final-scene-hq.png)

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
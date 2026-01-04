# Computer Graphics Assignment 2

## Project Overview
This project implements a ray tracing engine in two parts:
- **Part 1**: CPU-based ray tracer in C++ (offline rendering)
- **Part 2**: GPU-based ray tracer in WebGL2 (real-time rendering)

Both implementations support spheres, planes, multiple lighting models, reflections, refractions, and various material types.

---

## Part 1: CPU Ray Tracer (C++)

### Overview
A high-quality offline ray tracing engine implemented in C++ that generates high-resolution images from scene descriptions.

### Key Features
- **Object Types**: Spheres and planes
- **Material Types**: Standard (Phong), Mirror (reflective), Glass (refractive)
- **Lighting**: Ambient, directional lights, spotlights with shadows
- **Effects**: Checkerboard patterns on planes, recursive reflections, refraction through transparent spheres
- **Output**: 800x800 PNG images saved to `bin/results/`

### Implementation Details
- Object-oriented design with class hierarchy
- GLM library for mathematics
- STB Image Write for PNG output
- Scene files parsed from text format

### Build Instructions
```bash
cd Part1
make          # Build the project
make clean    # Clean build files
make test     # Build and run
```

### Usage
```bash
cd Part1/bin
raytracer.exe
```

Output images are saved in `Part1/bin/results/` with names matching scene files (e.g., `scene1.txt` → `scene1.png`).

### Requirements
- C++17 compatible compiler
- GLM library (included in `include/glm`)
- Make (for building)

For detailed documentation, see [Part1/README.md](Part1/README.md).

---

## Part 2: WebGL2 Real-Time Ray Tracer

### Overview
A real-time ray tracing implementation using WebGL2 fragment shaders. The entire ray tracing algorithm runs on the GPU, enabling interactive rendering in web browsers.

### Key Features
- **Real-Time Rendering**: GPU-accelerated ray tracing
- **Interactive**: Runs at interactive frame rates
- **Same Features as Part 1**: Supports all material types, lighting, and effects
- **Web-Based**: Runs in any WebGL2-compatible browser

### Implementation Details
- Fragment shader-based ray tracing (`raytracer.frag`)
- WebGL2 for GPU computation
- JavaScript for scene setup and WebGL management
- GL-Matrix library for matrix operations

### Setup Instructions
```bash
cd Part2
npm install
```

### Usage
1. Start a local web server:
   ```bash
   python -m http.server 8000
   # or
   npx http-server
   ```

2. Open `index.html` in a WebGL2-compatible browser

3. The raytracer runs in real-time with FPS counter displayed

### Requirements
- WebGL2-compatible browser (Chrome, Firefox, Edge)
- Local web server (required for loading shader files)
- Node.js (for npm install, optional)

### Modified Files
- **`shaders/raytracer.frag`** - Complete ray tracing implementation in fragment shader

For detailed documentation, see [Part2/README.md](Part2/README.md).

---

## Project Structure

```
ComputerGraphic-Ass2/
├── Part1/                 # CPU Ray Tracer (C++)
│   ├── src/               # Source code
│   ├── include/           # GLM library
│   ├── bin/               # Build output and results
│   ├── Makefile           # Build configuration
│   └── README.md          # Part 1 documentation
│
├── Part2/                 # WebGL2 Ray Tracer
│   ├── shaders/           # GLSL shaders
│   ├── *.js               # JavaScript files
│   ├── index.html         # Web page
│   ├── package.json       # Dependencies
│   └── README.md          # Part 2 documentation
│
└── README.md              # This file
```

---

## Comparison: Part 1 vs Part 2

| Feature | Part 1 (CPU) | Part 2 (GPU) |
|---------|--------------|--------------|
| **Platform** | C++ executable | Web browser |
| **Rendering** | Offline (pre-rendered) | Real-time (interactive) |
| **Performance** | High quality, slower | Real-time, optimized |
| **Output** | PNG images | Canvas display |
| **Scene Format** | Text files | JavaScript objects |
| **Max Objects** | Unlimited | 16 spheres, 1 plane |
| **Max Lights** | Unlimited | 4 lights |
| **Recursion Depth** | 5 levels | 5 levels |

---

## Common Features (Both Parts)

### Material Types
1. **Standard** - Phong shading with ambient, diffuse, and specular
2. **Mirror** - Perfect reflection with recursive ray tracing
3. **Glass** - Refraction with specular highlights

### Lighting
- Ambient lighting
- Directional lights
- Spotlights with cone angles
- Shadow calculation

### Special Effects
- Checkerboard pattern on planes
- Recursive reflections
- Refraction through transparent spheres
- Specular highlights

---

## Development Notes

### Part 1 Changes
- Refactored from original implementation to match example structure
- Renamed variables and functions for clarity
- Changed enum names (NORMAL→STANDARD, REFLECTIVE→MIRROR, TRANSPARENT→GLASS)
- Modified class member names and organization
- Output files saved to `results/` directory with scene-based naming

### Part 2 Changes
- Implemented complete ray tracing in `raytracer.frag` fragment shader
- All ray tracing logic runs on GPU
- Real-time rendering with FPS counter

---

## License
This project is part of a Computer Graphics course assignment.


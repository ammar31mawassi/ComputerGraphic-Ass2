# Ray Tracer Implementation

## Overview
This is a ray tracing engine implementation that renders 3D scenes with support for spheres, planes, various lighting models, reflections, and refractions.

## Project Structure

### Core Classes

#### Object Hierarchy
- **`Object.h/cpp`** - Base class for all scene objects
  - Manages material properties (color, shininess)
  - Defines material types: `STANDARD`, `MIRROR`, `GLASS`
  - Virtual methods for intersection and normal calculation

- **`Sphere.h/cpp`** - Sphere primitive
  - Implements ray-sphere intersection using geometric method
  - Calculates surface normals

- **`Plane.h/cpp`** - Plane primitive
  - Implements ray-plane intersection
  - Supports checkerboard pattern for planes

#### Ray and Intersection
- **`Ray.h/cpp`** - Ray representation
  - Stores origin and direction
  - Method `at(t)` computes point along ray

- **`Intersection.h/cpp`** - Intersection data structure
  - Stores intersection point and object reference
  - Utility for distance calculations

#### Lighting System
- **`Light.h`** - Base light class
  - Manages direction and intensity
  - Virtual methods for light type identification

- **`Ambient.h/cpp`** - Ambient light implementation
  - Provides uniform scene illumination

- **`DirectionalLight.h/cpp`** - Directional light
  - Infinite distance light source

- **`SpotLight.h/cpp`** - Spotlight implementation
  - Positional light with cone angle cutoff
  - Supports position and angle configuration

### Main Engine

#### `main.cpp` - Core Ray Tracing Engine

**Camera System:**
- `initCameraState()` - Initializes camera position and orientation
- `setupCamera(width, height)` - Sets up camera coordinate system and screen dimensions
- `createPixelRay(px, py, width, height)` - Generates ray through pixel coordinates

**Scene Parsing:**
- `processSceneLine(line, lights, objects, ambientColor)` - Parses individual scene file lines
  - Handles commands: `e` (eye), `u` (up), `f` (forward), `a` (ambient)
  - Processes objects: `o` (standard), `r` (reflective), `t` (transparent)
  - Manages lights: `d` (directional/spotlight), `p` (spotlight position), `i` (intensity), `c` (color)
- `loadSceneFile(filename, lights, objects, ambientColor)` - Loads and parses entire scene file

**Intersection Detection:**
- `invalidIntersection(pt)` - Checks if intersection point is valid
- `findNearestIntersection(ray, objects, origin, closestObj, intersectionPt)` - Finds closest object intersection along ray

**Shadow Calculation:**
- `checkShadow(pt, lightDir, lightDist, objects)` - Determines if point is in shadow from light source

**Material and Lighting:**
- `computeCheckerboard(baseColor, pt, n)` - Generates checkerboard pattern for planes
- `getObjectColor(obj, pt)` - Gets object color (with checkerboard for planes)
- `computeDiffuse(obj, pt, light, lightDir)` - Calculates diffuse lighting contribution
- `computeSpecular(obj, pt, eyePos, light, lightDir)` - Calculates specular highlights
- `computeLighting(obj, pt, eyePos, ambient, lights, objects)` - Computes total Phong lighting

**Ray Tracing:**
- `castRay(ray, objects, lights, ambient, recursionDepth)` - Main recursive ray tracing function
  - Handles reflections for mirror materials
  - Implements refraction for transparent materials (glass)
  - Adds specular highlights to transparent objects
  - Maximum recursion depth: 5

**Image Output:**
- `writeImage(filename, width, height, pixels)` - Saves rendered image to PNG
- `getOutputFilename(filepath)` - Generates output filename from scene file name
- Output saved to `bin/results/` directory with scene-based naming

## Key Features

### Material Types
1. **Standard** - Phong shading with ambient, diffuse, and specular components
2. **Mirror** - Perfect reflection (recursive ray tracing)
3. **Glass** - Refraction with specular highlights

### Lighting Models
- Ambient lighting (global illumination)
- Directional lights (infinite distance)
- Spotlights (positional with cone angle)
- Shadow calculation for all light types

### Special Effects
- Checkerboard pattern on planes
- Recursive reflections
- Refraction through transparent spheres
- Specular highlights on all materials

## Build Instructions

### Using Makefile
```bash
make          # Build the project
make clean    # Clean build files
make test     # Build and run
```

### Requirements
- C++17 compatible compiler (g++ or clang++)
- GLM library (OpenGL Mathematics)
  - Included in `include/glm` directory
- STB Image Write library (included in `src/stb/`)

## Usage

Run the executable from the `bin/` directory:
```bash
cd bin
raytracer.exe
```

The raytracer processes all scene files in `bin/res/` and generates output images in `bin/results/` with names matching the scene files (e.g., `scene1.txt` → `scene1.png`).

## Scene File Format

Scene files use a simple text format:
- `e x y z d` - Camera eye position and screen distance
- `u x y z h` - Up vector and screen height
- `f x y z w` - Forward vector and screen width
- `a r g b` - Ambient light color
- `d r g b w` - Light direction (w=0 directional, w=1 spotlight)
- `p x y z cutoff` - Spotlight position and angle
- `i r g b` - Light intensity
- `o/r/t` - Object type (standard/reflective/transparent)
- `x y z r` - Object geometry (r>0 sphere, r≤0 plane)
- `c r g b n` - Object color and shininess

## Output

Rendered images are saved as PNG files in `bin/results/`:
- `scene1.png`, `scene2.png`, `scene3.png`, etc.
- Resolution: 800x800 pixels
- Format: RGB (24-bit color)


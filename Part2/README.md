# Part 2: WebGL2 Real-Time Ray Tracer

## Overview
This is a real-time ray tracing implementation using WebGL2 fragment shaders. The ray tracing algorithm runs entirely on the GPU, allowing for interactive rendering in a web browser.

## Implementation Details

### Modified File
- **`shaders/raytracer.frag`** - Fragment shader containing the complete ray tracing implementation

### Key Features Implemented

#### Ray Tracing Core
- **Ray Generation**: Generates rays from camera through each pixel
- **Scene Intersection**: Finds intersections with spheres and planes
- **Recursive Ray Tracing**: Supports up to 5 levels of recursion for reflections and refractions

#### Intersection Functions
- `intersectSphere(ro, rd, s, tHit, nOut)` - Ray-sphere intersection using quadratic formula
- `intersectPlane(ro, rd, p, tHit, nOut)` - Ray-plane intersection
- `intersectScene(ro, rd)` - Finds closest intersection in the scene

#### Material Types
1. **Opaque (TYPE_OPAQUE = 0)** - Standard Phong shading
2. **Reflective (TYPE_REFLECTIVE = 1)** - Perfect mirror reflection
3. **Refractive (TYPE_REFRACTIVE = 2)** - Glass-like refraction with specular highlights

#### Lighting Model
- **Ambient Lighting**: Base scene illumination
- **Diffuse Lighting**: Lambertian reflection based on surface normal and light direction
- **Specular Highlights**: Phong specular reflection with configurable shininess
- **Shadow Calculation**: Determines if points are in shadow from light sources
- **Directional Lights**: Infinite distance light sources
- **Spotlights**: Positional lights with cone angle cutoff

#### Special Effects
- **Checkerboard Pattern**: Applied to planes using `checkerboardColor()` function
- **Reflection**: Recursive ray tracing for mirror surfaces
- **Refraction**: Snell's law implementation for transparent materials
  - Handles entering (air to glass) and exiting (glass to air) refraction
  - Accounts for total internal reflection
  - Adds specular highlights on transparent surfaces

### Shader Structure

#### Uniforms
- `Camera cam` - Camera position and orientation vectors
- `Sphere uSpheres[MAX_SPHERES]` - Array of up to 16 spheres
- `int uNumSpheres` - Number of active spheres
- `Light uLights[MAX_LIGHTS]` - Array of up to 4 lights
- `int uNumLights` - Number of active lights
- `Plane uPlane` - Single plane in the scene
- `float uTime` - Time value for animations
- `ivec2 uResolution` - Screen resolution

#### Data Structures
- `HitInfo` - Stores intersection information (point, normal, color, material type)
- `Camera` - Camera position and basis vectors
- `Sphere` - Sphere center, radius, color, and material type
- `Plane` - Plane point, normal, and color
- `Light` - Light position/direction, color, shininess, and cutoff angle

### Main Functions

1. **`traceRay(ro, rd, depth)`** - Main recursive ray tracing function
   - Handles material-specific rendering (opaque, reflective, refractive)
   - Manages recursion depth to prevent infinite loops
   - Returns final color for the ray

2. **`computeLighting(hit, eyePos)`** - Calculates Phong lighting
   - Computes ambient, diffuse, and specular components
   - Handles shadow calculations
   - Supports both directional and spotlight sources

3. **`checkShadow(hitPoint, lightDir, lightDist)`** - Shadow testing
   - Shoots shadow rays to determine occlusion
   - Handles both infinite (directional) and finite (spotlight) distances

4. **`refractRay(incident, normal, eta)`** - Refraction calculation
   - Implements Snell's law for transparent materials
   - Handles total internal reflection cases

## Usage

### Setup
1. Install dependencies:
   ```bash
   npm install
   ```

2. Serve the files using a local web server (required for WebGL):
   ```bash
   # Using Python
   python -m http.server 8000
   
   # Using Node.js http-server
   npx http-server
   ```

3. Open `index.html` in a WebGL2-compatible browser

### Controls
- The raytracer runs in real-time
- FPS counter displayed in top-left corner
- Scene can be modified through JavaScript in `main.js`

## Technical Notes

- **WebGL2 Required**: Uses `#version 300 es` for ES 3.0 features
- **High Precision**: Uses `precision highp float` for accurate calculations
- **Performance**: Optimized for GPU execution with parallel pixel processing
- **Limitations**: 
  - Maximum 16 spheres
  - Maximum 4 lights
  - Maximum recursion depth of 5
  - Single plane supported

## Files Structure

- `raytracer.frag` - Main fragment shader (modified file)
- `quad.vert` - Vertex shader for full-screen quad
- `main.js` - WebGL setup and scene management
- `camera.js` - Camera implementation
- `light.js` - Light definitions
- `sphere.js` - Sphere definitions
- `plane.js` - Plane definitions
- `index.html` - HTML page and canvas setup


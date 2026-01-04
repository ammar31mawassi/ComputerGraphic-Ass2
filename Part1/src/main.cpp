#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <algorithm>

#include "Illumination.h"
#include "GlobalLight.h"
#include "ConeLight.h"
#include "ParallelLight.h"
#include "Primitive.h"
#include "Sphere.h"
#include "Plane.h"
#include "RayCast.h"
#include "HitResult.h"

#include "stb/stb_image_write.h"

using namespace std;

// ============================================================================
// GLOBAL STATE: Viewport/Camera Configuration
// ============================================================================

Vec3 eyePosition;      // Camera/eye position in world space
Vec3 forwardDir;        // Forward direction vector (normalized)
Vec3 up;                // Up direction vector (normalized)
Vec3 rightDir;          // Right direction vector (normalized)
float focalLength = 0;   // Distance from eye to viewport plane
float viewportHeight = 0;// Height of viewport in world units
float viewportWidth = 0; // Width of viewport in world units

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Reset viewport to default configuration
// Implemented as stated in the PDF: Phase 1 camera defaults match the PDF (screen corners ±1)
void resetCamera() {
    eyePosition = Vec3(0, 0, 1);      // 1 unit in front of the Z=0 screen
    forwardDir  = Vec3(0, 0, -1);     // looks toward the screen
    up          = Vec3(0, 1, 0);
    rightDir    = Vec3(1, 0, 0);

    focalLength     = 1.0f;           // screen at Z=0 from eye at Z=1
    viewportHeight  = 2.0f;           // from -1 to 1
    viewportWidth   = 2.0f;           // from -1 to 1
}

// Check if point is finite (not infinity)
static bool isFinitePoint(const Vec3& pt) {
    return !(std::isinf(pt.x) || std::isinf(pt.y) || std::isinf(pt.z));
}

// Forward declarations
bool isOccluded(const Vec3& pt, const Vec3& lightDirection, const float lightDistance, 
                const std::vector<Primitive*>& objects);

// Calculate light direction and distance for a given light source
// Returns true if light is valid (not blocked by cone angle), false otherwise
static bool calculateLightDirection(Illumination* illum, const Vec3& pt, Vec3& outDirection, float& outDistance) {
    if (illum->isConeType()) {
        auto* cone = dynamic_cast<ConeLight*>(illum);
        Vec3 toLight = cone->getPosition() - pt;
        outDistance = glm::length(toLight);
        outDirection = glm::normalize(toLight);
        
        // Check if point is within cone angle
        Vec3 coneDirection = glm::normalize(cone->getDirection());
        float cosAngle = glm::dot(-outDirection, coneDirection);
        if (cosAngle < cone->getAngle()) return false;  // Outside cone
    } else {
        outDirection = glm::normalize(-illum->getDirection());
        outDistance = std::numeric_limits<float>::infinity();
    }
    return true;
}

// Calculate specular highlight for glass surfaces
static Vec3 calculateGlassSpecular(Primitive* obj, const Vec3& pt, const Vec3& normal, 
                                   const Vec3& viewDir, Illumination* illum, 
                                   const Vec3& lightDir, const std::vector<Primitive*>& objects) {
    if (isOccluded(pt, lightDir, std::numeric_limits<float>::infinity(), objects)) {
        return Vec3(0, 0, 0);
    }
    
    Vec3 reflectionDir = glm::normalize(glm::reflect(-lightDir, normal));
    float specularAngle = glm::max(glm::dot(reflectionDir, viewDir), 0.0f);
    float specularPower = glm::pow(specularAngle, obj->get_shininess());
    return Vec3(1, 1, 1) * specularPower * illum->getColor();
}

// ============================================================================
// SCENE PARSING
// ============================================================================

// Parse and execute a single command from scene file
void handleCommand(const string& line, vector<Illumination*>& illuminators, 
                   vector<Primitive*>& objects, Vec3& ambientLight) {
    stringstream ss(line);
    char cmd;
    ss >> cmd;
    vector<float> values;
    float val;
    while (ss >> val) values.push_back(val);

    switch (cmd) {
    case 'e':  // Eye position
        eyePosition = glm::vec3(values[0], values[1], values[2]); 
        if (values.size() > 3) focalLength = values[3]; 
        break;
    case 'u':  // Up vector
        up = glm::vec3(values[0], values[1], values[2]); 
        if (values.size() > 3) viewportHeight = values[3]; 
        break;
    case 'f':  // Forward direction
        forwardDir = glm::vec3(values[0], values[1], values[2]); 
        // Implemented as stated in the PDF: use the scene's screen width from f ... w (4th value)
        if (values.size() > 3) viewportWidth = values[3];
        break;
    case 'a':  // Ambient light color
        ambientLight = Vec3(values[0], values[1], values[2]); 
        break;
    case 'd':  // Directional or cone light
        if(values[3] == 0.0f) 
            illuminators.push_back(new ParallelLight(glm::vec3(values[0], values[1], values[2])));
        else 
            illuminators.push_back(new ConeLight(glm::vec3(values[0], values[1], values[2])));
        break;
    case 'p':  // Cone light position and angle
        for(Illumination* illum : illuminators) {
            if(illum->isConeType()) {
                auto* cone = dynamic_cast<ConeLight*>(illum);
                if(cone->getAngle() == -1.0f) {
                   cone->setAngle(values[3]); 
                   cone->setPosition(glm::vec3(values[0], values[1], values[2])); 
                   break;
                }
            }
        }
        break;
    case 'i':  // Light intensity/color
        for(Illumination* illum : illuminators) {
            if(!illum->isGlobalType() && !illum->isColorSet()){ 
                illum->setColor(values[0], values[1], values[2]); 
                break; 
            }    
        }
        break;
    case 'o': case 'r': case 't':  // Object (standard/mirror/glass)
        {
            MaterialType matType = (cmd == 'o') ? STANDARD : (cmd == 'r') ? MIRROR : GLASS;
            if(values[3] > 0) 
                objects.push_back(new Sphere(glm::vec3(values[0], values[1], values[2]), values[3], matType));
            else 
                objects.push_back(new Plane(glm::vec3(values[0], values[1], values[2]), values[3], matType));
        }
        break;
    case 'c':  // Color for object
        for(Primitive* obj : objects) {
            if(!obj->is_rgb_set()) { 
                obj->set_rgb(values[0], values[1], values[2], values[3]); 
                break; 
            }
        }
        break;
    default: break;
    }
}

// Read and parse entire scene file
int readScene(const string& filename, vector<Illumination*>& illuminators, 
              vector<Primitive*>& objects, Vec3& ambientLight) {
    ifstream file(filename);
    if (!file.is_open()) return -1;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        handleCommand(line, illuminators, objects, ambientLight);
    }
    return 0;
}

// ============================================================================
// CAMERA/VIEWPORT SETUP
// ============================================================================

// Configure viewport coordinate system based on image dimensions
// Implemented as stated in the PDF: only auto-compute width if it wasn't provided from 'f' command
void configureViewport(int width, int height) {
    forwardDir = glm::normalize(forwardDir);
    up = glm::normalize(up);
    rightDir = glm::normalize(glm::cross(forwardDir, up));
    float aspect = (float)width / height;
    if (viewportWidth == 0.0f) {
        viewportWidth = viewportHeight * aspect;
    }
}

// Create ray from camera through pixel at (px, py)
RayCast generateRay(int px, int py, int width, int height) {
    Vec3 viewportCenter = eyePosition + (forwardDir * focalLength);
    float u = (px + 0.5f) / width - 0.5f;   // Normalized x coordinate [-0.5, 0.5]
    float v = 0.5f - (py + 0.5f) / height;  // Normalized y coordinate [-0.5, 0.5]
    Vec3 pixelLocation = viewportCenter + (rightDir * (u * viewportWidth)) + (up * (v * viewportHeight));
    Vec3 rayDirection = glm::normalize(pixelLocation - eyePosition);
    return RayCast(eyePosition, rayDirection);
}

// ============================================================================
// RAY-OBJECT INTERSECTION
// ============================================================================

// Find closest intersection with scene objects
static bool closestHit(const RayCast& ray, const std::vector<Primitive*>& objects, 
                       const Vec3& rayOrigin, Primitive*& hitObject, Vec3& hitPoint) {
    float closestDistance = std::numeric_limits<float>::infinity();
    hitObject = nullptr;
    for (Primitive* obj : objects) {
        Vec3 intersection = obj->get_intersection(ray);
        if (!isFinitePoint(intersection)) continue;
        float distance = glm::length(intersection - rayOrigin);
        if (distance > 0.001f && distance < closestDistance) { 
            closestDistance = distance; 
            hitObject = obj; 
            hitPoint = intersection; 
        }
    }
    return hitObject != nullptr;
}

// Check if point is occluded from light source (shadow test)
bool isOccluded(const Vec3& pt, const Vec3& lightDirection, const float lightDistance, 
                const std::vector<Primitive*>& objects) {
    RayCast occlusionRay(pt + lightDirection * 0.01f, lightDirection); 
    for (Primitive* obj : objects) {
        Vec3 intersection = obj->get_intersection(occlusionRay);
        if (isFinitePoint(intersection)) {
            float distance = glm::length(intersection - pt);
            if (distance < lightDistance) return true; 
        }
    }
    return false;
}

// ============================================================================
// SHADING AND COLOR
// ============================================================================

// Sample checkerboard pattern for planes
Vec3 samplePattern(Vec3 baseColor, Vec3 pt, Vec3 n) {
    const float tileSize = 0.5f;
    float patternValue = 0;
    
    // Calculate pattern based on x and y coordinates
    if (pt.x < 0) {
        patternValue += floor((0.5 - pt.x) / tileSize);
    } else {
        patternValue += floor(pt.x / tileSize);
    }
    if (pt.y < 0) {
        patternValue += floor((0.5 - pt.y) / tileSize);
    } else {
        patternValue += floor(pt.y / tileSize);
    }
    patternValue = (patternValue * 0.5) - int(patternValue * 0.5);
    patternValue *= 2;

    // Return darker or lighter based on pattern
    return (patternValue > 0.5) ? (0.5f * baseColor) : baseColor;
}

// Get object color (with checkerboard pattern for planes)
Vec3 sampleColor(Primitive* obj, const Vec3& pt) {
    if (dynamic_cast<Plane*>(obj)) {
        return samplePattern(obj->get_rgb(), pt, obj->get_normal(pt));
    }
    return obj->get_rgb();
}

// Returns 1.0 for bright tiles and 0.5 for dark tiles (planes only)
// Implemented as stated in the PDF: checkerboard affects diffuse only, not ambient
float checkerCoeff(Primitive* obj, const Vec3& pt) {
    if (!dynamic_cast<Plane*>(obj)) return 1.0f;

    const float tileSize = 0.5f;
    float patternValue = 0;

    if (pt.x < 0) patternValue += floor((0.5f - pt.x) / tileSize);
    else          patternValue += floor(pt.x / tileSize);

    if (pt.y < 0) patternValue += floor((0.5f - pt.y) / tileSize);
    else          patternValue += floor(pt.y / tileSize);

    patternValue = (patternValue * 0.5f) - int(patternValue * 0.5f);
    patternValue *= 2.0f;

    return (patternValue > 0.5f) ? 0.5f : 1.0f;
}

// Calculate Lambertian diffuse shading component
// Implemented as stated in the PDF: checkerboard affects diffuse only
glm::vec3 lambertianShading(Primitive* obj, const glm::vec3& pt, Illumination* illum, 
                            const glm::vec3& lightDirection) {
    glm::vec3 normal = glm::normalize(obj->get_normal(pt));
    float nDotL = glm::max(glm::dot(normal, lightDirection), 0.0f);
    Vec3 kd = obj->get_rgb() * checkerCoeff(obj, pt);
    return kd * nDotL * illum->getColor();
}

// Calculate Phong specular highlight component
glm::vec3 phongHighlight(Primitive* obj, const glm::vec3& pt, const glm::vec3& eyePos, 
                          Illumination* illum, const glm::vec3& lightDirection) {
    glm::vec3 normal = glm::normalize(obj->get_normal(pt));
    glm::vec3 viewDirection = glm::normalize(eyePos - pt);
    glm::vec3 reflectionDirection = glm::normalize(glm::reflect(-lightDirection, normal));
    float viewDotReflect = glm::max(glm::dot(viewDirection, reflectionDirection), 0.0f);
    float specularPower = glm::pow(viewDotReflect, obj->get_shininess());
    glm::vec3 specularColor(0.7f, 0.7f, 0.7f);
    return specularColor * specularPower * illum->getColor();
}

// Calculate total illumination at point (ambient + diffuse + specular)
// Implemented as stated in the PDF: ambient uses base color only (checkerboard does not affect ambient)
Vec3 calculateIllumination(Primitive* obj, const Vec3& pt, const Vec3& eyePos, 
                           const Vec3& ambient, const std::vector<Illumination*>& illuminators, 
                           const std::vector<Primitive*>& objects) {
    Vec3 finalColor = obj->get_rgb() * ambient; 
    for (Illumination* illum : illuminators) {
        if (illum->isGlobalType()) continue;
        
        Vec3 lightDirection;
        float lightDistance;
        if (!calculateLightDirection(illum, pt, lightDirection, lightDistance)) continue;
        if (isOccluded(pt, lightDirection, lightDistance, objects)) continue;
        
        finalColor += lambertianShading(obj, pt, illum, lightDirection);
        finalColor += phongHighlight(obj, pt, eyePos, illum, lightDirection);
    }
    return finalColor;
}

// ============================================================================
// RAY TRACING
// ============================================================================

// Forward declaration
Vec3 traceRay(const RayCast& ray, const std::vector<Primitive*>& objects, 
              const std::vector<Illumination*>& illuminators, const Vec3& ambient, int bounceCount);

// Handle reflection: calculate reflected ray and trace recursively
static Vec3 handleReflection(Primitive* obj, const Vec3& hitPoint, const RayCast& ray,
                              const std::vector<Primitive*>& objects,
                              const std::vector<Illumination*>& illuminators,
                              const Vec3& ambient, int bounceCount) {
    Vec3 normal = obj->get_normal(hitPoint);
    Vec3 rayDirection = ray.getDirection();
    Vec3 reflectionDirection = glm::reflect(rayDirection, normal);
    RayCast reflectedRay(hitPoint + reflectionDirection * 0.001f, reflectionDirection);
    return traceRay(reflectedRay, objects, illuminators, ambient, bounceCount + 1);
}

// Handle refraction: calculate refracted ray through glass and trace recursively
static Vec3 handleRefraction(Primitive* obj, const Vec3& hitPoint, const RayCast& ray,
                              const std::vector<Primitive*>& objects,
                              const std::vector<Illumination*>& illuminators,
                              const Vec3& ambient, int bounceCount) {
    Vec3 normal = obj->get_normal(hitPoint);
    Vec3 rayDirection = ray.getDirection();
    
    // Entering: Air (n1=1.0) to Glass (n2=1.5)
    const float n1 = 1.0f, n2 = 1.5f; 
    float eta = n1 / n2;
    Vec3 refractedIn = glm::refract(rayDirection, normal, eta);
    
    // Total internal reflection: use reflection instead
    if (glm::length(refractedIn) < 0.01f) {
        refractedIn = glm::reflect(rayDirection, normal); 
    }

    // Find exit point by tracing through object
    RayCast internalRay(hitPoint + refractedIn * 0.01f, refractedIn);
    Vec3 exitPoint = obj->get_intersection(internalRay);
    
    Vec3 refractedColor(0, 0, 0);
    
    if (isFinitePoint(exitPoint)) {
        // Exiting: Glass (n2=1.5) to Air (n1=1.0)
        Vec3 exitNormal = obj->get_normal(exitPoint);
        Vec3 exitNormalInv = -exitNormal;
        
        eta = n2 / n1;
        Vec3 refractedOut = glm::refract(refractedIn, exitNormalInv, eta);
        
        if (glm::length(refractedOut) < 0.01f) refractedOut = refractedIn;

        RayCast exitRay(exitPoint + refractedOut * 0.01f, refractedOut);
        refractedColor = traceRay(exitRay, objects, illuminators, ambient, bounceCount + 1);
    }

    // Implemented as stated in the PDF: transparent objects use refracted color only (ignore material lighting)
    return refractedColor; 
}

// Recursive ray tracing: trace ray through scene and calculate color
Vec3 traceRay(const RayCast& ray, const std::vector<Primitive*>& objects, 
              const std::vector<Illumination*>& illuminators, const Vec3& ambient, int bounceCount) {
    // Limit recursion depth to prevent infinite loops
    if (bounceCount > 5) return Vec3(0, 0, 0); 
    
    Primitive* hitObject = nullptr;
    Vec3 hitPoint;
    
    // Find closest intersection
    if (!closestHit(ray, objects, ray.getOrigin(), hitObject, hitPoint)) {
        return Vec3(0, 0, 0);  // No hit: return black
    }

    // Handle reflective surfaces
    if (hitObject->is_reflective()) {
        return handleReflection(hitObject, hitPoint, ray, objects, illuminators, ambient, bounceCount);
    }

    // Handle transparent surfaces (refraction)
    if (hitObject->is_transparent()) {
        return handleRefraction(hitObject, hitPoint, ray, objects, illuminators, ambient, bounceCount);
    }

    // Standard material: calculate lighting
    return calculateIllumination(hitObject, hitPoint, ray.getOrigin(), ambient, illuminators, objects);
}

// ============================================================================
// IMAGE OUTPUT
// ============================================================================

// Save image buffer to PNG file
static bool savePNG(const std::string& filename, int width, int height, 
                    const std::vector<unsigned char>& pixels) {
    int stride = width * 3;
    return stbi_write_png(filename.c_str(), width, height, 3, pixels.data(), stride) != 0;
}

// Build output file path from scene file path
string buildOutputPath(const string& filepath) {
    size_t lastSlash = filepath.find_last_of("/\\");
    string filename = (lastSlash == string::npos) ? filepath : filepath.substr(lastSlash + 1);
    size_t lastDot = filename.find_last_of(".");
    if (lastDot != string::npos) {
        filename = filename.substr(0, lastDot);
    }
    return "results/" + filename + ".png";
}

// ============================================================================
// RENDERING
// ============================================================================

// Render single scene to image buffer
void renderImage(int width, int height, const std::vector<Primitive*>& objects,
                 const std::vector<Illumination*>& illuminators, const Vec3& ambientLight,
                 std::vector<unsigned char>& image) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            RayCast ray = generateRay(x, y, width, height);
            Vec3 color = traceRay(ray, objects, illuminators, ambientLight, 0);
            color = glm::clamp(color, Vec3(0.0f), Vec3(1.0f));
            
            int pixelIdx = 3 * (y * width + x);
            image[pixelIdx]     = (unsigned char)(255 * color.x);
            image[pixelIdx + 1] = (unsigned char)(255 * color.y);
            image[pixelIdx + 2] = (unsigned char)(255 * color.z);
        }
    }
}

// Process single scene file: load, render, and save
bool processScene(const string& filepath) {
    cout << "--------------------------------------" << endl;
    cout << "Processing: " << filepath << endl;
    resetCamera(); 
    
    vector<Illumination*> illuminators;
    vector<Primitive*> objects;
    Vec3 ambientLight(0.0f, 0.0f, 0.0f);

    // Load scene
    if (readScene(filepath, illuminators, objects, ambientLight) != 0) {
        cerr << "Failed to open: " << filepath << endl;
        return false;
    }

    // Setup viewport
    int width = 800, height = 800;
    configureViewport(width, height);

    // Render image
    vector<unsigned char> image(3 * width * height, 0);
    cout << "Rendering..." << endl;
    renderImage(width, height, objects, illuminators, ambientLight, image);
    
    // Save image
    string outputFile = buildOutputPath(filepath);
    if (!savePNG(outputFile, width, height, image)) {
        cerr << "Failed to write " << outputFile << endl;
        return false;
    }
    cout << "Saved: " << outputFile << endl;

    // Cleanup
    for (Illumination* illum : illuminators) delete illum;
    for (Primitive* obj : objects) delete obj;
    
    return true;
}

// ============================================================================
// MAIN ENTRY POINT
// ============================================================================

int main()
{
    // List of scene files to render
    vector<string> scenes = { 
        "res/scene1.txt", 
        "res/scene2.txt", 
        "res/scene3.txt",
        "res/scene4.txt",
        "res/scene5.txt",
        "res/scene6.txt",
        "res/scene21.txt",
        "res/scene41.txt",
        "res/scene51.txt"
    };

    // Process each scene
    for (const string& filepath : scenes) {
        processScene(filepath);
    }
    
    cout << "--------------------------------------" << endl;
    return 0;
}

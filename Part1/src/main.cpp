#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <algorithm>

#include "Light.h"
#include "Ambient.h"
#include "SpotLight.h"
#include "DirectionalLight.h"
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"
#include "Ray.h"
#include "Intersection.h"

#include "stb/stb_image_write.h"

using namespace std;

// Camera state variables
Vec3 camPos;
Vec3 viewDir;
Vec3 upVec;
Vec3 rightVec;
float screenDistance = 0;
float screenHeight = 0;
float screenWidth = 0;

// Initialize camera state to defaults
void initCameraState() {
    camPos = Vec3(0);
    viewDir = Vec3(0, 0, -1);
    upVec = Vec3(0, 1, 0);
    rightVec = Vec3(1, 0, 0);
    screenDistance = 10.0f;
    screenHeight = 10.0f;
    screenWidth = 0;
}

// Check if intersection point is invalid (infinity)
static bool invalidIntersection(const Vec3& pt) {
    return std::isinf(pt.x) || std::isinf(pt.y) || std::isinf(pt.z);
}

// Process a single line from scene file
void processSceneLine(const string& line, vector<Light*>& lights, vector<Object*>& objects, Vec3& ambientColor) {
    stringstream ss(line);
    char cmd;
    ss >> cmd;
    vector<float> params;
    float param;
    while (ss >> param) params.push_back(param);

    switch (cmd) {
    case 'e': 
        camPos = glm::vec3(params[0], params[1], params[2]); 
        if (params.size() > 3) screenDistance = params[3]; 
        break;
    case 'u': 
        upVec = glm::vec3(params[0], params[1], params[2]); 
        if (params.size() > 3) screenHeight = params[3]; 
        break;
    case 'f': viewDir = glm::vec3(params[0], params[1], params[2]); break;
    case 'a': ambientColor = Vec3(params[0], params[1], params[2]); break;
    case 'd':
        if(params[3] == 0.0f) lights.push_back(new Directional(glm::vec3(params[0], params[1], params[2])));
        else lights.push_back(new Spotlight(glm::vec3(params[0], params[1], params[2])));
        break;
    case 'p':
        for(Light* light : lights) {
            if(light->is_spotlight()) {
                auto* spot = dynamic_cast<Spotlight*>(light);
                if(spot->get_angle() == -1.0f) {
                   spot->set_angle(params[3]); 
                   spot->set_point(glm::vec3(params[0], params[1], params[2])); 
                   break;
                }
            }
        }
        break;
    case 'i':
        for(Light* light : lights) {
            if(!light->is_ambient() && !light->isIntensitySet()){ 
                light->set_intensity(params[0], params[1], params[2]); 
                break; 
            }    
        }
        break;
    case 'o': case 'r': case 't':
        MaterialType matType;
        if(cmd == 'o') matType = STANDARD; 
        else if(cmd == 'r') matType = MIRROR; 
        else matType = GLASS;
        if(params[3] > 0) 
            objects.push_back(new Sphere(glm::vec3(params[0], params[1], params[2]), params[3], matType));
        else 
            objects.push_back(new Plane(glm::vec3(params[0], params[1], params[2]), params[3], matType));
        break;
    case 'c':
        for(Object* obj : objects) {
            if(!obj->is_rgb_set()) { 
                obj->set_rgb(params[0], params[1], params[2], params[3]); 
                break; 
            }
        }
        break;
    default: break;
    }
}

// Load and parse scene file
int loadSceneFile(const string& filename, vector<Light*>& lights, vector<Object*>& objects, Vec3& ambientColor) {
    ifstream file(filename);
    if (!file.is_open()) return -1;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        processSceneLine(line, lights, objects, ambientColor);
    }
    return 0;
}

// Setup camera coordinate system
void setupCamera(int width, int height) {
    viewDir = glm::normalize(viewDir);
    upVec = glm::normalize(upVec);
    rightVec = glm::normalize(glm::cross(viewDir, upVec));
    float aspect = (float)width / height;
    screenWidth = screenHeight * aspect;
}

// Generate ray through pixel
Ray createPixelRay(int px, int py, int width, int height) {
    Vec3 screenCenter = camPos + (viewDir * screenDistance);
    float u = (px + 0.5f) / width - 0.5f; 
    float v = 0.5f - (py + 0.5f) / height; 
    Vec3 pixelPos = screenCenter + (rightVec * (u * screenWidth)) + (upVec * (v * screenHeight));
    Vec3 rayDir = glm::normalize(pixelPos - camPos);
    return Ray(camPos, rayDir);
}

// Find nearest intersection with scene objects
static bool findNearestIntersection(const Ray& ray, const std::vector<Object*>& objects, const Vec3& origin, Object*& closestObj, Vec3& intersectionPt) {
    float minDist = std::numeric_limits<float>::infinity();
    closestObj = nullptr;
    for (Object* obj : objects) {
        Vec3 pt = obj->get_intersection(ray);
        if (invalidIntersection(pt)) continue;
        float dist = glm::length(pt - origin);
        if (dist > 0.001f && dist < minDist) { 
            minDist = dist; 
            closestObj = obj; 
            intersectionPt = pt; 
        }
    }
    return closestObj != nullptr;
}

// Check if point is in shadow from light
bool checkShadow(const Vec3& pt, const Vec3& lightDir, const float lightDist, const std::vector<Object*>& objects) {
    Ray shadowRay(pt + lightDir * 0.01f, lightDir); 
    for (Object* obj : objects) {
        Vec3 p = obj->get_intersection(shadowRay);
        if (!invalidIntersection(p)) {
            float dist = glm::length(p - pt);
            if (dist < lightDist) return true; 
        }
    }
    return false;
}

// Compute checkerboard pattern color for planes
Vec3 computeCheckerboard(Vec3 baseColor, Vec3 pt, Vec3 n) {
    const float tileSize = 0.5f;
    float pattern = 0;
    
    if (pt.x < 0) {
        pattern += floor((0.5 - pt.x) / tileSize);
    } else {
        pattern += floor(pt.x / tileSize);
    }
    if (pt.y < 0) {
        pattern += floor((0.5 - pt.y) / tileSize);
    } else {
        pattern += floor(pt.y / tileSize);
    }
    pattern = (pattern * 0.5) - int(pattern * 0.5);
    pattern *= 2;

    return (pattern > 0.5) ? (0.5f * baseColor) : baseColor;
}

// Get object color (with checkerboard for planes)
Vec3 getObjectColor(Object* obj, const Vec3& pt) {
    if (dynamic_cast<Plane*>(obj)) {
        return computeCheckerboard(obj->get_rgb(), pt, obj->get_normal(pt));
    }
    return obj->get_rgb();
}

// Calculate diffuse lighting contribution
glm::vec3 computeDiffuse(Object* obj, const glm::vec3& pt, Light* light, const glm::vec3& lightDir) {
    glm::vec3 n = glm::normalize(obj->get_normal(pt));
    float ndotl = glm::max(glm::dot(n, lightDir), 0.0f);
    return getObjectColor(obj, pt) * ndotl * light->get_intensity();
}

// Calculate specular lighting contribution
glm::vec3 computeSpecular(Object* obj, const glm::vec3& pt, const glm::vec3& eyePos, Light* light, const glm::vec3& lightDir) {
    glm::vec3 n = glm::normalize(obj->get_normal(pt));
    glm::vec3 viewDir = glm::normalize(eyePos - pt);
    glm::vec3 reflectDir = glm::normalize(glm::reflect(-lightDir, n));
    float vdotr = glm::max(glm::dot(viewDir, reflectDir), 0.0f);
    float specPower = glm::pow(vdotr, obj->get_shininess());
    glm::vec3 specColor(0.7f, 0.7f, 0.7f);
    return specColor * specPower * light->get_intensity();
}

// Compute total lighting at point
Vec3 computeLighting(Object* obj, const Vec3& pt, const Vec3& eyePos, const Vec3& ambient, const std::vector<Light*>& lights, const std::vector<Object*>& objects) {
    Vec3 color = getObjectColor(obj, pt) * ambient; 
    for (Light* light : lights) {
        if (light->is_ambient()) continue;
        Vec3 lightDir;
        float lightDist;
        if (light->is_spotlight()) {
            auto* spot = dynamic_cast<Spotlight*>(light);
            Vec3 toLight = spot->get_point() - pt;
            lightDist = glm::length(toLight);
            lightDir = glm::normalize(toLight);
            Vec3 spotDir = glm::normalize(spot->get_direction());
            float cosAngle = glm::dot(-lightDir, spotDir);
            if (cosAngle < spot->get_angle()) continue; 
        } else {
            lightDir = glm::normalize(-light->get_direction());
            lightDist = std::numeric_limits<float>::infinity();
        }
        if (checkShadow(pt, lightDir, lightDist, objects)) continue;
        color += computeDiffuse(obj, pt, light, lightDir);
        color += computeSpecular(obj, pt, eyePos, light, lightDir);
    }
    return color;
}

// Recursive ray tracing
Vec3 castRay(const Ray& ray, const std::vector<Object*>& objects, const std::vector<Light*>& lights, const Vec3& ambient, int recursionDepth) {
    if (recursionDepth > 5) return Vec3(0,0,0); 
    
    Object* obj = nullptr;
    Vec3 pt;
    
    if (!findNearestIntersection(ray, objects, ray.get_startP(), obj, pt)) {
        return Vec3(0,0,0);
    }

    // Handle reflective surfaces
    if (obj->is_reflective()) {
        Vec3 n = obj->get_normal(pt);
        Vec3 dir = ray.get_directionV();
        Vec3 reflectDir = glm::reflect(dir, n);
        Ray reflectedRay(pt + reflectDir * 0.001f, reflectDir);
        return castRay(reflectedRay, objects, lights, ambient, recursionDepth + 1);
    }

    // Handle transparent surfaces (refraction)
    if (obj->is_transparent()) {
         Vec3 n = obj->get_normal(pt);
         Vec3 dir = ray.get_directionV();
         
         // Entering: Air to Glass
         float n1 = 1.0f, n2 = 1.5f; 
         float eta = n1 / n2;
         Vec3 refractedIn = glm::refract(dir, n, eta);
         
         if (glm::length(refractedIn) < 0.01f) {
             refractedIn = glm::reflect(dir, n); 
         }

         // Find exit point
         Ray internalRay(pt + refractedIn * 0.01f, refractedIn);
         Vec3 exitPt = obj->get_intersection(internalRay);
         
         Vec3 refractedColor(0,0,0);
         
         if (!invalidIntersection(exitPt)) {
             // Exiting: Glass to Air
             Vec3 nExit = obj->get_normal(exitPt);
             Vec3 nExitInv = -nExit;
             
             eta = n2 / n1;
             Vec3 refractedOut = glm::refract(refractedIn, nExitInv, eta);
             
             if (glm::length(refractedOut) < 0.01f) refractedOut = refractedIn;

             Ray exitRay(exitPt + refractedOut * 0.01f, refractedOut);
             refractedColor = castRay(exitRay, objects, lights, ambient, recursionDepth + 1);
         }

         // Add specular highlights
         Vec3 spec(0,0,0);
         for (Light* light : lights) {
             if (light->is_ambient()) continue;
             
             Vec3 lightDir;
             if (light->is_spotlight()) {
                 lightDir = glm::normalize(dynamic_cast<Spotlight*>(light)->get_point() - pt);
             } else {
                 lightDir = glm::normalize(-light->get_direction());
             }

             if (!checkShadow(pt, lightDir, std::numeric_limits<float>::infinity(), objects)) {
                Vec3 reflectDir = glm::normalize(glm::reflect(-lightDir, n));
                Vec3 viewDir = glm::normalize(ray.get_startP() - pt);
                float specAngle = glm::max(glm::dot(reflectDir, viewDir), 0.0f);
                float specPower = glm::pow(specAngle, obj->get_shininess());
                spec += Vec3(1,1,1) * specPower * light->get_intensity(); 
             }
         }
         
         return refractedColor + spec; 
    }

    return computeLighting(obj, pt, ray.get_startP(), ambient, lights, objects);
}

// Save image to PNG file
static bool writeImage(const std::string& filename, int width, int height, const std::vector<unsigned char>& pixels) {
    int stride = width * 3;
    return stbi_write_png(filename.c_str(), width, height, 3, pixels.data(), stride) != 0;
}

// Extract filename from path and change extension to .png
string getOutputFilename(const string& filepath) {
    size_t lastSlash = filepath.find_last_of("/\\");
    string filename = (lastSlash == string::npos) ? filepath : filepath.substr(lastSlash + 1);
    size_t lastDot = filename.find_last_of(".");
    if (lastDot != string::npos) {
        filename = filename.substr(0, lastDot);
    }
    return "results/" + filename + ".png";
}

int main()
{
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

    for (size_t idx = 0; idx < scenes.size(); ++idx) {
        string filepath = scenes[idx];
        cout << "--------------------------------------" << endl;
        cout << "Processing: " << filepath << endl;
        initCameraState(); 
        
        vector<Light*> lights;
        vector<Object*> objects;
        Vec3 ambient(0.0f, 0.0f, 0.0f);

        if (loadSceneFile(filepath, lights, objects, ambient) != 0) {
            cerr << "Failed to open: " << filepath << endl;
            continue;
        }

        int width = 800, height = 800;
        setupCamera(width, height);

        vector<unsigned char> image(3 * width * height, 0);

        cout << "Rendering..." << endl;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                Ray ray = createPixelRay(x, y, width, height);
                Vec3 color = castRay(ray, objects, lights, ambient, 0);
                color = glm::clamp(color, Vec3(0.0f), Vec3(1.0f));
                int pixelIdx = 3 * (y * width + x);
                image[pixelIdx]     = (unsigned char)(255 * color.x);
                image[pixelIdx + 1] = (unsigned char)(255 * color.y);
                image[pixelIdx + 2] = (unsigned char)(255 * color.z);
            }
        }
        
        string outputFile = getOutputFilename(filepath);

        if (!writeImage(outputFile, width, height, image)) 
            cerr << "Failed to write " << outputFile << endl;
        else 
            cout << "Saved: " << outputFile << endl;

        for (Light* light : lights) delete light;
        for (Object* obj : objects) delete obj;
    }
    cout << "--------------------------------------" << endl;
    return 0;
}

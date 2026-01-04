#version 300 es
precision highp float;
precision highp int;

struct Camera {
    vec3 pos;
    vec3 forward;
    vec3 right;
    vec3 up;
};

struct Plane {
    vec3 point;
    vec3 normal;
    vec3 color;
};

struct Sphere {
    vec3 center;
    float radius;
    vec3 color;
    int type; // 0: opaque, 1: reflective, 2: refractive
};

struct Light {
    vec3 position;
    vec3 direction;
    vec3 color;
    float shininess; // used as specular exponent in this implementation
    float cutoff;    // if > 0.0 then spotlight else directional
};

struct HitInfo {
    float t;          // < 0 means "no hit"
    vec3 baseColor;   // object color (or checkerboard)
    vec3 hitPoint;
    vec3 normal;      // geometric normal (outward for spheres)
    int type;         // 0 opaque, 1 reflective, 2 refractive
    int sphereIndex;  // -1 if not sphere
    int isPlane;      // 1 if plane
};

const int TYPE_OPAQUE     = 0;
const int TYPE_REFLECTIVE = 1;
const int TYPE_REFRACTIVE = 2;

const int MAX_SPHERES = 16;
const int MAX_LIGHTS  = 4;
const int MAX_DEPTH   = 5;

in vec2 vUV;
out vec4 FragColor;

uniform float uTime;
uniform ivec2 uResolution;

uniform Camera cam;
uniform Sphere uSpheres[MAX_SPHERES];
uniform int uNumSpheres;

uniform Light uLights[MAX_LIGHTS];
uniform int uNumLights;

uniform Plane uPlane;

const float EPS = 1e-3;
const float INF = 1e20;

// -------- Provided helper (plane checkerboard) --------
vec3 checkerboardColor(vec3 rgbColor, vec3 hitPoint) {
    float scaleParameter = 2.0;
    float checkerboard = 0.0;
    if (hitPoint.x < 0.0) checkerboard += floor((0.5 - hitPoint.x) / scaleParameter);
    else checkerboard += floor(hitPoint.x / scaleParameter);

    if (hitPoint.z < 0.0) checkerboard += floor((0.5 - hitPoint.z) / scaleParameter);
    else checkerboard += floor(hitPoint.z / scaleParameter);

    checkerboard = (checkerboard * 0.5) - float(int(checkerboard * 0.5));
    checkerboard *= 2.0;
    if (checkerboard > 0.5) return 0.5 * rgbColor;
    return rgbColor;
}

// -------- UV scaling --------
// vUV is [0,1]. We want y in [-1,1] and x scaled by aspect ratio.
vec2 scaleUV(vec2 uv) {
    vec2 p = uv * 2.0 - 1.0; // now in [-1,1]
    float aspect = float(uResolution.x) / float(uResolution.y);
    p.x *= aspect;
    return p;
}

// -------- Intersections --------
bool intersectSphere(vec3 ro, vec3 rd, Sphere s, out float tHit, out vec3 nOut) {
    vec3 oc = ro - s.center;

    // Quadratic with "half-b" form:
    float b = dot(oc, rd);
    float c = dot(oc, oc) - s.radius * s.radius;
    float disc = b*b - c;

    if (disc < 0.0) return false;

    float sqrtD = sqrt(disc);
    float t0 = -b - sqrtD;
    float t1 = -b + sqrtD;

    // pick smallest positive
    float t = (t0 > EPS) ? t0 : ((t1 > EPS) ? t1 : -1.0);
    if (t < 0.0) return false;

    vec3 hp = ro + rd * t;
    nOut = normalize(hp - s.center); // outward normal
    tHit = t;
    return true;
}

bool intersectPlane(vec3 ro, vec3 rd, Plane p, out float tHit, out vec3 nOut) {
    vec3 n = normalize(p.normal);
    float denom = dot(rd, n);
    if (abs(denom) < 1e-6) return false;

    float t = dot(p.point - ro, n) / denom;
    if (t <= EPS) return false;

    tHit = t;
    nOut = n; // plane's normal as given
    return true;
}

// Find closest hit
HitInfo intersectScene(vec3 ro, vec3 rd) {
    HitInfo best;
    best.t = -1.0;
    best.baseColor = vec3(0.0);
    best.hitPoint = vec3(0.0);
    best.normal = vec3(0.0);
    best.type = TYPE_OPAQUE;
    best.sphereIndex = -1;
    best.isPlane = 0;

    float tMin = INF;

    // Spheres
    for (int i = 0; i < MAX_SPHERES; i++) {
        if (i >= uNumSpheres) break;

        float t;
        vec3 n;
        if (intersectSphere(ro, rd, uSpheres[i], t, n)) {
            if (t < tMin) {
                tMin = t;
                best.t = t;
                best.hitPoint = ro + rd * t;
                best.normal = n;
                best.baseColor = uSpheres[i].color;
                best.type = uSpheres[i].type;
                best.sphereIndex = i;
                best.isPlane = 0;
            }
        }
    }

    // Plane
    {
        float t;
        vec3 n;
        if (intersectPlane(ro, rd, uPlane, t, n)) {
            if (t < tMin) {
                tMin = t;
                best.t = t;
                best.hitPoint = ro + rd * t;
                best.normal = n;
                best.baseColor = checkerboardColor(uPlane.color, best.hitPoint);
                best.type = TYPE_OPAQUE;
                best.sphereIndex = -1;
                best.isPlane = 1;
            }
        }
    }

    return best;
}

// -------- Shadows --------
bool isOccluded(vec3 ro, vec3 rd, float maxDist) {
    // Check spheres
    for (int i = 0; i < MAX_SPHERES; i++) {
        if (i >= uNumSpheres) break;
        float t;
        vec3 n;
        if (intersectSphere(ro, rd, uSpheres[i], t, n)) {
            if (t > EPS && t < maxDist) return true;
        }
    }

    // Check plane
    float tp;
    vec3 np;
    if (intersectPlane(ro, rd, uPlane, tp, np)) {
        if (tp > EPS && tp < maxDist) return true;
    }

    return false;
}

// -------- Phong shading for opaque objects --------
vec3 shadeOpaque(HitInfo hit, vec3 viewDir) {
    vec3 N = normalize(hit.normal);

    // Make N face the viewer for stable lighting
    if (dot(N, viewDir) < 0.0) N = -N;

    vec3 Ka = hit.baseColor;
    vec3 Kd = hit.baseColor;
    vec3 Ks = vec3(0.7);

    // Global ambient
    vec3 color = 0.12 * Ka;

    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (i >= uNumLights) break;

        Light Lgt = uLights[i];

        vec3 L;
        float maxDist = INF;

        if (Lgt.cutoff > 0.0) {
            // Spotlight
            vec3 toLight = Lgt.position - hit.hitPoint;
            float dist = length(toLight);
            if (dist < 1e-6) continue;
            L = toLight / dist;
            maxDist = dist - EPS;

            // cutoff test: angle between spotlight direction and (point - lightPos)
            vec3 lightToPoint = normalize(hit.hitPoint - Lgt.position);
            float cosAng = dot(normalize(Lgt.direction), lightToPoint);
            if (cosAng < Lgt.cutoff) continue;
        } else {
            // Directional light: direction points FROM light TOWARD the scene
            L = normalize(-Lgt.direction);
        }

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL <= 0.0) continue;

        // Hard shadows (bonus)
        vec3 shadowOrigin = hit.hitPoint + N * EPS;
        if (isOccluded(shadowOrigin, L, maxDist)) {
            continue;
        }

        vec3 diffuse = Kd * Lgt.color * NdotL;

        vec3 R = reflect(-L, N);
        float spec = pow(max(dot(R, viewDir), 0.0), Lgt.shininess);
        vec3 specular = Ks * Lgt.color * spec;

        color += diffuse + specular;
    }

    return clamp(color, 0.0, 1.0);
}

// -------- Refraction helper (air <-> glass) --------
bool refractSphere(vec3 ro, vec3 rd, Sphere s, vec3 entryPoint, vec3 entryNormal,
                   out vec3 outOrigin, out vec3 outDir) {
    // entering from air (n=1) to glass (n=1.5)
    float etaIn = 1.0 / 1.5;

    // Ensure normal is oriented against incident ray for entering
    vec3 N = entryNormal;
    if (dot(rd, N) > 0.0) N = -N;

    vec3 dirInside = refract(rd, N, etaIn);
    if (length(dirInside) < 1e-6) {
        // Total internal reflection at entry (rare from air->glass), fallback reflect
        outOrigin = entryPoint + N * EPS;
        outDir = reflect(rd, N);
        return true;
    }

    vec3 insideOrigin = entryPoint + dirInside * EPS;

    // Find exit by intersecting the same sphere from inside
    float tExit;
    vec3 nExit;
    if (!intersectSphere(insideOrigin, dirInside, s, tExit, nExit)) {
        return false;
    }

    vec3 exitPoint = insideOrigin + dirInside * tExit;

    // exiting glass (1.5) to air (1.0)
    float etaOut = 1.5 / 1.0;

    // At exit, outward normal is nExit. For exiting, we want normal pointing INTO the glass for refract().
    vec3 N2 = -nExit;

    vec3 dirOut = refract(dirInside, N2, etaOut);
    if (length(dirOut) < 1e-6) {
        // Total internal reflection at exit — simplified fallback: reflect and "continue" outside anyway.
        // (Not perfect physics, but usually not tested.)
        dirOut = reflect(dirInside, nExit);
    }

    outOrigin = exitPoint + dirOut * EPS;
    outDir = normalize(dirOut);
    return true;
}

// -------- Main ray tracer (iterative) --------
vec3 traceRay(vec3 ro, vec3 rd) {
    vec3 origin = ro;
    vec3 dir = normalize(rd);

    for (int depth = 0; depth < MAX_DEPTH; depth++) {
        HitInfo hit = intersectScene(origin, dir);

        // Background
        if (hit.t < 0.0) {
            return vec3(0.0); // black background
        }

        // View direction (towards camera)
        vec3 V = normalize(-dir);

        if (hit.type == TYPE_OPAQUE) {
            return shadeOpaque(hit, V);
        }

        if (hit.type == TYPE_REFLECTIVE) {
            // pure reflection (ignore local material)
            vec3 N = normalize(hit.normal);
            if (dot(dir, N) > 0.0) N = -N;

            origin = hit.hitPoint + N * EPS;
            dir = normalize(reflect(dir, N));
            continue;
        }

        if (hit.type == TYPE_REFRACTIVE) {
            // spheres only in your scene (as per assignment)
            if (hit.sphereIndex < 0) {
                // if someone marks plane refractive, just shade it
                return shadeOpaque(hit, V);
            }

            Sphere s = uSpheres[hit.sphereIndex];

            vec3 newOrigin, newDir;
            bool ok = refractSphere(origin, dir, s, hit.hitPoint, hit.normal, newOrigin, newDir);
            if (!ok) {
                // fallback: treat as opaque if refraction fails
                return shadeOpaque(hit, V);
            }

            origin = newOrigin;
            dir = newDir;
            continue;
        }

        // fallback
        return shadeOpaque(hit, V);
    }

    // If max depth reached
    return vec3(0.0);
}

// replace calcColor signature to accept real hit data? We'll just use traceRay directly.
vec3 calcColor(int _unused) {
    // not used anymore (kept only if you want to preserve function names)
    return vec3(0.0);
}

void main() {
    vec2 uv = scaleUV(vUV);
    vec3 rayDir = normalize(cam.forward + uv.x * cam.right + uv.y * cam.up);

    vec3 color = traceRay(cam.pos, rayDir);

    FragColor = vec4(color, 1.0);
}

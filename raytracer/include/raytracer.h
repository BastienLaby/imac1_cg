#ifndef RAYTRACER_H
#define RAYTRACER_H


#include <SDL/SDL.h>

#include "geometry.h"
#include "colors.h"
#include "shape.h"

typedef struct Ray {
    Point3D origin;
    Vector3D direction;
} Ray;

typedef struct Intersection {
    Point3D position;
    Color3f color;
} Intersection;

typedef struct Scene {
    Sphere spheres[10];
    int sphereCount;
} Scene;

Ray createRay(Point3D origin, Vector3D direction);
Scene createScene();
Point3D getPointAtParameter(Ray ray, float t);

void addSphereToScene(Scene* scene, Sphere sphere);

int intersectsSphere(Sphere sphere, Ray ray, Intersection* intersection);

int throwRayThroughScene(Scene scene, Ray ray, Intersection* intersection);
void simpleRaytracing(Scene scene, SDL_Surface* framebuffer);

#endif
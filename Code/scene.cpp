#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <time.h>

using namespace std;

void minClamp(Vector &vec, double minValue) {
    vec.r = fmax(vec.r, minValue);
    vec.g = fmax(vec.g, minValue);
    vec.b = fmax(vec.b, minValue);
}

Color Scene::getColor(Point const &hitPoint, ObjectPtr const &obj,
                      Vector const &N) {
    if (!obj->material.isTextured)
        return obj->material.color;
    else {
        Point pos = obj->getTextureCoordinates(hitPoint, N.normalized());
        return obj->texture.colorAt(pos.x, pos.y);
    }
}

float Scene::getRand(int min, int max)
{
	double range = (max - min);
	double div = RAND_MAX / range;
	return min + (rand() / div);
}

bool Scene::hitsObject(Ray const &ray)
{
	Hit min_hit(numeric_limits<double>::infinity(), Vector());
	ObjectPtr obj = nullptr;

	for (size_t idx = 0; idx != objects.size(); ++idx)
	{
		if (objects[idx]->shadow)
		{
			Hit hit(objects[idx]->intersect(ray));
			if (hit.t < min_hit.t)
			{
				min_hit = hit;
				obj = objects[idx];
			}
		}
	}
	return !(!obj);
}

Color Scene::trace(Ray const &ray, size_t depth) {
    // Find hit object and distance
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = nullptr;

    for (size_t idx = 0; idx != objects.size(); ++idx) {
        Hit hit(objects[idx]->intersect(ray));
        if (hit.t < min_hit.t) {
            min_hit = hit;
            obj = objects[idx];
        }
    }

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);
    if (depth > maxRecDepth) return Color(0.0, 0.0, 0.0);

    Material material = obj->material;  // The hit objects material
    Point hit = ray.at(min_hit.t);      // The hit point
    Vector N = min_hit.N;               // The normal at hit point
    Vector V = -ray.D;                  // The view vector

    Color materialColor = getColor(hit, obj, N);

    // Calculate the ambient illumination for the material, which is color's
    // initial value.
    Vector iA = materialColor * material.ka;
    Color color = iA;

    for (auto &light : lights) {
        Color shadowColor;
		bool isHit = false;
        if (shadows)
		{
            Vector newRayDir(light->position - hit);
            Vector offset = 0.000001 * newRayDir;
            Ray newRay(hit + offset, newRayDir);
			isHit = hitsObject(newRay);
        }

        // Get the verctor of the light direction.
        Vector L = light->position - hit;
        L.normalize();
        // Get the dot product of the light direction and the Normal at the hit
        // point.
        double NL = L.dot(N);
        // Make sure NL is on [0;1]
        NL = NL < 0.0 ? 0.0 : NL > 1.0 ? 1.0 : NL;
        // Get the R vector.
        Vector R = 2 * NL * N - L;
        R.normalize();

        double RV = R.dot(V);
        // Make sure RV is on [0;1]
        RV = RV < 0.0 ? 0.0 : RV > 1.0 ? 1.0 : RV;

        // Calculate the diffuse and specular illumination for this light and
        // add them to color.
        Vector iD = NL * materialColor * light->color * material.kd;
        Vector iS = pow(RV, material.n) * light->color * material.ks;

		if (isHit)
            color += Color(0.00, 0.00, 0.00);
        else
            color += iD + iS;
    }

    if (material.ks > 0)
	{
        Vector newRayDir = ray.D - N * 2 * N.dot(ray.D);
        Vector offset = 0.00000001 * newRayDir;
        Ray newRay(hit + offset, newRayDir);
        Color specColor = trace(newRay, depth + 1);
        if (specColor != Color(0.0, 0.0, 0.0))
            return color + specColor * material.ks;
    }
    return color;
}

Color Scene::renderSubPixels(size_t height, size_t width, size_t xPos,
                             size_t yPos) {
    double subPixelWidth = 1.0 / (superSampFac + 1);
    double subPixelWeight = 1.0 / (superSampFac * superSampFac);
    Color color;

    for (size_t x = 0; x < superSampFac; ++x)
        for (size_t y = 0; y < superSampFac; ++y) {
            Point subPixel(xPos + x * subPixelWidth, yPos - y * subPixelWidth,
                           0);
            Ray ray(eye, (subPixel - eye).normalized());
            color += trace(ray, 0) * subPixelWeight;
        }
    return color;
}

void Scene::render(Image &img) {
    size_t w = img.width();
    size_t h = img.height();

#pragma omp parallel for
    for (size_t y = 0; y < h; ++y)
        for (size_t x = 0; x < w; ++x) {
            Color col = renderSubPixels(h, w, x, h - 1 - y);
            col.clamp();
            img(x, y) = col;
        }
}

// --- Misc functions ----------------------------------------------------------

void Scene::addObject(ObjectPtr obj) { objects.push_back(obj); }

void Scene::addLight(Light const &light) {
    lights.push_back(LightPtr(new Light(light)));
}

void Scene::setEye(Triple const &position) { eye = position; }

void Scene::setShadows(bool val) { shadows = val; }

void Scene::setMaxRecDepth(size_t val) { maxRecDepth = val; }

void Scene::setSuperSampFac(size_t val) { superSampFac = val; }

unsigned Scene::getNumObject() { return objects.size(); }

unsigned Scene::getNumLights() { return lights.size(); }

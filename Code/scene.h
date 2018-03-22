#ifndef SCENE_H_
#define SCENE_H_

#include "light.h"
#include "object.h"
#include "triple.h"

#include <vector>

// Forward declerations
class Ray;
class Image;

class Scene {
    std::vector<ObjectPtr> objects;
    std::vector<LightPtr> lights;  // no ptr needed, but kept for consistency
    Point eye;
    bool shadows = false;
    size_t maxRecDepth = 0;
    size_t superSampFac = 1;

   public:
    // Trace a ray into the scene and return the color
    Color trace(Ray const &ray, size_t depth);

	// Get random float between min and max.
	float getRand(int min, int max);

	// Check if the ray hit another object. ONLY IF THAT OBJECT has shadows enabled.
	bool hitsObject(Ray const &ray);

    // Render the scene to the given image
    void render(Image &img);

    // Get the color at the hitPoint
    Color getColor(Point const &hitPoint, ObjectPtr const &obj,
                   Vector const &N);

    // Splits up a single ray into superSampFac * superSampFac rays and returns
    // the average color.
    Color renderSubPixels(size_t height, size_t width, size_t x, size_t y);

    void addObject(ObjectPtr obj);
    void addLight(Light const &light);
    void setEye(Triple const &position);
    void setShadows(bool val);
    void setMaxRecDepth(size_t val);
    void setSuperSampFac(size_t val);

    unsigned getNumObject();
    unsigned getNumLights();
};

#endif

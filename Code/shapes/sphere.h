#ifndef SPHERE_H_
#define SPHERE_H_

#include "../object.h"

class Sphere : public Object {
   public:
    Sphere(Point const &pos, double radius);
    Sphere(Point const &pos, double radius, Point const &rotation, size_t angle);

    virtual Hit intersect(Ray const &ray);
    virtual Point getTextureCoordinates(Point const &point, Vector const &N);

    Point  const position;
    double const r;
    Vector const rotation;
    size_t const angle;
};

#endif

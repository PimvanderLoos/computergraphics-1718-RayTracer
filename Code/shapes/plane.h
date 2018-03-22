#ifndef PLANE_H_
#define PLANE_H_

#include "../object.h"

class Plane : public Object {
   public:
    Plane(Point const &position, Vector &N, size_t const size);

    virtual Hit intersect(Ray const &ray);
    virtual Point getTextureCoordinates(Point const &point, Vector const &N);

    Point const position;
    size_t const size;
    Vector N;
};

#endif

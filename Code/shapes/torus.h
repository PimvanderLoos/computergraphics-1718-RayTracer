#ifndef TORUS_H_
#define TORUS_H_

#include "../object.h"

class Torus : public Object {
   public:
	Torus(Point const &center, Vector const &axis, double innerRadius, double outerRadius);
	Torus(Point const &center, Vector const &axis, double innerRadius, double outerRadius, Point const &rotation, size_t angle);

    virtual Hit intersect(Ray const &ray);
    bool intersectWithCap(Ray const &ray, double &t, Point p, Vector const &N);
    virtual Point getTextureCoordinates(Point const &point, Vector const &N);
	Vector getNormal(const Ray &ray, float distance) const;

    Point  const center;
	Vector const axis;
    double const ri;
	double const ro;
	Vector const rotation;
	size_t const angle;
};

#endif

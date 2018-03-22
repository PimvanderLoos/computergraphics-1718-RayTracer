#include "torus.h"
#include "solvers.h"

#include <cmath>
#include <iostream>

using namespace std;

Hit Torus::intersect(Ray const &ray)
{
	Vector L		= ray.O - center;
	double LDotL	= L.dot(L);
	double iRadSq	= ri * ri;
	double oRadSq	= ro * ro;
	double aDotL	= axis.dot(L);
	double aDotDir	= axis.dot(ray.D);

	double a		= 1 - aDotDir * aDotDir;
	double b		= 2 * (L.dot(ray.D) - aDotL * aDotDir);
	double c		= LDotL - aDotL * aDotL;
	double d		= LDotL + oRadSq - iRadSq;

	float B 		= 4 * ray.D.dot(L);
	float C 		= 2 * d + B * B * 0.25f - 4 * oRadSq * a;
	float D		 	= B * d - 4 * oRadSq * b;
	float E 		= d * d - 4 * oRadSq * c;

	double roots[4]	= {-1.0, -1.0, -1.0, -1.0};
	int numRoots	= Solvers::quartic(1, B, C, D, E, roots);

	if (numRoots   == 0)
		return Hit::NO_HIT();

	float closestRoot = numeric_limits<float>::infinity();
	for (int idx = 0; idx < 4; ++idx)
		if (roots[idx] > 0.000000 && roots[idx] < closestRoot)
			closestRoot = roots[idx];
	return Hit(closestRoot, getNormal(ray, closestRoot));
}


Vector Torus::getNormal(const Ray &ray, float distance) const
{
	Vector point = ray.at(distance);
	Vector centerToPoint = point - center;
	float centerToPointDotAxis = centerToPoint.dot(axis);
	Vector dir = centerToPoint - axis * centerToPointDotAxis;
	dir.normalize();
	Vector normal = point - center + dir * ro;
	normal.normalize();
	return normal;
}


Point Torus::getTextureCoordinates(Point const &point, Vector const &N)
{
	return Point(0.0,0.0,0.0);
}


Torus::Torus(Point const &center, Vector const &axis, double innerRadius, double outerRadius)
:
	center(center),
	axis(axis),
	ri(innerRadius),
	ro(outerRadius),
	angle(0)
{}


Torus::Torus(Point const &center, Vector const &axis, double innerRadius, double outerRadius, Vector const &rotation, size_t angle)
:
	center(center),
	axis(axis),
	ri(innerRadius),
	ro(outerRadius),
	rotation(rotation),
	angle(angle)
{}

#include "plane.h"

#include <cmath>
#include <iostream>

using namespace std;

Hit Plane::intersect(Ray const &ray)
{
	double epsilon = 0.00000001;
	double denom = N.dot(ray.D);

	if (abs(denom) > epsilon)
	{
	    double t = (position - ray.O).dot(N) / denom;

	    if (t >= 0)
	    {
	    		// Get the hitposition and check if it isn't too far on the x, y or z axis.
		    Point hitPos = ray.O + t * ray.D;
		    if (abs(hitPos.x - position.x) > size)
		    		return Hit::NO_HIT();
		    if (abs(hitPos.y - position.y) > size)
		    		return Hit::NO_HIT();
		    if (abs(hitPos.z - position.z) > size)
		    		return Hit::NO_HIT();
		    // Within bounds -> HIT!
		    return Hit(t, N);
	    }
	}
	return Hit::NO_HIT();
}


Point Plane::getTextureCoordinates(Point const &point, Vector const &N)
{
	return Point(0.0,0.0,0.0);
}


Plane::Plane(Point const &pos, Vector &Normal, size_t const size)
:
	position(pos),
	size(size),
	N(Normal)
{}

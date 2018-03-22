#include "example.h"

#include <cmath>

Hit Example::intersect(Ray const &ray)
{
    /* Your intersect calculation goes here */

    double t = 0 /* = ... */;
    Vector N /* = ... */;

    return Hit(t, N);
}


Point Example::getTextureCoordinates(Point const &point, Vector const &N)
{
	return Point(0.0,0.0,0.0);
}


Example::Example(/* YOUR DATAMEMBERS HERE */)
//:
// See sphere.cpp how to initialize your data members
{}

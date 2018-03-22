#include "sphere.h"
#include "solvers.h"

#include <cmath>

using namespace std;

Hit Sphere::intersect(Ray const &ray)
{
    // Sphere formula: ||x - position||^2 = r^2
    // Line formula:   x = ray.O + t * ray.D

    Vector L = ray.O - position;
    double a = ray.D.dot(ray.D);
    double b = 2 * ray.D.dot(L);
    double c = L.dot(L) - r * r;

    double t0;
    double t1;
    if (not Solvers::quadratic(a, b, c, t0, t1))
        return Hit::NO_HIT();

    // t0 is closest hit
    if (t0 < 0)  // check if it is not behind the camera
    {
        t0 = t1;    // try t1
        if (t0 < 0) // both behind the camera
            return Hit::NO_HIT();
    }

    // calculate normal
    Point hit = ray.at(t0);
    Vector N = (hit - position).normalized();

    // determine orientation of the normal
    if (N.dot(ray.D) > 0)
        N = -N;


    return Hit(t0, N);
}


Point Sphere::getTextureCoordinates(Point const &point, Vector const &N)
{
    double theta = angle * (M_PI / 180);
    Vector rot 	 = rotation.normalized();
    // Rodrigues rotation formula
    Vector v_rot = N * cos(theta) + (rot.cross(N)) * sin(theta) + rot * (rot.dot(N)) * (1 - cos(theta));


    double temp2 = atan2(v_rot.y, v_rot.x);

	if (temp2 < 0)
		temp2 = temp2 + (2 * M_PI);

	double v = acos(v_rot.z) / M_PI;
	double u = temp2 / (2 * M_PI);

	return Point(u, v, 0.0);
}


Sphere::Sphere(Point const &pos, double radius)
:
    position(pos),
    r(radius),
	angle(0)
{}

Sphere::Sphere(Point const &pos, double radius, Point const &rotation, size_t angle)
:
    position(pos),
    r(radius),
	rotation(rotation),
	angle(angle)
{}

#include "Sphere.h"
#include <iostream>
inline bool Quadratic(float a, float b, float c, float* t0, float* t1);

Sphere::Sphere(glm::vec3 center, float radius) :
	Shape(Transform(center)), radius(radius), position(center)
{
}

bool Sphere::Intersect(Ray ray, float* tHit, SurfaceInteraction* interaction) const
{
	//tray is ray transformed to object space
	Ray tray(glm::vec3(objectToWorld.inv * glm::vec4(ray.o, 1)), ray.d);
	//std::cout << objectToWorld.inv[0][3] << " " << objectToWorld.inv[1][3] << " " << objectToWorld.inv[2][3] << " " << ray.o.x << " "<<ray.o.y << " "<<ray.o.z << " " << tray.o.x << " " << tray.o.y << " " << tray.o.z << " "<<std::endl;
	glm::vec3 o = tray.o;
	glm::vec3 d = tray.d;
	//formula is in pbrt sphere section
	float a = glm::dot(d, d);
	float b = 2 * glm::dot(d, o);
	float c = glm::dot(o, o) - radius * radius;
	float t0, t1;

	if (!Quadratic(a, b, c, &t0, &t1)) return false;

	if (t0 >=ray.tMax|| t0<=0 || t1>= ray.tMax || t1 <=0 ) return false;
	float tShapeHit = t0;
	if (tShapeHit <=0)
	{
		tShapeHit = t1;
	}
	glm::vec3 objectSpaceIntersection = tray(tShapeHit);
	glm::vec3 worldSpaceIntersection = objectToWorld.mat  * glm::vec4(objectSpaceIntersection,1);
	//this is in object space so the sphere is centered at 0
	glm::vec3 normal = objectSpaceIntersection / radius;

	*tHit = tShapeHit;
	interaction->intersectPos = worldSpaceIntersection;
	interaction->normal = normal;
	return true;
}

inline bool Quadratic(float a, float b, float c, float* t0, float* t1) {
	double discrim = (double)b * (double)b - 4 * (double)a * (double)c;
	if (discrim < 0) return false;
	double rootDiscrim = std::sqrt(discrim);
	double q;
	if (b < 0) q = -.5 * (b - rootDiscrim);
	else       q = -.5 * (b + rootDiscrim);
	*t0 = q / a;
	*t1 = c / q;
	if (*t0 > *t1)
	{
		float temp = *t0;
		*t0 = *t1;
		*t1 = temp;
	}
	return true;
}
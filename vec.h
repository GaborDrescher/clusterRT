#ifndef RT_VECTOR_H
#define RT_VECTOR_H

#include "precision.h"
#include "config.h"

class vec3
{
	public:
	real x;
	real y;
	real z;

	vec3(real x_ = real(0), real y_ = real(0), real z_ = real(0)) : x(x_), y(y_), z(z_) {}

static void add(vec3 &r, const vec3 &a, const vec3 &b)
{
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;
}

static void sadd(vec3 &r, const vec3 &a, const real b)
{
	r.x = a.x + b;
	r.y = a.y + b;
	r.z = a.z + b;
}

static void sub(vec3 &r, const vec3 &a, const vec3 &b)
{
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;
}

static void mul(vec3 &r, const vec3 &a, const vec3 &b)
{
	r.x = a.x * b.x;
	r.y = a.y * b.y;
	r.z = a.z * b.z;
}

static real dot(const vec3 &a, const vec3 &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

static real normalize(vec3 &r, const vec3 &v)
{
	const real length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	const real invlen = real(1.0) / length;

	r.x = v.x * invlen;
	r.y = v.y * invlen;
	r.z = v.z * invlen;

	return length;
}

static void mod3(vec3 &r, const vec3 &v, const real d)
{
	r.x = fmod(v.x, d);
	r.y = fmod(v.y, d);
	r.z = fmod(v.z, d);
}

static void scale(vec3 &r, const vec3 &v, const real s)
{
	r.x = v.x * s;
	r.y = v.y * s;
	r.z = v.z * s;
}

static void reflect(vec3 &r, const vec3 &v, const vec3 &n)
{
	const real d = (n.x * v.x + n.y * v.y + n.z * v.z) * real(2.0);

	r.x = v.x - n.x * d;
	r.y = v.y - n.y * d;
	r.z = v.z - n.z * d;
}

static void refract(vec3 &r, const vec3 &v, const vec3 &n, const real eta)
{
	const real cI = -(n.x * v.x + n.y * v.y + n.z * v.z);
	const real cT2 = real(1.0) - eta * eta * (real(1.0) - cI * cI);
	const real m = eta * cI - sqrt(cT2);

	r.x = v.x * eta + n.x * m;
	r.y = v.y * eta + n.y * m;
	r.z = v.z * eta + n.z * m;
}	

static void step(vec3 &r, const real e, const vec3 &v)
{
	r.x = v.x < e ? real(0) : real(1);
	r.y = v.y < e ? real(0) : real(1);
	r.z = v.z < e ? real(0) : real(1);
}

static void minv(vec3 &r, vec3 &a, vec3 &b)
{
	r.x = a.x < b.x ? a.x : b.x;
	r.y = a.y < b.y ? a.y : b.y;
	r.z = a.z < b.z ? a.z : b.z;
}

static void mixv(vec3 &r, const vec3 &a, const vec3 &b, const real t)
{
	r.x = a.x + t * (b.x - a.x);
	r.y = a.y + t * (b.y - a.y);
	r.z = a.z + t * (b.z - a.z);
}

};

#endif

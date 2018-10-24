#ifndef RT_RT_H
#define RT_RT_H

#include <cmath>
#include "vec.h"
#include "precision.h"
#include "config.h"
#include "ball.h"
#include "sim.h"

using namespace std;

//intersection datatype
struct intersection_t
{
	vec3 position;
	real distance;
	vec3 normal;
	int dataIndex;
};

//node datatype
struct node_t
{
	//ray data
	vec3 origin;
	vec3 direction;

	//intersection data
	vec3 position;
	vec3 normal;
	real distance;
	real rfrDistance;
	int dataIndex;

	//node data
	vec3 color;
	int father;
};

static vec3 lightPosition(20.0, 10.0, 10.0);
static vec3 origin(0.0, 0.0, -20.0);//camera origin
static const vec3 bgColor(1.0f, 1.0f, 1.0f);

static const vec3 colors[] = {vec3(0.99, 140.0/255.0, 0.0),
							  vec3(0.0, 0.7, 0.9)};

static data_t data[NUM_OBJECTS];

class Trace
{
	private:

	real outside;
	intersection_t intersections[NUM_OBJECTS];
	node_t nodes[NUM_NODES];

//simple light
static void light(vec3 &r, const vec3 &position, const vec3 &direction, const vec3 &normal, const vec3 &color)
{
	vec3 toLight;
	vec3 reflected;

	vec3::sub(toLight, lightPosition, position);
	vec3::normalize(toLight, toLight);

	vec3::reflect(reflected, toLight, normal);
	vec3::normalize(reflected, reflected);

	real diffuse = max(vec3::dot(normal, toLight), real(0.0));

	real specular = pow(max(vec3::dot(reflected, direction), EPSILON), real(8.0));

	vec3::scale(r, color, diffuse + real(0.2));
	vec3::sadd(r, r, specular);

	//clamp
	r.x = r.x < real(0) ? real(0) : (r.x > real(1) ? real(1) : r.x);
	r.y = r.y < real(0) ? real(0) : (r.y > real(1) ? real(1) : r.y);
	r.z = r.z < real(0) ? real(0) : (r.z > real(1) ? real(1) : r.z);
}

////simple procedural checkerboard texture
//static real texture(const vec3 &normal)
//{
//	vec3 t;
//	vec3::sadd(t, normal, real(1));
//	vec3::scale(t, t, real(4));
//
//	vec3::mod3(t, t, real(2));
//	vec3::step(t, real(1), t);
//
//	const real out = real(0.5) * ((real(0.5) + fmod(t.x + t.y + t.z, real(2.0)) * real(0.5)) + real(1.0));
//
//	return out;
//	
//	//return 1;
//}

//calculate color for given node
void getColor(vec3 &r, const int n)
{
	const node_t &node = nodes[n];

	//or use only node.color here for speed or lulz
	light(r, node.position, node.direction, node.normal, node.color);

	//const real tex = texture(node.normal);

	//vec3::scale(r, r, tex);
}

void processIntersection(const int i, const int n)
{
	if (i < 0) return;

	node_t &node = nodes[n];
	intersection_t &inter = intersections[i];

	vec3 &pos = inter.position;
	vec3 &normal = inter.normal;

	vec3::scale(pos, node.direction, inter.distance);
	vec3::add(pos, pos, node.origin);

	vec3::sub(normal, inter.position, data[inter.dataIndex].position);
	vec3::normalize(normal, normal);
	vec3::scale(normal, normal, outside);
}

//ray sphere intersection
void intersect(int i, int d, int n)
{
	node_t &node = nodes[n];
	intersection_t &inter = intersections[i];
	data_t &dat = data[d];


	inter.distance = RT_INFINITY;
	inter.dataIndex = d;
	outside = real(1);

	vec3 cDirection;
	vec3::sub(cDirection, dat.position, node.origin);

	const real RayDirDot = vec3::dot(node.direction, node.direction);
	const real cDirRayDot = vec3::dot(cDirection, node.direction);
	const real cDirDot = vec3::dot(cDirection, cDirection);
	const real rSqr = dat.radius * dat.radius;

	if(cDirDot <= rSqr)
	{
		const real hc = (rSqr - cDirDot) / RayDirDot + cDirRayDot * cDirRayDot;
		inter.distance = cDirRayDot + sqrt(hc);
		outside = real(-1);
	}
	else
	{
		if(cDirRayDot < real(0)) return;

		const real hc = (rSqr - cDirDot) / RayDirDot + cDirRayDot * cDirRayDot;

		if (hc > real(0))
		{
			inter.distance = cDirRayDot - sqrt(hc);
		}
	}
}

//find the closest intersection
int getClosestIntersection(int n)
{
	//distance and index of the closest intersection
	real distance = RT_INFINITY;
	int i = -1;
	for (int o = 0; o < NUM_OBJECTS; ++o)
	{
		intersect(o, o, n);
		if (intersections[o].distance < distance)
		{
			distance = intersections[o].distance;
			i = o;
		}
	}

	//calculate normal and position of the intersection
	processIntersection(i, n);
	return i;
}

//reflection
void setReflectionNode(int i, int n, int depth)
{
	node_t &node = nodes[n];
	intersection_t &inter = intersections[i];

	if(i >= 0)
	{
		node.position = inter.position;
		node.normal = inter.normal;
		node.distance = inter.distance;

		node.dataIndex = inter.dataIndex;
		node.color = data[inter.dataIndex].color;
		if(depth == MAX_DEPTH)
		{
			getColor(node.color, n);
		}
	}
	else
	{
		node.distance = BACKGROUND;
		getColor(node.color, node.father);
	}
}

//refraction
void setRefractionNode(int i, int n, int depth)
{
	node_t &node = nodes[n];
	intersection_t &inter = intersections[i];

	if(i >= 0)
	{
		node.position = inter.position;
		node.normal = inter.normal;
		node.distance = inter.distance;

		node.dataIndex = inter.dataIndex;
		node.color = data[inter.dataIndex].color;
		if(depth == MAX_DEPTH)
		{
			getColor(node.color, n);
		}
	}
	else
	{
		node.distance = BACKGROUND;
		node.color = nodes[node.father].color;
	}
}

		public:
//process ray
void trace(vec3 &r, const vec3 &origin, const vec3 &direction)
{
	//fire primary ray
	nodes[0].origin = origin;
	nodes[0].direction = direction;
	int i = getClosestIntersection(0);
	if (i < 0) {
		r = bgColor;
		return;
	}
	//set node[0]
	setReflectionNode(i, 0, 0);

	//setup other nodes
	for (int n = 1; n < NUM_NODES; ++n)
	{
		nodes[n].distance = RT_INFINITY;
	}

	//generate node data -> fill the heap structure
	int width = 1;
	int n = 0;
	for (int depth = 1; depth <= MAX_DEPTH; ++depth)
	{
		for (int j = 0; j < width; ++j, ++n)
		{
			//calculate child node indices: rfr - refraction, rfl - reflection
			int rfr = n * 2 + 1;
			int rfl = rfr + 1;
			nodes[rfl].father = n;
			nodes[rfr].father = n;

			if (nodes[n].distance < BACKGROUND) {
				//calculate reflection ray
				vec3::reflect(nodes[rfl].direction, nodes[n].direction, nodes[n].normal);

				vec3::scale(nodes[rfl].origin, nodes[rfl].direction, EPSILON);
				vec3::add(nodes[rfl].origin, nodes[rfl].origin, nodes[n].position);


				int irfl = getClosestIntersection(rfl);
				setReflectionNode(irfl, rfl, depth);

				//calculate refraction ray
				vec3::refract(nodes[rfr].direction, nodes[n].direction, nodes[n].normal, real(1) / REFRACTION);
				vec3::add(nodes[rfr].origin, nodes[n].position, nodes[n].direction);

				intersect(0, nodes[n].dataIndex, rfr);
				processIntersection(0, rfr);
				nodes[rfr].rfrDistance = intersections[0].distance;

				vec3::refract(nodes[rfr].direction, nodes[rfr].direction,
					intersections[0].normal, REFRACTION);

				vec3::scale(nodes[rfr].origin, nodes[rfr].direction, EPSILON);
				vec3::add(nodes[rfr].origin, nodes[rfr].origin, intersections[0].position);

				int irfr = getClosestIntersection(rfr);
				setRefractionNode(irfr, rfr, depth);
			}
		}
		width *= 2;
	}

	//combine nodes: process two nodes each step and calculate color of father
	//for (int n = NUM_NODES - 1; n > 0; --n, --n)
	for (int n = NUM_NODES - 1; n > 0; n -= 2)
	{
		int fn = nodes[n].father;
		if(nodes[fn].distance >= BACKGROUND) continue;

		vec3 reflection;
		vec3 refraction;

		//get reflection
		if (nodes[n].distance <= BACKGROUND)
		{
			reflection = nodes[n].color;
		}

		//get refraction
		if (nodes[n-1].distance <= BACKGROUND)
		{
			const real scaling = exp(real(-0.15) * nodes[n-1].rfrDistance);
			vec3::mul(refraction, nodes[fn].color, nodes[n-1].color);
			vec3::scale(refraction, refraction, scaling);

			//refraction = scale(mul(nodes[fn].color, nodes[n-1].color),
			//	pow(2.718281828459045, -0.15f * nodes[n-1].rfrDistance));
		}

		//combine
		vec3 tmp;

		vec3::scale(tmp, nodes[fn].direction, real(-1));
		vec3::normalize(tmp, tmp);

		real fresnel = clamp(max(vec3::dot(tmp, nodes[fn].normal), real(0)), real(0), real(1));

		vec3::mixv(nodes[fn].color, refraction, reflection, fresnel);
	}

	r = nodes[0].color;
}


};

/* generates the scene data for the uniform array */
void generateData(real f)
{
	static int count = 0;
	static Sim sim(data, NUM_OBJECTS, vec3(-12,-6,-5), vec3(12,30,20));
	static const real dt(0.05);

	srand(42);

	if(count < 500)
	{
		for (int i = 0; i < NUM_OBJECTS; ++i)
		{
			real angle = f + (real)i / (real)(NUM_OBJECTS/2) * real(2) * real(M_PI) + real(M_PI) * real(0.5);

			//position and radius
			angle += (((real)(rand() % 1000)) / real(1000)) * real(0.01);
			real si = i < (NUM_OBJECTS/2) ? 1 : -1;
			real ad = i < (NUM_OBJECTS/2) ? 0 : 11;
			data[i].position.x = real(11.0) * cos(angle * si);
			data[i].position.y = real(3.5) * sin(angle*real(1.3)) * cos(angle*real(1.3)) + ad - 4;
			data[i].position.z = real(11.0) * sin(angle * si) + 15;
			data[i].radius = real(2.5);

			//set color
			data[i].color = colors[i%2];
		}

		++count;
	}
	else
	{
		sim.move(dt);
	}

	sim.collide();
}

void render(int w, int h, int b, int e, real clock, int inc, unsigned char* outdata)
{
	generateData(clock);
	real ratio = ((real)w)/((real)h);

	//real angle = (clock + real(M_PI) * real(0.5)) * real(1.73);
	//lightPosition.x = real(50.0) * cos(-angle);
	//lightPosition.y = real(0.0);
	//lightPosition.z = real(50.0) * sin(-angle);

	#pragma omp parallel for schedule(dynamic, 1)
	for (int i = b; i < e; i+=inc)
	{
		Trace tr;
		vec3 color;
		vec3 direction;
		real y = ((real)i/(real)(h-1)) * real(2) - real(1);

		for (int j = 0; j < w; j+=inc)
		{
			real x = ((real)j/(real)(w-1)) * real(2) - real(1);
			direction.x = ratio * x;
			direction.y = y;
			direction.z = real(2);
			vec3::normalize(direction, direction);
			tr.trace(color, origin, direction);

			for(int ai = 0; ai < inc; ++ai)
			{
				for(int bi = 0; bi < inc; ++bi)
				{
					if((i-bi+ai) >= e) break;
					if((j+bi) >= w) break;

					int index = (w*(i-b+ai) + j+bi) * 3;

					outdata[index] = (unsigned char)(clamp((int)(color.x * real(255.0)),0,255));
					outdata[index+1] = (unsigned char)(clamp((int)(color.y * real(255.0)),0,255));
					outdata[index+2] = (unsigned char)(clamp((int)(color.z * real(255.0)),0,255));

				}
			}
		}
	}
}

#endif

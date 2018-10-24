#ifndef RT_SIM_H
#define RT_SIM_H

#include "vec.h"
#include "precision.h"
#include "ball.h"

#define RESTITUTION 0.999

class Sim
{
	private:
	data_t *data;
	int numballs;
	vec3 minr;
	vec3 maxr;

	public:
	Sim(data_t *data_, int numballs_, vec3 AABB_min, vec3 AABB_max) : data(data_), numballs(numballs_), minr(AABB_min), maxr(AABB_max) {}

	void move(real dt)
	{
		vec3 grav(0,-10,0);
		vec3 tmp;

		for(int i = 0; i < numballs; ++i)
		{
			data_t &current = data[i];
			vec3::scale(tmp, grav, dt);
			vec3::add(current.vel, current.vel, tmp);
			vec3::scale(tmp, current.vel, dt);
			vec3::add(current.position, current.position, tmp);
		}
	}

	void collide()
	{
		for(int i = 0; i < numballs; ++i)
		{
			data_t &a = data[i];
			for(int k = i+1; k < numballs; ++k)
			{
				data_t &b = data[k];

				collideTwo(a, b);
			}
		}

		collideWithWalls();
	}

	private:
	static void collideTwo(data_t &a, data_t &b)
	{
		vec3 d;
		vec3::sub(d, b.position, a.position);

		// distance between circle centres, squared
		real distance_squared = vec3::dot(d, d);

		// combined radius squared
		real radius = b.radius + a.radius;
		real radius_squared = radius * radius;

		// circles too far apart
		if(distance_squared <= radius_squared)
		{
			// distance between circle centres
			real distance = sqrt(distance_squared);

			// normal of collision
			vec3 ncoll;
			vec3::scale(ncoll, d, real(1)/distance);

			// penetration distance
			real dcoll = (radius - distance);

			// inverse masses (0 means, infinite mass, object is static).
			real amass = a.radius * a.radius * a.radius * real(4.19);//sphere volume
			real bmass = b.radius * b.radius * b.radius * real(4.19);//sphere volume
			real ima = (amass > real(0)) ? (real(1) / amass) : real(0);
			real imb = (bmass > real(0)) ? (real(1) / bmass) : real(0);

			// separation vector
			vec3 separation_vector;
			vec3::scale(separation_vector, ncoll, dcoll / (ima + imb));

			vec3 tmp;
			// separate the circles
			vec3::scale(tmp, separation_vector, ima);
			vec3::sub(a.position, a.position, tmp);

			vec3::scale(tmp, separation_vector, imb);
			vec3::add(b.position, b.position, tmp);

			// combined velocity
			vec3 vcoll;
			vec3::sub(vcoll, b.vel, a.vel);

			// impact speed
			real vn = vec3::dot(vcoll, ncoll);

			// obejcts are moving away. dont reflect velocity
			if(vn > real(0)) return;

			// collision impulse
			real j = - (real(1) + RESTITUTION) * (vn) / (ima + imb);

			// collision impusle vector
			vec3 impulse;
			vec3::scale(impulse, ncoll, j);

			// change momentum of the circles
			vec3::scale(tmp, impulse, ima);
			vec3::sub(a.vel, a.vel, tmp);

			vec3::scale(tmp, impulse, imb);
			vec3::add(b.vel, b.vel, tmp);
		}
	}

	void collideWithWalls()
	{
		//very stable implementation

		for(int i = 0; i < numballs; ++i)
		{
			data_t &current = data[i];
			vec3 &pos = current.position;
			vec3 &vel = current.vel;

			if(pos.x > maxr.x)
			{
				pos.x = maxr.x;
				if(vel.x > real(0)) vel.x *= real(-RESTITUTION);
			}
			else if(pos.x < minr.x)
			{
				pos.x = minr.x;
				if(vel.x < real(0)) vel.x *= real(-RESTITUTION);
			}

			if(pos.y > maxr.y)
			{
				pos.y = maxr.y;
				if(vel.y > real(0)) vel.y *= real(-RESTITUTION);
			}
			else if(pos.y < minr.y)
			{
				pos.y = minr.y;
				if(vel.y < real(0)) vel.y *= real(-RESTITUTION);
			}

			if(pos.z > maxr.z)
			{
				pos.z = maxr.z;
				if(vel.z > real(0)) vel.z *= real(-RESTITUTION);
			}
			else if(pos.z < minr.z)
			{
				pos.z = minr.z;
				if(vel.z < real(0)) vel.z *= real(-RESTITUTION);
			}
		}
	}

};


#endif
